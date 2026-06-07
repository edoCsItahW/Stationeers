// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_nullary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 12:55
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_NULLARY_INS_HPP
#define COMPILER_AST_NULLARY_INS_HPP
#pragma once

#include "ast_node.hpp"

namespace stationeers {

    namespace ic10 {

        // NullaryInstructionBase（实现于ast.cpp）

        template<FString K>
        struct NullaryInstructionBase : AST<NullaryInstructionBase<K>> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            static constexpr auto keyword = K;

            using Args = std::tuple<>;

            NullaryInstructionBase() = default;

            NullaryInstructionBase(Pos pos);

            Args args;

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<typename... Ts>
            [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
        };

    }  // namespace ic10

#ifdef _MSC_VER

    #define DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, instructionBaseType, ...)         \
        using pascalCase##Instruction = instructionBaseType<#lowerCase, ##__VA_ARGS__>;            \
        template<>                                                                                 \
        struct TypeFor<ic10::TokenType::KEYWORD_##upperCase> {                                     \
            using type = pascalCase##Instruction;                                                  \
        };                                                                                         \
        extern template struct instructionBaseType<#lowerCase, ##__VA_ARGS__>;

#else

    #define DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, instructionBaseType, ...)         \
        using pascalCase##Instruction =                                                            \
            instructionBaseType<#lowerCase __VA_OPT__(, ) __VA_ARGS__>;                            \
        template<>                                                                                 \
        struct TypeFor<ic10::TokenType::KEYWORD_##upperCase> {                                     \
            using type = pascalCase##Instruction;                                                  \
        };                                                                                         \
        extern template struct instructionBaseType<#lowerCase __VA_OPT__(, ) __VA_ARGS__>;

#endif

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
