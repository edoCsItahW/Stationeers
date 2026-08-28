// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_quinary_ins.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10五元指令AST定义
 * @details 定义IC10中的五元指令(含五个操作数的指令),
 *        按 docs/grammar/parser/instructions/quinary.g4 操作数类型分组:
 *        - REG_TARGET DEVICE_HASH NAME_HASH LOGIC_PROP AGG_MODE : lbn
 *        - REG_TARGET DEVICE_HASH SLOT_IDX LOGIC_SLOT_PROP AGG_MODE : lbs
 * @note 实现位于ast_quinary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 quinary instruction AST definitions
 * @details Defines quinary instructions (instructions with five operands) in IC10, grouped by
 *        operand types per docs/grammar/parser/instructions/quinary.g4:
 *        - REG_TARGET DEVICE_HASH NAME_HASH LOGIC_PROP AGG_MODE : lbn
 *        - REG_TARGET DEVICE_HASH SLOT_IDX LOGIC_SLOT_PROP AGG_MODE : lbs
 * @note Implementation in ast_quinary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_QUINARY_INS_HPP
#define COMPILER_AST_QUINARY_INS_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {

    /**
     * @def DEFINE_QUINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义五元指令
     * @details 使用QuinaryInstructionBase定义一个五元指令类型并注册到InstructionMapper
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名(对应InstructionKeyword枚举值)
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define quinary instruction
     * @details Defines a quinary instruction type using QuinaryInstructionBase and registers it in
     * InstructionMapper
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name (InstructionKeyword enum value)
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_QUINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                          \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, QuinaryInstructionBase, __VA_ARGS__)

    // ---------- REG_TARGET DEVICE_HASH NAME_HASH LOGIC_PROP AGG_MODE ----------

    DEFINE_QUINARY_INSTRUCTION(
        lbn, Lbn, LBN, OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::NAME_HASH,
        OperandType::LOGIC_PROP, OperandType::AGG_MODE
    )

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    // ---------- REG_TARGET DEVICE_HASH SLOT_IDX LOGIC_SLOT_PROP AGG_MODE ----------

    DEFINE_QUINARY_INSTRUCTION(
        lbs, Lbs, LBS, OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::SLOT_IDX,
        OperandType::LOGIC_SLOT_PROP, OperandType::AGG_MODE
    )

#endif

    using QuinaryInstruction = ShallowErrorable<
        LbnInstruction
#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        ,
        LbsInstruction
#endif
        >;

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_QUINARY_INS_HPP
