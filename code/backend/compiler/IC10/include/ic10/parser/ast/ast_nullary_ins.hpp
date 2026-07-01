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
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap mappings.
 * @note Implementation in ast_nullary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_NULLARY_INS_HPP
#define COMPILER_AST_NULLARY_INS_HPP
#pragma once

#include "ast_node.hpp"

namespace stationeers {

    namespace ic10 {

        // NullaryInstructionBase（实现于ast.cpp）

        /**
         * @if zh
         *
         * @class NullaryInstructionBase
         * @brief 零元指令基类模板
         * @details 零元指令是不带操作数的指令基类,如HCF、YIELD等。
         *        使用CRTP模式和FString作为模板参数区分不同指令。
         *
         * @tparam K 指令关键字的FString类型
         *
         * @elseif en
         *
         * @class NullaryInstructionBase
         * @brief Nullary instruction base class template
         * @details Nullary instructions are instruction base classes without operands,
         *        such as HCF, YIELD, etc. Uses CRTP pattern and FString as template parameter
         *        to distinguish different instructions.
         *
         * @tparam K FString type of instruction keyword
         *
         * @endif
         */
        template<FString K>
        struct NullaryInstructionBase : AST<NullaryInstructionBase<K>> {
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
             * @brief 指令关键字
             * @elseif en
             * @brief Instruction keyword
             * @endif
             */
            static constexpr auto keyword = K;

            /**
             * @if zh
             * @brief 参数类型(空元组)
             * @elseif en
             * @brief Argument types (empty tuple)
             * @endif
             */
            using Args = std::tuple<>;

            /**
             * @if zh
             * @brief 默认构造函数
             * @elseif en
             * @brief Default constructor
             * @endif
             */
            NullaryInstructionBase() = default;

            /**
             * @if zh
             *
             * @brief 构造函数
             * @param pos 位置信息
             *
             * @elseif en
             *
             * @brief Constructor
             * @param pos Position information
             *
             * @endif
             */
            NullaryInstructionBase(Pos pos);

            /**
             * @if zh
             * @brief 参数元组(空)
             * @elseif en
             * @brief Argument tuple (empty)
             * @endif
             */
            Args args;

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

#ifdef _MSC_VER

    /**
     * @def DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, instructionBaseType, ...)
     * @if zh
     *
     * @brief 定义指令类型(MSVC版本)
     * @details 定义一个指令类型别名并注册到TypeMap中(MSVC编译器版本)
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名
     * @param instructionBaseType 指令基类模板
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define instruction type (MSVC version)
     * @details Defines an instruction type alias and registers it in TypeMap (MSVC compiler version)
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name
     * @param instructionBaseType Instruction base class template
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
    #define DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, instructionBaseType, ...)         \
        using pascalCase##Instruction = instructionBaseType<#lowerCase, ##__VA_ARGS__>;            \
        template<>                                                                                 \
        struct TypeFor<ic10::TokenType::KEYWORD_##upperCase> {                                     \
            using type = pascalCase##Instruction;                                                  \
        };                                                                                         \
        extern template struct instructionBaseType<#lowerCase, ##__VA_ARGS__>;

#else

    /**
     * @def DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, instructionBaseType, ...)
     * @if zh
     *
     * @brief 定义指令类型
     * @details 定义一个指令类型别名并注册到TypeMap中(标准C++版本)
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名
     * @param instructionBaseType 指令基类模板
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define instruction type
     * @details Defines an instruction type alias and registers it in TypeMap (standard C++ version)
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name
     * @param instructionBaseType Instruction base class template
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
    #define DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, instructionBaseType, ...)         \
        using pascalCase##Instruction =                                                            \
            instructionBaseType<#lowerCase __VA_OPT__(, ) __VA_ARGS__>;                            \
        template<>                                                                                 \
        struct TypeFor<ic10::TokenType::KEYWORD_##upperCase> {                                     \
            using type = pascalCase##Instruction;                                                  \
        };                                                                                         \
        extern template struct instructionBaseType<#lowerCase __VA_OPT__(, ) __VA_ARGS__>;

#endif

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
     * @details Defines a nullary instruction type using NullaryInstructionBase and registers it in TypeMap
     * @param upperCase Instruction uppercase underscore name
     * @param pascalCase Instruction PascalCase name
     * @param lowerCase Instruction lowercase name
     *
     * @endif
     */
#define DEFINE_NULLARY_INSTRUCTION(upperCase, pascalCase, lowerCase)                               \
    DEFINE_INSTRUCTION(upperCase, pascalCase, lowerCase, ic10::NullaryInstructionBase)

    DEFINE_NULLARY_INSTRUCTION(hcf, Hcf, HCF)

    DEFINE_NULLARY_INSTRUCTION(yield, Yield, YIELD)

    namespace ic10 {

        using NullaryInstruction = ShallowErrorable<
            HcfInstruction
#ifndef IC10_SIMPLE_MODE
            ,
            YieldInstruction
#endif
            >;

        using NullaryInstructionMap = TypeMap<
            TokenType, TokenType::KEYWORD_HCF
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_YIELD
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_nullary_ins.inl"

#endif  // COMPILER_AST_NULLARY_INS_HPP
