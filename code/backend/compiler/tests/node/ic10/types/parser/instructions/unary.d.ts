// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file unary.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:41
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {
    ErrorNode,
    OperandNode,
    RegisterOrIdentifierNode,
    DeviceReferenceNode,
    InstructionNode,
    OperandType
} from "../ast";


// 一元指令（1 个操作数）

export interface UnaryInstruction extends InstructionNode {
    type1: OperandType;
}

/**
 * @summary 一元指令概述
 *
 * @desc 一元指令是包含单个操作数的指令。
 * 根据操作数类型可分为以下几类：
 * - **UnaryInstruction_RI**：寄存器或标识符作为操作数
 * - **UnaryInstruction_DR**：设备引用作为操作数
 * - **UnaryInstruction_O**：通用操作数
 *
 * @elseif en
 * @summary Unary Instructions Overview
 *
 * @desc Unary instructions contain a single operand.
 * They are categorized by operand type:
 * - **UnaryInstruction_RI**: Register or identifier as operand
 * - **UnaryInstruction_DR**: Device reference as operand
 * - **UnaryInstruction_O**: General operand
 */

/**
 * @summary 含单个 RegisterOrIdentifier 操作数的一元指令基类
 *
 * @desc 操作数为寄存器或标识符节点。
 * @elseif en
 * @summary Base interface for unary instructions with single RegisterOrIdentifier operand
 *
 * @desc Operand is a register or identifier node.
 *
 * @public
 */
interface UnaryInstruction_RI extends UnaryInstruction {
    operand1: RegisterOrIdentifierNode;
}


/**
 * @summary SNANZ（Set if Not-A-Number Non-Zero）指令节点
 *
 * @desc 如果操作数不是数字且非零，则将目标寄存器设置为 1，否则设置为 0。
 * 用于检查特殊数值状态。
 * @elseif en
 * @summary SNANZ (Set if Not-A-Number Non-Zero) instruction node
 *
 * @desc Sets the target register to 1 if the operand is not a number and is non-zero, otherwise sets it to 0.
 * Used to check special numeric status.
 *
 * @example
 * ```typescript
 * // SNANZ r0 r1
 * // 如果 r1 是 NaN 且非零，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SnanzInstructionNode extends UnaryInstruction_RI {
    type: "snanzInstruction";
}


/**
 * @summary PEEK（读取内存）指令节点
 *
 * @desc 从指定地址读取数据到目标寄存器。
 * 用于读取堆栈或内存中的值。
 * @elseif en
 * @summary PEEK instruction node
 *
 * @desc Reads data from a specified address into the target register.
 * Used to read values from stack or memory.
 *
 * @example
 * ```typescript
 * // PEEK r0 r1
 * // 从地址 r1 读取数据到 r0
 * ```
 *
 * @public
 */
export interface PeekInstructionNode extends UnaryInstruction_RI {
    type: "peekInstruction";
}


/**
 * @summary RAND（随机数）指令节点
 *
 * @desc 生成一个随机数并存入目标寄存器。
 * 随机数范围通常为 0 到 1 之间的浮点数。
 * @elseif en
 * @summary RAND (Random) instruction node
 *
 * @desc Generates a random number and stores it in the target register.
 * The random number is typically a floating-point number between 0 and 1.
 *
 * @example
 * ```typescript
 * // RAND r0
 * // 生成随机数并存入 r0
 * ```
 *
 * @public
 */
export interface RandInstructionNode extends UnaryInstruction_RI {
    type: "randInstruction";
}


/**
 * @summary SEQZ（Set if EQual Zero）指令节点
 *
 * @desc 如果操作数等于零，则将目标寄存器设置为 1，否则设置为 0。
 * @elseif en
 * @summary SEQZ (Set if EQual Zero) instruction node
 *
 * @desc Sets the target register to 1 if the operand equals zero, otherwise sets it to 0.
 *
 * @example
 * ```typescript
 * // SEQZ r0 r1
 * // 如果 r1 == 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SeqzInstructionNode extends UnaryInstruction_RI {
    type: "seqzInstruction";
}


/**
 * @summary SNEZ（Set if Not Equal Zero）指令节点
 *
 * @desc 如果操作数不等于零，则将目标寄存器设置为 1，否则设置为 0。
 * @elseif en
 * @summary SNEZ (Set if Not Equal Zero) instruction node
 *
 * @desc Sets the target register to 1 if the operand does not equal zero, otherwise sets it to 0.
 *
 * @example
 * ```typescript
 * // SNEZ r0 r1
 * // 如果 r1 != 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SnezInstructionNode extends UnaryInstruction_RI {
    type: "snezInstruction";
}


/**
 * @summary SGEZ（Set if Greater than or Equal Zero）指令节点
 *
 * @desc 如果操作数大于等于零，则将目标寄存器设置为 1，否则设置为 0。
 * @elseif en
 * @summary SGEZ (Set if Greater than or Equal Zero) instruction node
 *
 * @desc Sets the target register to 1 if the operand is greater than or equal to zero, otherwise sets it to 0.
 *
 * @example
 * ```typescript
 * // SGEZ r0 r1
 * // 如果 r1 >= 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SgezInstructionNode extends UnaryInstruction_RI {
    type: "sgezInstruction";
}


/**
 * @summary SGTZ（Set if Greater Than Zero）指令节点
 *
 * @desc 如果操作数大于零，则将目标寄存器设置为 1，否则设置为 0。
 * @elseif en
 * @summary SGTZ (Set if Greater Than Zero) instruction node
 *
 * @desc Sets the target register to 1 if the operand is greater than zero, otherwise sets it to 0.
 *
 * @example
 * ```typescript
 * // SGTZ r0 r1
 * // 如果 r1 > 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SgtzInstructionNode extends UnaryInstruction_RI {
    type: "sgtzInstruction";
}


/**
 * @summary SLEZ（Set if Less than or Equal Zero）指令节点
 *
 * @desc 如果操作数小于等于零，则将目标寄存器设置为 1，否则设置为 0。
 * @elseif en
 * @summary SLEZ (Set if Less than or Equal Zero) instruction node
 *
 * @desc Sets the target register to 1 if the operand is less than or equal to zero, otherwise sets it to 0.
 *
 * @example
 * ```typescript
 * // SLEZ r0 r1
 * // 如果 r1 <= 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SlezInstructionNode extends UnaryInstruction_RI {
    type: "slezInstruction";
}


/**
 * @summary SLTZ（Set if Less Than Zero）指令节点
 *
 * @desc 如果操作数小于零，则将目标寄存器设置为 1，否则设置为 0。
 * @elseif en
 * @summary SLTZ (Set if Less Than Zero) instruction node
 *
 * @desc Sets the target register to 1 if the operand is less than zero, otherwise sets it to 0.
 *
 * @example
 * ```typescript
 * // SLTZ r0 r1
 * // 如果 r1 < 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SltzInstructionNode extends UnaryInstruction_RI {
    type: "sltzInstruction";
}


/**
 * @summary SNAN（Set if Not-A-Number）指令节点
 *
 * @desc 如果操作数是 NaN（非数字），则将目标寄存器设置为 1，否则设置为 0。
 * @elseif en
 * @summary SNAN (Set if Not-A-Number) instruction node
 *
 * @desc Sets the target register to 1 if the operand is NaN (Not a Number), otherwise sets it to 0.
 *
 * @example
 * ```typescript
 * // SNAN r0 r1
 * // 如果 r1 是 NaN，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SnanInstructionNode extends UnaryInstruction_RI {
    type: "snanInstruction";
}


/**
 * @summary POP（弹出堆栈）指令节点
 *
 * @desc 从堆栈中弹出一个值并存入目标寄存器。
 * 用于恢复之前保存的寄存器值。
 * @elseif en
 * @summary POP instruction node
 *
 * @desc Pops a value from the stack and stores it in the target register.
 * Used to restore previously saved register values.
 *
 * @example
 * ```typescript
 * // POP r0
 * // 弹出堆栈顶部值到 r0
 * ```
 *
 * @public
 */
export interface PopInstructionNode extends UnaryInstruction_RI {
    type: "popInstruction";
}


/**
 * @summary 含单个 DeviceReference 操作数的一元指令基类
 *
 * @desc 操作数为设备引用节点。
 * @elseif en
 * @summary Base interface for unary instructions with single DeviceReference operand
 *
 * @desc Operand is a device reference node.
 *
 * @public
 */
interface UnaryInstruction_DR extends UnaryInstruction {
    operand1: DeviceReferenceNode;
}


/**
 * @summary CLR（Clear Device）指令节点
 *
 * @desc 清除指定设备的所有数据或状态。
 * @elseif en
 * @summary CLR (Clear Device) instruction node
 *
 * @desc Clears all data or state of the specified device.
 *
 * @example
 * ```typescript
 * // CLR console1
 * // 清除 console1 设备的数据
 * ```
 *
 * @public
 */
export interface ClrInstructionNode extends UnaryInstruction_DR {
    type: "clrInstruction";
}


/**
 * @summary 含单个 Operand 操作数的一元指令基类
 *
 * @desc 操作数为通用操作数类型。
 * @elseif en
 * @summary Base interface for unary instructions with single Operand operand
 *
 * @desc Operand is a general operand type.
 *
 * @public
 */
interface UnaryInstruction_O extends UnaryInstruction {
    operand1: OperandNode;
}


/**
 * @summary SLEEP（休眠）指令节点
 *
 * @desc 让 IC 芯片进入休眠状态一段时间。
 * 休眠期间芯片不执行任何操作，用于节流或等待外部事件。
 * @elseif en
 * @summary SLEEP instruction node
 *
 * @desc Puts the IC chip into sleep mode for a specified duration.
 * The chip does not execute any operations during sleep, used for throttling or waiting for external events.
 *
 * @example
 * ```typescript
 * // SLEEP 10
 * // 休眠 10 个时间单位
 * ```
 *
 * @public
 */
export interface SleepInstructionNode extends UnaryInstruction_O {
    type: "sleepInstruction";
}


/**
 * @summary PUSH（压入堆栈）指令节点
 *
 * @desc 将操作数的值压入堆栈。
 * 用于保存寄存器值以便后续恢复。
 * @elseif en
 * @summary PUSH instruction node
 *
 * @desc Pushes the value of the operand onto the stack.
 * Used to save register values for later restoration.
 *
 * @example
 * ```typescript
 * // PUSH r0
 * // 将 r0 的值压入堆栈
 * ```
 *
 * @public
 */
export interface PushInstructionNode extends UnaryInstruction_O {
    type: "pushInstruction";
}


/**
 * @summary JAL（Jump And Link）指令节点
 *
 * @desc 跳转到指定地址并将返回地址保存到链接寄存器（通常是 r15）。
 * 用于函数调用。
 * @elseif en
 * @summary JAL (Jump And Link) instruction node
 *
 * @desc Jumps to the specified address and saves the return address to the link register (typically r15).
 * Used for function calls.
 *
 * @example
 * ```typescript
 * // JAL main
 * // 跳转到 main 标签，保存返回地址到 r15
 * ```
 *
 * @public
 */
export interface JalInstructionNode extends UnaryInstruction_O {
    type: "jalInstruction";
}


/**
 * @summary JR（Jump Register）指令节点
 *
 * @desc 跳转到寄存器中存储的地址。
 * 用于间接跳转，通常配合 JAL 使用。
 * @elseif en
 * @summary JR (Jump Register) instruction node
 *
 * @desc Jumps to the address stored in a register.
 * Used for indirect jumps, often used with JAL.
 *
 * @example
 * ```typescript
 * // JR r15
 * // 跳转到 r15 中存储的地址（返回）
 * ```
 *
 * @public
 */
export interface JrInstructionNode extends UnaryInstruction_O {
    type: "jrInstruction";
}


/**
 * @summary J（Jump）指令节点
 *
 * @desc 无条件跳转到指定地址或标签。
 * @elseif en
 * @summary J (Jump) instruction node
 *
 * @desc Unconditionally jumps to the specified address or label.
 *
 * @example
 * ```typescript
 * // J loop
 * // 跳转到 loop 标签
 * ```
 *
 * @public
 */
export interface JInstructionNode extends UnaryInstruction_O {
    type: "jInstruction";
}


/**
 * @summary 一元指令联合类型
 *
 * @desc 包含所有一元指令类型的联合，包括可能的错误节点。
 * @elseif en
 * @summary Unary instruction union type
 *
 * @desc Union of all unary instruction types, including possible error nodes.
 *
 * @public
 */
export type UnaryInstructionNode =
    | SnanzInstructionNode
    | PeekInstructionNode
    | RandInstructionNode
    | SeqzInstructionNode
    | SnezInstructionNode
    | SgezInstructionNode
    | SgtzInstructionNode
    | SlezInstructionNode
    | SltzInstructionNode
    | SnanInstructionNode
    | PopInstructionNode
    | ClrInstructionNode
    | SleepInstructionNode
    | PushInstructionNode
    | JalInstructionNode
    | JrInstructionNode
    | JInstructionNode
    | ErrorNode;