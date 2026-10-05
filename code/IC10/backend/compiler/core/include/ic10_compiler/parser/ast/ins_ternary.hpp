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
 *        - DEVICE_REF LOGIC_PROP NUM_VALUE : s
 *        - DEVICE_HASH LOGIC_PROP NUM_VALUE : sb
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
 *        - DEVICE_REF LOGIC_PROP NUM_VALUE : s
 *        - DEVICE_HASH LOGIC_PROP NUM_VALUE : sb
 *        - DEVICE_REF LOGIC_PROP JUMP_LINE : bdnvl, bdnvs
 *        - NUM_VALUE NUM_VALUE JUMP_LINE  : beq, beqal, bne, bneal, bge, bgeal, bgt, bgtal,
 *          ble, bleal, blt, bltal, bapz, bapzal, bnaz, bnazal, breq, brne, brge, brgt, brle,
 *          brlt, brapz, brnaz
 * @note Implementation in ast_ternary_ins.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef IC10_COMPILER_CORE_INS_TERNARY_HPP
#define IC10_COMPILER_CORE_INS_TERNARY_HPP
#pragma once

#include "instructions.hpp"

namespace stationeers::ic10 {


    /**
     * @def DEFINE_TERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, memberAccess, ...)
     * @if zh
     *
     * @brief 定义三元指令
     * @details 使用TernaryInstructionBase定义一个三元指令类型并注册到InstructionMapper
     * @param lowerCase 指令小写名
     * @param pascalCase 指令PascalCase名
     * @param upperCase 指令大写下划线名(对应InstructionKeyword枚举值)
     * @param memberAccess 该指令对逻辑属性的读写方向(@ref Access；不涉及逻辑属性时为 @ref Access::None)
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
     * @param memberAccess Read/write direction needed for a logic property (@ref Access; @ref Access::None if none)
     * @param ... Variadic parameters (operand types)
     *
     * @endif
     */
#define DEFINE_TERNARY_INSTRUCTION(lowerCase, pascalCase, upperCase, memberAccess, ...)     \
    DEFINE_INSTRUCTION(lowerCase, pascalCase, upperCase, memberAccess, TernaryInstructionBase, __VA_ARGS__)

    // ---------- REG_TARGET NUM_VALUE NUM_VALUE ----------

    DEFINE_TERNARY_INSTRUCTION(
        add, Add, ADD, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE

    DEFINE_TERNARY_INSTRUCTION(
        atan2, Atan2, ATAN2, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        div, Div, DIV, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        max, Max, MAX, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        min, Min, MIN, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        mod, Mod, MOD, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        mul, Mul, MUL, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        pow, Pow, POW, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sub, Sub, SUB, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        rol, Rol, ROL, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        ror, Ror, ROR, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(and, And, AND, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE)

    DEFINE_TERNARY_INSTRUCTION(
        nor, Nor, NOR, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        or, Or, OR, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sla, Sla, SLA, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sll, Sll, SLL, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sra, Sra, SRA, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        srl, Srl, SRL, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        xor, Xor, XOR, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sapz, Sapz, SAPZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        snaz, Snaz, SNAZ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        seq, Seq, SEQ, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sne, Sne, SNE, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sge, Sge, SGE, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sgt, Sgt, SGT, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        sle, Sle, SLE, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    DEFINE_TERNARY_INSTRUCTION(
        slt, Slt, SLT, Access::None, OperandType::REG_TARGET, OperandType::NUM_VALUE, OperandType::NUM_VALUE
    )

    // ---------- REG_TARGET DEVICE_REF ADDRESS ----------

    DEFINE_TERNARY_INSTRUCTION(
        get, Get, GET, Access::None, OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::ADDRESS
    )

    // ---------- REG_TARGET DEVICE_REF_STRICT REAGENT_HASH ----------

    DEFINE_TERNARY_INSTRUCTION(
        rmap, Rmap, RMAP, Access::None, OperandType::REG_TARGET, OperandType::DEVICE_REF_STRICT,
        OperandType::REAGENT_HASH
    )

    // ---------- DEVICE_REF ADDRESS NUM_VALUE ----------

    DEFINE_TERNARY_INSTRUCTION(
        put, Put, PUT, Access::None, OperandType::DEVICE_REF, OperandType::ADDRESS, OperandType::NUM_VALUE
    )

    // ---------- REG_TARGET DEVICE_REF LOGIC_PROP ----------

    DEFINE_TERNARY_INSTRUCTION(
        l, L, L, Access::Read, OperandType::REG_TARGET, OperandType::DEVICE_REF, OperandType::LOGIC_PROP
    )

    // ---------- DEVICE_REF LOGIC_PROP NUM_VALUE ----------

    // s 的第三个操作数是写入设备的「值」，非目标寄存器：数字字面量/常量别名/枚举常量均合法
    DEFINE_TERNARY_INSTRUCTION(
        s, S, S, Access::Write, OperandType::DEVICE_REF, OperandType::LOGIC_PROP, OperandType::NUM_VALUE
    )

    // ---------- DEVICE_HASH LOGIC_PROP NUM_VALUE ----------

    DEFINE_TERNARY_INSTRUCTION(
        sb, Sb, SB, Access::Write, OperandType::DEVICE_HASH, OperandType::LOGIC_PROP, OperandType::NUM_VALUE
    )

    // ---------- DEVICE_REF LOGIC_PROP JUMP_LINE ----------

    DEFINE_TERNARY_INSTRUCTION(
        bdnvl, Bdnvl, BDNVL, Access::Read, OperandType::DEVICE_REF, OperandType::LOGIC_PROP,
        OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bdnvs, Bdnvs, BDNVS, Access::Read, OperandType::DEVICE_REF, OperandType::LOGIC_PROP,
        OperandType::JUMP_LINE
    )

    // ---------- NUM_VALUE NUM_VALUE JUMP_LINE ----------

    DEFINE_TERNARY_INSTRUCTION(
        beq, Beq, BEQ, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        beqal, Beqal, BEQAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bne, Bne, BNE, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bneal, Bneal, BNEAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bge, Bge, BGE, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bgeal, Bgeal, BGEAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bgt, Bgt, BGT, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bgtal, Bgtal, BGTAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        ble, Ble, BLE, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bleal, Bleal, BLEAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        blt, Blt, BLT, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bltal, Bltal, BLTAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bapz, Bapz, BAPZ, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bapzal, Bapzal, BAPZAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bnaz, Bnaz, BNAZ, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        bnazal, Bnazal, BNAZAL, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE,
        OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        breq, Breq, BREQ, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brne, Brne, BRNE, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brge, Brge, BRGE, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brgt, Brgt, BRGT, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brle, Brle, BRLE, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brlt, Brlt, BRLT, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brapz, Brapz, BRAPZ, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
    )

    DEFINE_TERNARY_INSTRUCTION(
        brnaz, Brnaz, BRNAZ, Access::None, OperandType::NUM_VALUE, OperandType::NUM_VALUE, OperandType::JUMP_LINE
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

#endif  // IC10_COMPILER_CORE_INS_TERNARY_HPP
