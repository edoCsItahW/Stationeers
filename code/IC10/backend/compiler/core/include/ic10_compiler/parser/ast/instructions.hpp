// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file instructions.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/30 23:58
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_INSTRUCTIONS_HPP
#define STATIONEERS_INSTRUCTIONS_HPP
#pragma once

#include "../instruction_dispatcher.hpp"
#include "common/utils/fstr.hpp"
#include "semantic_operand.hpp"

namespace stationeers::ic10 {

    template<typename >
    struct is_instruction : std::false_type {};

    template<template<FString K, OperandType...> class Instruction, FString K, OperandType... Vs>
        requires requires { Instruction<K, Vs...>::keyword == K; }
    struct is_instruction<Instruction<K, Vs...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_instruction_v = is_instruction<T>::value;

    template<typename T>
    concept IsInstruction = is_instruction_v<T>;

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
        NullaryInstructionBase(Pos pos) noexcept;

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
        template<FString... Vs, AstJsonAble... Params>
            requires(sizeof...(Vs) == sizeof...(Params))
        [[nodiscard]] std::string jsonBase(Params&&... params) const;
    };

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
    template<FString K, OperandType V1>
    struct UnaryInstructionBase : NullaryInstructionBase<K> {
        static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

        static constexpr auto type1 = V1;

        using O1 = operand_type_t<V1>;

        O1 operand1;

        using Args = operand_type_list<V1>;

        Args args;

        UnaryInstructionBase() = default;

        UnaryInstructionBase(Pos pos, O1 o1);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;

        template<FString... Vs, AstJsonAble... Params>
            requires(sizeof...(Vs) == sizeof...(Params))
        [[nodiscard]] std::string jsonBase(Params&&... params) const;
    };

    /**
     * @if zh
     *
     * @class BinaryInstructionBase
     * @brief 二元指令基类模板
     * @details 二元指令是带两个操作数的指令基类。
     *        继承自UnaryInstructionBase,添加第二个操作数。
     *
     * @tparam K 指令关键字的FString类型
     * @tparam V1 第一个操作数类型枚举
     * @tparam V2 第二个操作数类型枚举
     *
     * @elseif en
     *
     * @class BinaryInstructionBase
     * @brief Binary instruction base class template
     * @details Binary instructions are instruction base classes with two operands.
     *        Inherits from UnaryInstructionBase, adding second operand.
     *
     * @tparam K FString type of instruction keyword
     * @tparam V1 OperandType enum for first operand
     * @tparam V2 OperandType enum for second operand
     *
     * @endif
     */
    template<FString K, OperandType V1, OperandType V2>
    struct BinaryInstructionBase : UnaryInstructionBase<K, V1> {
        static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

        static constexpr auto type2 = V2;

        using O1 = operand_type_t<V1>;

        using O2 = operand_type_t<V2>;

        O2 operand2;

        using Args = operand_type_list<V1, V2>;

        Args args;

        BinaryInstructionBase() = default;

        BinaryInstructionBase(Pos pos, O1 o1, O2 o2);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;

        template<FString... Vs, AstJsonAble... Params>
            requires(sizeof...(Vs) == sizeof...(Params))
        [[nodiscard]] std::string jsonBase(Params&&... params) const;
    };

    /**
     * @if zh
     *
     * @class TernaryInstructionBase
     * @brief 三元指令基类模板
     * @details 三元指令是带三个操作数的指令基类。
     *        继承自BinaryInstructionBase,添加第三个操作数。
     *
     * @tparam K  指令关键字的FString类型
     * @tparam V1 第一个操作数类型枚举
     * @tparam V2 第二个操作数类型枚举
     * @tparam V3 第三个操作数类型枚举
     *
     * @elseif en
     *
     * @class TernaryInstructionBase
     * @brief Ternary instruction base class template
     * @details Ternary instructions are instruction base classes with three operands.
     *        Inherits from BinaryInstructionBase, adding third operand.
     *
     * @tparam K  FString type of instruction keyword
     * @tparam V1 OperandType enum for first operand
     * @tparam V2 OperandType enum for second operand
     * @tparam V3 OperandType enum for third operand
     *
     * @endif
     */
    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    struct TernaryInstructionBase : BinaryInstructionBase<K, V1, V2> {
        static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

        static constexpr auto type3 = V3;

        using O1 = operand_type_t<V1>;

        using O2 = operand_type_t<V2>;

        using O3 = operand_type_t<V3>;

        O3 operand3;

        using Args = operand_type_list<V1, V2, V3>;

        Args args;

        TernaryInstructionBase() = default;

        TernaryInstructionBase(Pos pos, O1 o1, O2 o2, O3 o3);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;

        template<FString... Vs, AstJsonAble... Params>
            requires(sizeof...(Vs) == sizeof...(Params))
        [[nodiscard]] std::string jsonBase(Params&&... params) const;
    };

    /**
     * @if zh
     *
     * @class QuaternaryInstructionBase
     * @brief 四元指令基类模板
     * @details 四元指令是带四个操作数的指令基类。
     *        继承自TernaryInstructionBase,添加第四个操作数。
     *
     * @tparam K  指令关键字的FString类型
     * @tparam V1 第一个操作数类型枚举
     * @tparam V2 第二个操作数类型枚举
     * @tparam V3 第三个操作数类型枚举
     * @tparam V4 第四个操作数类型枚举
     *
     * @elseif en
     *
     * @class QuaternaryInstructionBase
     * @brief Quaternary instruction base class template
     * @details Quaternary instructions are instruction base classes with four operands.
     *        Inherits from TernaryInstructionBase, adding fourth operand.
     *
     * @tparam K  FString type of instruction keyword
     * @tparam V1 OperandType enum for first operand
     * @tparam V2 OperandType enum for second operand
     * @tparam V3 OperandType enum for third operand
     * @tparam V4 OperandType enum for fourth operand
     *
     * @endif
     */
    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    struct QuaternaryInstructionBase : TernaryInstructionBase<K, V1, V2, V3> {
        static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

        static constexpr auto type4 = V4;

        using O1 = operand_type_t<V1>;

        using O2 = operand_type_t<V2>;

        using O3 = operand_type_t<V3>;

        using O4 = operand_type_t<V4>;

        O4 operand4;

        using Args = operand_type_list<V1, V2, V3, V4>;

        Args args;

        QuaternaryInstructionBase() = default;

        QuaternaryInstructionBase(Pos pos, O1 o1, O2 o2, O3 o3, O4 o4);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;

        template<FString... Vs, AstJsonAble... Params>
            requires(sizeof...(Vs) == sizeof...(Params))
        [[nodiscard]] std::string jsonBase(Params&&... params) const;
    };

    /**
     * @if zh
     *
     * @class QuinaryInstructionBase
     * @brief 五元指令基类模板
     * @details 五元指令是带五个操作数的指令基类。
     *        继承自QuaternaryInstructionBase,添加第五个操作数。
     *
     * @tparam K  指令关键字的FString类型
     * @tparam V1 第一个操作数类型枚举
     * @tparam V2 第二个操作数类型枚举
     * @tparam V3 第三个操作数类型枚举
     * @tparam V4 第四个操作数类型枚举
     * @tparam V5 第五个操作数类型枚举
     *
     * @elseif en
     *
     * @class QuinaryInstructionBase
     * @brief Quinary instruction base class template
     * @details Quinary instructions are instruction base classes with five operands.
     *        Inherits from QuaternaryInstructionBase, adding fifth operand.
     *
     * @tparam K  FString type of instruction keyword
     * @tparam V1 OperandType enum for first operand
     * @tparam V2 OperandType enum for second operand
     * @tparam V3 OperandType enum for third operand
     * @tparam V4 OperandType enum for fourth operand
     * @tparam V5 OperandType enum for fifth operand
     *
     * @endif
     */
    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
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

    /**
     * @if zh
     *
     * @class SenaryInstructionBase
     * @brief 六元指令基类模板
     * @details 六元指令是带六个操作数的指令基类。
     *        继承自QuinaryInstructionBase,添加第六个操作数。
     *
     * @tparam K  指令关键字的FString类型
     * @tparam V1 第一个操作数类型枚举
     * @tparam V2 第二个操作数类型枚举
     * @tparam V3 第三个操作数类型枚举
     * @tparam V4 第四个操作数类型枚举
     * @tparam V5 第五个操作数类型枚举
     * @tparam V6 第六个操作数类型枚举
     *
     * @elseif en
     *
     * @class SenaryInstructionBase
     * @brief Senary instruction base class template
     * @details Senary instructions are instruction base classes with six operands.
     *        Inherits from QuinaryInstructionBase, adding sixth operand.
     *
     * @tparam K  FString type of instruction keyword
     * @tparam V1 OperandType enum for first operand
     * @tparam V2 OperandType enum for second operand
     * @tparam V3 OperandType enum for third operand
     * @tparam V4 OperandType enum for fourth operand
     * @tparam V5 OperandType enum for fifth operand
     * @tparam V6 OperandType enum for sixth operand
     *
     * @endif
     */
    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
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

        template<FString... Vs, AstJsonAble... Params>
            requires(sizeof...(Vs) == sizeof...(Params))
        [[nodiscard]] std::string jsonBase(Params&&... params) const;
    };

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
     * @details Defines an instruction type alias and registers it in TypeMap (MSVC compiler
     * version)
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
        struct InstructionMapper<InstructionKeyword::upperCase> {                                  \
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

}  // namespace stationeers::ic10

#include "instructions.inl"

#endif  // STATIONEERS_INSTRUCTIONS_HPP
