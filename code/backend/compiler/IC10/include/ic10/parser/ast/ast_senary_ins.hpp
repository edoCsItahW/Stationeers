// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_senary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:23
 * @if zh
 * @brief IC10六元指令AST定义
 * @details 定义IC10中的六元指令(含六个操作数的指令),如LBNS等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。同时定义ExecutableInstruction类型别名,
 *        用于表示所有可执行指令类型的联合。
 * @note 实现位于ast_senary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 senary instruction AST definitions
 * @details Defines senary instructions (instructions with six operands) in IC10, such as LBNS, etc.
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap mappings.
 *        Also defines ExecutableInstruction type alias for the union of all executable instruction types.
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
            FString K, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6>
        struct SenaryInstructionBase : QuinaryInstructionBase<K, O1, O2, O3, O4, O5> {
            /**
             * @if zh
             * @brief 节点名称
             * @elseif en
             * @brief Node name
             * @endif
             */
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            /**
             * @if zh
             * @brief 第六个操作数
             * @elseif en
             * @brief Sixth operand
             * @endif
             */
            O6 operand6;

            /**
             * @if zh
             * @brief 参数类型元组
             * @elseif en
             * @brief Argument types tuple
             * @endif
             */
            using Args = std::tuple<O1, O2, O3, O4, O5, O6>;

            /**
             * @if zh
             * @brief 参数元组
             * @elseif en
             * @brief Argument tuple
             * @endif
             */
            Args args;

            /**
             * @if zh
             * @brief 默认构造函数
             * @elseif en
             * @brief Default constructor
             * @endif
             */
            SenaryInstructionBase() = default;

            /**
             * @if zh
             *
             * @brief 构造函数
             * @param pos 位置信息
             * @param op1 第一个操作数
             * @param op2 第二个操作数
             * @param op3 第三个操作数
             * @param op4 第四个操作数
             * @param op5 第五个操作数
             * @param op6 第六个操作数
             *
             * @elseif en
             *
             * @brief Constructor
             * @param pos Position information
             * @param op1 First operand
             * @param op2 Second operand
             * @param op3 Third operand
             * @param op4 Fourth operand
             * @param op5 Fifth operand
             * @param op6 Sixth operand
             *
             * @endif
             */
            SenaryInstructionBase(Pos pos, O1 op1, O2 op2, O3 op3, O4 op4, O5 op5, O6 op6);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            /**
             * @if zh
             *
             * @brief JSON基类辅助函数
             * @tparam ...Ts 字段类型包
             * @param ... fields 字段名和值的对
             * @return JSON格式字符串
             *
             * @elseif en
             *
             * @brief JSON base helper function
             * @tparam ...Ts Field type pack
             * @param ... fields Pairs of field names and values
             * @return JSON format string
             *
             * @endif
             */
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

    // 六元指令 - 操作数类型 RegisterOrIdentifier, Operand, Operand, SlotIndex,
    // LogicSlotType, BatchMode
    DEFINE_SENARY_INSTRUCTION(
        lbns, Lbns, LBNS, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::SlotIndex,
        ic10::LogicSlotType, ic10::BatchMode
    )

    namespace ic10 {

        using SenaryInstructionMap_RI_O_O_SI_LS_BM = TypeMap<TokenType, TokenType::KEYWORD_LBNS>;

        using SenaryInstruction = ShallowErrorable<LbnsInstruction>;

        using SenaryInstructionMap = TypeMap<TokenType, TokenType::KEYWORD_LBNS>;

        using ExecutableInstruction = Errorable<
            NullaryInstruction, UnaryInstruction, BinaryInstruction, TernaryInstruction,
            QuaternaryInstruction, QuinaryInstruction, SenaryInstruction>;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_senary_ins.inl"

#endif  // COMPILER_AST_SENARY_INS_HPP
