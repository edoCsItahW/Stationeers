// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file quinary.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:50
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {
    ErrorNode,
    RegisterOrIdentifierNode,
    OperandNode,
    LogicTypeNode,
    BatchModeNode,
    SlotIndexNode,
    LogicSlotTypeNode, 
    OperandType
} from "../ast";
import {TernaryInstruction} from "./ternary";


// 五元指令（5 个操作数）

export interface QuinaryInstruction extends TernaryInstruction {
    type5: OperandType;
}

/**
 * @summary 五元指令概述
 *
 * @desc 五元指令是包含五个操作数的指令。
 * 根据操作数类型可分为以下几类：
 * - **QuinaryInstruction_RI_O_O_LT_BM**：寄存器/标识符 + 操作数 + 操作数 + 逻辑类型 + 批处理模式
 * - **QuinaryInstruction_RI_O_SI_LS_BM**：寄存器/标识符 + 操作数 + 槽索引 + 逻辑槽类型 + 批处理模式
 *
 * @elseif en
 * @summary Quinary Instructions Overview
 *
 * @desc Quinary instructions contain five operands.
 * They are categorized by operand types:
 * - **QuinaryInstruction_RI_O_O_LT_BM**: Register/identifier + operand + operand + logic type + batch mode
 * - **QuinaryInstruction_RI_O_SI_LS_BM**: Register/identifier + operand + slot index + logic slot type + batch mode
 */

/**
 * @summary 含 RegisterOrIdentifier + Operand + Operand + LogicType + BatchMode 操作数的五元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二、三个为操作数，第四个为逻辑类型，第五个为批处理模式。
 * @elseif en
 * @summary Base interface for quinary instructions with RegisterOrIdentifier + Operand + Operand + LogicType + BatchMode operands
 *
 * @desc First operand is a register or identifier, second and third are operands, fourth is a logic type, fifth is a batch mode.
 *
 * @public
 */
interface QuinaryInstruction_RI_O_O_LT_BM extends QuinaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: OperandNode;
    operand3: OperandNode;
    operand4: LogicTypeNode;
    operand5: BatchModeNode;
}


/**
 * @summary LBN（读取设备库存批量数值）指令节点
 *
 * @desc 从设备的库存中批量读取多个数值数据。
 * @elseif en
 * @summary LBN (Load Batch Numerical) instruction node
 *
 * @desc Batch reads multiple numerical data from a device's inventory.
 *
 * @example
 * ```typescript
 * // LBN r0 tank1 Water Oxygen Exact
 * // 从 tank1 批量读取 Water 和 Oxygen 的数值数据
 * ```
 *
 * @public
 */
export interface LbnInstructionNode extends QuinaryInstruction_RI_O_O_LT_BM {
    type: "lbnInstruction";
}


/**
 * @summary 含 RegisterOrIdentifier + Operand + SlotIndex + LogicSlotType + BatchMode 操作数的五元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为操作数，第三个为槽索引，第四个为逻辑槽类型，第五个为批处理模式。
 * @elseif en
 * @summary Base interface for quinary instructions with RegisterOrIdentifier + Operand + SlotIndex + LogicSlotType + BatchMode operands
 *
 * @desc First operand is a register or identifier, second is an operand, third is a slot index, fourth is a logic slot type, fifth is a batch mode.
 *
 * @public
 */
interface QuinaryInstruction_RI_O_SI_LS_BM extends QuinaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: OperandNode;
    operand3: SlotIndexNode;
    operand4: LogicSlotTypeNode;
    operand5: BatchModeNode;
}


/**
 * @summary LBS（读取设备库存批量槽）指令节点
 *
 * @desc 从设备的库存槽中批量读取数据。
 * @elseif en
 * @summary LBS (Load Batch Slot) instruction node
 *
 * @desc Batch reads data from a device's inventory slots.
 *
 * @example
 * ```typescript
 * // LBS r0 tank1 0 Quantity Exact
 * // 从 tank1 的槽批量读取数据
 * ```
 *
 * @public
 */
export interface LbsInstructionNode extends QuinaryInstruction_RI_O_SI_LS_BM {
    type: "lbsInstruction";
}


/**
 * @summary 五元指令联合类型
 *
 * @desc 包含所有五元指令类型的联合，包括可能的错误节点。
 * @elseif en
 * @summary Quinary instruction union type
 *
 * @desc Union of all quinary instruction types, including possible error nodes.
 *
 * @public
 */
export type QuinaryInstructionNode =
    | LbnInstructionNode
    | LbsInstructionNode
    | ErrorNode;
