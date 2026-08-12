// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file nullary.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/09 17:10
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/executor/executor.hpp"

namespace stationeers::ic10 {

    void Executor::executeIns(const HcfInstruction&, Flag& flag) {
        ctx_.halt();
        flag.halted = true;
    }

    void Executor::executeIns(const YieldInstruction&, Flag&) {}

}  // namespace stationeers::ic10