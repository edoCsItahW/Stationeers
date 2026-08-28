// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_senary_ins.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10六元指令AST定义
 * @details 定义IC10中的六元指令(含六个操作数的指令),
 *        按 docs/grammar/parser/instructions/senary.g4:
 *        - REG_TARGET DEVICE_HASH NAME_HASH SLOT_IDX LOGIC_SLOT_PROP AGG_MODE : lbns
 * @note 实现位于ast_senary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 senary instruction AST definitions
 * @details Defines senary instructions (instructions with six operands) in IC10, per
 *        docs/grammar/parser/instructions/senary.g4:
 *        - REG_TARGET DEVICE_HASH NAME_HASH SLOT_IDX LOGIC_SLOT_PROP AGG_MODE : lbns
 * @note Implementation in ast_senary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_SENARY_INS_HPP
#define COMPILER_AST_SENARY_INS_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {


    /**
     * @def DEFINE_SENARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义六元指令
     * @details 使用SenaryInstructionBase定义一个六元指令类型并注册到InstructionMapper
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名(对应InstructionKeyword枚举值)
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define senary instruction
     * @details Defines a senary instruction type using SenaryInstructionBase and registers it in
     * InstructionMapper
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name (InstructionKeyword enum value)
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_SENARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                           \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, SenaryInstructionBase, __VA_ARGS__)

    // ---------- REG_TARGET DEVICE_HASH NAME_HASH SLOT_IDX LOGIC_SLOT_PROP AGG_MODE ----------

    DEFINE_SENARY_INSTRUCTION(
        lbns, Lbns, LBNS, OperandType::REG_TARGET, OperandType::DEVICE_HASH, OperandType::NAME_HASH,
        OperandType::SLOT_IDX, OperandType::LOGIC_SLOT_PROP, OperandType::AGG_MODE
    )

    using SenaryInstruction = ShallowErrorable<LbnsInstruction>;

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_SENARY_INS_HPP
