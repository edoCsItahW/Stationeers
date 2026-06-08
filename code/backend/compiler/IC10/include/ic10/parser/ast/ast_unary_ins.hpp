// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_unary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 12:58
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_UNARY_INS_HPP
#define COMPILER_AST_UNARY_INS_HPP
#pragma once

#include "ast_nullary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        template<FString K, typename O1>
        struct UnaryInstructionBase : NullaryInstructionBase<K> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            O1 operand1;

            using Args = std::tuple<O1>;

            Args args;

            UnaryInstructionBase() = default;

            UnaryInstructionBase(Pos pos, O1 op1);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<typename... Ts>
            [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
        };

    }  // namespace ic10

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
