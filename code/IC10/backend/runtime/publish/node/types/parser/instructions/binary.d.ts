// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file binary.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:43
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {ErrorNode, RegisterOrIdentifierNode, OperandNode, DeviceReferenceNode, OperandType} from "../ast";
import {UnaryInstruction} from "./unary";


// 二元指令（2 个操作数）

export interface BinaryInstruction extends UnaryInstruction {
    type2: OperandType;
}


/**
 * @summary 二元指令概述
 *
 * @desc 二元指令是包含两个操作数的指令。
 * 根据操作数类型可分为以下几类：
 * - **BinaryInstruction_RI_O**：寄存器/标识符 + 操作数
 * - **BinaryInstruction_DR_O**：设备引用 + 操作数
 * - **BinaryInstruction_RI_DR**：寄存器/标识符 + 设备引用
 * - **BinaryInstruction_O_O**：操作数 + 操作数
 *
 * @elseif en
 * @summary Binary Instructions Overview
 *
 * @desc Binary instructions contain two operands.
 * They are categorized by operand types:
 * - **BinaryInstruction_RI_O**: Register/identifier + operand
 * - **BinaryInstruction_DR_O**: Device reference + operand
 * - **BinaryInstruction_RI_DR**: Register/identifier + device reference
 * - **BinaryInstruction_O_O**: Operand + operand
 */

/**
 * @summary 含 RegisterOrIdentifier + Operand 操作数的二元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为通用操作数。
 * @elseif en
 * @summary Base interface for binary instructions with RegisterOrIdentifier + Operand operands
 *
 * @desc First operand is a register or identifier, second is a general operand.
 *
 * @public
 */
interface BinaryInstruction_RI_O extends BinaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: OperandNode;
}


/**
 * @summary ABS（绝对值）指令节点
 *
 * @desc 计算操作数的绝对值并存入目标寄存器。
 * @elseif en
 * @summary ABS (Absolute Value) instruction node
 *
 * @desc Computes the absolute value of the operand and stores it in the target register.
 *
 * @example
 * ```typescript
 * // ABS r0 r1
 * // 计算 r1 的绝对值并存入 r0
 * ```
 *
 * @public
 */
export interface AbsInstructionNode extends BinaryInstruction_RI_O {
    type: "absInstruction";
}


/**
 * @summary ACOS（反余弦）指令节点
 *
 * @desc 计算操作数的反余弦值（弧度制）。
 * @elseif en
 * @summary ACOS (Arc Cosine) instruction node
 *
 * @desc Computes the arc cosine of the operand (in radians).
 *
 * @example
 * ```typescript
 * // ACOS r0 r1
 * // 计算 r1 的反余弦值并存入 r0
 * ```
 *
 * @public
 */
export interface AcosInstructionNode extends BinaryInstruction_RI_O {
    type: "acosInstruction";
}


/**
 * @summary ASIN（反正弦）指令节点
 *
 * @desc 计算操作数的反正弦值（弧度制）。
 * @elseif en
 * @summary ASIN (Arc Sine) instruction node
 *
 * @desc Computes the arc sine of the operand (in radians).
 *
 * @example
 * ```typescript
 * // ASIN r0 r1
 * // 计算 r1 的反正弦值并存入 r0
 * ```
 *
 * @public
 */
export interface AsinInstructionNode extends BinaryInstruction_RI_O {
    type: "asinInstruction";
}


/**
 * @summary ATAN（反正切）指令节点
 *
 * @desc 计算操作数的反正切值（弧度制）。
 * @elseif en
 * @summary ATAN (Arc Tangent) instruction node
 *
 * @desc Computes the arc tangent of the operand (in radians).
 *
 * @example
 * ```typescript
 * // ATAN r0 r1
 * // 计算 r1 的反正切值并存入 r0
 * ```
 *
 * @public
 */
export interface AtanInstructionNode extends BinaryInstruction_RI_O {
    type: "atanInstruction";
}


/**
 * @summary CEIL（向上取整）指令节点
 *
 * @desc 计算大于或等于操作数的最小整数。
 * @elseif en
 * @summary CEIL (Ceiling) instruction node
 *
 * @desc Computes the smallest integer greater than or equal to the operand.
 *
 * @example
 * ```typescript
 * // CEIL r0 r1
 * // 计算 r1 向上取整的结果并存入 r0
 * ```
 *
 * @public
 */
export interface CeilInstructionNode extends BinaryInstruction_RI_O {
    type: "ceilInstruction";
}


/**
 * @summary COS（余弦）指令节点
 *
 * @desc 计算操作数的余弦值（弧度制）。
 * @elseif en
 * @summary COS (Cosine) instruction node
 *
 * @desc Computes the cosine of the operand (in radians).
 *
 * @example
 * ```typescript
 * // COS r0 r1
 * // 计算 r1 的余弦值并存入 r0
 * ```
 *
 * @public
 */
export interface CosInstructionNode extends BinaryInstruction_RI_O {
    type: "cosInstruction";
}


/**
 * @summary EXP（指数）指令节点
 *
 * @desc 计算 e 的操作数次幂。
 * @elseif en
 * @summary EXP (Exponential) instruction node
 *
 * @desc Computes e raised to the power of the operand.
 *
 * @example
 * ```typescript
 * // EXP r0 r1
 * // 计算 e^r1 并存入 r0
 * ```
 *
 * @public
 */
export interface ExpInstructionNode extends BinaryInstruction_RI_O {
    type: "expInstruction";
}


/**
 * @summary FLOOR（向下取整）指令节点
 *
 * @desc 计算小于或等于操作数的最大整数。
 * @elseif en
 * @summary FLOOR instruction node
 *
 * @desc Computes the largest integer less than or equal to the operand.
 *
 * @example
 * ```typescript
 * // FLOOR r0 r1
 * // 计算 r1 向下取整的结果并存入 r0
 * ```
 *
 * @public
 */
export interface FloorInstructionNode extends BinaryInstruction_RI_O {
    type: "floorInstruction";
}


/**
 * @summary LOG（自然对数）指令节点
 *
 * @desc 计算操作数的自然对数（以 e 为底）。
 * @elseif en
 * @summary LOG (Natural Logarithm) instruction node
 *
 * @desc Computes the natural logarithm of the operand (base e).
 *
 * @example
 * ```typescript
 * // LOG r0 r1
 * // 计算 ln(r1) 并存入 r0
 * ```
 *
 * @public
 */
export interface LogInstructionNode extends BinaryInstruction_RI_O {
    type: "logInstruction";
}


/**
 * @summary ROUND（四舍五入）指令节点
 *
 * @desc 将操作数四舍五入到最接近的整数。
 * @elseif en
 * @summary ROUND instruction node
 *
 * @desc Rounds the operand to the nearest integer.
 *
 * @example
 * ```typescript
 * // ROUND r0 r1
 * // 计算 r1 四舍五入的结果并存入 r0
 * ```
 *
 * @public
 */
export interface RoundInstructionNode extends BinaryInstruction_RI_O {
    type: "roundInstruction";
}


/**
 * @summary SIN（正弦）指令节点
 *
 * @desc 计算操作数的正弦值（弧度制）。
 * @elseif en
 * @summary SIN (Sine) instruction node
 *
 * @desc Computes the sine of the operand (in radians).
 *
 * @example
 * ```typescript
 * // SIN r0 r1
 * // 计算 r1 的正弦值并存入 r0
 * ```
 *
 * @public
 */
export interface SinInstructionNode extends BinaryInstruction_RI_O {
    type: "sinInstruction";
}


/**
 * @summary SQRT（平方根）指令节点
 *
 * @desc 计算操作数的平方根。
 * @elseif en
 * @summary SQRT (Square Root) instruction node
 *
 * @desc Computes the square root of the operand.
 *
 * @example
 * ```typescript
 * // SQRT r0 r1
 * // 计算 sqrt(r1) 并存入 r0
 * ```
 *
 * @public
 */
export interface SqrtInstructionNode extends BinaryInstruction_RI_O {
    type: "sqrtInstruction";
}


/**
 * @summary TAN（正切）指令节点
 *
 * @desc 计算操作数的正切值（弧度制）。
 * @elseif en
 * @summary TAN (Tangent) instruction node
 *
 * @desc Computes the tangent of the operand (in radians).
 *
 * @example
 * ```typescript
 * // TAN r0 r1
 * // 计算 r1 的正切值并存入 r0
 * ```
 *
 * @public
 */
export interface TanInstructionNode extends BinaryInstruction_RI_O {
    type: "tanInstruction";
}


/**
 * @summary TRUNC（截断）指令节点
 *
 * @desc 截断操作数的小数部分，只保留整数部分。
 * @elseif en
 * @summary TRUNC (Truncate) instruction node
 *
 * @desc Truncates the decimal part of the operand, keeping only the integer part.
 *
 * @example
 * ```typescript
 * // TRUNC r0 r1
 * // 计算 truncate(r1) 并存入 r0
 * ```
 *
 * @public
 */
export interface TruncInstructionNode extends BinaryInstruction_RI_O {
    type: "truncInstruction";
}


/**
 * @summary NOT（按位取反）指令节点
 *
 * @desc 对操作数进行按位取反运算。
 * @elseif en
 * @summary NOT (Bitwise NOT) instruction node
 *
 * @desc Performs bitwise NOT operation on the operand.
 *
 * @example
 * ```typescript
 * // NOT r0 r1
 * // 计算 ~r1 并存入 r0
 * ```
 *
 * @public
 */
export interface NotInstructionNode extends BinaryInstruction_RI_O {
    type: "notInstruction";
}


/**
 * @summary MOVE（移动）指令节点
 *
 * @desc 将第二个操作数的值复制到第一个操作数（目标寄存器）。
 * @elseif en
 * @summary MOVE instruction node
 *
 * @desc Copies the value of the second operand to the first operand (target register).
 *
 * @example
 * ```typescript
 * // MOVE r0 r1
 * // 将 r1 的值复制到 r0
 * ```
 *
 * @public
 */
export interface MoveInstructionNode extends BinaryInstruction_RI_O {
    type: "moveInstruction";
}


/**
 * @summary SUB（减法）指令节点
 *
 * @desc 从第一个操作数中减去第二个操作数，结果存入目标寄存器。
 * @elseif en
 * @summary SUB (Subtraction) instruction node
 *
 * @desc Subtracts the second operand from the first operand, storing the result in the target register.
 *
 * @example
 * ```typescript
 * // SUB r0 r1 r2
 * // 计算 r1 - r2 并存入 r0
 * ```
 *
 * @public
 */
export interface SubInstructionNode extends BinaryInstruction_RI_O {
    type: "subInstruction";
}


/**
 * @summary 含 DeviceReference + Operand 操作数的二元指令基类
 *
 * @desc 第一个操作数为设备引用，第二个为通用操作数。
 * @elseif en
 * @summary Base interface for binary instructions with DeviceReference + Operand operands
 *
 * @desc First operand is a device reference, second is a general operand.
 *
 * @public
 */
interface BinaryInstruction_DR_O extends BinaryInstruction {
    operand1: DeviceReferenceNode;
    operand2: OperandNode;
}


/**
 * @summary BDNS（Branches on Device Not Stalled）指令节点
 *
 * @desc 如果指定设备未处于停滞状态，则跳转到目标地址。
 * @elseif en
 * @summary BDNS (Branches on Device Not Stalled) instruction node
 *
 * @desc Jumps to the target address if the specified device is not stalled.
 *
 * @example
 * ```typescript
 * // BDNS console1 loop
 * // 如果 console1 未停滞，跳转到 loop
 * ```
 *
 * @public
 */
export interface BdnsInstructionNode extends BinaryInstruction_DR_O {
    type: "bdnsInstruction";
}


/**
 * @summary BDNSAL（Branches on Device Not Stalled And Link）指令节点
 *
 * @desc 如果指定设备未处于停滞状态，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BDNSAL (Branches on Device Not Stalled And Link) instruction node
 *
 * @desc Jumps to the target address if the specified device is not stalled, and saves the return address.
 *
 * @example
 * ```typescript
 * // BDNSAL console1 loop
 * // 如果 console1 未停滞，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BdnsalInstructionNode extends BinaryInstruction_DR_O {
    type: "bdnsalInstruction";
}


/**
 * @summary BDSE（Branches on Device Stalled）指令节点
 *
 * @desc 如果指定设备处于停滞状态，则跳转到目标地址。
 * @elseif en
 * @summary BDSE (Branches on Device Stalled) instruction node
 *
 * @desc Jumps to the target address if the specified device is stalled.
 *
 * @example
 * ```typescript
 * // BDSE console1 loop
 * // 如果 console1 停滞，跳转到 loop
 * ```
 *
 * @public
 */
export interface BdseInstructionNode extends BinaryInstruction_DR_O {
    type: "bdseInstruction";
}


/**
 * @summary BDSEAL（Branches on Device Stalled And Link）指令节点
 *
 * @desc 如果指定设备处于停滞状态，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BDSEAL (Branches on Device Stalled And Link) instruction node
 *
 * @desc Jumps to the target address if the specified device is stalled, and saves the return address.
 *
 * @example
 * ```typescript
 * // BDSEAL console1 loop
 * // 如果 console1 停滞，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BdsealInstructionNode extends BinaryInstruction_DR_O {
    type: "bdsealInstruction";
}


/**
 * @summary BRDNS（Branches Register on Device Not Stalled）指令节点
 *
 * @desc 如果指定设备未处于停滞状态，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRDNS (Branches Register on Device Not Stalled) instruction node
 *
 * @desc Jumps to the address stored in the register if the specified device is not stalled.
 *
 * @example
 * ```typescript
 * // BRDNS r0 console1
 * // 如果 console1 未停滞，跳转到 r0 存储的地址
 * ```
 *
 * @public
 */
export interface BrdnsInstructionNode extends BinaryInstruction_DR_O {
    type: "brdnsInstruction";
}


/**
 * @summary BRDSE（Branches Register on Device Stalled）指令节点
 *
 * @desc 如果指定设备处于停滞状态，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRDSE (Branches Register on Device Stalled) instruction node
 *
 * @desc Jumps to the address stored in the register if the specified device is stalled.
 *
 * @example
 * ```typescript
 * // BRDSE r0 console1
 * // 如果 console1 停滞，跳转到 r0 存储的地址
 * ```
 *
 * @public
 */
export interface BrdseInstructionNode extends BinaryInstruction_DR_O {
    type: "brdseInstruction";
}


/**
 * @summary 含 RegisterOrIdentifier + DeviceReference 操作数的二元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为设备引用。
 * @elseif en
 * @summary Base interface for binary instructions with RegisterOrIdentifier + DeviceReference operands
 *
 * @desc First operand is a register or identifier, second is a device reference.
 *
 * @public
 */
interface BinaryInstruction_RI_DR extends BinaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: DeviceReferenceNode;
}


/**
 * @summary SDNS（Sets Device Not Stalled）指令节点
 *
 * @desc 将指定设备设置为非停滞状态。
 * @elseif en
 * @summary SDNS (Sets Device Not Stalled) instruction node
 *
 * @desc Sets the specified device to not stalled state.
 *
 * @example
 * ```typescript
 * // SDNS r0 console1
 * // 将 console1 设置为非停滞状态
 * ```
 *
 * @public
 */
export interface SdnsInstructionNode extends BinaryInstruction_RI_DR {
    type: "sdnsInstruction";
}


/**
 * @summary SDSE（Sets Device Stalled）指令节点
 *
 * @desc 将指定设备设置为停滞状态。
 * @elseif en
 * @summary SDSE (Sets Device Stalled) instruction node
 *
 * @desc Sets the specified device to stalled state.
 *
 * @example
 * ```typescript
 * // SDSE r0 console1
 * // 将 console1 设置为停滞状态
 * ```
 *
 * @public
 */
export interface SdseInstructionNode extends BinaryInstruction_RI_DR {
    type: "sdseInstruction";
}


/**
 * @summary 含 Operand + Operand 操作数的二元指令基类
 *
 * @desc 两个操作数都为通用操作数类型。
 * @elseif en
 * @summary Base interface for binary instructions with Operand + Operand operands
 *
 * @desc Both operands are general operand types.
 *
 * @public
 */
interface BinaryInstruction_O_O extends BinaryInstruction {
    operand1: OperandNode;
    operand2: OperandNode;
}


/**
 * @summary POKE（写入内存）指令节点
 *
 * @desc 将数据写入到指定的内存地址。
 * @elseif en
 * @summary POKE instruction node
 *
 * @desc Writes data to a specified memory address.
 *
 * @example
 * ```typescript
 * // POKE r0 r1
 * // 将 r0 的值写入地址 r1
 * ```
 *
 * @public
 */
export interface PokeInstructionNode extends BinaryInstruction_O_O {
    type: "pokeInstruction";
}


/**
 * @summary BEQZ（Branch if EQual Zero）指令节点
 *
 * @desc 如果操作数等于零，则跳转到目标地址。
 * @elseif en
 * @summary BEQZ (Branch if EQual Zero) instruction node
 *
 * @desc Jumps to the target address if the operand equals zero.
 *
 * @example
 * ```typescript
 * // BEQZ r0 loop
 * // 如果 r0 == 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BeqzInstructionNode extends BinaryInstruction_O_O {
    type: "beqzInstruction";
}


/**
 * @summary BEQZAL（Branch if EQual Zero And Link）指令节点
 *
 * @desc 如果操作数等于零，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BEQZAL (Branch if EQual Zero And Link) instruction node
 *
 * @desc Jumps to the target address if the operand equals zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BEQZAL r0 loop
 * // 如果 r0 == 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BeqzalInstructionNode extends BinaryInstruction_O_O {
    type: "beqzalInstruction";
}


/**
 * @summary BNEZ（Branch if Not Equal Zero）指令节点
 *
 * @desc 如果操作数不等于零，则跳转到目标地址。
 * @elseif en
 * @summary BNEZ (Branch if Not Equal Zero) instruction node
 *
 * @desc Jumps to the target address if the operand does not equal zero.
 *
 * @example
 * ```typescript
 * // BNEZ r0 loop
 * // 如果 r0 != 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BnezInstructionNode extends BinaryInstruction_O_O {
    type: "bnezInstruction";
}


/**
 * @summary BNEZAL（Branch if Not Equal Zero And Link）指令节点
 *
 * @desc 如果操作数不等于零，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BNEZAL (Branch if Not Equal Zero And Link) instruction node
 *
 * @desc Jumps to the target address if the operand does not equal zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BNEZAL r0 loop
 * // 如果 r0 != 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BnezalInstructionNode extends BinaryInstruction_O_O {
    type: "bnezalInstruction";
}


/**
 * @summary BGEZ（Branch if Greater than or Equal Zero）指令节点
 *
 * @desc 如果操作数大于等于零，则跳转到目标地址。
 * @elseif en
 * @summary BGEZ (Branch if Greater than or Equal Zero) instruction node
 *
 * @desc Jumps to the target address if the operand is greater than or equal to zero.
 *
 * @example
 * ```typescript
 * // BGEZ r0 loop
 * // 如果 r0 >= 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BgezInstructionNode extends BinaryInstruction_O_O {
    type: "bgezInstruction";
}


/**
 * @summary BGEZAL（Branch if Greater than or Equal Zero And Link）指令节点
 *
 * @desc 如果操作数大于等于零，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BGEZAL (Branch if Greater than or Equal Zero And Link) instruction node
 *
 * @desc Jumps to the target address if the operand is greater than or equal to zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BGEZAL r0 loop
 * // 如果 r0 >= 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BgezalInstructionNode extends BinaryInstruction_O_O {
    type: "bgezalInstruction";
}


/**
 * @summary BGTZ（Branch if Greater Than Zero）指令节点
 *
 * @desc 如果操作数大于零，则跳转到目标地址。
 * @elseif en
 * @summary BGTZ (Branch if Greater Than Zero) instruction node
 *
 * @desc Jumps to the target address if the operand is greater than zero.
 *
 * @example
 * ```typescript
 * // BGTZ r0 loop
 * // 如果 r0 > 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BgtzInstructionNode extends BinaryInstruction_O_O {
    type: "bgtzInstruction";
}


/**
 * @summary BGTZAL（Branch if Greater Than Zero And Link）指令节点
 *
 * @desc 如果操作数大于零，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BGTZAL (Branch if Greater Than Zero And Link) instruction node
 *
 * @desc Jumps to the target address if the operand is greater than zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BGTZAL r0 loop
 * // 如果 r0 > 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BgtzalInstructionNode extends BinaryInstruction_O_O {
    type: "bgtzalInstruction";
}


/**
 * @summary BLEZ（Branch if Less than or Equal Zero）指令节点
 *
 * @desc 如果操作数小于等于零，则跳转到目标地址。
 * @elseif en
 * @summary BLEZ (Branch if Less than or Equal Zero) instruction node
 *
 * @desc Jumps to the target address if the operand is less than or equal to zero.
 *
 * @example
 * ```typescript
 * // BLEZ r0 loop
 * // 如果 r0 <= 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BlezInstructionNode extends BinaryInstruction_O_O {
    type: "blezInstruction";
}


/**
 * @summary BLEZAL（Branch if Less than or Equal Zero And Link）指令节点
 *
 * @desc 如果操作数小于等于零，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BLEZAL (Branch if Less than or Equal Zero And Link) instruction node
 *
 * @desc Jumps to the target address if the operand is less than or equal to zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BLEZAL r0 loop
 * // 如果 r0 <= 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BlezalInstructionNode extends BinaryInstruction_O_O {
    type: "blezalInstruction";
}


/**
 * @summary BLTZ（Branch if Less Than Zero）指令节点
 *
 * @desc 如果操作数小于零，则跳转到目标地址。
 * @elseif en
 * @summary BLTZ (Branch if Less Than Zero) instruction node
 *
 * @desc Jumps to the target address if the operand is less than zero.
 *
 * @example
 * ```typescript
 * // BLTZ r0 loop
 * // 如果 r0 < 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BltzInstructionNode extends BinaryInstruction_O_O {
    type: "bltzInstruction";
}


/**
 * @summary BLTZAL（Branch if Less Than Zero And Link）指令节点
 *
 * @desc 如果操作数小于零，则跳转到目标地址并保存返回地址。
 * @elseif en
 * @summary BLTZAL (Branch if Less Than Zero And Link) instruction node
 *
 * @desc Jumps to the target address if the operand is less than zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BLTZAL r0 loop
 * // 如果 r0 < 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BltzalInstructionNode extends BinaryInstruction_O_O {
    type: "bltzalInstruction";
}


/**
 * @summary BNAN（Branch if Not-A-Number）指令节点
 *
 * @desc 如果操作数是 NaN，则跳转到目标地址。
 * @elseif en
 * @summary BNAN (Branch if Not-A-Number) instruction node
 *
 * @desc Jumps to the target address if the operand is NaN.
 *
 * @example
 * ```typescript
 * // BNAN r0 loop
 * // 如果 r0 是 NaN，跳转到 loop
 * ```
 *
 * @public
 */
export interface BnanInstructionNode extends BinaryInstruction_O_O {
    type: "bnanInstruction";
}


/**
 * @summary BNANZ（Branch if Not-A-Number Non-Zero）指令节点
 *
 * @desc 如果操作数是 NaN 且非零，则跳转到目标地址。
 * @elseif en
 * @summary BNANZ (Branch if Not-A-Number Non-Zero) instruction node
 *
 * @desc Jumps to the target address if the operand is NaN and non-zero.
 *
 * @example
 * ```typescript
 * // BNANZ r0 loop
 * // 如果 r0 是 NaN 且非零，跳转到 loop
 * ```
 *
 * @public
 */
export interface BnanzInstructionNode extends BinaryInstruction_O_O {
    type: "bnanzInstruction";
}


/**
 * @summary BREQZ（Branches Register if EQual Zero）指令节点
 *
 * @desc 如果操作数等于零，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BREQZ (Branches Register if EQual Zero) instruction node
 *
 * @desc Jumps to the address stored in the register if the operand equals zero.
 *
 * @example
 * ```typescript
 * // BREQZ r0 r1
 * // 如果 r0 == 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BreqzInstructionNode extends BinaryInstruction_O_O {
    type: "breqzInstruction";
}


/**
 * @summary BRNZ（Branches Register if Not Zero）指令节点
 *
 * @desc 如果操作数不等于零，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRNZ (Branches Register if Not Zero) instruction node
 *
 * @desc Jumps to the address stored in the register if the operand does not equal zero.
 *
 * @example
 * ```typescript
 * // BRNZ r0 r1
 * // 如果 r0 != 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrnzInstructionNode extends BinaryInstruction_O_O {
    type: "brnzInstruction";
}


/**
 * @summary BRGEZ（Branches Register if Greater than or Equal Zero）指令节点
 *
 * @desc 如果操作数大于等于零，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRGEZ (Branches Register if Greater than or Equal Zero) instruction node
 *
 * @desc Jumps to the address stored in the register if the operand is greater than or equal to zero.
 *
 * @example
 * ```typescript
 * // BRGEZ r0 r1
 * // 如果 r0 >= 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrgezInstructionNode extends BinaryInstruction_O_O {
    type: "brgezInstruction";
}


/**
 * @summary BRGTZ（Branches Register if Greater Than Zero）指令节点
 *
 * @desc 如果操作数大于零，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRGTZ (Branches Register if Greater Than Zero) instruction node
 *
 * @desc Jumps to the address stored in the register if the operand is greater than zero.
 *
 * @example
 * ```typescript
 * // BRGTZ r0 r1
 * // 如果 r0 > 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrgtzInstructionNode extends BinaryInstruction_O_O {
    type: "brgtzInstruction";
}


/**
 * @summary BRLEZ（Branches Register if Less than or Equal Zero）指令节点
 *
 * @desc 如果操作数小于等于零，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRLEZ (Branches Register if Less than or Equal Zero) instruction node
 *
 * @desc Jumps to the address stored in the register if the operand is less than or equal to zero.
 *
 * @example
 * ```typescript
 * // BRLEZ r0 r1
 * // 如果 r0 <= 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrlezInstructionNode extends BinaryInstruction_O_O {
    type: "brlezInstruction";
}


/**
 * @summary BRLTZ（Branches Register if Less Than Zero）指令节点
 *
 * @desc 如果操作数小于零，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRLTZ (Branches Register if Less Than Zero) instruction node
 *
 * @desc Jumps to the address stored in the register if the operand is less than zero.
 *
 * @example
 * ```typescript
 * // BRLTZ r0 r1
 * // 如果 r0 < 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrltzInstructionNode extends BinaryInstruction_O_O {
    type: "brltzInstruction";
}


/**
 * @summary BRNAN（Branches Register if Not-A-Number）指令节点
 *
 * @desc 如果操作数是 NaN，则跳转到寄存器中存储的地址。
 * @elseif en
 * @summary BRNAN (Branches Register if Not-A-Number) instruction node
 *
 * @desc Jumps to the address stored in the register if the operand is NaN.
 *
 * @example
 * ```typescript
 * // BRNAN r0 r1
 * // 如果 r0 是 NaN，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrnanInstructionNode extends BinaryInstruction_O_O {
    type: "brnanInstruction";
}


/**
 * @summary 二元指令联合类型
 *
 * @desc 包含所有二元指令类型的联合，包括可能的错误节点。
 * @elseif en
 * @summary Binary instruction union type
 *
 * @desc Union of all binary instruction types, including possible error nodes.
 *
 * @public
 */
export type BinaryInstructionNode =
    | AbsInstructionNode
    | AcosInstructionNode
    | AsinInstructionNode
    | AtanInstructionNode
    | CeilInstructionNode
    | CosInstructionNode
    | ExpInstructionNode
    | FloorInstructionNode
    | LogInstructionNode
    | RoundInstructionNode
    | SinInstructionNode
    | SqrtInstructionNode
    | TanInstructionNode
    | TruncInstructionNode
    | NotInstructionNode
    | MoveInstructionNode
    | SubInstructionNode
    | BdnsInstructionNode
    | BdnsalInstructionNode
    | BdseInstructionNode
    | BdsealInstructionNode
    | BrdnsInstructionNode
    | BrdseInstructionNode
    | SdnsInstructionNode
    | SdseInstructionNode
    | PokeInstructionNode
    | BeqzInstructionNode
    | BeqzalInstructionNode
    | BnezInstructionNode
    | BnezalInstructionNode
    | BgezInstructionNode
    | BgezalInstructionNode
    | BgtzInstructionNode
    | BgtzalInstructionNode
    | BlezInstructionNode
    | BlezalInstructionNode
    | BltzInstructionNode
    | BltzalInstructionNode
    | BnanInstructionNode
    | BnanzInstructionNode
    | BreqzInstructionNode
    | BrnzInstructionNode
    | BrgezInstructionNode
    | BrgtzInstructionNode
    | BrlezInstructionNode
    | BrltzInstructionNode
    | BrnanInstructionNode
    | ErrorNode;