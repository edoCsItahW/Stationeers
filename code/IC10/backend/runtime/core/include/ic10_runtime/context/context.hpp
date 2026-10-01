// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:36
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_CONTEXT_HPP
#define IC10_RUNTIME_CONTEXT_HPP
#pragma once

#include "ic10_compiler/semantic/semantic.hpp"
#include "common/utils/bidirectional_map.hpp"
#include "common/exception/diagnostic.hpp"
#include "ic10_runtime/memory/memory.hpp"
#include "ic10_compiler/pch/ast.hpp"
#include "ic10_runtime/manager.hpp"
#include "ic10_runtime/config.hpp"

namespace stationeers::ic10 {

    class Context {
    public:
        /**
         * @if zh
         * @brief 构造执行上下文
         * @param program 已解析的程序
         * @param symbols 语义分析产出的符号表（别名、常量、标签）
         * @param config 运行时配置
         * @param types 语义分析产出的类型表（设备/枚举注解）；枚举常量操作数（如 `Color.Green`）
         *              的求值依赖它，缺失时枚举操作数无法求值
         *
         * @else
         * @brief Construct the execution context
         * @param program The parsed program
         * @param symbols The symbol table produced by semantic analysis (aliases, constants, labels)
         * @param config Runtime configuration
         * @param types The type table produced by semantic analysis (device/enum annotations);
         *              evaluating enum operands such as `Color.Green` depends on it, and without it
         *              enum operands cannot be evaluated
         *
         * @endif
         */
        Context(
            const Program& program, const SymbolTable& symbols, const Config& config = {},
            const TypeTable& types = {}
        );

        Context(Context&&) noexcept = default;
        Context& operator=(Context&&) noexcept = default;

        template<typename T = std::size_t>
        [[nodiscard]] T getPC() const noexcept;

        template<typename T>
            requires std::is_arithmetic_v<T>
        void setPC(T pc) noexcept;

        void advancePC() noexcept;

        void halt() noexcept;

        [[nodiscard]] bool halted() const noexcept;

        [[nodiscard]] bool isSleeping() const noexcept;

        void sleep(double seconds);

        void tick() noexcept;

        template<typename T = std::size_t>
        [[nodiscard]] std::optional<T> getAddr(int line) noexcept;

        template<typename T = int>
        [[nodiscard]] std::optional<T> getLine(std::size_t addr) noexcept;

        std::optional<std::reference_wrapper<const Statement>> currentStatement() const noexcept;

        std::optional<std::shared_ptr<Symbol>> resolve(const std::string& name) const;

        template<typename Self>
        auto& getDiagnostics(this Self& self) noexcept;

        void setReporter(DiagnosticReporter<IC10RuntimeMsgPack>* reporter) noexcept;

        Program program;

        SymbolTable symbols;

        Memory memory;

        Manager manager;

        Config cfg;

        /// @brief 类型表（设备/枚举注解），枚举常量操作数的求值来源
        TypeTable types;

    private:
        std::size_t currentTick_ = 0;

        std::size_t sleepUntilTick_ = 0;

        std::size_t pc_;

        DiagnosticReporter<IC10RuntimeMsgPack>* reporter_ = nullptr;

        BiMap<int, std::size_t> addrs_;

        bool halted_;

        void buildAddrs();

        /**
         * @if zh
         * @brief 初始化设备：构建型号表并按源码声明的型号绑定端口
         * @else
         * @brief Initialise devices: build the device type table and bind declared ports
         * @endif
         */
        void initDevices();

        /**
         * @if zh
         * @brief 收集声明了型号的设备符号
         * @return 符号指针列表（生命周期由符号表保证）
         * @else
         * @brief Collect the device symbols that declare a type
         * @return Symbol pointers (their lifetime is owned by the symbol table)
         * @endif
         */
        [[nodiscard]] std::vector<const Symbol*> deviceSymbols() const;
    };

}  // namespace stationeers::ic10

#include "context.inl"

#endif  // IC10_RUNTIME_CONTEXT_HPP
