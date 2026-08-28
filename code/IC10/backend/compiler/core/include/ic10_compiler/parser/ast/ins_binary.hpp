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
 * @details 定义IC10中的二元指令(含两个操作数的指令),按 docs/grammar/parser/instructions/binary.g4
 *        中的操作数类型分组:
 *        - REG_TARGET + NUM_VALUE : abs、acos、asin、atan、ceil、cos、exp、floor、log、round、
 *          sin、sqrt、tan、trunc、not、move、sgn、seqz、snez、sgez、sgtz、slez、sltz、snan、snanz
 *        - DEVICE_REF + JUMP_LINE : bdns、bdnsal、bdse、bdseal、brdns、brdse
 *        - REG_TARGET + DEVICE_REF : sdns、sdse
 *        - NUM_VALUE + JUMP_LINE / ADDRESS + NUM_VALUE : poke、beqz、beqzal、bnez、bnezal、
 *          bgez、bgezal、bgtz、bgtzal、blez、blezal、bltz、bltzal、bnan、breqz、brnez、brgez、
 *          brgtz、brlez、brltz、brnan
 *        使用模板元编程自动生成指令类型和 TypeMap 映射。
 * @note 实现位于ast_binary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 binary instruction AST definitions
 * @details Defines binary instructions (instructions with two operands) in IC10, grouped by
 *        operand types per docs/grammar/parser/instructions/binary.g4:
 *        - REG_TARGET + NUM_VALUE : abs, acos, asin, atan, ceil, cos, exp, floor, log, round,
 *          sin, sqrt, tan, trunc, not, move, sgn, seqz, snez, sgez, sgtz, slez, sltz, snan, snanz
 *        - DEVICE_REF + JUMP_LINE : bdns, bdnsal, bdse, bdseal, brdns, brdse
 *        - REG_TARGET + DEVICE_REF : sdns, sdse
 *        - NUM_VALUE + JUMP_LINE / ADDRESS + NUM_VALUE : poke, beqz, beqzal, bnez, bnezal,
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

#include "instructions.hpp"

namespace stationeers::ic10 {

    /**
     * @def DEFINE_BINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义二元指令
     * @details 使用BinaryInstructionBase定义一个二元指令类型并注册到InstructionMapper
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名(对应InstructionKeyword枚举值)
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define binary instruction
     * @details Defines a binary instruction type using BinaryInstructionBase and registers it in
     * InstructionMapper
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name (InstructionKeyword enum value)
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_BINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                           \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, BinaryInstructionBase, __VA_ARGS__)

    // ---------- REG_TARGET NUM_VALUE ----------

    DEFINE_BINARY_INSTRUCTION(abs, Abs, ABS, OperandType::REG_TARGET, OperandType::NUM_VALUE)

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    DEFINE_BINARY_INSTRUCTION(acos, Acos, ACOS, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(asin, Asin, ASIN, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(atan, Atan, ATAN, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(ceil, Ceil, CEIL, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(cos, Cos, COS, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(exp, Exp, EXP, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(floor, Floor, FLOOR, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(log, Log, LOG, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(round, Round, ROUND, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sin, Sin, SIN, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sqrt, Sqrt, SQRT, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(tan, Tan, TAN, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(trunc, Trunc, TRUNC, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(not, Not, NOT, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(move, Move, MOVE, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sgn, Sgn, SGN, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(seqz, Seqz, SEQZ, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(snez, Snez, SNEZ, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sgez, Sgez, SGEZ, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sgtz, Sgtz, SGTZ, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(slez, Slez, SLEZ, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sltz, Sltz, SLTZ, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    #ifdef SNAN
        #undef SNAN
    #endif
    DEFINE_BINARY_INSTRUCTION(snan, Snan, SNAN, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(snanz, Snanz, SNANZ, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    // ---------- DEVICE_REF JUMP_LINE ----------

    DEFINE_BINARY_INSTRUCTION(bdns, Bdns, BDNS, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bdnsal, Bdnsal, BDNSAL, OperandType::DEVICE_REF, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bdse, Bdse, BDSE, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bdseal, Bdseal, BDSEAL, OperandType::DEVICE_REF, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(brdns, Brdns, BRDNS, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brdse, Brdse, BRDSE, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    // ---------- REG_TARGET DEVICE_REF ----------

    DEFINE_BINARY_INSTRUCTION(sdns, Sdns, SDNS, OperandType::REG_TARGET, OperandType::DEVICE_REF)

    DEFINE_BINARY_INSTRUCTION(sdse, Sdse, SDSE, OperandType::REG_TARGET, OperandType::DEVICE_REF)

    // ---------- ADDRESS/NUM_VALUE + NUM_VALUE/JUMP_LINE ----------

    DEFINE_BINARY_INSTRUCTION(poke, Poke, POKE, OperandType::ADDRESS, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(beqz, Beqz, BEQZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        beqzal, Beqzal, BEQZAL, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bnez, Bnez, BNEZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bnezal, Bnezal, BNEZAL, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bgez, Bgez, BGEZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bgezal, Bgezal, BGEZAL, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bgtz, Bgtz, BGTZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bgtzal, Bgtzal, BGTZAL, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(blez, Blez, BLEZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        blezal, Blezal, BLEZAL, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bltz, Bltz, BLTZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bltzal, Bltzal, BLTZAL, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bnan, Bnan, BNAN, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(breqz, Breqz, BREQZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brnez, Brnez, BRNEZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brgez, Brgez, BRGEZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brgtz, Brgtz, BRGTZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brlez, Brlez, BRLEZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brltz, Brltz, BRLTZ, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brnan, Brnan, BRNAN, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

#endif

    using BinaryInstruction = ShallowErrorable<
        AbsInstruction
#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        ,
        PokeInstruction, BdnsInstruction, SdnsInstruction, AcosInstruction, AsinInstruction,
        AtanInstruction, CeilInstruction, CosInstruction, ExpInstruction, FloorInstruction,
        LogInstruction, RoundInstruction, SinInstruction, SqrtInstruction, TanInstruction,
        TruncInstruction, NotInstruction, MoveInstruction, SgnInstruction, SeqzInstruction,
        SnezInstruction, SgezInstruction, SgtzInstruction, SlezInstruction, SltzInstruction,
        SnanInstruction, SnanzInstruction, BeqzInstruction, BeqzalInstruction, BnezInstruction,
        BnezalInstruction, BgezInstruction, BgezalInstruction, BgtzInstruction, BgtzalInstruction,
        BlezInstruction, BlezalInstruction, BltzInstruction, BltzalInstruction, BnanInstruction,
        BdnsalInstruction, BdseInstruction, BdsealInstruction, BreqzInstruction, BrnezInstruction,
        BrgezInstruction, BrgtzInstruction, BrlezInstruction, BrltzInstruction, BrnanInstruction,
        BrdnsInstruction, BrdseInstruction, SdseInstruction
#endif

        >;

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_BINARY_INS_HPP
