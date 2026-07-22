// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file quaternary.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:48
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {
    ErrorNode,
    RegisterOrIdentifierNode,
    OperandNode,
    DeviceReferenceNode,
    SlotIndexNode,
    LogicSlotTypeNode,
    LogicTypeNode,
    BatchModeNode,
    OperandType
} from "../ast";
import { TernaryInstruction } from "./ternary";


// 四元指令（4 个操作数）

export interface QuaternaryInstruction extends TernaryInstruction {
    type4: OperandType;
}

/**
 * @summary 四元指令概述
 *
 * @desc 四元指令是包含四个操作数的指令。
 * 根据操作数类型可分为以下几类：
 * - **QuaternaryInstruction_RI_O_O_O**：寄存器/标识符 + 操作数 + 操作数 + 操作数
 * - **QuaternaryInstruction_DR_SI_LS_RI**：设备引用 + 槽索引 + 逻辑槽类型 + 寄存器/标识符
 * - **QuaternaryInstruction_RI_O_LT_BM**：寄存器/标识符 + 操作数 + 逻辑类型 + 批处理模式
 * - **QuaternaryInstruction_O_O_LT_RI**：操作数 + 操作数 + 逻辑类型 + 寄存器/标识符
 * - **QuaternaryInstruction_O_SI_LS_RI**：操作数 + 槽索引 + 逻辑槽类型 + 寄存器/标识符
 * - **QuaternaryInstruction_O_O_O_O**：操作数 + 操作数 + 操作数 + 操作数
 * - **QuaternaryInstruction_RI_DR_SI_LS**：寄存器/标识符 + 设备引用 + 槽索引 + 逻辑槽类型
 *
 * @elseif en
 * @summary Quaternary Instructions Overview
 *
 * @desc Quaternary instructions contain four operands.
 * They are categorized by operand types:
 * - **QuaternaryInstruction_RI_O_O_O**: Register/identifier + operand + operand + operand
 * - **QuaternaryInstruction_DR_SI_LS_RI**: Device reference + slot index + logic slot type + register/identifier
 * - **QuaternaryInstruction_RI_O_LT_BM**: Register/identifier + operand + logic type + batch mode
 * - **QuaternaryInstruction_O_O_LT_RI**: Operand + operand + logic type + register/identifier
 * - **QuaternaryInstruction_O_SI_LS_RI**: Operand + slot index + logic slot type + register/identifier
 * - **QuaternaryInstruction_O_O_O_O**: Operand + operand + operand + operand
 * - **QuaternaryInstruction_RI_DR_SI_LS**: Register/identifier + device reference + slot index + logic slot type
 */

/**
 * @summary 含 RegisterOrIdentifier + Operand + Operand + Operand 操作数的四元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，后三个为通用操作数。
 * @elseif en
 * @summary Base interface for quaternary instructions with RegisterOrIdentifier + Operand + Operand + Operand operands
 *
 * @desc First operand is a register or identifier, last three are general operands.
 *
 * @public
 */
interface QuaternaryInstruction_RI_O_O_O extends QuaternaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: OperandNode;
    operand3: OperandNode;
    operand4: OperandNode;
}


/**
 * @summary LERP（线性插值）指令节点
 *
 * @desc 计算两个值之间的线性插值。
 * 结果 = start + (end - start) * t
 * @elseif en
 * @summary LERP (Linear Interpolation) instruction node
 *
 * @desc Computes linear interpolation between two values.
 * Result = start + (end - start) * t
 *
 * @example
 * ```typescript
 * // LERP r0 r1 r2 r3
 * // 计算从 r1 到 r2 的线性插值，t = r3
 * ```
 *
 * @public
 */
export interface LerpInstructionNode extends QuaternaryInstruction_RI_O_O_O {
    type: "lerpInstruction";
}


/**
 * @summary EXT（提取位）指令节点
 *
 * @desc 从源操作数中提取指定的位范围。
 * @elseif en
 * @summary EXT (Extract Bits) instruction node
 *
 * @desc Extracts a specified range of bits from the source operand.
 *
 * @example
 * ```typescript
 * // EXT r0 r1 r2 r3
 * // 从 r1 中提取位，结果存入 r0
 * ```
 *
 * @public
 */
export interface ExtInstructionNode extends QuaternaryInstruction_RI_O_O_O {
    type: "extInstruction";
}


/**
 * @summary INS（插入位）指令节点
 *
 * @desc 将位插入到目标操作数中的指定位置。
 * @elseif en
 * @summary INS (Insert Bits) instruction node
 *
 * @desc Inserts bits into a specified position in the target operand.
 *
 * @example
 * ```typescript
 * // INS r0 r1 r2 r3
 * // 将 r1 的位插入到 r0 的指定位置
 * ```
 *
 * @public
 */
export interface InsInstructionNode extends QuaternaryInstruction_RI_O_O_O {
    type: "insInstruction";
}


/**
 * @summary SAP（设置所有属性）指令节点
 *
 * @desc 设置设备的所有属性值。
 * @elseif en
 * @summary SAP (Set All Properties) instruction node
 *
 * @desc Sets all property values of a device.
 *
 * @example
 * ```typescript
 * // SAP r0 r1 r2 r3
 * // 设置设备的多个属性值
 * ```
 *
 * @public
 */
export interface SapInstructionNode extends QuaternaryInstruction_RI_O_O_O {
    type: "sapInstruction";
}


/**
 * @summary SNA（设置数值所有属性）指令节点
 *
 * @desc 设置设备的数值类型所有属性。
 * @elseif en
 * @summary SNA (Set Numerical All) instruction node
 *
 * @desc Sets all numerical properties of a device.
 *
 * @example
 * ```typescript
 * // SNA r0 r1 r2 r3
 * // 设置设备的数值类型属性
 * ```
 *
 * @public
 */
export interface SnaInstructionNode extends QuaternaryInstruction_RI_O_O_O {
    type: "snaInstruction";
}


/**
 * @summary SELECT（选择）指令节点
 *
 * @desc 根据条件从两个值中选择一个。
 * 如果第一个操作数非零，返回第二个操作数，否则返回第三个操作数。
 * @elseif en
 * @summary SELECT instruction node
 *
 * @desc Selects one of two values based on a condition.
 * If the first operand is non-zero, returns the second operand, otherwise returns the third operand.
 *
 * @example
 * ```typescript
 * // SELECT r0 r1 r2 r3
 * // 如果 r1 非零则 r0 = r2，否则 r0 = r3
 * ```
 *
 * @public
 */
export interface SelectInstructionNode extends QuaternaryInstruction_RI_O_O_O {
    type: "selectInstruction";
}


/**
 * @summary 含 DeviceReference + SlotIndex + LogicSlotType + RegisterOrIdentifier 操作数的四元指令基类
 *
 * @desc 第一个操作数为设备引用，第二个为槽索引，第三个为逻辑槽类型，第四个为寄存器或标识符。
 * @elseif en
 * @summary Base interface for quaternary instructions with DeviceReference + SlotIndex + LogicSlotType + RegisterOrIdentifier operands
 *
 * @desc First operand is a device reference, second is a slot index, third is a logic slot type, fourth is a register or identifier.
 *
 * @public
 */
interface QuaternaryInstruction_DR_SI_LS_RI extends QuaternaryInstruction {
    operand1: DeviceReferenceNode;
    operand2: SlotIndexNode;
    operand3: LogicSlotTypeNode;
    operand4: RegisterOrIdentifierNode;
}


/**
 * @summary SS（设置设备槽）指令节点
 *
 * @desc 将值设置到设备的指定槽中。
 * @elseif en
 * @summary SS (Set Slot) instruction node
 *
 * @desc Sets a value to a specified slot of a device.
 *
 * @example
 * ```typescript
 * // SS console1 0 Color r0
 * // 将 r0 的值设置到 console1 的 0 号槽的 Color 属性
 * ```
 *
 * @public
 */
export interface SsInstructionNode extends QuaternaryInstruction_DR_SI_LS_RI {
    type: "ssInstruction";
}


/**
 * @summary 含 RegisterOrIdentifier + Operand + LogicType + BatchMode 操作数的四元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为操作数，第三个为逻辑类型，第四个为批处理模式。
 * @elseif en
 * @summary Base interface for quaternary instructions with RegisterOrIdentifier + Operand + LogicType + BatchMode operands
 *
 * @desc First operand is a register or identifier, second is an operand, third is a logic type, fourth is a batch mode.
 *
 * @public
 */
interface QuaternaryInstruction_RI_O_LT_BM extends QuaternaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: OperandNode;
    operand3: LogicTypeNode;
    operand4: BatchModeNode;
}


/**
 * @summary LB（读取设备库存批量）指令节点
 *
 * @desc 从设备的库存中批量读取多个物品的数据。
 * @elseif en
 * @summary LB (Load Batch) instruction node
 *
 * @desc Batch reads data of multiple items from a device's inventory.
 *
 * @example
 * ```typescript
 * // LB r0 tank1 Mode Exact
 * // 从 tank1 批量读取数据
 * ```
 *
 * @public
 */
export interface LbInstructionNode extends QuaternaryInstruction_RI_O_LT_BM {
    type: "lbInstruction";
}


/**
 * @summary 含 Operand + Operand + LogicType + RegisterOrIdentifier 操作数的四元指令基类
 *
 * @desc 前两个操作数为通用操作数，第三个为逻辑类型，第四个为寄存器或标识符。
 * @elseif en
 * @summary Base interface for quaternary instructions with Operand + Operand + LogicType + RegisterOrIdentifier operands
 *
 * @desc First two operands are general operands, third is a logic type, fourth is a register or identifier.
 *
 * @public
 */
interface QuaternaryInstruction_O_O_LT_RI extends QuaternaryInstruction {
    operand1: OperandNode;
    operand2: OperandNode;
    operand3: LogicTypeNode;
    operand4: RegisterOrIdentifierNode;
}


/**
 * @summary SBN（设置设备库存批量数值）指令节点
 *
 * @desc 从设备库存批量读取数值数据。
 * @elseif en
 * @summary SBN (Set Batch Numerical) instruction node
 *
 * @desc Batch reads numerical data from a device's inventory.
 *
 * @example
 * ```typescript
 * // SBN r0 r1 Mode r2
 * // 从设备库存批量读取数值数据到 r2
 * ```
 *
 * @public
 */
export interface SbnInstructionNode extends QuaternaryInstruction_O_O_LT_RI {
    type: "sbnInstruction";
}


/**
 * @summary 含 Operand + SlotIndex + LogicSlotType + RegisterOrIdentifier 操作数的四元指令基类
 *
 * @desc 第一个操作数为通用操作数，第二个为槽索引，第三个为逻辑槽类型，第四个为寄存器或标识符。
 * @elseif en
 * @summary Base interface for quaternary instructions with Operand + SlotIndex + LogicSlotType + RegisterOrIdentifier operands
 *
 * @desc First operand is a general operand, second is a slot index, third is a logic slot type, fourth is a register or identifier.
 *
 * @public
 */
interface QuaternaryInstruction_O_SI_LS_RI extends QuaternaryInstruction {
    operand1: OperandNode;
    operand2: SlotIndexNode;
    operand3: LogicSlotTypeNode;
    operand4: RegisterOrIdentifierNode;
}


/**
 * @summary SBS（设置设备槽批量）指令节点
 *
 * @desc 批量将值设置到设备的多个槽中。
 * @elseif en
 * @summary SBS (Set Batch Slot) instruction node
 *
 * @desc Batch sets values to multiple slots of a device.
 *
 * @example
 * ```typescript
 * // SBS tank1 0 Quantity r0
 * // 批量设置设备的槽值
 * ```
 *
 * @public
 */
export interface SbsInstructionNode extends QuaternaryInstruction_O_SI_LS_RI {
    type: "sbsInstruction";
}


/**
 * @summary 含 Operand + Operand + Operand + Operand 操作数的四元指令基类
 *
 * @desc 四个操作数都为通用操作数类型。
 * @elseif en
 * @summary Base interface for quaternary instructions with Operand + Operand + Operand + Operand operands
 *
 * @desc All four operands are general operand types.
 *
 * @public
 */
interface QuaternaryInstruction_O_O_O_O extends QuaternaryInstruction {
    operand1: OperandNode;
    operand2: OperandNode;
    operand3: OperandNode;
    operand4: OperandNode;
}


/**
 * @summary BAP（Branches on All Predicate）指令节点
 *
 * @desc 如果所有谓词条件都满足，则跳转到目标地址。
 * @elseif en
 * @summary BAP (Branches on All Predicate) instruction node
 *
 * @desc Jumps to the target address if all predicate conditions are satisfied.
 *
 * @example
 * ```typescript
 * // BAP loop r0 r1 r2
 * // 如果所有条件满足，跳转到 loop
 * ```
 *
 * @public
 */
export interface BapInstructionNode extends QuaternaryInstruction_O_O_O_O {
    type: "bapInstruction";
}


/**
 * @summary BAPAL（Branches on All Predicate And Link）指令节点
 *
 * @desc 如果所有谓词条件都满足，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BAPAL (Branches on All Predicate And Link) instruction node
 *
 * @desc Jumps to the target address if all predicate conditions are satisfied, and saves the return address.
 *
 * @example
 * ```typescript
 * // BAPAL loop r0 r1 r2
 * // 如果所有条件满足，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BapalInstructionNode extends QuaternaryInstruction_O_O_O_O {
    type: "bapalInstruction";
}


/**
 * @summary BNA（Branches on No predicate true）指令节点
 *
 * @desc 如果所有谓词条件都不满足，则跳转到目标地址。
 * @elseif en
 * @summary BNA (Branches on No predicate true) instruction node
 *
 * @desc Jumps to the target address if no predicate condition is satisfied.
 *
 * @example
 * ```typescript
 * // BNA loop r0 r1 r2
 * // 如果所有条件都不满足，跳转到 loop
 * ```
 *
 * @public
 */
export interface BnaInstructionNode extends QuaternaryInstruction_O_O_O_O {
    type: "bnaInstruction";
}


/**
 * @summary BNAAL（Branches on No predicate true And Link）指令节点
 *
 * @desc 如果所有谓词条件都不满足，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BNAAL (Branches on No predicate true And Link) instruction node
 *
 * @desc Jumps to the target address if no predicate condition is satisfied, and saves the return address.
 *
 * @example
 * ```typescript
 * // BNAAL loop r0 r1 r2
 * // 如果所有条件都不满足，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BnaalInstructionNode extends QuaternaryInstruction_O_O_O_O {
    type: "bnaalInstruction";
}


/**
 * @summary BRAP（Branches Register on All Predicate）指令节点
 *
 * @desc 如果所有谓词条件都满足，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRAP (Branches Register on All Predicate) instruction node
 *
 * @desc Jumps to the address stored in the register if all predicate conditions are satisfied.
 *
 * @example
 * ```typescript
 * // BRAP r0 r1 r2 r3
 * // 如果所有条件满足，跳转到 r0 存储的地址
 * ```
 *
 * @public
 */
export interface BrapInstructionNode extends QuaternaryInstruction_O_O_O_O {
    type: "brapInstruction";
}


/**
 * @summary BRNA（Branches Register on No predicate true）指令节点
 *
 * @desc 如果所有谓词条件都不满足，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRNA (Branches Register on No predicate true) instruction node
 *
 * @desc Jumps to the address stored in the register if no predicate condition is satisfied.
 *
 * @example
 * ```typescript
 * // BRNA r0 r1 r2 r3
 * // 如果所有条件都不满足，跳转到 r0 存储的地址
 * ```
 *
 * @public
 */
export interface BrnaInstructionNode extends QuaternaryInstruction_O_O_O_O {
    type: "brnaInstruction";
}


/**
 * @summary 含 RegisterOrIdentifier + DeviceReference + SlotIndex + LogicSlotType 操作数的四元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为槽索引，第四个为逻辑槽类型。
 * @elseif en
 * @summary Base interface for quaternary instructions with RegisterOrIdentifier + DeviceReference + SlotIndex + LogicSlotType operands
 *
 * @desc First operand is a register or identifier, second is a device reference, third is a slot index, fourth is a logic slot type.
 *
 * @public
 */
interface QuaternaryInstruction_RI_DR_SI_LS extends QuaternaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: DeviceReferenceNode;
    operand3: SlotIndexNode;
    operand4: LogicSlotTypeNode;
}


/**
 * @summary LS（读取设备槽）指令节点
 *
 * @desc 从设备的指定槽中读取数据到寄存器。
 * @elseif en
 * @summary LS (Load Slot) instruction node
 *
 * @desc Reads data from a specified slot of a device into the register.
 *
 * @example
 * ```typescript
 * // LS r0 console1 0 Color
 * // 从 console1 的 0 号槽读取 Color 数据到 r0
 * ```
 *
 * @public
 */
export interface LsInstructionNode extends QuaternaryInstruction_RI_DR_SI_LS {
    type: "lsInstruction";
}


/**
 * @summary 四元指令联合类型
 *
 * @desc 包含所有四元指令类型的联合，包括可能的错误节点。
 * @elseif en
 * @summary Quaternary instruction union type
 *
 * @desc Union of all quaternary instruction types, including possible error nodes.
 *
 * @public
 */
export type QuaternaryInstructionNode =
    | LerpInstructionNode
    | ExtInstructionNode
    | InsInstructionNode
    | SapInstructionNode
    | SnaInstructionNode
    | SelectInstructionNode
    | SsInstructionNode
    | LbInstructionNode
    | SbnInstructionNode
    | SbsInstructionNode
    | BapInstructionNode
    | BapalInstructionNode
    | BnaInstructionNode
    | BnaalInstructionNode
    | BrapInstructionNode
    | BrnaInstructionNode
    | LsInstructionNode
    | ErrorNode;
