// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file quaternary.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/09 17:11
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/executor/executor.hpp"
#include "ic10_runtime/value/value.hpp"

namespace stationeers::ic10 {

    // ========================================================================
    // 四元 — RI_RON_RON_RON 组
    // ========================================================================

    void Executor::executeIns(const ClampInstruction& ins, Flag&) {
        auto val = operandValue(ins.operand2);
        auto lo  = operandValue(ins.operand3);
        auto hi  = operandValue(ins.operand4);
        if (val && lo && hi)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::clamp(*val, *lo, *hi));
    }

    void Executor::executeIns(const LerpInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        auto t = operandValue(ins.operand4);
        if (a && b && t) {
            double ratio = std::clamp(*t, 0.0, 1.0);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, *a + (*b - *a) * ratio);
        }
    }

    void Executor::executeIns(const ExtInstruction& ins, Flag&) {
        auto src = operandValue(ins.operand2);
        auto pos = operandValue(ins.operand3);
        auto len = operandValue(ins.operand4);
        if (src && pos && len) {
            auto bits       = static_cast<uint64_t>(*src);
            int p           = static_cast<int>(*pos);
            int l           = static_cast<int>(*len);
            uint64_t mask   = (l >= 64) ? ~uint64_t{0} : ((uint64_t{1} << l) - 1);
            uint64_t result = (bits >> p) & mask;
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
        }
    }

    void Executor::executeIns(const InsInstruction& ins, Flag&) {
        auto src = operandValue(ins.operand2);
        auto pos = operandValue(ins.operand3);
        auto len = operandValue(ins.operand4);
        if (src && pos && len) {
            auto cur = ctx_.memory.getReg(getValue<Register>(ins.operand1).value);

            auto curBits  = static_cast<uint64_t>(cur);
            auto srcBits  = static_cast<uint64_t>(*src);
            int p         = static_cast<int>(*pos);
            int l         = static_cast<int>(*len);
            uint64_t mask = (l >= 64) ? ~uint64_t{0} : ((uint64_t{1} << l) - 1);

            curBits &= ~(mask << p);
            curBits |= (srcBits & mask) << p;

            ctx_.memory.setReg(
                getValue<Register>(ins.operand1).value, curBits
            );
        }
    }

    void Executor::executeIns(const SapInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        auto c        = operandValue(ins.operand4);
        double result = 0.0;
        if (a && b && c) {
            double eps = std::max(*c * std::max(std::abs(*a), std::abs(*b)), EPSILON_TIMES_8);
            result     = (std::abs(*a - *b) <= eps) ? 1.0 : 0.0;
        }
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SnaInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        auto c        = operandValue(ins.operand4);
        double result = 0.0;
        if (a && b && c) {
            double eps = std::max(*c * std::max(std::abs(*a), std::abs(*b)), EPSILON_TIMES_8);
            result     = (std::abs(*a - *b) > eps) ? 1.0 : 0.0;
        }
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SelectInstruction& ins, Flag&) {
        auto cond = operandValue(ins.operand2);
        auto b    = operandValue(ins.operand3);
        auto c    = operandValue(ins.operand4);
        if (cond && b && c)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, (*cond != 0.0) ? *b : *c);
    }

    // ========================================================================
    // 四元 — DR_SI_LS_RI 组 (ss)
    // ========================================================================

    void Executor::executeIns(const SsInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            auto idx  = operandValue(ins.operand2);
            auto slot = getValue<Identifier>(ins.operand3).value;
            auto val  = operandValue(ins.operand4);
            if (idx && val) dev->writeSlot(static_cast<std::size_t>(*idx), slot, *val);
        }
    }

    // ========================================================================
    // 四元 — RI_RON_LT_BM 组 (lb)
    // ========================================================================

    void Executor::executeIns(const LbInstruction& ins, Flag&) {
        auto hash = operandValue(ins.operand2);
        if (!hash) return;

        auto devices = ctx_.manager.findDevicesByType(static_cast<int64_t>(*hash));
        if (devices.empty()) {
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, 0.0);
            return;
        }

        auto prop    = getValue<Identifier>(ins.operand3).value;
        auto modeVal = operandValue(ins.operand4);
        if (!modeVal) return;

        int mode      = static_cast<int>(*modeVal);  // 0=Avg, 1=Sum, 2=Min, 3=Max
        double result = 0.0;
        bool first    = true;
        for (auto* dev : devices) {
            double val = dev->readLogic(prop);
            switch (mode) {
                case 0:  // Average
                case 1:  // Sum
                    result += val;
                    break;
                case 2:  // Min
                    if (first || val < result) result = val;
                    break;
                case 3:  // Max
                    if (first || val > result) result = val;
                    break;
            }
            first = false;
        }
        if (mode == 0) result /= static_cast<double>(devices.size());
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    // ========================================================================
    // 四元 — RON_RON_LT_RI 组 (sbn)
    // ========================================================================

    void Executor::executeIns(const SbnInstruction& ins, Flag&) {
        auto typeHash = operandValue(ins.operand1);
        auto nameHash = operandValue(ins.operand2);
        if (!typeHash || !nameHash) return;

        auto prop = getValue<Identifier>(ins.operand3).value;
        auto val  = operandValue(ins.operand4);
        if (!val) return;

        auto devices = ctx_.manager.findDevicesByTypeAndName(
            static_cast<int64_t>(*typeHash), static_cast<int64_t>(*nameHash)
        );
        for (auto* dev : devices) dev->writeLogic(prop, *val);
    }

    // ========================================================================
    // 四元 — RON_SI_LS_RI 组 (sbs)
    // ========================================================================

    void Executor::executeIns(const SbsInstruction& ins, Flag&) {
        auto hash = operandValue(ins.operand1);
        if (!hash) return;

        auto devices = ctx_.manager.findDevicesByType(static_cast<int64_t>(*hash));
        /// @todo 批量写入，遍历所有设备
        auto idx  = operandValue(ins.operand2);
        auto slot = getValue<Identifier>(ins.operand3).value;
        auto val  = operandValue(ins.operand4);
        if (idx && val)
            for (auto* dev : devices) dev->writeSlot(static_cast<std::size_t>(*idx), slot, *val);
    }

    // ========================================================================
    // 四元 — RON_RON_RON_RON 组
    // ========================================================================

    static bool checkApprox(double a, double b, double c) noexcept {
        double eps = std::max(c * std::max(std::abs(a), std::abs(b)), EPSILON_TIMES_8);
        return std::abs(a - b) <= eps;
    }

    void Executor::executeIns(const BapInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        auto c = operandValue(ins.operand3);
        if (a && b && c && checkApprox(*a, *b, *c)) {
            if (auto target = operandValue(ins.operand4); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BapalInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        auto c = operandValue(ins.operand3);
        if (a && b && c && checkApprox(*a, *b, *c)) {
            if (auto target = operandValue(ins.operand4); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnaInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        auto c = operandValue(ins.operand3);
        if (a && b && c && !checkApprox(*a, *b, *c)) {
            if (auto target = operandValue(ins.operand4); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnaalInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        auto c = operandValue(ins.operand3);
        if (a && b && c && !checkApprox(*a, *b, *c)) {
            if (auto target = operandValue(ins.operand4); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrapInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        auto c = operandValue(ins.operand3);
        if (a && b && c && checkApprox(*a, *b, *c)) {
            if (auto offset = operandValue(ins.operand4); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrnaInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        auto c = operandValue(ins.operand3);
        if (a && b && c && !checkApprox(*a, *b, *c)) {
            if (auto offset = operandValue(ins.operand4); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    // ========================================================================
    // 四元 — RI_DR_SI_LS 组 (ls)
    // ========================================================================

    void Executor::executeIns(const LsInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand2);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            auto idx  = operandValue(ins.operand3);
            auto slot = getValue<Identifier>(ins.operand4).value;
            if (idx)
                ctx_.memory.setReg(
                    getValue<Register>(ins.operand1).value,
                    dev->readSlot(static_cast<std::size_t>(*idx), slot)
                );
        }
    }

    // ========================================================================
    // 四元 — RI_DR_RM_JT 组 (lr)
    // ========================================================================

    void Executor::executeIns(const LrInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand2);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            if (auto mode = operandValue(ins.operand3); mode)
                ctx_.memory.setReg(
                    getValue<Register>(ins.operand1).value,
                    dev->readReagent(static_cast<int>(*mode))
                );
        }
    }


}  // namespace stationeers::ic10