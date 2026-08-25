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

#include "ic10_compiler/semantic/semantic.hpp"
#include "common/exception/diagnostic.hpp"
#include "ic10_runtime/config.hpp"
#include "executor/executor.hpp"
#include "context/context.hpp"


namespace stationeers::ic10 {

    class Engine {
    public:
        Engine(const Program& program, const SymbolTable& symbols, const Config& config);

        void runTick();

        void runFull();

        /**
         * @brief 单步执行原语：执行恰好一条语句。
         * @details runTick()/runFull() 均基于该原语循环实现；调试器的“逐步骤”也应调用本方法。
         * @return true 表示仍可继续执行；false 表示已 halt（或执行失败/暂停）。
         */
        bool step();

        Context& getContext() noexcept;

        const std::vector<Diagnostic>& getDiagnostics() const noexcept;

    private:
        DiagnosticReporter<IC10RuntimeMsgPack> reporter_;

        Context context_;

        Executor executor_;

        Config cfg_;
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_ENGINE_HPP
