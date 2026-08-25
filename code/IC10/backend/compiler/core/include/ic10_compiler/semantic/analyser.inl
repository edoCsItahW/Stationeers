// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:49
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_ANALYSER_INL
#define COMPILER_ANALYSER_INL
#pragma once

#include "ic10_compiler/locals/local.hpp"

namespace stationeers::ic10 {

    template<ICMsgId I, auto... Vs>
        requires(
            ...
            && (std::is_same_v<decltype(Vs), BasicType>
                || std::is_same_v<decltype(Vs), TypeCategory>)
        )
    bool Analyser::checkOperandType(const std::shared_ptr<Symbol>& symbol, auto&& arg) const {
        auto flag = (... || ([&]<auto V> {
                         using T = decltype(V);

                         if constexpr (std::is_same_v<T, BasicType>)
                             return symbol->type.kind == V;

                         else
                             return symbol->type.category == V;
                     }.template operator()<Vs>()));

        if (!flag) reporter_->errorWith<I>(arg.start(), arg.end(), symbol->name);

        return flag;
    }

    // 通用指令访问器：遍历指令的所有操作数（args 元组），按操作数类型分派处理
    template<template<auto, auto...> class Ins, FString V, OperandType... Vs>
    Task<> Analyser::operator()(const Ins<V, Vs...>& ins) {
        // 折叠表达式依次处理指令的所有操作数
        std::apply(
            [&](const auto&... args) -> Task<> {
                // 每条指令开始前重置设备上下文，避免上一条指令的残留影响当前指令
                // （fire-and-forget 模式下挂起的 lambda 不会恢复，重置不会影响已挂起的 lambda）
                pendingDeviceSymbol_.reset();
                (((void)co_await process<Vs>(args)), ...);
                co_return;
            },
            ins.args
        );

        co_return;
    }

    template<OperandType Type>
    Task<> Analyser::process(const auto& variant) {
        // 单个操作数处理：对 variant 分派
        // - Identifier: 解析符号（可能为前向引用，需等待 Future）
        // - ErrorNode : Parser 已上报，跳过避免重复诊断
        // - 其他类型  : 递归访问（走叶节点访问器或其他专用访问器）
        (void)co_await std::visit(
            [&]<typename T>(const T& arg) -> Task<> {
                using U = std::decay_t<T>;

                // Identifier: 需要解析符号
                if constexpr (std::is_same_v<U, Identifier>) {
                    // 标准库优先类型：逻辑网络名/插槽名/试剂模式/批量模式
                    // 这些标识符不是 IC10 语言级别的符号（别名/常量/标签），
                    // 不应走符号表 resolve（否则协程会永久挂起，依赖 failAllPending 恢复，
                    // 在 fire-and-forget 模式下恢复链路不可靠），直接用标准库验证。
                    if constexpr (
                        Type == OperandType::LOGIC_TYPE || Type == OperandType::LOGIC_SLOT
                        || Type == OperandType::REAGENT_MODE || Type == OperandType::BATCH_MODE
                    ) {
                        // 构造 dummy symbol：若已有设备上下文，带入 typeName 供设备特定检查
                        auto sym = std::make_shared<Symbol>(arg.value, type_of<std::false_type>);

                        if (pendingDeviceSymbol_)
                            sym->type.typeName = pendingDeviceSymbol_->symbol->type.typeName;

                        // 设备特定检查通过（返回 true）则跳过通用 IdentifierChecker
                        if (pendingDeviceSymbol_
                            && checkWithDeviceContext<Type>(
                                *sym, *pendingDeviceSymbol_->symbol, pendingDeviceSymbol_->start,
                                pendingDeviceSymbol_->end
                            ))
                            co_return;

                        // 通用检查：有 typeName 走设备分支，无 typeName 走标准库枚举分支
                        IdentifierChecker<Type>::check(this, sym, arg);
                    }
                    // 设备引用/别名：需 resolve 以获取设备符号（可能为前向引用）
                    // 注意：必须通过 resolveSymbol（Task<shared_ptr<Symbol>>）而非直接 resolve，
                    // 因为 process 是 Task<void>，其 coro_state_weak_ 未设置，无法注册为 Future
                    // 等待者， 直接 co_await resolve 会导致协程永久挂起且 rethrow 永远不被调用。
                    // resolveSymbol 是非 void Task，可正确注册为等待者，被 failAllPending
                    // 恢复后上报 IE0_1。
                    else if constexpr (
                        Type == OperandType::DEV_REF || Type == OperandType::DEV_ALIAS
                    ) {
                        auto result = co_await resolveSymbol(arg.value, arg.position);

                        // resolveSymbol 失败时已由内部 rethrow 上报 IE0_1，result.value() 为
                        // nullptr
                        if (result.has_value() && result.value())
                            pendingDeviceSymbol_ = {
                                std::move(result.value()), arg.start(), arg.end()
                            };
                    }
                    // 其他符号表类型：别名/常量/标签/寄存器等，走 resolveSymbol 流程
                    else {
                        auto result = co_await resolveSymbol(arg.value, arg.position);

                        // resolveSymbol 失败时已上报 IE0_1，仅在对称解析成功时做类型检查
                        if (result.has_value() && result.value())
                            IdentifierChecker<Type>::check(this, result.value(), arg);
                    }

                    co_return;
                }

                else if constexpr (std::is_same_v<U, Device>) {
                    if constexpr (Type == OperandType::DEV_REF || Type == OperandType::DEV_ALIAS) {
                        std::shared_ptr<Symbol> devSym;

                        if (auto it = symbolTable_->builtinSymbols.find(arg.value);
                            it != symbolTable_->builtinSymbols.end())
                            devSym = std::make_shared<Symbol>(it->second);
                        else
                            devSym = std::make_shared<Symbol>(arg.value, type_of<Device>);

                        pendingDeviceSymbol_ = {devSym, arg.start(), arg.end()};

                        co_return;

                    } else
                        (void)co_await this->operator()(arg);
                }

                else if constexpr (
                    std::is_same_v<U, Integer> || std::is_same_v<U, HexNumber>
                    || std::is_same_v<U, BinaryNumber> || std::is_same_v<U, Float>
                ) {
                    if constexpr (Type == OperandType::SLOT_IDX) {
                        if (pendingDeviceSymbol_
                            && !checkSlotIndexWithDevice(arg, pendingDeviceSymbol_->symbol))
                            co_return;
                    }

                    (void)co_await this->operator()(arg);  // 常规递归
                }

                // 其他类型: 递归访问
                else
                    (void)co_await this->operator()(arg);

                co_return;
            },
            variant
        );

        co_return;
    }

    template<OperandType Type>
    bool Analyser::checkWithDeviceContext(
        const Symbol& currentSym, const Symbol& devSym, const Pos& start, const Pos& end
    ) const {
        auto typeName = devSym.type.typeName;
        if (!typeName) return false;

        if (const auto* ct = typeTable_->find(*typeName);
            ct && std::holds_alternative<DeviceType>(*ct)) {
            if constexpr (
                const auto& dt = std::get<DeviceType>(*ct); Type == OperandType::LOGIC_SLOT
            ) {
                if (!std::ranges::contains(
                        dt.logicSlots | std::views::transform(&DeviceLogicSlot::name),
                        currentSym.name
                    ))
                    reporter_->errorWith<ICMsgId::IWA11_2>(start, end, currentSym.name, *typeName);

            } else if constexpr (Type == OperandType::LOGIC_TYPE) {
                if (!std::ranges::contains(
                        dt.logics | std::views::transform(&DeviceLogic::name), currentSym.name
                    ))
                    reporter_->errorWith<ICMsgId::IWA14_2>(start, end, currentSym.name, *typeName);

            } else if constexpr (Type == OperandType::SLOT_IDX) {
                if (currentSym.value) {
                    if (!std::ranges::contains(
                            dt.slots | std::views::transform(&DeviceSlot::index), *currentSym.value
                        ))
                        reporter_->errorWith<ICMsgId::IWA16_2>(
                            start, end, *currentSym.value, *typeName
                        );

                } else
                    // 无法获取数值 → 回落
                    return false;

            } else
                return false;

            // 设备类型已找到：无论逻辑名是否匹配，检查都已在此完成，
            // 调用方不应再回落到 IdentifierChecker（否则会重复上报相同诊断）。
            // Device type was found: regardless of whether the logic name matched,
            // the check is complete here. The caller must NOT fall through to
            // IdentifierChecker (otherwise the same diagnostic is reported twice).
            return true;
        }

        return false;
    }

    template<OperandType Type>
    bool Analyser::checkGlobalEnum(
        const std::string& name, const Pos& start, const Pos& end
    ) const {
        // 仅对映射到标准库枚举的操作数类型生效（LOGIC_TYPE/LOGIC_SLOT/REAGENT_MODE/BATCH_MODE）
        // 这些类型即使标识符未定义，也需检查值是否属于对应标准库枚举，
        // 若不属于则上报"不是已知的X"，且不再上报未定义标识符。
        if constexpr (constexpr auto enumName = operand_type_name_v<Type>; enumName != "~") {
            if (auto* enumType = typeTable_->find(std::string(enumName));
                enumType && std::holds_alternative<EnumType>(*enumType)) {
                if (!std::ranges::contains(
                        std::get<EnumType>(*enumType).values
                            | std::views::transform(&EnumValueEntry::name),
                        name
                    )) {
                    // 按操作数类型分派到对应诊断消息
                    if constexpr (Type == OperandType::LOGIC_SLOT)
                        reporter_->errorWith<ICMsgId::IWA12_1>(start, end, name);
                    else if constexpr (Type == OperandType::LOGIC_TYPE)
                        reporter_->errorWith<ICMsgId::IWA15_1>(start, end, name);
                    else if constexpr (Type == OperandType::REAGENT_MODE)
                        reporter_->errorWith<ICMsgId::IWA13_1>(start, end, name);
                    else if constexpr (Type == OperandType::BATCH_MODE)
                        reporter_->errorWith<ICMsgId::IWA17_1>(start, end, name);
                }

                // 无论是否找到都返回 true：找到则合法；未找到则已上报"不是已知的X"，
                // 阻止调用方再上报未定义标识符（避免重复诊断）
                return true;
            }

            // 标准库枚举类型缺失：上报 IEA8_1，并阻止未定义标识符上报
            reporter_->errorWith<ICMsgId::IEA8_1>(start, end, std::string(enumName));
            return true;
        }

        // 类型未映射到标准库枚举：返回 false，由调用方按未定义标识符处理
        return false;
    }

    bool Analyser::checkSlotIndexWithDevice(
        const auto& number, const std::shared_ptr<Symbol>& devSym
    ) {
        auto typeName = devSym->type.typeName;
        if (!typeName) return false;

        if (const auto* ct = typeTable_->find(*typeName);
            ct && std::holds_alternative<DeviceType>(*ct)) {
            const auto& dt = std::get<DeviceType>(*ct);

            const std::string& idxStr = number.value;

            bool found =
                std::ranges::contains(dt.slots | std::views::transform(&DeviceSlot::index), idxStr);

            if (!found)
                // 上报错误：该数字不在设备槽范围内
                reporter_->errorWith<ICMsgId::IWA16_2>(
                    number.start(), number.end(), idxStr, *typeName
                );

            return found;
        }

        return false;
    }

    // 泛型 fallback 访问器：未匹配到专用访问器的节点走此分支，统一上报 IEA5（未知语法类型）
    template<typename T>
    Task<> Analyser::operator()(T&& arg) {
        using U = std::remove_cvref_t<T>;

        // 如果是 tuple-like 类型，尝试提取内部元素
        if constexpr (requires { std::get<0>(arg); })
            std::apply(
                [this](const auto& innerArg) {
                    reporter_->errorWith<ICMsgId::IEA6>(
                        innerArg.start(), innerArg.end(), std::string(U::nodeName)
                    );
                },
                arg
            );
        else
            reporter_->errorWith<ICMsgId::IEA6>(arg.start(), arg.end(), std::string(U::nodeName));

        co_return;
    }

    template<OperandType Type>
    bool Analyser::IdentifierChecker<Type>::check(
        const Analyser*, const std::shared_ptr<Symbol>&, auto&&
    ) noexcept {
        return true;
    }

    // Register or Identifier - Register | Identifier
    bool Analyser::IdentifierChecker<OperandType::REG_IDENT>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        return self->checkOperandType<ICMsgId::IWA1_1, BasicType::REGISTER>(symbol, arg);
    }

    // Device Alias - Device | Identifier
    bool Analyser::IdentifierChecker<OperandType::DEV_ALIAS>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        return self->checkOperandType<ICMsgId::IWA2_1, BasicType::DEVICE>(symbol, arg);
    }

    // Register or Number - Register | Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::REG_NUM>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        return self->checkOperandType<ICMsgId::IWA3_1, BasicType::REGISTER, BasicType::INTEGER, BasicType::FLOAT, TypeCategory::NUMBER>(
            symbol, arg
        );
    }

    // Device Reference - Device | Register | Identifier
    bool Analyser::IdentifierChecker<OperandType::DEV_REF>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        return self->checkOperandType<ICMsgId::IWA4_1, BasicType::DEVICE, BasicType::REGISTER>(
            symbol, arg
        );
    }

    // Logic Slot - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::LOGIC_SLOT>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // 有类型注释
        if (symbol->type.typeName && self->typeTable_->find(*symbol->type.typeName)) {
            const auto& deviceTypeName = *symbol->type.typeName;

            return std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    // 类型为设备，一般不会是枚举，在别名定义已被上报
                    if constexpr (std::is_same_v<U, DeviceType>) {
                        bool flag = std::ranges::contains(
                            type.logicSlots | std::views::transform(&DeviceLogicSlot::name),
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

                    if constexpr (std::is_same_v<U, EnumType>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumValueEntry::name), arg.value
                        );

                        if (!flag)
                            self->reporter_->errorWith<ICMsgId::IWA12_1>(
                                arg.start(), arg.end(), arg.value
                            );

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

    // Reagent Mode - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::REAGENT_MODE>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        if (auto reagentModePtr = self->typeTable_->find("ReagentMode"); reagentModePtr) {
            auto isReagentMode = std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    if constexpr (std::is_same_v<U, EnumType>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumValueEntry::name), arg.value
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

    // Jump Target - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::JUMP_TARGET>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        return self->checkOperandType<ICMsgId::IWA7_1, TypeCategory::LABEL, TypeCategory::NUMBER, BasicType::REGISTER>(
            symbol, arg
        );
    }

    // Logic Type - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::LOGIC_TYPE>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        // 有类型注释
        if (symbol->type.typeName && self->typeTable_->find(*symbol->type.typeName)) {
            const auto& deviceTypeName = *symbol->type.typeName;

            return std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    // 类型为设备，一般不会是枚举，在别名定义已被上报
                    if constexpr (std::is_same_v<U, DeviceType>) {
                        auto view = type.logics | std::views::enumerate;

                        if (auto it = std::ranges::find_if(
                                view,
                                [&](const auto& pair) {
                                    return std::get<1>(pair).name == arg.value;
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

                    if constexpr (std::is_same_v<U, EnumType>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumValueEntry::name), arg.value
                        );

                        if (!flag)
                            self->reporter_->errorWith<ICMsgId::IWA15_1>(
                                arg.start(), arg.end(), arg.value
                            );

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

    // Slot Index - [Number]
    bool Analyser::IdentifierChecker<OperandType::SLOT_IDX>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        if (symbol->type.typeName && self->typeTable_->find(*symbol->type.typeName)) {
            const auto& deviceTypeName = *symbol->type.typeName;

            return std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    // 类型为设备，一般不会是枚举，在别名定义已被上报
                    if constexpr (std::is_same_v<U, DeviceType>) {
                        if (symbol->value) {
                            bool flag = std::ranges::contains(
                                type.slots | std::views::transform(&DeviceSlot::index),
                                *symbol->value
                            );

                            if (!flag)
                                self->reporter_->errorWith<ICMsgId::IWA16_2>(
                                    arg.start(), arg.end(), *symbol->value, deviceTypeName
                                );

                            return flag;
                        }
                    }
                },
                *self->typeTable_->find(deviceTypeName)
            );
        }

        return self->checkOperandType<ICMsgId::IWA9_1, TypeCategory::NUMBER>(symbol, arg);
    }

    // Batch Mode - Identifier | [Number]
    bool Analyser::IdentifierChecker<OperandType::BATCH_MODE>::check(
        const Analyser* self, const std::shared_ptr<Symbol>& symbol, auto&& arg
    ) {
        if (auto batchModePtr = self->typeTable_->find("BatchMode"); batchModePtr) {
            auto isBatchMode = std::visit(
                [&]<typename T>(T&& type) {
                    using U = std::remove_cvref_t<T>;

                    if constexpr (std::is_same_v<U, EnumType>) {
                        bool flag = std::ranges::contains(
                            type.values | std::views::transform(&EnumValueEntry::name), arg.value
                        );

                        if (!flag)
                            self->reporter_->errorWith<ICMsgId::IWA17_1>(
                                arg.start(), arg.end(), arg.value
                            );

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

}  // namespace stationeers::ic10

#endif  // COMPILER_ANALYSER_INL
