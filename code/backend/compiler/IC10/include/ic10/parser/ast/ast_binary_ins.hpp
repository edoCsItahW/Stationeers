// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_binary_ins.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_BINARY_INS_HPP
#define COMPILER_AST_BINARY_INS_HPP
#pragma once

#include "ast_unary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        template<FString K, typename O1, typename O2>
        struct BinaryInstructionBase : UnaryInstructionBase<K, O1> {
            static constexpr auto nodeName = fstr_concat_v<K, "Instruction">;

            O2 operand2;

            using Args = std::tuple<O1, O2>;

            Args args;

            BinaryInstructionBase() = default;

            BinaryInstructionBase(Pos pos, O1 op1, O2 op2);

            [[nodiscard]] Pos end() const override;

            [[nodiscard]] std::string toString() const override;

            [[nodiscard]] std::string toJSON() const override;

            template<typename... Ts>
            [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
        };

    }  // namespace ic10

#define DEFINE_BINARY_INSTRUCTION(upperCase, pascalCase, lowerCase, ...)                           \
    DEFINE_INSTRUCTION(upperCase, pascalCase, lowerCase, ic10::BinaryInstructionBase, __VA_ARGS__)

    // 二元指令别名
    DEFINE_BINARY_INSTRUCTION(abs, Abs, ABS, ic10::RegisterOrIdentifier, ic10::Operand)

#ifndef IC10_SIMPLE_MODE

    DEFINE_BINARY_INSTRUCTION(acos, Acos, ACOS, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(asin, Asin, ASIN, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(atan, Atan, ATAN, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(ceil, Ceil, CEIL, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(cos, Cos, COS, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(exp, Exp, EXP, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(floor, Floor, FLOOR, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(log, Log, LOG, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(round, Round, ROUND, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(sin, Sin, SIN, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(sqrt, Sqrt, SQRT, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(tan, Tan, TAN, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(trunc, Trunc, TRUNC, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(not, Not, NOT, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(move, Move, MOVE, ic10::RegisterOrIdentifier, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(sub, Sub, SUB, ic10::RegisterOrIdentifier, ic10::Operand)

#endif


    namespace ic10 {

        using BinaryInstructionMap_RI_O = TypeMap<
            TokenType, TokenType::KEYWORD_ABS
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_ACOS, TokenType::KEYWORD_ASIN, TokenType::KEYWORD_ATAN,
            TokenType::KEYWORD_CEIL, TokenType::KEYWORD_COS, TokenType::KEYWORD_EXP,
            TokenType::KEYWORD_FLOOR, TokenType::KEYWORD_LOG, TokenType::KEYWORD_ROUND,
            TokenType::KEYWORD_SIN, TokenType::KEYWORD_SQRT, TokenType::KEYWORD_TAN,
            TokenType::KEYWORD_TRUNC, TokenType::KEYWORD_NOT, TokenType::KEYWORD_MOVE,
            TokenType::KEYWORD_SUB
#endif
            >;

    }  // namespace ic10

    DEFINE_BINARY_INSTRUCTION(bdns, Bdns, BDNS, ic10::DeviceReference, ic10::Operand)

#ifndef IC10_SIMPLE_MODE

    DEFINE_BINARY_INSTRUCTION(bdnsal, Bdnsal, BDNSAL, ic10::DeviceReference, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(bdse, Bdse, BDSE, ic10::DeviceReference, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(bdseal, Bdseal, BDSEAL, ic10::DeviceReference, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(brdns, Brdns, BRDNS, ic10::DeviceReference, ic10::Operand)

    DEFINE_BINARY_INSTRUCTION(brdse, Brdse, BRDSE, ic10::DeviceReference, ic10::Operand)

#endif

    namespace ic10 {

        using BinaryInstructionMap_DR_O = TypeMap<
            TokenType, TokenType::KEYWORD_BDNS
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_BDNSAL, TokenType::KEYWORD_BDSE, TokenType::KEYWORD_BDSEAL,
            TokenType::KEYWORD_BRDNS, TokenType::KEYWORD_BRDSE
#endif
            >;

    }  // namespace ic10

    DEFINE_BINARY_INSTRUCTION(sdns, Sdns, SDNS, ic10::RegisterOrIdentifier, ic10::DeviceReference)

#ifndef IC10_SIMPLE_MODE

    DEFINE_BINARY_INSTRUCTION(sdse, Sdse, SDSE, ic10::RegisterOrIdentifier, ic10::DeviceReference)

#endif

    namespace ic10 {

        using BinaryInstructionMap_RI_DR = TypeMap<
            TokenType, TokenType::KEYWORD_SDNS
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_SDSE
#endif
            >;

    }  // namespace ic10

    DEFINE_BINARY_INSTRUCTION(poke, Poke, POKE, ic10::Operand, ic10::Operand)

#ifndef IC10_SIMPLE_MODE

    DEFINE_BINARY_INSTRUCTION(beqz, Beqz, BEQZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(beqzal, Beqzal, BEQZAL, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bnez, Bnez, BNEZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bnezal, Bnezal, BNEZAL, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bgez, Bgez, BGEZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bgezal, Bgezal, BGEZAL, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bgtz, Bgtz, BGTZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bgtzal, Bgtzal, BGTZAL, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(blez, Blez, BLEZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(blezal, Blezal, BLEZAL, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bltz, Bltz, BLTZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bltzal, Bltzal, BLTZAL, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bnan, Bnan, BNAN, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(bnanz, Bnanz, BNANZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(breqz, Breqz, BREQZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(brnz, Brnz, BRNZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(brgez, Brgez, BRGEZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(brgtz, Brgtz, BRGTZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(brlez, Brlez, BRLEZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(brltz, Brltz, BRLTZ, ic10::Operand, ic10::Operand)
    DEFINE_BINARY_INSTRUCTION(brnan, Brnan, BRNAN, ic10::Operand, ic10::Operand)

#endif

    namespace ic10 {

        using BinaryInstructionMap_O_O = TypeMap<
            TokenType, TokenType::KEYWORD_POKE
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_BEQZ, TokenType::KEYWORD_BEQZAL, TokenType::KEYWORD_BNEZ,
            TokenType::KEYWORD_BNEZAL, TokenType::KEYWORD_BGEZ, TokenType::KEYWORD_BGEZAL,
            TokenType::KEYWORD_BGTZ, TokenType::KEYWORD_BGTZAL, TokenType::KEYWORD_BLEZ,
            TokenType::KEYWORD_BLEZAL, TokenType::KEYWORD_BLTZ, TokenType::KEYWORD_BLTZAL,
            TokenType::KEYWORD_BNAN, TokenType::KEYWORD_BNANZ, TokenType::KEYWORD_BREQZ,
            TokenType::KEYWORD_BRNZ, TokenType::KEYWORD_BRGEZ, TokenType::KEYWORD_BRGTZ,
            TokenType::KEYWORD_BRLEZ, TokenType::KEYWORD_BRLTZ, TokenType::KEYWORD_BRNAN
#endif
            >;

        using BinaryInstruction = ShallowErrorable<
            AbsInstruction, PokeInstruction, BdnsInstruction, SdnsInstruction
#ifndef IC10_SIMPLE_MODE
            ,
            AcosInstruction, AsinInstruction, AtanInstruction, CeilInstruction, CosInstruction,
            ExpInstruction, FloorInstruction, LogInstruction, RoundInstruction, SinInstruction,
            SqrtInstruction, TanInstruction, TruncInstruction, NotInstruction, MoveInstruction,
            BeqzInstruction, BeqzalInstruction, BnezInstruction, BnezalInstruction, BgezInstruction,
            BgezalInstruction, BgtzInstruction, BgtzalInstruction, BlezInstruction,
            BlezalInstruction, BltzInstruction, BltzalInstruction, BnanInstruction,
            BnanzInstruction, BdnsalInstruction, BdseInstruction, BdsealInstruction,
            BreqzInstruction, BrnzInstruction, BrgezInstruction, BrgtzInstruction, BrlezInstruction,
            BrltzInstruction, BrnanInstruction, BrdnsInstruction, BrdseInstruction, SubInstruction,
            SdseInstruction
#endif
            >;

        using BinaryInstructionMap = TypeMap<
            TokenType, TokenType::KEYWORD_ABS, TokenType::KEYWORD_POKE, TokenType::KEYWORD_BDNS,
            TokenType::KEYWORD_SDNS
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_ACOS, TokenType::KEYWORD_ASIN, TokenType::KEYWORD_ATAN,
            TokenType::KEYWORD_CEIL, TokenType::KEYWORD_COS, TokenType::KEYWORD_EXP,
            TokenType::KEYWORD_FLOOR, TokenType::KEYWORD_LOG, TokenType::KEYWORD_ROUND,
            TokenType::KEYWORD_SIN, TokenType::KEYWORD_SQRT, TokenType::KEYWORD_TAN,
            TokenType::KEYWORD_TRUNC, TokenType::KEYWORD_NOT, TokenType::KEYWORD_MOVE,
            TokenType::KEYWORD_BEQZ, TokenType::KEYWORD_BEQZAL, TokenType::KEYWORD_BNEZ,
            TokenType::KEYWORD_BNEZAL, TokenType::KEYWORD_BGEZ, TokenType::KEYWORD_BGEZAL,
            TokenType::KEYWORD_BGTZ, TokenType::KEYWORD_BGTZAL, TokenType::KEYWORD_BLEZ,
            TokenType::KEYWORD_BLEZAL, TokenType::KEYWORD_BLTZ, TokenType::KEYWORD_BLTZAL,
            TokenType::KEYWORD_BNAN, TokenType::KEYWORD_BNANZ, TokenType::KEYWORD_BREQZ,
            TokenType::KEYWORD_BRNZ, TokenType::KEYWORD_BRGEZ, TokenType::KEYWORD_BRGTZ,
            TokenType::KEYWORD_BRLEZ, TokenType::KEYWORD_BRLTZ, TokenType::KEYWORD_BRNAN,
            TokenType::KEYWORD_BDNSAL, TokenType::KEYWORD_BDSE, TokenType::KEYWORD_BDSEAL,
            TokenType::KEYWORD_BRDNS, TokenType::KEYWORD_BRDSE, TokenType::KEYWORD_SDSE
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_binary_ins.inl"

#endif  // COMPILER_AST_BINARY_INS_HPP
