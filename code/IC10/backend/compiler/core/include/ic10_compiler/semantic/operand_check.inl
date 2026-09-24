// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file operand_check.inl
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @if zh
 * @brief @ref Analyser::IdentifierChecker 各特化的实现：操作数标识符的语义合法性判定
 * @details 设计意图、调用链、返回值约定与各操作数的接受范围/消息编号见 `operand_check.hpp`；本文件
 *          是它们的落地实现。实现层面有三点值得先知道：
 *          - **主模板是「不检查」**（见文件上方定义）：只有真正需要判定的 15 个 `OperandType` 才有
 *            特化，其余默认放行 —— 新增 `OperandType` 时**必须**确认是有意放行还是漏写特化。
 *          - **三级判定的重复结构**：`LOGIC_PROP` / `LOGIC_SLOT_PROP` / `AGG_MODE` / `REAGENT_MODE`
 *            都遵循「设备注解命中 → 全局枚举命中 → 退化为 `NUMBER` 类别」，四者只是查的表与消息编号
 *            不同，改动一处时请对照另三处。
 *          - **「上报后返回 `true`」是刻意的**：让调用方跳过后续检查，避免同一位置重复报错。
 * @see operand_check.hpp 设计说明、调用链与消息编号表
 * @see analyser.inl `handleOperand` 如何选择调用或不调用本文件
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Implementations of the @ref Analyser::IdentifierChecker specializations: semantic legality of
 *        operand identifiers
 * @details The intent, call chain, return-value convention and the accepted-range/message-id table live
 *          in `operand_check.hpp`; this file is where they land. Three implementation notes up front:
 *          - **The primary template means "no check"** (defined at the top of this file): only the 15
 *            `OperandType`s that genuinely need judging have a specialization, everything else passes by
 *            default — so when adding an `OperandType`, be sure whether the pass-through is intended or
 *            a missing specialization.
 *          - **The three-level decision repeats**: `LOGIC_PROP` / `LOGIC_SLOT_PROP` / `AGG_MODE` /
 *            `REAGENT_MODE` all follow "device annotation hit → global enum hit → fall back to category
 *            `NUMBER`", differing only in the table consulted and the message id; change one and check
 *            the other three.
 *          - **"Report, then return `true`" is deliberate**: it makes the caller skip further checks and
 *            avoids duplicate diagnostics at one position.
 * @warning `SLOT_IDX`'s negative check uses `std::stoi`, which throws `std::out_of_range` beyond `int`
 *          range and is not caught here — a potential crash site (marked in place with a `// TODO:`).
 * @see operand_check.hpp design notes, call chain and message-id table
 * @see analyser.inl how `handleOperand` decides whether to call this file at all
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_OPERAND_CHECK_INL
#define IC10_COMPILER_CORE_OPERAND_CHECK_INL
#pragma once

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 主模板：**不做任何检查**，一律判为合法
     * @details 没有特化的 `OperandType` 走这里。因为参数未命名，本函数不看符号也不看操作数。
     * @warning 这是「默认放行」而非「默认拒绝」：新增 `OperandType` 却忘记补特化时，既不会编译失败
     *          （与 `semantic_operand.hpp` 的 fail-fast 相反），也不会有任何诊断 —— 请有意识地确认。
     * @return 恒为 `true`
     *
     * @elseif en
     *
     * @brief Primary template: **no check at all**, always valid
     * @details Any `OperandType` without a specialization lands here. The parameters are unnamed, so this
     *          looks at neither the symbol nor the operand.
     * @warning This is "pass by default", not "reject by default": adding an `OperandType` and forgetting
     *          its specialization produces neither a compile error (unlike the fail-fast in
     *          `semantic_operand.hpp`) nor any diagnostic — confirm such pass-throughs deliberately.
     * @return Always `true`
     *
     * @endif
     */
    template<OperandType Type>
    bool Analyser::IdentifierChecker<Type>::check(
        const Analyser*, const std::shared_ptr<Symbol>&, auto&&
    ) noexcept {
        // 无需检查
        return true;
    }

    // Register or Identifier - Register | Identifier
    bool Analyser::IdentifierChecker<OperandType::REG_TARGET>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // r? : 目标寄存器（写入结果），必须是纯寄存器
        return self->checkOperandType<ICMsgId::IWA1_1, BasicType::REGISTER>(symbol, arg);
    }

    // Register or Number - Register | Identifier | Enum | [Number]
    bool Analyser::IdentifierChecker<OperandType::NUM_VALUE>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // r?|num : 通用数值操作数（读取值参与运算/比较/赋值）
        return self->checkOperandType<
            ICMsgId::IWA3_1, BasicType::REGISTER, BasicType::INTEGER, BasicType::FLOAT,
            BasicType::ENUM, TypeCategory::NUMBER>(symbol, arg);
    }

    // Jump Line - Identifier | Register | [Number]
    bool Analyser::IdentifierChecker<OperandType::JUMP_LINE>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // int, r?|num : （跳转指令中）跳转目标行号（绝对或相对），需解析为整数行号。
        return self->checkOperandType<
            ICMsgId::IWA7_1, TypeCategory::LABEL, TypeCategory::NUMBER, BasicType::REGISTER>(
            symbol, arg
        );
    }

    // Address - Register | Number（get/put/poke 的堆栈地址索引）
    bool Analyser::IdentifierChecker<OperandType::ADDRESS>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // address(r?|num) : 堆栈地址索引（用于 get/put/poke）
        return self->checkOperandType<ICMsgId::IWA19_1, BasicType::REGISTER, TypeCategory::NUMBER>(
            symbol, arg
        );
    }

    // Slot Index - [Number]
    bool Analyser::IdentifierChecker<OperandType::SLOT_IDX>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // slotIdx : 设备槽位索引（需为非负整数）

        // 根据设备上下文，对设备槽位索引进行范围检查
        if (symbol->type.typeName && self->typeTable_->find(*symbol->type.typeName)) {
            const auto& deviceTypeName = *symbol->type.typeName;

            return std::visit(
                [&]<typename T, typename U = std::remove_cvref_t<T>>(T&& type) {
                    // 类型为设备，一般不会是枚举，在别名定义已被上报
                    if constexpr (std::is_same_v<U, DeviceAnnotation>) {
                        if (symbol->value) {
                            bool flag = std::ranges::contains(
                                type.slots | std::views::transform(&DeviceAnnotationSlot::value),
                                *symbol->value
                            );

                            if (!flag)
                                self->reporter_->errorWith<ICMsgId::IWA16_2>(
                                    arg.start(), arg.end(), *symbol->value, deviceTypeName
                                );

                            return flag;
                        }

                        return false;
                    } else
                        return false;
                },
                *self->typeTable_->find(deviceTypeName)
            );
        }


        if (self->checkOperandType<ICMsgId::IWA9_1, TypeCategory::NUMBER>(symbol, arg)) {
            // 负数检查
            if (symbol->value && std::stoi(*symbol->value) < 0)
                // 报告同时返回true避免重复报告
                self->reporter_->error<ICMsgId::IEA9>(arg.start(), arg.end());

            return true;
        }

        return false;
    }

    // Hardware Id - Register | Number（clrd 的设备硬件 ID）
    bool Analyser::IdentifierChecker<OperandType::HARDWARE_ID>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // id(r?|num) | （clrd/getd 等）设备数字硬件 ID（非端口名）
        return self->checkOperandType<
            ICMsgId::IWA20_1, BasicType::REGISTER, BasicType::ENUM, TypeCategory::NUMBER>(
            symbol, arg
        );
    }

    // Reagent Hash - Register | Number（rmap 的试剂哈希）
    bool Analyser::IdentifierChecker<OperandType::REAGENT_HASH>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // reagentHash(r?|num) : 试剂材料哈希值（用于配方查询）
        return self->checkOperandType<
            ICMsgId::IWA21_1, BasicType::REGISTER, BasicType::ENUM, TypeCategory::NUMBER>(
            symbol, arg
        );
    }

    // Device Reference - Device | Register | Identifier
    bool Analyser::IdentifierChecker<OperandType::DEVICE_REF>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // device(d?|r?|id), d? : 设备引用（可为静态/动态端口或设备别名）
        return self->checkOperandType<ICMsgId::IWA4_1, BasicType::DEVICE, BasicType::REGISTER>(
            symbol, arg
        );
    }

    // Device Reference Strict - Device（clr/rmap 的 d?）
    bool Analyser::IdentifierChecker<OperandType::DEVICE_REF_STRICT>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // d? : （clr 指令独有）严格设备端口（不接受别名或动态设备）
        return self->checkOperandType<ICMsgId::IWA2_1, BasicType::DEVICE>(symbol, arg);
    }

    // Logic Prop - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::LOGIC_PROP>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // logicType : 设备逻辑属性路径（仅单标识符，如 Pressure、Activate）

        // 数值形式为旧语法，进行警告
        if (symbol->type.category == TypeCategory::NUMBER) {
            self->reporter_->warnWith<ICMsgId::IWA24>(arg.start(), arg.end());

            return true;
        }

        // 有类型注释
        if (symbol->type.typeName && self->typeTable_->find(*symbol->type.typeName)) {
            const auto& deviceTypeName = *symbol->type.typeName;

            return std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    // 类型为设备，一般不会是枚举，在别名定义已被上报
                    if constexpr (std::is_same_v<U, DeviceAnnotation>) {
                        auto view = type.logics | std::views::enumerate;

                        if (auto it = std::ranges::find_if(
                                view,
                                [&](const auto& pair) {
                                    return std::get<1>(pair).value == arg.value;
                                }
                            );
                            it == view.end())
                            self->reporter_->errorWith<ICMsgId::IWA14_2>(
                                arg.start(), arg.end(), arg.value, deviceTypeName
                            );

                        // TODO: 检查读写权限
                        // else {
                        //     auto& [idx, slot] = *it;
                        //
                        //
                        // }

                        return true;
                    }

                    return false;
                },
                *self->typeTable_->find(deviceTypeName)
            );
        }

        // 无类型注释
        if (auto logicSlotTypePtr = self->typeTable_->find("LogicType"); logicSlotTypePtr) {
            auto isLogicSlot = std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    if constexpr (std::is_same_v<U, EnumAnnotation>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumAnnotationValue::name),
                            arg.value
                        );

                        if (!flag) {
                            self->reporter_->errorWith<ICMsgId::IWA15_1>(
                                arg.start(), arg.end(), arg.value
                            );

                            return true;  // 返回 true 以避免重复报告错误
                        }

                        return flag;
                    }

                    return false;
                },
                *logicSlotTypePtr
            );

            if (!isLogicSlot)
                return self->checkOperandType<ICMsgId::IWA8_1, TypeCategory::NUMBER>(symbol, arg);

        } else
            self->reporter_->errorWith<ICMsgId::IEA8_1>(arg.start(), arg.end(), "LogicType");

        return false;
    }

    // Logic Slot Prop - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::LOGIC_SLOT_PROP>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // logicSlotType : 槽位物品逻辑属性路径（仅单标识符）

        // 数值形式为旧语法，进行警告
        if (symbol->type.category == TypeCategory::NUMBER) {
            self->reporter_->warnWith<ICMsgId::IWA24>(arg.start(), arg.end());

            return true;
        }

        // 有类型注释
        if (symbol->type.typeName && self->typeTable_->find(*symbol->type.typeName)) {
            const auto& deviceTypeName = *symbol->type.typeName;

            return std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    // 类型为设备，一般不会是枚举，在别名定义已被上报
                    if constexpr (std::is_same_v<U, DeviceAnnotation>) {
                        bool flag = std::ranges::contains(
                            type.logicSlots
                                | std::views::transform(&DeviceAnnotationLogicSlot::name),
                            arg.value
                        );

                        if (!flag)
                            self->reporter_->errorWith<ICMsgId::IWA11_2>(
                                arg.start(), arg.end(), arg.value, deviceTypeName
                            );

                        return flag;
                    }

                    return false;
                },
                *self->typeTable_->find(deviceTypeName)
            );
        }

        // 无类型注释
        if (auto logicSlotTypePtr = self->typeTable_->find("LogicSlotType"); logicSlotTypePtr) {
            auto isLogicSlot = std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    if constexpr (std::is_same_v<U, EnumAnnotation>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumAnnotationValue::name),
                            arg.value
                        );

                        if (!flag) {
                            self->reporter_->errorWith<ICMsgId::IWA12_1>(
                                arg.start(), arg.end(), arg.value
                            );

                            return true;  // 返回 true 以避免重复报告错误
                        }

                        return flag;
                    }

                    return false;
                },
                *logicSlotTypePtr
            );

            if (!isLogicSlot)
                return self->checkOperandType<ICMsgId::IWA5_1, TypeCategory::NUMBER>(symbol, arg);

        } else
            self->reporter_->errorWith<ICMsgId::IEA8_1>(arg.start(), arg.end(), "LogicSlotType");

        return false;
    }

    // Batch Mode - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::AGG_MODE>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // batchMode : 聚合模式（如 Average、Sum 或枚举常量）

        if (auto batchModePtr = self->typeTable_->find("BatchMode"); batchModePtr) {
            auto isBatchMode = std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    if constexpr (std::is_same_v<U, EnumAnnotation>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumAnnotationValue::name),
                            arg.value
                        );

                        if (!flag) {
                            self->reporter_->errorWith<ICMsgId::IWA17_1>(
                                arg.start(), arg.end(), arg.value
                            );

                            return true;  // 返回 true 以避免重复报告错误
                        }

                        return flag;
                    }

                    return false;
                },
                *batchModePtr
            );

            if (!isBatchMode)
                return self->checkOperandType<ICMsgId::IWA10_1, TypeCategory::NUMBER>(symbol, arg);
        } else
            self->reporter_->errorWith<ICMsgId::IEA8_1>(arg.start(), arg.end(), "BatchMode");

        return false;
    }

    // Reagent Mode - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::REAGENT_MODE>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // reagentMode : 试剂模式（如 Contents、Recipe）

        if (auto reagentModePtr = self->typeTable_->find("ReagentMode"); reagentModePtr) {
            auto isReagentMode = std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    if constexpr (std::is_same_v<U, EnumAnnotation>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumAnnotationValue::name),
                            arg.value
                        );

                        if (!flag)
                            self->reporter_->errorWith<ICMsgId::IWA13_1>(
                                arg.start(), arg.end(), arg.value
                            );

                        return flag;
                    }

                    return false;
                },
                *reagentModePtr
            );

            if (!isReagentMode)
                return self->checkOperandType<ICMsgId::IWA6_1, TypeCategory::NUMBER>(symbol, arg);
        } else
            self->reporter_->errorWith<ICMsgId::IEA8_1>(arg.start(), arg.end(), "ReagentMode");

        return false;
    }

    // Device Hash - Number（lb/lbn/lbs/lbns/sb/sbn/sbs 的设备类型哈希）
    bool Analyser::IdentifierChecker<OperandType::DEVICE_HASH>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // deviceHash : 设备类型哈希值（支持字面量、常量别名、枚举、HASH 宏）
        return self->checkOperandType<ICMsgId::IWA22_1, TypeCategory::NUMBER>(symbol, arg);
    }

    // Name Hash - Number（lbn/lbns/sbn 的设备名称哈希）
    bool Analyser::IdentifierChecker<OperandType::NAME_HASH>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // nameHash : 设备名称哈希值
        return self->checkOperandType<ICMsgId::IWA23_1, TypeCategory::NUMBER>(symbol, arg);
    }

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_OPERAND_CHECK_INL
