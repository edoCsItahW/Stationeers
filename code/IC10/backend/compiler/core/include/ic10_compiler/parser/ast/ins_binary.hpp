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
#ifndef IC10_COMPILER_CORE_INS_BINARY_HPP
#define IC10_COMPILER_CORE_INS_BINARY_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {

    /**
     * @def DEFINE_BINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, memberAccess, ...)
     * @if zh
     *
     * @brief 定义二元指令
     * @details 使用BinaryInstructionBase定义一个二元指令类型并注册到InstructionMapper
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名(对应InstructionKeyword枚举值)
     * @param memberAccess 该指令对逻辑属性的读写方向(@ref Access；不涉及逻辑属性时为 @ref Access::None)
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
     * @param memberAccess Read/write direction needed for a logic property (@ref Access; @ref Access::None if none)
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_BINARY_INSTRUCTION(lowerCase, pascalCase, upperCase, memberAccess, ...)      \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, memberAccess, BinaryInstructionBase, __VA_ARGS__)

    // ---------- REG_TARGET NUM_VALUE ----------

    DEFINE_BINARY_INSTRUCTION(abs, Abs, ABS, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    DEFINE_BINARY_INSTRUCTION(acos, Acos, ACOS, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(asin, Asin, ASIN, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(atan, Atan, ATAN, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(ceil, Ceil, CEIL, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(cos, Cos, COS, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(exp, Exp, EXP, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(floor, Floor, FLOOR, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(log, Log, LOG, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(round, Round, ROUND, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sin, Sin, SIN, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sqrt, Sqrt, SQRT, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(tan, Tan, TAN, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(trunc, Trunc, TRUNC, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(not, Not, NOT, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(move, Move, MOVE, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sgn, Sgn, SGN, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(seqz, Seqz, SEQZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(snez, Snez, SNEZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sgez, Sgez, SGEZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sgtz, Sgtz, SGTZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(slez, Slez, SLEZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(sltz, Sltz, SLTZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    #ifdef SNAN
        #undef SNAN
    #endif
    DEFINE_BINARY_INSTRUCTION(snan, Snan, SNAN, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(snanz, Snanz, SNANZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE)

    // ---------- DEVICE_REF JUMP_LINE ----------

    DEFINE_BINARY_INSTRUCTION(bdns, Bdns, BDNS, Access::None, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bdnsal, Bdnsal, BDNSAL, Access::None, OperandType::DEVICE_REF, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bdse, Bdse, BDSE, Access::None, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bdseal, Bdseal, BDSEAL, Access::None, OperandType::DEVICE_REF, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(brdns, Brdns, BRDNS, Access::None, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brdse, Brdse, BRDSE, Access::None, OperandType::DEVICE_REF, OperandType::JUMP_LINE)

    // ---------- REG_TARGET DEVICE_REF ----------

    DEFINE_BINARY_INSTRUCTION(sdns, Sdns, SDNS, Access::None, OperandType::REG_TARGET, OperandType::DEVICE_REF)

    DEFINE_BINARY_INSTRUCTION(sdse, Sdse, SDSE, Access::None, OperandType::REG_TARGET, OperandType::DEVICE_REF)

    // ---------- ADDRESS/NUM_VALUE + NUM_VALUE/JUMP_LINE ----------

    DEFINE_BINARY_INSTRUCTION(poke, Poke, POKE, Access::None, OperandType::ADDRESS, OperandType::NUM_VALUE)

    DEFINE_BINARY_INSTRUCTION(beqz, Beqz, BEQZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        beqzal, Beqzal, BEQZAL, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bnez, Bnez, BNEZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bnezal, Bnezal, BNEZAL, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bgez, Bgez, BGEZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bgezal, Bgezal, BGEZAL, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bgtz, Bgtz, BGTZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bgtzal, Bgtzal, BGTZAL, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(blez, Blez, BLEZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        blezal, Blezal, BLEZAL, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bltz, Bltz, BLTZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(
        bltzal, Bltzal, BLTZAL, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_BINARY_INSTRUCTION(bnan, Bnan, BNAN, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(breqz, Breqz, BREQZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brnez, Brnez, BRNEZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brgez, Brgez, BRGEZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brgtz, Brgtz, BRGTZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brlez, Brlez, BRLEZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brltz, Brltz, BRLTZ, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

    DEFINE_BINARY_INSTRUCTION(brnan, Brnan, BRNAN, Access::None, OperandType::NUM_VALUE, OperandType::JUMP_LINE)

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

#endif  // IC10_COMPILER_CORE_INS_BINARY_HPP
