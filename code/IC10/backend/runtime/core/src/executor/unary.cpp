// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file unary.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/09 17:10
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/executor/executor.hpp"
#include "ic10_runtime/value/value.hpp"
#include <random>

namespace stationeers::ic10 {

    // ========================================================================
    // 一元 — RI 组
    // ========================================================================

    void Executor::executeIns(const PeekInstruction& ins, Flag&) {
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, ctx_.memory.peek());
    }

    void Executor::executeIns(const RandInstruction& ins, Flag&) {
        thread_local std::mt19937 gen{std::random_device{}()};
        thread_local std::uniform_real_distribution dist{0.0, 1.0};

        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, dist(gen));
    }

    void Executor::executeIns(const PopInstruction& ins, Flag&) {
        const auto val = ctx_.memory.pop();
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, val);
    }

    // ========================================================================
    // 一元 — DAR 组
    // ========================================================================

    void Executor::executeIns(const ClrInstruction& ins, Flag&) {
        auto dev = getValue<Device>(ins.operand1);

        if (IDevice* device = ctx_.manager.getDevice(dev.value); device) device->clearStack();
    }

    // ========================================================================
    // 一元 — RON 组
    // ========================================================================

    void Executor::executeIns(const SleepInstruction& ins, Flag& flag) {
        if (auto secs = operandValue(ins.operand1); secs) {
            ctx_.sleep(*secs);

            flag.paused = true;
        }
    }

    void Executor::executeIns(const ClrdInstruction& ins, Flag&) {
        auto idxOpt = operandValue(ins.operand1);
        if (!idxOpt) return;

        int devIdx = static_cast<int>(*idxOpt);
        IDevice* dev = nullptr;
        if (devIdx == -1)
            dev = ctx_.manager.getDevice("db");
        else if (devIdx >= 0 && devIdx <= 5)
            dev = ctx_.manager.getDevice("d" + std::to_string(devIdx));

        if (dev) dev->clearStack();
    }

    void Executor::executeIns(const PushInstruction& ins, Flag&) {
        if (auto val = operandValue(ins.operand1); val) ctx_.memory.push(*val);
    }

    // ========================================================================
    // 一元 — JT 组
    // ========================================================================

    void Executor::executeIns(const JalInstruction& ins, Flag& flag) {
        if (const auto line = operandValue(ins.operand1); line) {
            const auto pc = ctx_.getAddr(*line);

            ctx_.memory.setReg("ra", ctx_.getPC() + 1);
            ctx_.setPC(pc);
            flag.jumped = true;
        }
    }

    void Executor::executeIns(const JrInstruction& ins, Flag& flag) {
        // jr: 相对跳转 pc += offset
        if (auto offset = operandValue(ins.operand1); offset) {
            ctx_.setPC(ctx_.getPC<std::size_t>() + static_cast<std::size_t>(*offset));
            flag.jumped = true;
        }
    }

    void Executor::executeIns(const JInstruction& ins, Flag& flag) {
        if (auto line = operandValue(ins.operand1); line) {
            const auto pc = ctx_.getAddr(*line);

            ctx_.setPC(pc);
            flag.jumped = true;
        }
    }

}  // namespace stationeers::ic10
