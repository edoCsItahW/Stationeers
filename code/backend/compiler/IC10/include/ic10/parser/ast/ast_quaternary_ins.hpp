// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_quaternary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_QUATERNARY_INS_HPP
#define COMPILER_AST_QUATERNARY_INS_HPP
#pragma once

#include "ast_ternary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        // QuaternaryInstructionBase（实现于ast.cpp）

        template<FString K, typename O1, typename O2, typename O3, typename O4>
        struct QuaternaryInstructionBase : TernaryInstructionBase<K, O1, O2, O3> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            O4 operand4;

            using Args = std::tuple<O1, O2, O3, O4>;

            Args args;

            QuaternaryInstructionBase() = default;

            QuaternaryInstructionBase(Pos pos, O1 op1, O2 op2, O3 op3, O4 op4);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<typename... Ts>
            [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
        };

    }  // namespace ic10

// 四元指令别名
#define DEFINE_QUATERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                       \
    DEFINE_INSTRUCTION(                                                                            \
        lowerCase, pascalCase, upperCase, ic10::QuaternaryInstructionBase, __VA_ARGS__             \
    )

    // 四元指令 - 操作数类型 RegisterOrIdentifier, Operand, Operand, Operand
    DEFINE_QUATERNARY_INSTRUCTION(
        lerp, Lerp, LERP, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_QUATERNARY_INSTRUCTION(
        ext, Ext, EXT, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        ins, Ins, INS, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        sap, Sap, SAP, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        sna, Sna, SNA, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        select, Select, SELECT, ic10::RegisterOrIdentifier, ic10::Operand, ic10::Operand,
        ic10::Operand
    )

#endif


    namespace ic10 {

        using QuaternaryInstructionMap_RI_O_O_O = TypeMap<
            TokenType, TokenType::KEYWORD_LERP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_EXT, TokenType::KEYWORD_INS, TokenType::KEYWORD_SAP,
            TokenType::KEYWORD_SNA, TokenType::KEYWORD_SELECT
#endif
            >;

    }  // namespace ic10

    // 四元指令 - 操作数类型 DeviceReference, SlotIndex, LogicSlotType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        ss, Ss, SS, ic10::DeviceReference, ic10::SlotIndex, ic10::LogicSlotType,
        ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using QuaternaryInstructionMap_DR_SI_LS_RI = TypeMap<TokenType, TokenType::KEYWORD_SS>;

    }

    // 四元指令 - 操作数类型 RegisterOrIdentifier, Operand, LogicType, BatchMode
    DEFINE_QUATERNARY_INSTRUCTION(
        lb, Lb, LB, ic10::RegisterOrIdentifier, ic10::Operand, ic10::LogicType, ic10::BatchMode
    )

    namespace ic10 {

        using QuaternaryInstructionMap_RI_O_LT_BM = TypeMap<TokenType, TokenType::KEYWORD_LB>;

    }

    // 四元指令 - 操作数类型 Operand, Operand, LogicType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        sbn, Sbn, SBN, ic10::Operand, ic10::Operand, ic10::LogicType, ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using QuaternaryInstructionMap_O_O_LT_RI = TypeMap<TokenType, TokenType::KEYWORD_SBN>;

    }

    // 四元指令 - 操作数类型 Operand, SlotIndex, LogicSlotType, RegisterOrIdentifier
    DEFINE_QUATERNARY_INSTRUCTION(
        sbs, Sbs, SBS, ic10::Operand, ic10::SlotIndex, ic10::LogicSlotType,
        ic10::RegisterOrIdentifier
    )

    namespace ic10 {

        using QuaternaryInstructionMap_O_SI_LS_RI = TypeMap<TokenType, TokenType::KEYWORD_SBS>;

    }

    // 四元指令 - 操作数类型 Operand, Operand, Operand, Operand
    DEFINE_QUATERNARY_INSTRUCTION(
        bap, Bap, BAP, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )

#ifndef IC10_SIMPLE_MODE

    DEFINE_QUATERNARY_INSTRUCTION(
        bapal, Bapal, BAPAL, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        bna, Bna, BNA, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        bnaal, Bnaal, BNAAL, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        brap, Brap, BRAP, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )
    DEFINE_QUATERNARY_INSTRUCTION(
        brna, Brna, BRNA, ic10::Operand, ic10::Operand, ic10::Operand, ic10::Operand
    )

#endif

    namespace ic10 {

        using QuaternaryInstructionMap_O_O_O_O = TypeMap<
            TokenType, TokenType::KEYWORD_BAP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_BAPAL, TokenType::KEYWORD_BNA, TokenType::KEYWORD_BNAAL,
            TokenType::KEYWORD_BRAP, TokenType::KEYWORD_BRNA
#endif
            >;

    }  // namespace ic10

    // 四元指令 - 操作数类型 RegisterOrIdentifier, DeviceReference, SlotIndex, LogicSlotType
    DEFINE_QUATERNARY_INSTRUCTION(
        ls, Ls, LS, ic10::RegisterOrIdentifier, ic10::DeviceReference, ic10::SlotIndex,
        ic10::LogicSlotType
    )

    namespace ic10 {

        using QuaternaryInstructionMap_RI_DR_SI_LS = TypeMap<TokenType, TokenType::KEYWORD_LS>;

        using QuaternaryInstruction = ShallowErrorable<
            LerpInstruction, SsInstruction, LbInstruction, SbnInstruction, SbsInstruction,
            BapInstruction, LsInstruction
#ifndef IC10_SIMPLE_MODE
            ,
            ExtInstruction, InsInstruction, BapalInstruction, BnaInstruction, BnaalInstruction,
            BrapInstruction, BrnaInstruction, SapInstruction, SnaInstruction, SelectInstruction
#endif
            >;

        using QuaternaryInstructionMap = TypeMap<
            TokenType, TokenType::KEYWORD_LERP, TokenType::KEYWORD_SS, TokenType::KEYWORD_LB,
            TokenType::KEYWORD_SBN, TokenType::KEYWORD_SBS, TokenType::KEYWORD_BAP
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_LS, TokenType::KEYWORD_EXT, TokenType::KEYWORD_INS,
            TokenType::KEYWORD_SAP, TokenType::KEYWORD_SNA, TokenType::KEYWORD_SELECT,
            TokenType::KEYWORD_BAPAL, TokenType::KEYWORD_BNA, TokenType::KEYWORD_BNAAL,
            TokenType::KEYWORD_BRAP, TokenType::KEYWORD_BRNA, TokenType::KEYWORD_SAP,
            TokenType::KEYWORD_SNA
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_quaternary_ins.inl"

#endif  // COMPILER_AST_QUATERNARY_INS_HPP
