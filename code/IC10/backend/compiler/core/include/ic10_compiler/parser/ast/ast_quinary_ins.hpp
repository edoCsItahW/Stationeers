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
 * @details 定义IC10中的五元指令(含五个操作数的指令),如LBN、LBS等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 *        LBN与LBS的操作数类型使用RegisterOrNumber而非通用的Operand,
 *        移除了不合适的Device等类型,确保操作数仅为寄存器或数字。
 * @note 实现位于ast_quinary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 quinary instruction AST definitions
 * @details Defines quinary instructions (instructions with five operands) in IC10, such as LBN,
 * LBS, etc. Uses template metaprogramming to automatically generate instruction types and TypeMap
 * mappings. LBN and LBS operand types use RegisterOrNumber instead of the generic Operand, removing
 * inappropriate types such as Device, ensuring operands are registers or numbers only.
 * @note Implementation in ast_quinary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_QUINARY_INS_HPP
#define COMPILER_AST_QUINARY_INS_HPP
#pragma once

#include "ast_quaternary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        // QuinaryInstructionBase（实现于ast.cpp）

        /**
         * @if zh
         *
         * @class QuinaryInstructionBase
         * @brief 五元指令基类模板
         * @details 五元指令是带五个操作数的指令基类。
         *        继承自QuaternaryInstructionBase,添加第五个操作数。
         *
         * @tparam K 指令关键字的FString类型
         * @tparam O1 第一个操作数类型
         * @tparam O2 第二个操作数类型
         * @tparam O3 第三个操作数类型
         * @tparam O4 第四个操作数类型
         * @tparam O5 第五个操作数类型
         *
         * @elseif en
         *
         * @class QuinaryInstructionBase
         * @brief Quinary instruction base class template
         * @details Quinary instructions are instruction base classes with five operands.
         *        Inherits from QuaternaryInstructionBase, adding fifth operand.
         *
         * @tparam K FString type of instruction keyword
         * @tparam O1 Type of first operand
         * @tparam O2 Type of second operand
         * @tparam O3 Type of third operand
         * @tparam O4 Type of fourth operand
         * @tparam O5 Type of fifth operand
         *
         * @endif
         */
        template<
            FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4,
            OperandType V5>
        struct QuinaryInstructionBase : QuaternaryInstructionBase<K, V1, V2, V3, V4> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            static constexpr auto type5 = V5;

            using O1 = operand_type_t<V1>;

            using O2 = operand_type_t<V2>;

            using O3 = operand_type_t<V3>;

            using O4 = operand_type_t<V4>;

            using O5 = operand_type_t<V5>;

            O5 operand5;

            using Args = operand_type_list<V1, V2, V3, V4, V5>;

            Args args;

            QuinaryInstructionBase() = default;

            QuinaryInstructionBase(Pos pos, O1 o1, O2 o2, O3 o3, O4 o4, O5 o5);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<FString... Vs, AstJsonAble... Params>
                requires(sizeof...(Vs) == sizeof...(Params))
            [[nodiscard]] std::string jsonBase(Params&&... params) const;
        };

    }  // namespace ic10

    /**
     * @def DEFINE_QUINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义五元指令
     * @details 使用QuinaryInstructionBase定义一个五元指令类型并注册到TypeMap
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define quinary instruction
     * @details Defines a quinary instruction type using QuinaryInstructionBase and registers it in
     * TypeMap
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_QUINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                          \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, ic10::QuinaryInstructionBase, __VA_ARGS__)

    // 五元指令 - 操作数类型 RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, LogicType,
    // BatchMode
    DEFINE_QUINARY_INSTRUCTION(
        lbn, Lbn, LBN, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM,
        ic10::OperandType::REG_NUM, ic10::OperandType::LOGIC_TYPE, ic10::OperandType::BATCH_MODE
    )

    namespace ic10 {

        using QuinaryInstructionMap_RI_RON_RON_LT_BM = TypeMap<TokenType, TokenType::KEYWORD_LBN>;

    }

    // 五元指令 - 操作数类型 RegisterOrIdentifier, RegisterOrNumber, SlotIndex, LogicSlotType,
    // BatchMode
    DEFINE_QUINARY_INSTRUCTION(
        lbs, Lbs, LBS, ic10::OperandType::REG_IDENT, ic10::OperandType::REG_NUM,
        ic10::OperandType::SLOT_IDX, ic10::OperandType::LOGIC_SLOT, ic10::OperandType::BATCH_MODE
    )

    namespace ic10 {

        using QuinaryInstructionMap_RI_RON_SI_LS_BM = TypeMap<TokenType, TokenType::KEYWORD_LBS>;

        using QuinaryInstruction = ShallowErrorable<LbnInstruction, LbsInstruction>;

        /**
         * @if zh
         *
         * @brief 五元指令映射表
         * @details 将指令关键字Token类型映射到对应的五元指令类型。
         *        包含LBN和LBS两个五元指令,用于语法分析阶段根据关键字确定指令类型。
         *
         * @elseif en
         *
         * @brief Quinary instruction map
         * @details Maps instruction keyword Token types to corresponding quinary instruction types.
         *        Contains two quinary instructions, LBN and LBS, used during parsing to determine
         *        instruction type based on keywords.
         *
         * @endif
         */
        using QuinaryInstructionMap =
            TypeMap<TokenType, TokenType::KEYWORD_LBN, TokenType::KEYWORD_LBS>;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_quinary_ins.inl"

#endif  // COMPILER_AST_QUINARY_INS_HPP
