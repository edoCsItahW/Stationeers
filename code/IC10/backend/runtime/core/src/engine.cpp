// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file engine.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:40
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/engine.hpp"
#include "common/exception/debug.hpp"

namespace stationeers::ic10 {

    Engine::Engine(const Program& program, const SymbolTable& symbols, const Config& config)
        : context_(program, symbols, config)
        , executor_(context_)
        , cfg_(config) {
        context_.setReporter(&reporter_);
        executor_.setReporter(&reporter_);
    }

    const std::vector<Diagnostic>& Engine::getDiagnostics() const noexcept {
        return reporter_.getDiagnostics();
    }

    bool Engine::step() {
        if (context_.halted()) return false;

        auto stmtOpt = context_.currentStatement();
        if (!stmtOpt) {
            context_.halt();
            return false;
        }

        auto result = safeExecute([&] { return executor_.execute(); });

        if (!result.success || !result.result) context_.halt();

        return !context_.halted();
    }

    void Engine::runTick() {
        if (context_.isSleeping()) return;

        std::size_t count = 0;
        while (count < cfg_.maxInstructions && step()) ++count;

        context_.tick();
    }

    void Engine::runFull() {
        std::size_t total = 0;
        while (step())
            if (cfg_.maxTotalInstructions > 0
                && ++total >= static_cast<std::size_t>(cfg_.maxTotalInstructions)) {
                context_.halt();
                break;
            }
    }

    Context& Engine::getContext() noexcept { return context_; }

}  // namespace stationeers::ic10
