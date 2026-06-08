// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_quinary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:21
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_QUINARY_INS_HPP
#define COMPILER_AST_QUINARY_INS_HPP
#pragma once

#include "ast_quaternary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        // QuinaryInstructionBase（实现于ast.cpp）

        template<FString K, typename O1, typename O2, typename O3, typename O4, typename O5>
        struct QuinaryInstructionBase : QuaternaryInstructionBase<K, O1, O2, O3, O4> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            O5 operand5;

            using Args = std::tuple<O1, O2, O3, O4, O5>;

            Args args;

            QuinaryInstructionBase() = default;

            QuinaryInstructionBase(Pos pos, O1 op1, O2 op2, O3 op3, O4 op4, O5 op5);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<typename... Ts>
            [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
        };

    }  // namespace ic10

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
