// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_unary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 12:58
 * @if zh
 * @brief IC10一元指令AST定义
 * @details 定义IC10中的一元指令(含一个操作数的指令),如SNANZ、PEEK、RAND等。
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_unary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 unary instruction AST definitions
 * @details Defines unary instructions (instructions with one operand) in IC10, such as SNANZ, PEEK, RAND, etc.
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap mappings.
 * @note Implementation in ast_unary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_UNARY_INS_HPP
#define COMPILER_AST_UNARY_INS_HPP
#pragma once

#include "ast_nullary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        /**
         * @if zh
         *
         * @class UnaryInstructionBase
         * @brief 一元指令基类模板
         * @details 一元指令是带一个操作数的指令基类。
         *        继承自NullaryInstructionBase,添加一个操作数。
         *
         * @tparam K 指令关键字的FString类型
         * @tparam O1 第一个操作数类型
         *
         * @elseif en
         *
         * @class UnaryInstructionBase
         * @brief Unary instruction base class template
         * @details Unary instructions are instruction base classes with one operand.
         *        Inherits from NullaryInstructionBase, adding one operand.
         *
         * @tparam K FString type of instruction keyword
         * @tparam O1 Type of first operand
         *
         * @endif
         */
        template<FString K, typename O1>
        struct UnaryInstructionBase : NullaryInstructionBase<K> {
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
             * @brief 第一个操作数
             * @elseif en
             * @brief First operand
             * @endif
             */
            O1 operand1;

            /**
             * @if zh
             * @brief 参数类型元组
             * @elseif en
             * @brief Argument types tuple
             * @endif
             */
            using Args = std::tuple<O1>;

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
            UnaryInstructionBase() = default;

            /**
             * @if zh
             *
             * @brief 构造函数
             * @param pos 位置信息
             * @param op1 第一个操作数
             *
             * @elseif en
             *
             * @brief Constructor
             * @param pos Position information
             * @param op1 First operand
             *
             * @endif
             */
            UnaryInstructionBase(Pos pos, O1 op1);

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
     * @details Defines a unary instruction type using UnaryInstructionBase and registers it in TypeMap
     * @param upperCase Instruction uppercase underscore name
     * @param pascalCase Instruction PascalCase name
     * @param lowerCase Instruction lowercase name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_UNARY_INSTRUCTION(upperCase, pascalCase, lowerCase, ...)                            \
    DEFINE_INSTRUCTION(upperCase, pascalCase, lowerCase, ic10::UnaryInstructionBase, __VA_ARGS__)

    DEFINE_UNARY_INSTRUCTION(snanz, Snanz, SNANZ, ic10::RegisterOrIdentifier)

#ifndef IC10_SIMPLE_MODE

    DEFINE_UNARY_INSTRUCTION(peek, Peek, PEEK, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(rand, Rand, RAND, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(seqz, Seqz, SEQZ, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(snez, Snez, SNEZ, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(sgez, Sgez, SGEZ, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(sgtz, Sgtz, SGTZ, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(slez, Slez, SLEZ, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(sltz, Sltz, SLTZ, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(snan, Snan, SNAN, ic10::RegisterOrIdentifier)

    DEFINE_UNARY_INSTRUCTION(pop, Pop, POP, ic10::RegisterOrIdentifier)

#endif

    namespace ic10 {

        using UnaryInstructionMap_RI = TypeMap<
            TokenType, TokenType::KEYWORD_SNANZ
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_PEEK, TokenType::KEYWORD_RAND, TokenType::KEYWORD_SEQZ,
            TokenType::KEYWORD_SNEZ, TokenType::KEYWORD_SGEZ, TokenType::KEYWORD_SGTZ,
            TokenType::KEYWORD_SLEZ, TokenType::KEYWORD_SLTZ, TokenType::KEYWORD_SNAN,
            TokenType::KEYWORD_POP
#endif
            >;

    }  // namespace ic10


    DEFINE_UNARY_INSTRUCTION(clr, Clr, CLR, ic10::DeviceReference)

    namespace ic10 {

        using UnaryInstructionMap_DR = TypeMap<TokenType, TokenType::KEYWORD_CLR>;

    }

    DEFINE_UNARY_INSTRUCTION(sleep, Sleep, SLEEP, ic10::Operand)

#ifndef IC10_SIMPLE_MODE

    DEFINE_UNARY_INSTRUCTION(push, Push, PUSH, ic10::Operand)

    DEFINE_UNARY_INSTRUCTION(jal, Jal, JAL, ic10::Operand)

    DEFINE_UNARY_INSTRUCTION(jr, Jr, JR, ic10::Operand)

    DEFINE_UNARY_INSTRUCTION(j, J, J, ic10::Operand)

#endif

    namespace ic10 {

        using UnaryInstructionMap_O = TypeMap<
            TokenType, TokenType::KEYWORD_SLEEP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_PUSH, TokenType::KEYWORD_JAL, TokenType::KEYWORD_JR,
            TokenType::KEYWORD_J
#endif
            >;

        using UnaryInstruction = ShallowErrorable<
            SnanzInstruction, ClrInstruction, SleepInstruction
#ifndef IC10_SIMPLE_MODE
            ,
            PeekInstruction, RandInstruction, SeqzInstruction, SnezInstruction, SgezInstruction,
            SgtzInstruction, SlezInstruction, SltzInstruction, SnanInstruction, PopInstruction,
            PushInstruction, JalInstruction, JrInstruction, JInstruction
#endif
            >;

        using UnaryInstructionMap = TypeMap<
            TokenType, TokenType::KEYWORD_SNANZ, TokenType::KEYWORD_CLR, TokenType::KEYWORD_SLEEP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_PEEK, TokenType::KEYWORD_RAND, TokenType::KEYWORD_SEQZ,
            TokenType::KEYWORD_SNEZ, TokenType::KEYWORD_SGEZ, TokenType::KEYWORD_SGTZ,
            TokenType::KEYWORD_SLEZ, TokenType::KEYWORD_SLTZ, TokenType::KEYWORD_SNAN,
            TokenType::KEYWORD_POP, TokenType::KEYWORD_PUSH, TokenType::KEYWORD_JAL,
            TokenType::KEYWORD_JR, TokenType::KEYWORD_J
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_unary_ins.inl"

#endif  // COMPILER_AST_UNARY_INS_HPP
