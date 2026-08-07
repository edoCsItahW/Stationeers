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
#include <iostream>

namespace stationeers::ic10 {

    Engine::Engine(const Program& program, const SymbolTable& symbols, const Config& config)
        : context_(program, symbols, config)
        , executor_(context_)
        , cfg_(config) {}

    void Engine::runTick() {
        if (context_.isSleeping()) return;

        std::size_t count = 0;
        while (!context_.halted() && count < cfg_.maxInstructions) {
            if (!executor_.execute()) break;

            ++count;
        }

        context_.tick();
    }

    void Engine::runFull() {
        while (!context_.halted())
            if (!executor_.execute()) break;
    }

    Context& Engine::getContext() { return context_; }

}  // namespace stationeers::ic10
