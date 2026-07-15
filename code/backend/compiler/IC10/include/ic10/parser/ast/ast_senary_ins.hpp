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
 * @details 定义IC10中的六元指令(含六个操作数的指令),如LBNS等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。同时定义ExecutableInstruction类型别名,
 *        用于表示所有可执行指令类型的联合。
 *        LBNS的操作数类型使用RegisterOrNumber而非通用的Operand,
 *        移除了不合适的Device等类型,确保操作数仅为寄存器或数字。
 * @note 实现位于ast_senary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 senary instruction AST definitions
 * @details Defines senary instructions (instructions with six operands) in IC10, such as LBNS, etc.
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap mappings.
 *        Also defines ExecutableInstruction type alias for the union of all executable instruction types.
 *        LBNS operand types use RegisterOrNumber instead of the generic Operand,
 *        removing inappropriate types such as Device, ensuring operands are registers or numbers only.
 * @note Implementation in ast_senary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_SENARY_INS_HPP
#define COMPILER_AST_SENARY_INS_HPP
#pragma once

#include "ast_quinary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        // SenaryInstructionBase（实现于ast.cpp）

        /**
         * @if zh
         *
         * @class SenaryInstructionBase
         * @brief 六元指令基类模板
         * @details 六元指令是带六个操作数的指令基类。
         *        继承自QuinaryInstructionBase,添加第六个操作数。
         *
         * @tparam K 指令关键字的FString类型
         * @tparam O1 第一个操作数类型
         * @tparam O2 第二个操作数类型
         * @tparam O3 第三个操作数类型
         * @tparam O4 第四个操作数类型
         * @tparam O5 第五个操作数类型
         * @tparam O6 第六个操作数类型
         *
         * @elseif en
         *
         * @class SenaryInstructionBase
         * @brief Senary instruction base class template
         * @details Senary instructions are instruction base classes with six operands.
         *        Inherits from QuinaryInstructionBase, adding sixth operand.
         *
         * @tparam K FString type of instruction keyword
         * @tparam O1 Type of first operand
         * @tparam O2 Type of second operand
         * @tparam O3 Type of third operand
         * @tparam O4 Type of fourth operand
         * @tparam O5 Type of fifth operand
         * @tparam O6 Type of sixth operand
         *
         * @endif
         */
        template<
            FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5, OperandType V6>
        struct SenaryInstructionBase : QuinaryInstructionBase<K, V1, V2, V3, V4, V5> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            static constexpr auto type6 = V6;

            using O1 = operand_type_t<V1>;

            using O2 = operand_type_t<V2>;

            using O3 = operand_type_t<V3>;

            using O4 = operand_type_t<V4>;

            using O5 = operand_type_t<V5>;

            using O6 = operand_type_t<V6>;

            O6 operand6;

            using Args = operand_type_list<V1, V2, V3, V4, V5, V6>;

            Args args;

            SenaryInstructionBase() = default;

            SenaryInstructionBase(Pos pos, O1 o1, O2 o2, O3 o3, O4 o4, O5 o5, O6 o6);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<typename... Ts>
            [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
        };

    }  // namespace ic10

    /**
     * @def DEFINE_SENARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义六元指令
     * @details 使用SenaryInstructionBase定义一个六元指令类型并注册到TypeMap
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define senary instruction
     * @details Defines a senary instruction type using SenaryInstructionBase and registers it in TypeMap
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_SENARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                           \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, ic10::SenaryInstructionBase, __VA_ARGS__)

    // 六元指令 - 操作数类型 RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, SlotIndex,
    // LogicSlotType, BatchMode
    DEFINE_SENARY_INSTRUCTION(
        lbns, Lbns, LBNS, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM, ic10::OperandType::SLOT_IDX, ic10::OperandType::LOGIC_SLOT, ic10::OperandType::BATCH_MODE
    )

    namespace ic10 {

        using SenaryInstructionMap_RI_RON_RON_SI_LS_BM = TypeMap<TokenType, TokenType::KEYWORD_LBNS>;

        using SenaryInstruction = ShallowErrorable<LbnsInstruction>;

        /**
         * @if zh
         *
         * @brief 六元指令映射表
         * @details 将指令关键字Token类型映射到对应的六元指令类型。
         *        包含LBNS六元指令,用于语法分析阶段根据关键字确定指令类型。
         *
         * @elseif en
         *
         * @brief Senary instruction map
         * @details Maps instruction keyword Token types to corresponding senary instruction types.
         *        Contains the LBNS senary instruction, used during parsing to determine
         *        instruction type based on keywords.
         *
         * @endif
         */
        using SenaryInstructionMap = TypeMap<TokenType, TokenType::KEYWORD_LBNS>;

        using ExecutableInstruction = Errorable<
            NullaryInstruction, UnaryInstruction, BinaryInstruction, TernaryInstruction,
            QuaternaryInstruction, QuinaryInstruction, SenaryInstruction>;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_senary_ins.inl"

#endif  // COMPILER_AST_SENARY_INS_HPP
