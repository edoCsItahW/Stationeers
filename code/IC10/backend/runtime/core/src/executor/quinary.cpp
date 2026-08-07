// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file quinary.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/09 17:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/executor/executor.hpp"
#include "ic10_runtime/value/value.hpp"

namespace stationeers::ic10 {

    // ========================================================================
    // 五元
    // ========================================================================

    void Executor::executeIns(const LbnInstruction& ins, Flag&) {
        auto typeHash = operandValue(ins.operand2);
        auto nameHash = operandValue(ins.operand3);
        if (!typeHash || !nameHash) return;

        auto devices = ctx_.manager.findDevicesByTypeAndName(
            static_cast<int64_t>(*typeHash), static_cast<int64_t>(*nameHash));
        if (devices.empty()) {
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, 0.0);
            return;
        }

        auto prop    = getValue<Identifier>(ins.operand4).value;
        auto modeVal = operandValue(ins.operand5);
        if (!modeVal) return;

        int mode = static_cast<int>(*modeVal);
        double result = 0.0;
        bool first = true;
        for (auto* dev : devices) {
            double val = dev->readLogic(prop);
            switch (mode) {
                case 0:
                case 1: result += val; break;
                case 2: if (first || val < result) result = val; break;
                case 3: if (first || val > result) result = val; break;
            }
            first = false;
        }
        if (mode == 0) result /= static_cast<double>(devices.size());
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const LbsInstruction& ins, Flag&) {
        auto hash = operandValue(ins.operand2);
        if (!hash) return;

        auto devices = ctx_.manager.findDevicesByType(static_cast<int64_t>(*hash));
        if (devices.empty()) {
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, 0.0);
            return;
        }

        auto idx     = operandValue(ins.operand3);
        auto slot    = getValue<Identifier>(ins.operand4).value;
        auto modeVal = operandValue(ins.operand5);
        if (!idx || !modeVal) return;

        int mode = static_cast<int>(*modeVal);
        double result = 0.0;
        bool first = true;
        for (auto* dev : devices) {
            double val = dev->readSlot(static_cast<std::size_t>(*idx), slot);
            switch (mode) {
                case 0:
                case 1: result += val; break;
                case 2: if (first || val < result) result = val; break;
                case 3: if (first || val > result) result = val; break;
            }
            first = false;
        }
        if (mode == 0) result /= static_cast<double>(devices.size());
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

}  // namespace stationeers::ic10