// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:40
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/context/context.hpp"
#include <optional>
#include <ranges>
#include <string>
#include <utility>

namespace stationeers::ic10 {

    namespace {

        /**
         * @if zh
         * @brief 取符号指向的静态端口
         * @param symbol 设备符号
         * @return 端口名；符号不是静态设备（如动态端口别名 `dr0`）时为空
         * @else
         * @brief Obtain the static port a symbol points at
         * @param symbol The device symbol
         * @return The port name; empty when the symbol is not a static device (e.g. `dr0`)
         * @endif
         */
        std::optional<std::string> symbolPort(const Symbol& symbol) {
            // 别名的值是端口文本（d0），内建端口符号没有值，名字本身就是端口
            const std::string& text = symbol.value && !symbol.value->empty() ? *symbol.value : symbol.name;

            if (!isStaticPort(text)) return std::nullopt;

            return text;
        }

    }  // namespace

    Context::Context(
        const Program& program, const SymbolTable& symbols, const Config& config,
        const TypeTable& types
    )
        : program(program)
        , symbols(symbols)
        , memory(config)
        , cfg(config)
        , types(types)
        , pc_(0)
        , halted_(false) {
        initDevices();
        buildAddrs();
    }

    void Context::initDevices() {
        DeviceRegistry registry;

        // 1) 程序自身的 `#>` 设备块
        for (const auto& statement : program.statements)
            if (const auto* annotation = std::get_if<DeviceAnnotation>(&statement.raw()); annotation)
                registry.add(*annotation);

        // 2) 设备符号引用到的型号：型号可能来自其它编译单元（如标准库），只能按名字查类型表
        const auto devices = deviceSymbols();

        for (const Symbol* symbol : devices)
            if (symbol->type.typeName) registry.addFrom(types, *symbol->type.typeName);

        manager.setRegistry(std::move(registry));

        // 3) 静态端口按源码声明的型号自动绑定；动态端口在运行期换算后才知道落在哪个端口，故不预绑定
        for (const Symbol* symbol : devices)
            if (const auto port = symbolPort(*symbol); port) manager.bindTyped(*port, *symbol->type.typeName);
    }

    std::vector<const Symbol*> Context::deviceSymbols() const {
        std::vector<const Symbol*> result;

        const auto collect = [&result](const Symbol& symbol) {
            if (symbol.type.kind != BasicType::DEVICE || !symbol.type.typeName) return;

            result.push_back(&symbol);
        };

        // 内建端口 d0-d5：别名带类型提示时语义阶段会把类型名一并写到它们身上
        for (const auto& symbol : symbols.builtinSymbols | std::views::values)
            collect(symbol);

        for (auto it = symbols.begin(); it != symbols.end(); ++it) {
            const auto& entry = it->second;

            if (!entry.ready()) continue;

            const auto& resolved = entry.future.get();

            if (resolved.has_value()) collect(*resolved.value());
        }

        return result;
    }

    void Context::setReporter(DiagnosticReporter<IC10RuntimeMsgPack>* reporter) noexcept {
        reporter_ = reporter;
        memory.setReporter(reporter);
    }

    void Context::advancePC() noexcept { ++pc_; }

    void Context::buildAddrs() {
        auto size = program.statements.size();
        for (std::size_t i{0}; i < size; ++i)
            std::visit(
                [this, &i](const auto& stmt) {
                    if (!addrs_.contains(stmt.position.line()))
                        addrs_.insert(stmt.position.line(), i);
                },
                // Statement 是语句变体的句柄，取内部变体后才能访问分支
                program.statements[i].raw()
            );
    }

    void Context::halt() noexcept { halted_ = true; }

    bool Context::halted() const noexcept { return halted_; }

    bool Context::isSleeping() const noexcept { return sleepUntilTick_ > currentTick_; }

    void Context::sleep(double seconds) {
        if (cfg.tickDuration <= 0)
            reporter_->emplace<IRMsgId::IEC2_1>(
                {ValueError{IRLoc::msgFormat<IRMsgId::IEC2_1>("tickDuration")}}
            );

        sleepUntilTick_ = currentTick_ + static_cast<std::size_t>(seconds / cfg.tickDuration);
    }

    void Context::tick() noexcept { ++currentTick_; }

    std::optional<std::reference_wrapper<const Statement>>
    Context::currentStatement() const noexcept {
        if (pc_ >= program.statements.size()) return std::nullopt;

        return std::cref(program.statements[pc_]);
    }

    std::optional<std::shared_ptr<Symbol>> Context::resolve(const std::string& name) const {
        if (const auto& it = symbols.find(name); it != symbols.end() && it->second.ready())
            return *it->second.future.get();

        return std::nullopt;
    }


}  // namespace stationeers::ic10
