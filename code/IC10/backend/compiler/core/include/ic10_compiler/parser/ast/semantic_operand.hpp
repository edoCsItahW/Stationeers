// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semantic_operand.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/31 00:01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_SEMANTIC_OPERAND_HPP
#define STATIONEERS_SEMANTIC_OPERAND_HPP
#pragma once

#include "node.hpp"

namespace stationeers::ic10 {

    enum class OperandType {
        REG_TARGET,
        REG_OR_DEV,
        NUM_VALUE,
        JUMP_LINE,
        ADDRESS,
        SLOT_IDX,
        HARDWARE_ID,
        REAGENT_HASH,
        DEVICE_REF,
        DEVICE_REF_STRICT,
        LOGIC_PROP,
        LOGIC_SLOT_PROP,
        AGG_MODE,
        REAGENT_MODE,
        DEVICE_HASH,
        NAME_HASH,
        CONST_NUM
    };

    template<OperandType>
    struct operand_type;

    template<>
    struct operand_type<OperandType::REG_TARGET> {
        using type = RegTarget;
    };

    template<>
    struct operand_type<OperandType::REG_OR_DEV> {  // alias专用
        using type = RegOrDev;
    };

    template<>
    struct operand_type<OperandType::NUM_VALUE> {
        using type = NumValue;
    };

    template<>
    struct operand_type<OperandType::JUMP_LINE> {
        using type = JumpLine;
    };

    template<>
    struct operand_type<OperandType::ADDRESS> {
        using type = Address;
    };

    template<>
    struct operand_type<OperandType::SLOT_IDX> {
        using type = SlotIdx;
    };

    template<>
    struct operand_type<OperandType::HARDWARE_ID> {
        using type = HardwareId;
    };

    template<>
    struct operand_type<OperandType::REAGENT_HASH> {
        using type = ReagentHash;
    };

    template<>
    struct operand_type<OperandType::DEVICE_REF> {
        using type = DeviceRef;
    };

    template<>
    struct operand_type<OperandType::DEVICE_REF_STRICT> {
        using type = DeviceRefStrict;
    };

    template<>
    struct operand_type<OperandType::LOGIC_PROP> {
        using type = LogicProp;
    };

    template<>
    struct operand_type<OperandType::LOGIC_SLOT_PROP> {
        using type = LogicSlotProp;
    };

    template<>
    struct operand_type<OperandType::AGG_MODE> {
        using type = AggMode;
    };

    template<>
    struct operand_type<OperandType::REAGENT_MODE> {
        using type = ReagentMode;
    };

    template<>
    struct operand_type<OperandType::DEVICE_HASH> {
        using type = DeviceHash;
    };

    template<>
    struct operand_type<OperandType::NAME_HASH> {
        using type = NameHash;
    };

    template<>
    struct operand_type<OperandType::CONST_NUM> {
        using type = ConstNum;
    };

    template<OperandType V>
    using operand_type_t = operand_type<V>::type;

    template<OperandType... Vs>
    using operand_type_list = std::tuple<operand_type_t<Vs>...>;

}  // namespace stationeers::ic10

#endif  // STATIONEERS_SEMANTIC_OPERAND_HPP
