// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_unary_ins.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10一元指令AST定义
 * @details 定义IC10中的一元指令(含一个操作数的指令),按操作数类型分为四组:
 *        - RI组(RegisterOrIdentifier操作数): peek、rand、pop
 *        - DAR组(DeviceAliasRef操作数): clr
 *        - RON组(RegisterOrNumber操作数): sleep、clrd、push
 *        - JT组(JumpTarget操作数): jal、jr、j
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_unary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 unary instruction AST definitions
 * @details Defines unary instructions (instructions with one operand) in IC10, grouped by operand
 *        type into four groups:
 *        - RI group (RegisterOrIdentifier operand): peek, rand, pop
 *        - DAR group (DeviceAliasRef operand): clr
 *        - RON group (RegisterOrNumber operand): sleep, clrd, push
 *        - JT group (JumpTarget operand): jal, jr, j
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap
 *        mappings.
 * @note Implementation in ast_unary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_UNARY_INS_HPP
#define COMPILER_AST_UNARY_INS_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {

    /**
     * @def DEFINE_UNARY_INSTRUCTION(upperCase, pascalCase, lowerCase, ...)
     * @if zh
     *
     * @brief 定义一元指令
     * @details 使用UnaryInstructionBase定义一个一元指令类型并注册到TypeMap
     * @param upperCase 指令大写下划线名
     * @param pascalCase 指令PascalCase名
     * @param lowerCase 指令小写名
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define unary instruction
     * @details Defines a unary instruction type using UnaryInstructionBase and registers it in
     * TypeMap
     * @param upperCase Instruction uppercase underscore name
     * @param pascalCase Instruction PascalCase name
     * @param lowerCase Instruction lowercase name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_UNARY_INSTRUCTION(upperCase, pascalCase, lowerCase, ...)                            \
    DEFINE_INSTRUCTION(upperCase, pascalCase, lowerCase, UnaryInstructionBase, __VA_ARGS__)

    DEFINE_UNARY_INSTRUCTION(peek, Peek, PEEK, OperandType::REG_TARGET)

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    DEFINE_UNARY_INSTRUCTION(rand, Rand, RAND, OperandType::REG_TARGET)

    DEFINE_UNARY_INSTRUCTION(pop, Pop, POP, OperandType::REG_TARGET)

    DEFINE_UNARY_INSTRUCTION(clr, Clr, CLR, OperandType::DEVICE_REF_STRICT)

    DEFINE_UNARY_INSTRUCTION(sleep, Sleep, SLEEP, OperandType::NUM_VALUE)

    DEFINE_UNARY_INSTRUCTION(clrd, Clrd, CLRD, OperandType::HARDWARE_ID)

    DEFINE_UNARY_INSTRUCTION(push, Push, PUSH, OperandType::NUM_VALUE)

    DEFINE_UNARY_INSTRUCTION(jal, Jal, JAL, OperandType::JUMP_LINE)

    DEFINE_UNARY_INSTRUCTION(jr, Jr, JR, OperandType::JUMP_LINE)

    DEFINE_UNARY_INSTRUCTION(j, J, J, OperandType::JUMP_LINE)

#endif


    using UnaryInstruction = ShallowErrorable<
        PeekInstruction
#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        ,
        ClrInstruction, SleepInstruction, ClrdInstruction, RandInstruction, PopInstruction,
        PushInstruction, JalInstruction, JrInstruction, JInstruction
#endif
        >;

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_UNARY_INS_HPP
