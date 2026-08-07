// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file engine.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:37
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_ENGINE_HPP
#define IC10_RUNTIME_ENGINE_HPP
#pragma once

#include "context/context.hpp"
#include "executor/executor.hpp"
#include "ic10_compiler/parser/ast/ast.hpp"
#include "ic10_compiler/semantic/semantic.hpp"
#include "ic10_runtime/config.hpp"

namespace stationeers::ic10 {

    class Engine {
    public:
        Engine(const Program& program, const SymbolTable& symbols, const Config& config);

        void runTick();

        void runFull();

        Context& getContext();

    private:
        Context context_;

        Executor executor_;

        Config cfg_;
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_ENGINE_HPP
