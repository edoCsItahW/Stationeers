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
#include "ic10_runtime/memory/memory.hpp"
#include "ic10_compiler/pch/ast.hpp"
#include "ic10_runtime/manager.hpp"
#include "ic10_runtime/config.hpp"

namespace stationeers::ic10 {

    class Context {
    public:
        Context(const Program& program, const SymbolTable& symbols, const Config& config);

        Context(Context&&) noexcept = default;
        Context& operator=(Context&&) noexcept = default;

        template<typename T = std::size_t>
        [[nodiscard]] T getPC() const;

        template<typename T>
            requires std::is_arithmetic_v<T>
        void setPC(T pc);

        void advancePC();

        void halt();

        [[nodiscard]] bool halted() const;

        [[nodiscard]] bool isSleeping() const;

        void sleep(double seconds);

        void tick();

        template<typename T = std::size_t, typename U>
            requires std::is_arithmetic_v<std::decay_t<U>>
        [[nodiscard]] T getAddr(U&& line);

        std::optional<std::reference_wrapper<const Statement>> currentStatement() const;

        std::optional<std::shared_ptr<Symbol>> resolve(const std::string& name) const;

        Program program;

        SymbolTable symbols;

        Memory memory;

        Manager manager;

        Config cfg;

    private:
        std::size_t currentTick_ = 0;

        std::size_t sleepUntilTick_ = 0;

        std::size_t pc_;

        std::unordered_map<std::size_t, std::size_t> addrs_;

        bool halted_;

        void buildAddrs();
    };

}  // namespace stationeers::ic10

#include "context.inl"

#endif  // IC10_RUNTIME_CONTEXT_HPP
