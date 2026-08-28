// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_nullary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 12:55
 * @if zh
 * @brief IC10零元指令AST定义
 * @details 定义IC10中的零元指令(无操作数指令),如HCF、YIELD等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_nullary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 nullary instruction AST definitions
 * @details Defines nullary instructions (no-operand instructions) in IC10, such as HCF, YIELD, etc.
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap
 * mappings.
 * @note Implementation in ast_nullary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_NULLARY_INS_HPP
#define COMPILER_AST_NULLARY_INS_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {

    /**
     * @def DEFINE_NULLARY_INSTRUCTION(upperCase, pascalCase, lowerCase)
     * @if zh
     *
     * @brief 定义零元指令
     * @details 使用NullaryInstructionBase定义一个零元指令类型并注册到TypeMap
     * @param upperCase 指令大写下划线名
     * @param pascalCase 指令PascalCase名
     * @param lowerCase 指令小写名
     *
     * @elseif en
     *
     * @brief Define nullary instruction
     * @details Defines a nullary instruction type using NullaryInstructionBase and registers it in
     * TypeMap
     * @param upperCase Instruction uppercase underscore name
     * @param pascalCase Instruction PascalCase name
     * @param lowerCase Instruction lowercase name
     *
     * @endif
     */
#define DEFINE_NULLARY_INSTRUCTION(upperCase, pascalCase, lowerCase)                               \
    DEFINE_INSTRUCTION(upperCase, pascalCase, lowerCase, ic10::NullaryInstructionBase)

    DEFINE_NULLARY_INSTRUCTION(hcf, Hcf, HCF)

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    DEFINE_NULLARY_INSTRUCTION(yield, Yield, YIELD)

#endif


    using NullaryInstruction = ShallowErrorable<
        HcfInstruction
#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        ,
        YieldInstruction
#endif
        >;

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_NULLARY_INS_HPP
