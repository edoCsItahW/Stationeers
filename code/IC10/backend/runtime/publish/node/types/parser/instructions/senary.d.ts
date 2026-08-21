// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file senary.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:51
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {
    ErrorNode,
    RegisterOrIdentifierNode,
    OperandNode,
    SlotIndexNode,
    LogicSlotTypeNode,
    BatchModeNode,
    OperandType
} from "../ast";
import {QuinaryInstruction} from "./quinary";


// 六元指令（6 个操作数）

export interface SenaryInstruction extends QuinaryInstruction {
    type6: OperandType;
}


/**
 * @summary 六元指令概述
 *
 * @desc 六元指令是包含六个操作数的指令。
 * 主要用于复杂的库存操作指令。
 *
 * @elseif en
 * @summary Senary Instructions Overview
 *
 * @desc Senary instructions contain six operands.
 * Mainly used for complex inventory operations.
 */

/**
 * @summary 含 RegisterOrIdentifier + Operand + Operand + SlotIndex + LogicSlotType + BatchMode 操作数的六元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二、三个为操作数，第四个为槽索引，第五个为逻辑槽类型，第六个为批处理模式。
 * @elseif en
 * @summary Base interface for senary instructions with RegisterOrIdentifier + Operand + Operand + SlotIndex + LogicSlotType + BatchMode operands
 *
 * @desc First operand is a register or identifier, second and third are operands, fourth is a slot index, fifth is a logic slot type, sixth is a batch mode.
 *
 * @public
 */
interface SenaryInstruction_RI_O_O_SI_LS_BM extends SenaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: OperandNode;
    operand3: OperandNode;
    operand4: SlotIndexNode;
    operand5: LogicSlotTypeNode;
    operand6: BatchModeNode;
}


/**
 * @summary LBNS（读取设备库存批量数值槽）指令节点
 *
 * @desc 从设备的库存槽中批量读取多个数值数据。
 * @elseif en
 * @summary LBNS (Load Batch Numerical Slot) instruction node
 *
 * @desc Batch reads multiple numerical data from a device's inventory slots.
 *
 * @example
 * ```typescript
 * // LBNS r0 tank1 0 Water Oxygen Exact
 * // 从 tank1 的槽批量读取多个数值数据
 * ```
 *
 * @public
 */
export interface LbnsInstructionNode extends SenaryInstruction_RI_O_O_SI_LS_BM {
    type: "lbnsInstruction";
}


/**
 * @summary 六元指令联合类型
 *
 * @desc 包含所有六元指令类型的联合，包括可能的错误节点。
 * @elseif en
 * @summary Senary instruction union type
 *
 * @desc Union of all senary instruction types, including possible error nodes.
 *
 * @public
 */
export type SenaryInstructionNode =
    | LbnsInstructionNode
    | ErrorNode;