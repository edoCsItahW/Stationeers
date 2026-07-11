// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_binary_ins.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10二元指令AST定义
 * @details 定义IC10中的二元指令(含两个操作数的指令),按操作数类型分为四组:
 *        - RI_RON组(RegisterOrIdentifier, RegisterOrNumber): abs、acos、asin、atan、ceil、
 *          cos、exp、floor、log、round、sin、sqrt、tan、trunc、not、move、sgn、seqz、snez、
 *          sgez、sgtz、slez、sltz、snan、snanz
 *        - DR_RON组(DeviceReference, RegisterOrNumber): bdns、bdnsal、bdse、bdseal、brdns、brdse
 *        - RI_DR组(RegisterOrIdentifier, DeviceReference): sdns、sdse
 *        - RON_RON组(RegisterOrNumber, RegisterOrNumber): poke、beqz、beqzal、bnez、bnezal、
 *          bgez、bgezal、bgtz、bgtzal、blez、blezal、bltz、bltzal、bnan、breqz、brnez、brgez、
 *          brgtz、brlez、brltz、brnan
 *        使用模板元编程自动生成指令类型和TypeMap映射。
 * @note 实现位于ast_binary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 binary instruction AST definitions
 * @details Defines binary instructions (instructions with two operands) in IC10, grouped by operand
 *        type into four groups:
 *        - RI_RON group (RegisterOrIdentifier, RegisterOrNumber): abs, acos, asin, atan, ceil,
 *          cos, exp, floor, log, round, sin, sqrt, tan, trunc, not, move, sgn, seqz, snez,
 *          sgez, sgtz, slez, sltz, snan, snanz
 *        - DR_RON group (DeviceReference, RegisterOrNumber): bdns, bdnsal, bdse, bdseal, brdns, brdse
 *        - RI_DR group (RegisterOrIdentifier, DeviceReference): sdns, sdse
 *        - RON_RON group (RegisterOrNumber, RegisterOrNumber): poke, beqz, beqzal, bnez, bnezal,
 *          bgez, bgezal, bgtz, bgtzal, blez, blezal, bltz, bltzal, bnan, breqz, brnez, brgez,
 *          brgtz, brlez, brltz, brnan
 *        Uses template metaprogramming to automatically generate instruction types and TypeMap
 *        mappings.
 * @note Implementation in ast_binary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_BINARY_INS_HPP
#define COMPILER_AST_BINARY_INS_HPP
#pragma once

#include "ast_unary_ins.hpp"

namespace stationeers {

    namespace ic10 {

        /**
         * @if zh
         *
         * @class BinaryInstructionBase
         * @brief 二元指令基类模板
         * @details 二元指令是带两个操作数的指令基类。
         *        继承自UnaryInstructionBase,添加第二个操作数。
         *
         * @tparam K 指令关键字的FString类型
         * @tparam O1 第一个操作数类型
         * @tparam O2 第二个操作数类型
         *
         * @elseif en
         *
         * @class BinaryInstructionBase
         * @brief Binary instruction base class template
         * @details Binary instructions are instruction base classes with two operands.
         *        Inherits from UnaryInstructionBase, adding second operand.
         *
         * @tparam K FString type of instruction keyword
         * @tparam O1 Type of first operand
         * @tparam O2 Type of second operand
         *
         * @endif
         */
        template<FString K, typename O1, typename O2>
        struct BinaryInstructionBase : UnaryInstructionBase<K, O1> {
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
             * @brief 第二个操作数
             * @elseif en
             * @brief Second operand
             * @endif
             */
            O2 operand2;

            /**
             * @if zh
             * @brief 参数类型元组
             * @elseif en
             * @brief Argument types tuple
             * @endif
             */
            using Args = std::tuple<O1, O2>;

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
            BinaryInstructionBase() = default;

            /**
             * @if zh
             *
             * @brief 构造函数
             * @param pos 位置信息
             * @param op1 第一个操作数
             * @param op2 第二个操作数
             *
             * @elseif en
             *
             * @brief Constructor
             * @param pos Position information
             * @param op1 First operand
             * @param op2 Second operand
             *
             * @endif
             */
            BinaryInstructionBase(Pos pos, O1 op1, O2 op2);

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
     * @def DEFINE_BINARY_INSTRUCTION(upperCase, pascalCase, lowerCase, ...)
     * @if zh
     *
     * @brief 定义二元指令
     * @details 使用BinaryInstructionBase定义一个二元指令类型并注册到TypeMap
     * @param upperCase 指令大写下划线名
     * @param pascalCase 指令PascalCase名
     * @param lowerCase 指令小写名
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define binary instruction
     * @details Defines a binary instruction type using BinaryInstructionBase and registers it in TypeMap
     * @param upperCase Instruction uppercase underscore name
     * @param pascalCase Instruction PascalCase name
     * @param lowerCase Instruction lowercase name
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
    #define DEFINE_BINARY_INSTRUCTION(upperCase, pascalCase, lowerCase, ...)                           \
        DEFINE_INSTRUCTION(upperCase, pascalCase, lowerCase, ic10::BinaryInstructionBase, __VA_ARGS__)

    // 二元指令别名
    DEFINE_BINARY_INSTRUCTION(abs, Abs, ABS, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

#ifndef IC10_SIMPLE_MODE

    DEFINE_BINARY_INSTRUCTION(acos, Acos, ACOS, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(asin, Asin, ASIN, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(atan, Atan, ATAN, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(ceil, Ceil, CEIL, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(cos, Cos, COS, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(exp, Exp, EXP, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(floor, Floor, FLOOR, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(log, Log, LOG, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(round, Round, ROUND, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(sin, Sin, SIN, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(sqrt, Sqrt, SQRT, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(tan, Tan, TAN, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(trunc, Trunc, TRUNC, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(not, Not, NOT, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(move, Move, MOVE, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(sgn, Sgn, SGN, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(seqz, Seqz, SEQZ, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(snez, Snez, SNEZ, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(sgez, Sgez, SGEZ, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(sgtz, Sgtz, SGTZ, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(slez, Slez, SLEZ, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(sltz, Sltz, SLTZ, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

#ifdef SNAN
#undef SNAN
#endif
    DEFINE_BINARY_INSTRUCTION(snan, Snan, SNAN, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(snanz, Snanz, SNANZ, ic10::RegisterOrIdentifier, ic10::RegisterOrNumber)

#endif


    namespace ic10 {

        /**
         * @if zh
         * @brief RI_RON组二元指令TypeMap
         * @details 包含操作数类型为(RegisterOrIdentifier, RegisterOrNumber)的二元指令:
         *          abs、acos、asin、atan、ceil、cos、exp、floor、log、round、sin、sqrt、tan、
         *          trunc、not、move、sgn、seqz、snez、sgez、sgtz、slez、sltz、snan、snanz。
         * @elseif en
         * @brief RI_RON group binary instruction TypeMap
         * @details Contains binary instructions with operand types (RegisterOrIdentifier, RegisterOrNumber):
         *          abs, acos, asin, atan, ceil, cos, exp, floor, log, round, sin, sqrt, tan,
         *          trunc, not, move, sgn, seqz, snez, sgez, sgtz, slez, sltz, snan, snanz.
         * @endif
         */
        using BinaryInstructionMap_RI_RON = TypeMap<
            TokenType, TokenType::KEYWORD_ABS
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_ACOS, TokenType::KEYWORD_ASIN, TokenType::KEYWORD_ATAN,
            TokenType::KEYWORD_CEIL, TokenType::KEYWORD_COS, TokenType::KEYWORD_EXP,
            TokenType::KEYWORD_FLOOR, TokenType::KEYWORD_LOG, TokenType::KEYWORD_ROUND,
            TokenType::KEYWORD_SIN, TokenType::KEYWORD_SQRT, TokenType::KEYWORD_TAN,
            TokenType::KEYWORD_TRUNC, TokenType::KEYWORD_NOT, TokenType::KEYWORD_MOVE,
            TokenType::KEYWORD_SGN, TokenType::KEYWORD_SEQZ, TokenType::KEYWORD_SNEZ,
            TokenType::KEYWORD_SGEZ, TokenType::KEYWORD_SGTZ, TokenType::KEYWORD_SLEZ,
            TokenType::KEYWORD_SLTZ, TokenType::KEYWORD_SNAN, TokenType::KEYWORD_SNANZ
#endif
            >;

    }  // namespace ic10

    DEFINE_BINARY_INSTRUCTION(bdns, Bdns, BDNS, ic10::DeviceReference, ic10::RegisterOrNumber)

#ifndef IC10_SIMPLE_MODE

    DEFINE_BINARY_INSTRUCTION(bdnsal, Bdnsal, BDNSAL, ic10::DeviceReference, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(bdse, Bdse, BDSE, ic10::DeviceReference, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(bdseal, Bdseal, BDSEAL, ic10::DeviceReference, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(brdns, Brdns, BRDNS, ic10::DeviceReference, ic10::RegisterOrNumber)

    DEFINE_BINARY_INSTRUCTION(brdse, Brdse, BRDSE, ic10::DeviceReference, ic10::RegisterOrNumber)

#endif

    namespace ic10 {

        /**
         * @if zh
         * @brief DR_RON组二元指令TypeMap
         * @details 包含操作数类型为(DeviceReference, RegisterOrNumber)的二元指令:
         *          bdns、bdnsal、bdse、bdseal、brdns、brdse。
         * @elseif en
         * @brief DR_RON group binary instruction TypeMap
         * @details Contains binary instructions with operand types (DeviceReference, RegisterOrNumber):
         *          bdns, bdnsal, bdse, bdseal, brdns, brdse.
         * @endif
         */
        using BinaryInstructionMap_DR_RON = TypeMap<
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

        /**
         * @if zh
         * @brief RI_DR组二元指令TypeMap
         * @details 包含操作数类型为(RegisterOrIdentifier, DeviceReference)的二元指令:
         *          sdns、sdse。
         * @elseif en
         * @brief RI_DR group binary instruction TypeMap
         * @details Contains binary instructions with operand types (RegisterOrIdentifier, DeviceReference):
         *          sdns, sdse.
         * @endif
         */
        using BinaryInstructionMap_RI_DR = TypeMap<
            TokenType, TokenType::KEYWORD_SDNS
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_SDSE
#endif
            >;

    }  // namespace ic10

    DEFINE_BINARY_INSTRUCTION(poke, Poke, POKE, ic10::RegisterOrNumber, ic10::RegisterOrNumber)

#ifndef IC10_SIMPLE_MODE

    DEFINE_BINARY_INSTRUCTION(beqz, Beqz, BEQZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(beqzal, Beqzal, BEQZAL, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bnez, Bnez, BNEZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bnezal, Bnezal, BNEZAL, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bgez, Bgez, BGEZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bgezal, Bgezal, BGEZAL, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bgtz, Bgtz, BGTZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bgtzal, Bgtzal, BGTZAL, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(blez, Blez, BLEZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(blezal, Blezal, BLEZAL, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bltz, Bltz, BLTZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bltzal, Bltzal, BLTZAL, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(bnan, Bnan, BNAN, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(breqz, Breqz, BREQZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(brnez, Brnez, BRNEZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(brgez, Brgez, BRGEZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(brgtz, Brgtz, BRGTZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(brlez, Brlez, BRLEZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(brltz, Brltz, BRLTZ, ic10::RegisterOrNumber, ic10::RegisterOrNumber)
    DEFINE_BINARY_INSTRUCTION(brnan, Brnan, BRNAN, ic10::RegisterOrNumber, ic10::RegisterOrNumber)

#endif

    namespace ic10 {

        /**
         * @if zh
         * @brief RON_RON组二元指令TypeMap
         * @details 包含操作数类型为(RegisterOrNumber, RegisterOrNumber)的二元指令:
         *          poke、beqz、beqzal、bnez、bnezal、bgez、bgezal、bgtz、bgtzal、blez、blezal、
         *          bltz、bltzal、bnan、breqz、brnez、brgez、brgtz、brlez、brltz、brnan。
         * @elseif en
         * @brief RON_RON group binary instruction TypeMap
         * @details Contains binary instructions with operand types (RegisterOrNumber, RegisterOrNumber):
         *          poke, beqz, beqzal, bnez, bnezal, bgez, bgezal, bgtz, bgtzal, blez, blezal,
         *          bltz, bltzal, bnan, breqz, brnez, brgez, brgtz, brlez, brltz, brnan.
         * @endif
         */
        using BinaryInstructionMap_RON_RON = TypeMap<
            TokenType, TokenType::KEYWORD_POKE
#ifndef IC10_SIMPLE_MODE
            ,
            TokenType::KEYWORD_BEQZ, TokenType::KEYWORD_BEQZAL, TokenType::KEYWORD_BNEZ,
            TokenType::KEYWORD_BNEZAL, TokenType::KEYWORD_BGEZ, TokenType::KEYWORD_BGEZAL,
            TokenType::KEYWORD_BGTZ, TokenType::KEYWORD_BGTZAL, TokenType::KEYWORD_BLEZ,
            TokenType::KEYWORD_BLEZAL, TokenType::KEYWORD_BLTZ, TokenType::KEYWORD_BLTZAL,
            TokenType::KEYWORD_BNAN, TokenType::KEYWORD_BREQZ, TokenType::KEYWORD_BRNEZ,
            TokenType::KEYWORD_BRGEZ, TokenType::KEYWORD_BRGTZ,
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
            SgnInstruction, SeqzInstruction, SnezInstruction, SgezInstruction, SgtzInstruction,
            SlezInstruction, SltzInstruction, SnanInstruction, SnanzInstruction,
            BeqzInstruction, BeqzalInstruction, BnezInstruction, BnezalInstruction, BgezInstruction,
            BgezalInstruction, BgtzInstruction, BgtzalInstruction, BlezInstruction,
            BlezalInstruction, BltzInstruction, BltzalInstruction, BnanInstruction,
            BdnsalInstruction, BdseInstruction, BdsealInstruction,
            BreqzInstruction, BrnezInstruction, BrgezInstruction, BrgtzInstruction, BrlezInstruction,
            BrltzInstruction, BrnanInstruction, BrdnsInstruction, BrdseInstruction,
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
            TokenType::KEYWORD_SGN, TokenType::KEYWORD_SEQZ, TokenType::KEYWORD_SNEZ,
            TokenType::KEYWORD_SGEZ, TokenType::KEYWORD_SGTZ, TokenType::KEYWORD_SLEZ,
            TokenType::KEYWORD_SLTZ, TokenType::KEYWORD_SNAN, TokenType::KEYWORD_SNANZ,
            TokenType::KEYWORD_BEQZ, TokenType::KEYWORD_BEQZAL, TokenType::KEYWORD_BNEZ,
            TokenType::KEYWORD_BNEZAL, TokenType::KEYWORD_BGEZ, TokenType::KEYWORD_BGEZAL,
            TokenType::KEYWORD_BGTZ, TokenType::KEYWORD_BGTZAL, TokenType::KEYWORD_BLEZ,
            TokenType::KEYWORD_BLEZAL, TokenType::KEYWORD_BLTZ, TokenType::KEYWORD_BLTZAL,
            TokenType::KEYWORD_BNAN, TokenType::KEYWORD_BREQZ, TokenType::KEYWORD_BRNEZ,
            TokenType::KEYWORD_BRGEZ, TokenType::KEYWORD_BRGTZ,
            TokenType::KEYWORD_BRLEZ, TokenType::KEYWORD_BRLTZ, TokenType::KEYWORD_BRNAN,
            TokenType::KEYWORD_BDNSAL, TokenType::KEYWORD_BDSE, TokenType::KEYWORD_BDSEAL,
            TokenType::KEYWORD_BRDNS, TokenType::KEYWORD_BRDSE, TokenType::KEYWORD_SDSE
#endif
            >;

    }  // namespace ic10

}  // namespace stationeers

#include "ast_binary_ins.inl"

#endif  // COMPILER_AST_BINARY_INS_HPP
