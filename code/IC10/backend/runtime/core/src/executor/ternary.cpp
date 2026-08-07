// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ternary.cpp
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
    // 三元 — RI_RON_RON 组
    // ========================================================================

    void Executor::executeIns(const AddInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, *a + *b);
    }

    void Executor::executeIns(const Atan2Instruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::atan2(*a, *b));
    }

    void Executor::executeIns(const DivInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, *a / *b);
    }

    void Executor::executeIns(const MaxInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::max(*a, *b));
    }

    void Executor::executeIns(const MinInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::min(*a, *b));
    }

    void Executor::executeIns(const ModInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::fmod(*a, *b));
    }

    void Executor::executeIns(const MulInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, *a * *b);
    }

    void Executor::executeIns(const PowInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::pow(*a, *b));
    }

    void Executor::executeIns(const SubInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) ctx_.memory.setReg(getValue<Register>(ins.operand1).value, *a - *b);
    }

    void Executor::executeIns(const RolInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bits       = static_cast<uint64_t>(*a);
            int shift       = static_cast<int>(*b) % 64;
            uint64_t result = shift == 0 ? bits : (bits << shift) | (bits >> (64 - shift));
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
        }
    }

    void Executor::executeIns(const RorInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bits       = static_cast<uint64_t>(*a);
            int shift       = static_cast<int>(*b) % 64;
            uint64_t result = (shift == 0) ? bits : (bits >> shift) | (bits << (64 - shift));
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
        }
    }

    void Executor::executeIns(const AndInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bitsA = static_cast<uint64_t>(*a);
            auto bitsB = static_cast<uint64_t>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, bitsA & bitsB);
        }
    }

    void Executor::executeIns(const NorInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bitsA = static_cast<int64_t>(*a);
            auto bitsB = static_cast<int64_t>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, ~(bitsA | bitsB));
        }
    }

    void Executor::executeIns(const OrInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bitsA = static_cast<uint64_t>(*a);
            auto bitsB = static_cast<uint64_t>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, bitsA | bitsB);
        }
    }

    void Executor::executeIns(const SlaInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bits = static_cast<int64_t>(*a);
            int shift = static_cast<int>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, bits << shift);
        }
    }

    void Executor::executeIns(const SllInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bits = static_cast<uint64_t>(*a);
            int shift = static_cast<int>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, bits << shift);
        }
    }

    void Executor::executeIns(const SraInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bits = static_cast<int64_t>(*a);
            int shift = static_cast<int>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, bits >> shift);
        }
    }

    void Executor::executeIns(const SrlInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bits = static_cast<uint64_t>(*a);
            int shift = static_cast<int>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, bits >> shift);
        }
    }

    void Executor::executeIns(const XorInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        auto b = operandValue(ins.operand3);
        if (a && b) {
            auto bitsA = static_cast<uint64_t>(*a);
            auto bitsB = static_cast<uint64_t>(*b);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, bitsA ^ bitsB);
        }
    }

    void Executor::executeIns(const SapzInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        double result = 0.0;
        // Set if Approximately Positive or Zero: 值大于约等于 0
        // 的下容差即视为成立（包含正值、零及容差内的微小负值）
        if (a) result = (*a >= -std::numeric_limits<double>::denorm_min() * 8.0) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SnazInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        double result = 0.0;
        // Set if Approximately Negative: 值小于约等于 0
        // 的下容差才视为成立（排除容差内的近零微小值）
        if (a) result = (*a < -std::numeric_limits<double>::denorm_min() * 8.0) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SeqInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        double result = (a && b && *a == *b) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SneInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        double result = (a && b && *a != *b) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SgeInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        double result = (a && b && *a >= *b) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SgtInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        double result = (a && b && *a > *b) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SleInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        double result = (a && b && *a <= *b) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    void Executor::executeIns(const SltInstruction& ins, Flag&) {
        auto a        = operandValue(ins.operand2);
        auto b        = operandValue(ins.operand3);
        double result = (a && b && *a < *b) ? 1.0 : 0.0;
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
    }

    // ========================================================================
    // 三元 — RI_DR_RON 组 (get)
    // ========================================================================

    void Executor::executeIns(const GetInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand2);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            if (auto addr = operandValue(ins.operand3); addr)
                ctx_.memory.setReg(
                    getValue<Register>(ins.operand1).value,
                    dev->readStack(static_cast<std::size_t>(*addr))
                );
        }
    }

    // ========================================================================
    // 三元 — RI_DAR_RON 组 (rmap)
    // ========================================================================

    void Executor::executeIns(const RmapInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand2);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            if (auto hash = operandValue(ins.operand3); hash)
                ctx_.memory.setReg(
                    getValue<Register>(ins.operand1).value,
                    dev->queryReagentAmount(static_cast<int64_t>(*hash))
                );
        }
    }

    // ========================================================================
    // 三元 — DR_RON_RON 组 (put)
    // ========================================================================

    void Executor::executeIns(const PutInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            auto addr = operandValue(ins.operand2);
            auto val  = operandValue(ins.operand3);

            if (addr && val) dev->writeStack(static_cast<std::size_t>(*addr), *val);
        }
    }

    // ========================================================================
    // 三元 — RI_DR_LT 组 (l)
    // ========================================================================

    void Executor::executeIns(const LInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand2);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            auto prop = getValue<Identifier>(ins.operand3).value;
            auto val  = dev->readLogic(prop);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, val);
        }
    }

    // ========================================================================
    // 三元 — DR_LT_RI 组 (s)
    // ========================================================================

    void Executor::executeIns(const SInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            auto prop = getValue<Identifier>(ins.operand2).value;

            if (auto val = operandValue(ins.operand3); val) dev->writeLogic(prop, *val);
        }
    }

    // ========================================================================
    // 三元 — RON_LT_RI 组 (sb)
    // ========================================================================

    void Executor::executeIns(const SbInstruction& ins, Flag&) {
        auto hash = operandValue(ins.operand1);
        if (!hash) return;

        auto prop = getValue<Identifier>(ins.operand2).value;
        auto val  = operandValue(ins.operand3);
        if (!val) return;

        if (auto dev = ctx_.manager.findDeviceByType(static_cast<std::size_t>(*hash)); dev)
            dev->writeLogic(prop, *val);
    }

    // ========================================================================
    // 三元 — DR_LT_RON 组 (bdnvl / bdnvs)
    // ========================================================================

    void Executor::executeIns(const BdnvlInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            auto prop = getValue<Identifier>(ins.operand2).value;
            if (!dev->canReadLogic(prop)) {
                if (auto target = operandValue(ins.operand3); target) {
                    ctx_.setPC(*target);
                    flag.jumped = true;
                }
            }
        }
    }

    void Executor::executeIns(const BdnvsInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            auto prop = getValue<Identifier>(ins.operand2).value;
            if (!dev->canWriteLogic(prop)) {
                if (auto target = operandValue(ins.operand3); target) {
                    ctx_.setPC(*target);
                    flag.jumped = true;
                }
            }
        }
    }

    // ========================================================================
    // 三元 — RON_RON_RON 组
    // ========================================================================

    void Executor::executeIns(const BeqInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a == *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BeqalInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a == *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BneInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a != *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnealInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a != *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgeInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a >= *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgealInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a >= *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgtInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a > *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgtalInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a > *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BleInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a <= *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BlealInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a <= *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BltInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a < *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BltalInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a < *b) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BapzInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        // Branch if Approximately Positive or Zero: 值大于约等于 0 的下容差即视为成立
        if (a && *a >= -std::numeric_limits<double>::denorm_min() * 8.0) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BapzalInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        // Branch if Approximately Positive or Zero and Link: 值大于约等于 0 的下容差即视为成立
        if (a && *a >= -std::numeric_limits<double>::denorm_min() * 8.0) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnazInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        // Branch if Approximately Negative: 值小于约等于 0 的下容差才视为成立
        if (a && *a < -std::numeric_limits<double>::denorm_min() * 8.0) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnazalInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        // Branch if Approximately Negative and Link: 值小于约等于 0 的下容差才视为成立
        if (a && *a < -std::numeric_limits<double>::denorm_min() * 8.0) {
            if (auto target = operandValue(ins.operand3); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BreqInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a == *b) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrneInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a != *b) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrgeInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a >= *b) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrgtInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a > *b) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrleInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a <= *b) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrltInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        auto b = operandValue(ins.operand2);
        if (a && b && *a < *b) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrapzInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        if (a && std::abs(*a) <= std::numeric_limits<double>::denorm_min() * 8.0) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrnazInstruction& ins, Flag& flag) {
        auto a = operandValue(ins.operand1);
        if (a && std::abs(*a) > std::numeric_limits<double>::denorm_min() * 8.0) {
            if (auto offset = operandValue(ins.operand3); offset) {
                ctx_.setPC(ctx_.getPC() + *offset);
                flag.jumped = true;
            }
        }
    }


}  // namespace stationeers::ic10