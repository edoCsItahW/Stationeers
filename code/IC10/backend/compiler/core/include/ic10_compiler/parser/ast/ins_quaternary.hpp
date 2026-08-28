// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_quaternary_ins.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10四元指令AST定义
 * @details 定义IC10中的四元指令(含四个操作数的指令),
 *        按 docs/grammar/parser/instructions/quaternary.g4 操作数类型分组:
 *        - REG_TARGET NUM_VALUE NUM_VALUE NUM_VALUE: clamp、lerp、ext、ins、sap、sna、select
 *        - DEVICE_REF SLOT_IDX LOGIC_SLOT_PROP REG_TARGET : ss
 *        - REG_TARGET DEVICE_HASH LOGIC_PROP AGG_MODE : lb
 *        - DEVICE_HASH NAME_HASH LOGIC_PROP REG_TARGET : sbn
 *        - DEVICE_HASH SLOT_IDX LOGIC_SLOT_PROP REG_TARGET : sbs
 *        - NUM_VALUE NUM_VALUE NUM_VALUE JUMP_LINE : bap、bapal、bna、bnaal、brap、brna
 *        - REG_TARGET DEVICE_REF SLOT_IDX LOGIC_SLOT_PROP : ls
 *        - REG_TARGET DEVICE_REF REAGENT_MODE NUM_VALUE : lr
 * @note 实现位于ast_quaternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 quaternary instruction AST definitions
 * @details Defines quaternary instructions (instructions with four operands) in IC10, grouped by
 *        operand types per docs/grammar/parser/instructions/quaternary.g4:
 *        - REG_TARGET NUM_VALUE NUM_VALUE NUM_VALUE: clamp, lerp, ext, ins, sap, sna, select
 *        - DEVICE_REF SLOT_IDX LOGIC_SLOT_PROP REG_TARGET : ss
 *        - REG_TARGET DEVICE_HASH LOGIC_PROP AGG_MODE : lb
 *        - DEVICE_HASH NAME_HASH LOGIC_PROP REG_TARGET : sbn
 *        - DEVICE_HASH SLOT_IDX LOGIC_SLOT_PROP REG_TARGET : sbs
 *        - NUM_VALUE NUM_VALUE NUM_VALUE JUMP_LINE : bap, bapal, bna, bnaal, brap, brna
 *        - REG_TARGET DEVICE_REF SLOT_IDX LOGIC_SLOT_PROP : ls
 *        - REG_TARGET DEVICE_REF REAGENT_MODE NUM_VALUE : lr
 * @note Implementation in ast_quaternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_QUATERNARY_INS_HPP
#define COMPILER_AST_QUATERNARY_INS_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {

    /**
     * @def DEFINE_QUATERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义四元指令
     * @details 使用QuaternaryInstructionBase定义一个四元指令类型并注册到InstructionMapper
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名(对应InstructionKeyword枚举值)
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define quaternary instruction
     * @details Defines a quaternary instruction type using QuaternaryInstructionBase and registers
     * it in InstructionMapper
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name (InstructionKeyword enum value)
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_QUATERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                       \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, QuaternaryInstructionBase, __VA_ARGS__)

    // ---------- REG_TARGET NUM_VALUE NUM_VALUE NUM_VALUE ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        clamp, Clamp, CLAMP, OperandType::REG_TARGET, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    DEFINE_QUATERNARY_INSTRUCTION(
        lerp, Lerp, LERP, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        ext, Ext, EXT, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        ins, Ins, INS, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        sap, Sap, SAP, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        sna, Sna, SNA, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        select, Select, SELECT, OperandType::REG_TARGET, OperandType::NUM_VALUE,
        OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    // ---------- DEVICE_REF SLOT_IDX LOGIC_SLOT_PROP REG_TARGET ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        ss, Ss, SS, OperandType::DEVICE_REF, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT_PROP,
        OperandType::REG_TARGET
    )

    // ---------- REG_TARGET DEVICE_HASH LOGIC_PROP AGG_MODE ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        lb, Lb, LB, OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::LOGIC_PROP,
        OperandType::AGG_MODE
    )

    // ---------- DEVICE_HASH NAME_HASH LOGIC_PROP REG_TARGET ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        sbn, Sbn, SBN, OperandType::DEVICE_HASH, OperandType::NAME_HASH, OperandType::LOGIC_PROP,
        OperandType::REG_TARGET
    )

    // ---------- DEVICE_HASH SLOT_IDX LOGIC_SLOT_PROP REG_TARGET ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        sbs, Sbs, SBS, OperandType::DEVICE_HASH, OperandType::SLOT_IDX,
        OperandType::LOGIC_SLOT_PROP, OperandType::REG_TARGET
    )

    // ---------- NUM_VALUE NUM_VALUE NUM_VALUE JUMP_LINE ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        bap, Bap, BAP, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        bapal, Bapal, BAPAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        bna, Bna, BNA, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        bnaal, Bnaal, BNAAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        brap, Brap, BRAP, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_QUATERNARY_INSTRUCTION(
        brna, Brna, BRNA, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    // ---------- REG_TARGET DEVICE_REF SLOT_IDX LOGIC_SLOT_PROP ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        ls, Ls, LS, OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::SLOT_IDX,
        OperandType::LOGIC_SLOT_PROP
    )

    // ---------- REG_TARGET DEVICE_REF REAGENT_MODE NUM_VALUE ----------

    DEFINE_QUATERNARY_INSTRUCTION(
        lr, Lr, LR, OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::REAGENT_MODE,
        OperandType::NUM_VALUE
    )

#endif

    using QuaternaryInstruction = ShallowErrorable<
        ClampInstruction
#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        ,
        LerpInstruction, SsInstruction, LbInstruction, SbnInstruction, SbsInstruction,
        BapInstruction, LsInstruction, LrInstruction, ExtInstruction, InsInstruction,
        BapalInstruction, BnaInstruction, BnaalInstruction, BrapInstruction, BrnaInstruction,
        SapInstruction, SnaInstruction, SelectInstruction
#endif
        >;

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_QUATERNARY_INS_HPP
