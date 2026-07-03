// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_quinary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:21
 * @if zh
 * @brief IC10五元指令AST定义
 * @details 定义IC10中的五元指令(含五个操作数的指令),如LBN、LBS等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_quinary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 quinary instruction AST definitions
 * @details Defines quinary instructions (instructions with five operands) in IC10, such as LBN, LBS, etc.
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap mappings.
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
        template<FString K, typename O1, typename O2, typename O3, typename O4, typename O5>
        struct QuinaryInstructionBase : QuaternaryInstructionBase<K, O1, O2, O3, O4> {
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
             * @brief 第五个操作数
             * @elseif en
             * @brief Fifth operand
             * @endif
             */
            O5 operand5;

            /**
             * @if zh
             * @brief 参数类型元组
             * @elseif en
             * @brief Argument types tuple
             * @endif
             */
            using Args = std::tuple<O1, O2, O3, O4, O5>;

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
            QuinaryInstructionBase() = default;

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
             *
             * @endif
             */
            QuinaryInstructionBase(Pos pos, O1 op1, O2 op2, O3 op3, O4 op4, O5 op5);

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
     * @details Defines a quinary instruction type using QuinaryInstructionBase and registers it in TypeMap
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_QUINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                          \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, ic10::QuinaryInstructionBase, __VA_ARGS__)

    // 五元指令 - 操作数类型 RegisterOrIdentifier, Operand, Operand, LogicType, BatchMode
    DEFINE_QUINARY_INSTRUCTION(
        lbn, Lbn, LBN, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::LogicType,
        ic10::BatchMode
    )

    namespace ic10 {

        using QuinaryInstructionMap_RI_O_O_LT_BM = TypeMap<TokenType, TokenType::KEYWORD_LBN>;

    }

    // 五元指令 - 操作数类型 RegisterOrIdentifier, Operand, SlotIndex, LogicSlotType,
    // BatchMode
    DEFINE_QUINARY_INSTRUCTION(
        lbs, Lbs, LBS, ic10::RegisterOrIdentifier, ic10::Operand, ic10::SlotIndex, ic10::LogicSlotType,
        ic10::BatchMode
    )

    namespace ic10 {

        using QuinaryInstructionMap_RI_O_SI_LS_BM = TypeMap<TokenType, TokenType::KEYWORD_LBS>;

        using QuinaryInstruction = ShallowErrorable<LbnInstruction, LbsInstruction>;

        using QuinaryInstructionMap =
            TypeMap<TokenType, TokenType::KEYWORD_LBN, TokenType::KEYWORD_LBS>;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_quinary_ins.inl"

#endif  // COMPILER_AST_QUINARY_INS_HPP
