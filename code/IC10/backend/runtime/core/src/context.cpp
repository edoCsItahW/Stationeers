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

namespace stationeers::ic10 {

    Context::Context(const Program& program, const SymbolTable& symbols, const Config& config)
        : program(program)
        , symbols(symbols)
        , pc_(0)
        , halted_(false)
        , memory(config)
        , cfg(config) {
        buildAddrs();
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
                program.statements[i]
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
