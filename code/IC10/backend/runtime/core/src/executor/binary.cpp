// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file binary.cpp
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
    // 二元 — RI_RON 组
    // ========================================================================

    void Executor::executeIns(const AbsInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::abs(*a));
    }

    void Executor::executeIns(const AcosInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::acos(*a));
    }

    void Executor::executeIns(const AsinInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::asin(*a));
    }

    void Executor::executeIns(const AtanInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::atan(*a));
    }

    void Executor::executeIns(const CeilInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::ceil(*a));
    }

    void Executor::executeIns(const CosInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::cos(*a));
    }

    void Executor::executeIns(const ExpInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::exp(*a));
    }

    void Executor::executeIns(const FloorInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::floor(*a));
    }

    void Executor::executeIns(const LogInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::log(*a));
    }

    void Executor::executeIns(const RoundInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::round(*a));
    }

    void Executor::executeIns(const SinInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::sin(*a));
    }

    void Executor::executeIns(const SqrtInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::sqrt(*a));
    }

    void Executor::executeIns(const TanInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::tan(*a));
    }

    void Executor::executeIns(const TruncInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, std::trunc(*a));
    }

    void Executor::executeIns(const NotInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a) {
            auto bits = static_cast<int64_t>(*a);
            ctx_.memory.setReg(
                getValue<Register>(ins.operand1).value, static_cast<double>(~bits)
            );
        }
    }

    void Executor::executeIns(const MoveInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a)
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, *a);
    }

    void Executor::executeIns(const SgnInstruction& ins, Flag&) {
        if (auto a = operandValue(ins.operand2); a) {
            double result = (*a > 0.0) ? 1.0 : ((*a < 0.0) ? -1.0 : 0.0);
            ctx_.memory.setReg(getValue<Register>(ins.operand1).value, result);
        }
    }

    void Executor::executeIns(const SeqzInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, (a && *a == 0.0) ? 1.0 : 0.0);
    }

    void Executor::executeIns(const SnezInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, (a && *a != 0.0) ? 1.0 : 0.0);
    }

    void Executor::executeIns(const SgezInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, (a && *a >= 0.0) ? 1.0 : 0.0);
    }

    void Executor::executeIns(const SgtzInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, (a && *a > 0.0) ? 1.0 : 0.0);
    }

    void Executor::executeIns(const SlezInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, (a && *a <= 0.0) ? 1.0 : 0.0);
    }

    void Executor::executeIns(const SltzInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, (a && *a < 0.0) ? 1.0 : 0.0);
    }

    void Executor::executeIns(const SnanInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(
            getValue<Register>(ins.operand1).value, (a && std::isnan(*a)) ? 1.0 : 0.0
        );
    }

    void Executor::executeIns(const SnanzInstruction& ins, Flag&) {
        auto a = operandValue(ins.operand2);
        ctx_.memory.setReg(
            getValue<Register>(ins.operand1).value, (a && !std::isnan(*a)) ? 1.0 : 0.0
        );
    }

    // ========================================================================
    // 二元 — DR_RON 组
    // ========================================================================

    void Executor::executeIns(const BdnsInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); !dev) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BdnsalInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); !dev) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BdseInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BdsealInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrdnsInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); !dev)
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
    }

    void Executor::executeIns(const BrdseInstruction& ins, Flag& flag) {
        auto devOp = std::get_if<Device>(&ins.operand1);
        if (!devOp) return;

        if (IDevice* dev = ctx_.manager.getDevice(devOp->value); dev)
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
    }

    // ========================================================================
    // 二元 — RI_DR 组
    // ========================================================================

    void Executor::executeIns(const SdnsInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand2);
        if (!devOp) return;

        IDevice* dev = ctx_.manager.getDevice(devOp->value);

        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, dev ? 1.0 : 0.0);
    }

    void Executor::executeIns(const SdseInstruction& ins, Flag&) {
        auto devOp = std::get_if<Device>(&ins.operand2);
        if (!devOp) return;

        IDevice* dev = ctx_.manager.getDevice(devOp->value);

        ctx_.memory.setReg(getValue<Register>(ins.operand1).value, dev ? 1.0 : 0.0);
    }

    // ========================================================================
    // 二元 — RON_RON 组
    // ========================================================================

    void Executor::executeIns(const PokeInstruction& ins, Flag&) {
        auto addr = operandValue(ins.operand1);
        auto val  = operandValue(ins.operand2);

        if (addr && val) {
            // poke 语义：索引 0 = 栈顶 (sp-1)，索引 1 = 栈顶下一元素 (sp-2)
            if (ctx_.memory.getSP() <= 0) return;
            std::size_t relIdx = static_cast<std::size_t>(*addr);
            if (relIdx >= ctx_.memory.getSP()) return;
            std::size_t absIdx = ctx_.memory.getSP<std::size_t>() - 1 - relIdx;

            ctx_.memory.poke(absIdx, *val, [&](const std::string& str) {
                return RangeError(str, ins.start(), ins.end());
            });
        }
    }

    void Executor::executeIns(const BeqzInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);

        if (val && std::abs(*val) < EPSILON_TIMES_8) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BeqzalInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);

        if (val && std::abs(*val) < EPSILON_TIMES_8) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnezInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);

        if (val && std::abs(*val) >= EPSILON_TIMES_8) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnezalInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);

        if (val && std::abs(*val) >= EPSILON_TIMES_8) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgezInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val >= 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgezalInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val >= 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgtzInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val > 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BgtzalInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val > 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BlezInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val <= 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BlezalInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val <= 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BltzInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val < 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BltzalInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val < 0.0) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.memory.setReg("ra", ctx_.getPC() + 1);
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BnanInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && std::isnan(*val)) {
            if (auto target = operandValue(ins.operand2); target) {
                ctx_.setPC(*target);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BreqzInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && std::abs(*val) < EPSILON_TIMES_8) {
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrnezInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && std::abs(*val) >= EPSILON_TIMES_8) {
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrgezInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val >= 0.0) {
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrgtzInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val > 0.0) {
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrlezInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val <= 0.0) {
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrltzInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && *val < 0.0) {
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
        }
    }

    void Executor::executeIns(const BrnanInstruction& ins, Flag& flag) {
        auto val = operandValue(ins.operand1);
        if (val && std::isnan(*val)) {
            if (auto offset = operandValue(ins.operand2); offset) {
                ctx_.setPC(*offset);
                flag.jumped = true;
            }
        }
    }


}  // namespace stationeers::ic10