// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_ternary_ins.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10三元指令AST定义
 * @details 定义IC10中的三元指令(含三个操作数的指令),
 *        按 docs/grammar/parser/instructions/ternary.g4 的操作数类型分组:
 *        - REG_TARGET NUM_VALUE NUM_VALUE : add、and、nor、or、sla、sll、sra、srl、xor、sub、
 *          mul、div、mod、pow、max、min、atan2、rol、ror、sapz、snaz、seq、sne、sge、sgt、
 *          sle、slt
 *        - REG_TARGET DEVICE_REF ADDRESS  : get
 *        - REG_TARGET DEVICE_REF_STRICT REAGENT_HASH : rmap
 *        - DEVICE_REF ADDRESS NUM_VALUE  : put
 *        - REG_TARGET DEVICE_REF LOGIC_PROP : l
 *        - DEVICE_REF LOGIC_PROP REG_TARGET : s
 *        - DEVICE_HASH LOGIC_PROP REG_TARGET : sb
 *        - DEVICE_REF LOGIC_PROP JUMP_LINE : bdnvl、bdnvs
 *        - NUM_VALUE NUM_VALUE JUMP_LINE  : beq、beqal、bne、bneal、bge、bgeal、bgt、bgtal、
 *          ble、bleal、blt、bltal、bapz、bapzal、bnaz、bnazal、breq、brne、brge、brgt、brle、
 *          brlt、brapz、brnaz
 * @note 实现位于ast_ternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 ternary instruction AST definitions
 * @details Defines ternary instructions (instructions with three operands) in IC10, grouped by
 *        operand types per docs/grammar/parser/instructions/ternary.g4:
 *        - REG_TARGET NUM_VALUE NUM_VALUE : add, and, nor, or, sla, sll, sra, srl, xor, sub,
 *          mul, div, mod, pow, max, min, atan2, rol, ror, sapz, snaz, seq, sne, sge, sgt,
 *          sle, slt
 *        - REG_TARGET DEVICE_REF ADDRESS  : get
 *        - REG_TARGET DEVICE_REF_STRICT REAGENT_HASH : rmap
 *        - DEVICE_REF ADDRESS NUM_VALUE  : put
 *        - REG_TARGET DEVICE_REF LOGIC_PROP : l
 *        - DEVICE_REF LOGIC_PROP REG_TARGET : s
 *        - DEVICE_HASH LOGIC_PROP REG_TARGET : sb
 *        - DEVICE_REF LOGIC_PROP JUMP_LINE : bdnvl, bdnvs
 *        - NUM_VALUE NUM_VALUE JUMP_LINE  : beq, beqal, bne, bneal, bge, bgeal, bgt, bgtal,
 *          ble, bleal, blt, bltal, bapz, bapzal, bnaz, bnazal, breq, brne, brge, brgt, brle,
 *          brlt, brapz, brnaz
 * @note Implementation in ast_ternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_TERNARY_INS_HPP
#define COMPILER_AST_TERNARY_INS_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {


    /**
     * @def DEFINE_TERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)
     * @if zh
     *
     * @brief 定义三元指令
     * @details 使用TernaryInstructionBase定义一个三元指令类型并注册到InstructionMapper
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名(对应InstructionKeyword枚举值)
     * @param ... 可变参数(操作数类型)
     *
     * @elseif en
     *
     * @brief Define ternary instruction
     * @details Defines a ternary instruction type using TernaryInstructionBase and registers it in
     * InstructionMapper
     * @param lowerCase Instruction lowercase name
     * @param pascalCase Instruction PascalCase name
     * @param upperCase Instruction uppercase underscore name (InstructionKeyword enum value)
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_TERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, ...)                          \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, TernaryInstructionBase, __VA_ARGS__)

    // ---------- REG_TARGET NUM_VALUE NUM_VALUE ----------

    DEFINE_TERNARY_INSTRUCTION(
        add, Add, ADD, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    DEFINE_TERNARY_INSTRUCTION(
        atan2, Atan2, ATAN2, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        div, Div, DIV, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        max, Max, MAX, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        min, Min, MIN, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        mod, Mod, MOD, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        mul, Mul, MUL, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        pow, Pow, POW, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sub, Sub, SUB, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        rol, Rol, ROL, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        ror, Ror, ROR, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(and, And, AND, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)

    DEFINE_TERNARY_INSTRUCTION(
        nor, Nor, NOR, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        or, Or, OR, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sla, Sla, SLA, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sll, Sll, SLL, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sra, Sra, SRA, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        srl, Srl, SRL, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        xor, Xor, XOR, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sapz, Sapz, SAPZ, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        snaz, Snaz, SNAZ, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        seq, Seq, SEQ, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sne, Sne, SNE, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sge, Sge, SGE, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sgt, Sgt, SGT, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sle, Sle, SLE, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        slt, Slt, SLT, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    // ---------- REG_TARGET DEVICE_REF ADDRESS ----------

    DEFINE_TERNARY_INSTRUCTION(
        get, Get, GET, OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::ADDRESS
    )

    // ---------- REG_TARGET DEVICE_REF_STRICT REAGENT_HASH ----------

    DEFINE_TERNARY_INSTRUCTION(
        rmap, Rmap, RMAP, OperandType::REG_TARGET, OperandType::DEVICE_REF_STRICT,
        OperandType::REAGENT_HASH
    )

    // ---------- DEVICE_REF ADDRESS NUM_VALUE ----------

    DEFINE_TERNARY_INSTRUCTION(
        put, Put, PUT, OperandType::DEVICE_REF, OperandType::ADDRESS, OperandType::NUM_VALUE
    )

    // ---------- REG_TARGET DEVICE_REF LOGIC_PROP ----------

    DEFINE_TERNARY_INSTRUCTION(
        l, L, L, OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::LOGIC_PROP
    )

    // ---------- DEVICE_REF LOGIC_PROP REG_TARGET ----------

    DEFINE_TERNARY_INSTRUCTION(
        s, S, S, OperandType::DEVICE_REF, OperandType::LOGIC_PROP, OperandType::REG_TARGET
    )

    // ---------- DEVICE_HASH LOGIC_PROP REG_TARGET ----------

    DEFINE_TERNARY_INSTRUCTION(
        sb, Sb, SB, OperandType::DEVICE_HASH, OperandType::LOGIC_PROP, OperandType::REG_TARGET
    )

    // ---------- DEVICE_REF LOGIC_PROP JUMP_LINE ----------

    DEFINE_TERNARY_INSTRUCTION(
        bdnvl, Bdnvl, BDNVL, OperandType::DEVICE_REF, OperandType::LOGIC_PROP,
        OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bdnvs, Bdnvs, BDNVS, OperandType::DEVICE_REF, OperandType::LOGIC_PROP,
        OperandType::JUMP_LINE
    )

    // ---------- NUM_VALUE NUM_VALUE JUMP_LINE ----------

    DEFINE_TERNARY_INSTRUCTION(
        beq, Beq, BEQ, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        beqal, Beqal, BEQAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bne, Bne, BNE, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bneal, Bneal, BNEAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bge, Bge, BGE, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bgeal, Bgeal, BGEAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bgt, Bgt, BGT, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bgtal, Bgtal, BGTAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        ble, Ble, BLE, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bleal, Bleal, BLEAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        blt, Blt, BLT, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bltal, Bltal, BLTAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bapz, Bapz, BAPZ, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bapzal, Bapzal, BAPZAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bnaz, Bnaz, BNAZ, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bnazal, Bnazal, BNAZAL, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        breq, Breq, BREQ, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brne, Brne, BRNE, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brge, Brge, BRGE, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brgt, Brgt, BRGT, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brle, Brle, BRLE, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brlt, Brlt, BRLT, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brapz, Brapz, BRAPZ, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brnaz, Brnaz, BRNAZ, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

#endif


    using TernaryInstruction = ShallowErrorable<
        AddInstruction
#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        ,
        GetInstruction, PutInstruction, LInstruction, SInstruction, SbInstruction, BeqInstruction,
        BdnvlInstruction, Atan2Instruction, DivInstruction, MaxInstruction, MinInstruction,
        ModInstruction, MulInstruction, PowInstruction, SubInstruction, RolInstruction,
        RorInstruction, AndInstruction, NorInstruction, OrInstruction, SlaInstruction,
        SllInstruction, SraInstruction, SrlInstruction, XorInstruction, RmapInstruction,
        BeqalInstruction, BneInstruction, BnealInstruction, BgeInstruction, BgealInstruction,
        BgtInstruction, BgtalInstruction, BleInstruction, BlealInstruction, BltInstruction,
        BltalInstruction, BapzInstruction, BapzalInstruction, BnazInstruction, BnazalInstruction,
        BdnvsInstruction, BreqInstruction, BrneInstruction, BrgeInstruction, BrgtInstruction,
        BrleInstruction, BrltInstruction, BrapzInstruction, BrnazInstruction, SapzInstruction,
        SnazInstruction, SeqInstruction, SneInstruction, SgeInstruction, SgtInstruction,
        SleInstruction, SltInstruction
#endif
        >;

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_TERNARY_INS_HPP
