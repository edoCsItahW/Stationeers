// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ternary.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:45
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {
    ErrorNode,
    RegisterOrIdentifierNode,
    OperandNode,
    DeviceReferenceNode,
    LogicTypeNode,
    ReagentModeNode, 
    OperandType
} from "../ast";
import {BinaryInstruction} from "./binary";


// 三元指令（3 个操作数）

export interface TernaryInstruction extends BinaryInstruction {
    type3: OperandType;
}


/**
 * @summary 三元指令概述
 *
 * @desc 三元指令是包含三个操作数的指令。
 * 根据操作数类型可分为以下几类：
 * - **TernaryInstruction_RI_O_O**：寄存器/标识符 + 操作数 + 操作数
 * - **TernaryInstruction_RI_DR_O**：寄存器/标识符 + 设备引用 + 操作数
 * - **TernaryInstruction_DR_O_O**：设备引用 + 操作数 + 操作数
 * - **TernaryInstruction_RI_DR_LT**：寄存器/标识符 + 设备引用 + 逻辑类型
 * - **TernaryInstruction_RI_DR_RM**：寄存器/标识符 + 设备引用 + 试剂模式
 * - **TernaryInstruction_DR_LT_RI**：设备引用 + 逻辑类型 + 寄存器/标识符
 * - **TernaryInstruction_O_LT_RI**：操作数 + 逻辑类型 + 寄存器/标识符
 * - **TernaryInstruction_DR_LT_O**：设备引用 + 逻辑类型 + 操作数
 * - **TernaryInstruction_O_O_O**：操作数 + 操作数 + 操作数
 *
 * @elseif en
 * @summary Ternary Instructions Overview
 *
 * @desc Ternary instructions contain three operands.
 * They are categorized by operand types:
 * - **TernaryInstruction_RI_O_O**: Register/identifier + operand + operand
 * - **TernaryInstruction_RI_DR_O**: Register/identifier + device reference + operand
 * - **TernaryInstruction_DR_O_O**: Device reference + operand + operand
 * - **TernaryInstruction_RI_DR_LT**: Register/identifier + device reference + logic type
 * - **TernaryInstruction_RI_DR_RM**: Register/identifier + device reference + reagent mode
 * - **TernaryInstruction_DR_LT_RI**: Device reference + logic type + register/identifier
 * - **TernaryInstruction_O_LT_RI**: Operand + logic type + register/identifier
 * - **TernaryInstruction_DR_LT_O**: Device reference + logic type + operand
 * - **TernaryInstruction_O_O_O**: Operand + operand + operand
 */

/**
 * @summary 含 RegisterOrIdentifier + Operand + Operand 操作数的三元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二、三个为通用操作数。
 * @elseif en
 * @summary Base interface for ternary instructions with RegisterOrIdentifier + Operand + Operand operands
 *
 * @desc First operand is a register or identifier, second and third are general operands.
 *
 * @public
 */
interface TernaryInstruction_RI_O_O extends TernaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: OperandNode;
    operand3: OperandNode;
}


/**
 * @summary ADD（加法）指令节点
 *
 * @desc 将第二个和第三个操作数相加，结果存入目标寄存器。
 * @elseif en
 * @summary ADD (Addition) instruction node
 *
 * @desc Adds the second and third operands, storing the result in the target register.
 *
 * @example
 * ```typescript
 * // ADD r0 r1 r2
 * // 计算 r1 + r2 并存入 r0
 * ```
 *
 * @public
 */
export interface AddInstructionNode extends TernaryInstruction_RI_O_O {
    type: "addInstruction";
}


/**
 * @summary ATAN2（反正切2）指令节点
 *
 * @desc 计算第二个操作数除以第三个操作数的反正切值（弧度制）。
 * @elseif en
 * @summary ATAN2 (Arc Tangent 2) instruction node
 *
 * @desc Computes the arc tangent of the second operand divided by the third operand (in radians).
 *
 * @example
 * ```typescript
 * // ATAN2 r0 r1 r2
 * // 计算 atan2(r1, r2) 并存入 r0
 * ```
 *
 * @public
 */
export interface Atan2InstructionNode extends TernaryInstruction_RI_O_O {
    type: "atan2Instruction";
}


/**
 * @summary DIV（除法）指令节点
 *
 * @desc 将第二个操作数除以第三个操作数，结果存入目标寄存器。
 * @elseif en
 * @summary DIV (Division) instruction node
 *
 * @desc Divides the second operand by the third operand, storing the result in the target register.
 *
 * @example
 * ```typescript
 * // DIV r0 r1 r2
 * // 计算 r1 / r2 并存入 r0
 * ```
 *
 * @public
 */
export interface DivInstructionNode extends TernaryInstruction_RI_O_O {
    type: "divInstruction";
}


/**
 * @summary MAX（最大值）指令节点
 *
 * @desc 比较第二个和第三个操作数，将较大值存入目标寄存器。
 * @elseif en
 * @summary MAX (Maximum) instruction node
 *
 * @desc Compares the second and third operands, storing the larger value in the target register.
 *
 * @example
 * ```typescript
 * // MAX r0 r1 r2
 * // 将 r1 和 r2 中的较大值存入 r0
 * ```
 *
 * @public
 */
export interface MaxInstructionNode extends TernaryInstruction_RI_O_O {
    type: "maxInstruction";
}


/**
 * @summary MIN（最小值）指令节点
 *
 * @desc 比较第二个和第三个操作数，将较小值存入目标寄存器。
 * @elseif en
 * @summary MIN (Minimum) instruction node
 *
 * @desc Compares the second and third operands, storing the smaller value in the target register.
 *
 * @example
 * ```typescript
 * // MIN r0 r1 r2
 * // 将 r1 和 r2 中的较小值存入 r0
 * ```
 *
 * @public
 */
export interface MinInstructionNode extends TernaryInstruction_RI_O_O {
    type: "minInstruction";
}


/**
 * @summary MOD（取模）指令节点
 *
 * @desc 计算第二个操作数除以第三个操作数的余数。
 * @elseif en
 * @summary MOD (Modulo) instruction node
 *
 * @desc Computes the remainder of the second operand divided by the third operand.
 *
 * @example
 * ```typescript
 * // MOD r0 r1 r2
 * // 计算 r1 % r2 并存入 r0
 * ```
 *
 * @public
 */
export interface ModInstructionNode extends TernaryInstruction_RI_O_O {
    type: "modInstruction";
}


/**
 * @summary MUL（乘法）指令节点
 *
 * @desc 将第二个和第三个操作数相乘，结果存入目标寄存器。
 * @elseif en
 * @summary MUL (Multiplication) instruction node
 *
 * @desc Multiplies the second and third operands, storing the result in the target register.
 *
 * @example
 * ```typescript
 * // MUL r0 r1 r2
 * // 计算 r1 * r2 并存入 r0
 * ```
 *
 * @public
 */
export interface MulInstructionNode extends TernaryInstruction_RI_O_O {
    type: "mulInstruction";
}


/**
 * @summary POW（幂运算）指令节点
 *
 * @desc 计算第二个操作数的第三次操作数次幂。
 * @elseif en
 * @summary POW (Power) instruction node
 *
 * @desc Computes the second operand raised to the power of the third operand.
 *
 * @example
 * ```typescript
 * // POW r0 r1 r2
 * // 计算 r1 ^ r2 并存入 r0
 * ```
 *
 * @public
 */
export interface PowInstructionNode extends TernaryInstruction_RI_O_O {
    type: "powInstruction";
}


/**
 * @summary AND（按位与）指令节点
 *
 * @desc 对第二个和第三个操作数进行按位与运算。
 * @elseif en
 * @summary AND (Bitwise AND) instruction node
 *
 * @desc Performs bitwise AND operation on the second and third operands.
 *
 * @example
 * ```typescript
 * // AND r0 r1 r2
 * // 计算 r1 & r2 并存入 r0
 * ```
 *
 * @public
 */
export interface AndInstructionNode extends TernaryInstruction_RI_O_O {
    type: "andInstruction";
}


/**
 * @summary NOR（按位或非）指令节点
 *
 * @desc 对第二个和第三个操作数进行按位或运算，然后取反。
 * @elseif en
 * @summary NOR (Bitwise NOR) instruction node
 *
 * @desc Performs bitwise OR operation on the second and third operands, then negates the result.
 *
 * @example
 * ```typescript
 * // NOR r0 r1 r2
 * // 计算 ~(r1 | r2) 并存入 r0
 * ```
 *
 * @public
 */
export interface NorInstructionNode extends TernaryInstruction_RI_O_O {
    type: "norInstruction";
}


/**
 * @summary OR（按位或）指令节点
 *
 * @desc 对第二个和第三个操作数进行按位或运算。
 * @elseif en
 * @summary OR (Bitwise OR) instruction node
 *
 * @desc Performs bitwise OR operation on the second and third operands.
 *
 * @example
 * ```typescript
 * // OR r0 r1 r2
 * // 计算 r1 | r2 并存入 r0
 * ```
 *
 * @public
 */
export interface OrInstructionNode extends TernaryInstruction_RI_O_O {
    type: "orInstruction";
}


/**
 * @summary SLA（算术左移）指令节点
 *
 * @desc 将第二个操作数左移第三个操作数指定的位数（算术左移）。
 * @elseif en
 * @summary SLA (Shift Left Arithmetic) instruction node
 *
 * @desc Arithmetically shifts the second operand left by the number of bits specified by the third operand.
 *
 * @example
 * ```typescript
 * // SLA r0 r1 r2
 * // 将 r1 左移 r2 位并存入 r0
 * ```
 *
 * @public
 */
export interface SlaInstructionNode extends TernaryInstruction_RI_O_O {
    type: "slaInstruction";
}


/**
 * @summary SLL（逻辑左移）指令节点
 *
 * @desc 将第二个操作数左移第三个操作数指定的位数（逻辑左移）。
 * @elseif en
 * @summary SLL (Shift Left Logical) instruction node
 *
 * @desc Logically shifts the second operand left by the number of bits specified by the third operand.
 *
 * @example
 * ```typescript
 * // SLL r0 r1 r2
 * // 将 r1 左移 r2 位并存入 r0
 * ```
 *
 * @public
 */
export interface SllInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sllInstruction";
}


/**
 * @summary SRA（算术右移）指令节点
 *
 * @desc 将第二个操作数右移第三个操作数指定的位数（算术右移，符号位扩展）。
 * @elseif en
 * @summary SRA (Shift Right Arithmetic) instruction node
 *
 * @desc Arithmetically shifts the second operand right by the number of bits specified by the third operand (sign-extended).
 *
 * @example
 * ```typescript
 * // SRA r0 r1 r2
 * // 将 r1 右移 r2 位并存入 r0
 * ```
 *
 * @public
 */
export interface SraInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sraInstruction";
}


/**
 * @summary SRL（逻辑右移）指令节点
 *
 * @desc 将第二个操作数右移第三个操作数指定的位数（逻辑右移，零扩展）。
 * @elseif en
 * @summary SRL (Shift Right Logical) instruction node
 *
 * @desc Logically shifts the second operand right by the number of bits specified by the third operand (zero-extended).
 *
 * @example
 * ```typescript
 * // SRL r0 r1 r2
 * // 将 r1 右移 r2 位并存入 r0
 * ```
 *
 * @public
 */
export interface SrlInstructionNode extends TernaryInstruction_RI_O_O {
    type: "srlInstruction";
}


/**
 * @summary XOR（按位异或）指令节点
 *
 * @desc 对第二个和第三个操作数进行按位异或运算。
 * @elseif en
 * @summary XOR (Bitwise XOR) instruction node
 *
 * @desc Performs bitwise XOR operation on the second and third operands.
 *
 * @example
 * ```typescript
 * // XOR r0 r1 r2
 * // 计算 r1 ^ r2 并存入 r0
 * ```
 *
 * @public
 */
export interface XorInstructionNode extends TernaryInstruction_RI_O_O {
    type: "xorInstruction";
}


/**
 * @summary SAPZ（Set if And Predicate Zero）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与结果为零，则将目标寄存器设置为1。
 * @elseif en
 * @summary SAPZ (Set if And Predicate Zero) instruction node
 *
 * @desc Sets the target register to 1 if the bitwise AND of the second and third operands is zero.
 *
 * @example
 * ```typescript
 * // SAPZ r0 r1 r2
 * // 如果 (r1 & r2) == 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SapzInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sapzInstruction";
}


/**
 * @summary SNAZ（Set if And Predicate Non-Zero）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与结果非零，则将目标寄存器设置为1。
 * @elseif en
 * @summary SNAZ (Set if And Predicate Non-Zero) instruction node
 *
 * @desc Sets the target register to 1 if the bitwise AND of the second and third operands is non-zero.
 *
 * @example
 * ```typescript
 * // SNAZ r0 r1 r2
 * // 如果 (r1 & r2) != 0，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SnazInstructionNode extends TernaryInstruction_RI_O_O {
    type: "snazInstruction";
}


/**
 * @summary SEQ（Set if EQual）指令节点
 *
 * @desc 如果第二个操作数等于第三个操作数，则将目标寄存器设置为1。
 * @elseif en
 * @summary SEQ (Set if EQual) instruction node
 *
 * @desc Sets the target register to 1 if the second operand equals the third operand.
 *
 * @example
 * ```typescript
 * // SEQ r0 r1 r2
 * // 如果 r1 == r2，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SeqInstructionNode extends TernaryInstruction_RI_O_O {
    type: "seqInstruction";
}


/**
 * @summary SNE（Set if Not Equal）指令节点
 *
 * @desc 如果第二个操作数不等于第三个操作数，则将目标寄存器设置为1。
 * @elseif en
 * @summary SNE (Set if Not Equal) instruction node
 *
 * @desc Sets the target register to 1 if the second operand does not equal the third operand.
 *
 * @example
 * ```typescript
 * // SNE r0 r1 r2
 * // 如果 r1 != r2，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SneInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sneInstruction";
}


/**
 * @summary SGE（Set if Greater than or Equal）指令节点
 *
 * @desc 如果第二个操作数大于等于第三个操作数，则将目标寄存器设置为1。
 * @elseif en
 * @summary SGE (Set if Greater than or Equal) instruction node
 *
 * @desc Sets the target register to 1 if the second operand is greater than or equal to the third operand.
 *
 * @example
 * ```typescript
 * // SGE r0 r1 r2
 * // 如果 r1 >= r2，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SgeInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sgeInstruction";
}


/**
 * @summary SGT（Set if Greater Than）指令节点
 *
 * @desc 如果第二个操作数大于第三个操作数，则将目标寄存器设置为1。
 * @elseif en
 * @summary SGT (Set if Greater Than) instruction node
 *
 * @desc Sets the target register to 1 if the second operand is greater than the third operand.
 *
 * @example
 * ```typescript
 * // SGT r0 r1 r2
 * // 如果 r1 > r2，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SgtInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sgtInstruction";
}


/**
 * @summary SLE（Set if Less than or Equal）指令节点
 *
 * @desc 如果第二个操作数小于等于第三个操作数，则将目标寄存器设置为1。
 * @elseif en
 * @summary SLE (Set if Less than or Equal) instruction node
 *
 * @desc Sets the target register to 1 if the second operand is less than or equal to the third operand.
 *
 * @example
 * ```typescript
 * // SLE r0 r1 r2
 * // 如果 r1 <= r2，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SleInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sleInstruction";
}


/**
 * @summary SLT（Set if Less Than）指令节点
 *
 * @desc 如果第二个操作数小于第三个操作数，则将目标寄存器设置为1。
 * @elseif en
 * @summary SLT (Set if Less Than) instruction node
 *
 * @desc Sets the target register to 1 if the second operand is less than the third operand.
 *
 * @example
 * ```typescript
 * // SLT r0 r1 r2
 * // 如果 r1 < r2，则 r0 = 1
 * ```
 *
 * @public
 */
export interface SltInstructionNode extends TernaryInstruction_RI_O_O {
    type: "sltInstruction";
}


/**
 * @summary 含 RegisterOrIdentifier + DeviceReference + Operand 操作数的三元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为通用操作数。
 * @elseif en
 * @summary Base interface for ternary instructions with RegisterOrIdentifier + DeviceReference + Operand operands
 *
 * @desc First operand is a register or identifier, second is a device reference, third is a general operand.
 *
 * @public
 */
interface TernaryInstruction_RI_DR_O extends TernaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: DeviceReferenceNode;
    operand3: OperandNode;
}


/**
 * @summary GET（获取设备属性）指令节点
 *
 * @desc 从设备获取指定类型的属性值到目标寄存器。
 * @elseif en
 * @summary GET (Get Device Property) instruction node
 *
 * @desc Gets the property of a specified type from the device into the target register.
 *
 * @example
 * ```typescript
 * // GET r0 console1 Setting
 * // 从 console1 获取 Setting 属性到 r0
 * ```
 *
 * @public
 */
export interface GetInstructionNode extends TernaryInstruction_RI_DR_O {
    type: "getInstruction";
}


/**
 * @summary RMAP（重新映射设备）指令节点
 *
 * @desc 重新映射目标设备到源设备。
 * @elseif en
 * @summary RMAP (ReMAP Device) instruction node
 *
 * @desc Remaps the target device to the source device.
 *
 * @example
 * ```typescript
 * // RMAP r0 console1 console2
 * // 将 console1 重新映射到 console2
 * ```
 *
 * @public
 */
export interface RmapInstructionNode extends TernaryInstruction_RI_DR_O {
    type: "rmapInstruction";
}


/**
 * @summary 含 DeviceReference + Operand + Operand 操作数的三元指令基类
 *
 * @desc 第一个操作数为设备引用，第二、三个为通用操作数。
 * @elseif en
 * @summary Base interface for ternary instructions with DeviceReference + Operand + Operand operands
 *
 * @desc First operand is a device reference, second and third are general operands.
 *
 * @public
 */
interface TernaryInstruction_DR_O_O extends TernaryInstruction {
    operand1: DeviceReferenceNode;
    operand2: OperandNode;
    operand3: OperandNode;
}


/**
 * @summary PUT（放置物品到设备）指令节点
 *
 * @desc 将物品从库存放置到目标设备。
 * @elseif en
 * @summary PUT instruction node
 *
 * @desc Places items from inventory to the target device.
 *
 * @example
 * ```typescript
 * // PUT console1 r0 r1
 * // 将物品从 r0 放置到 console1
 * ```
 *
 * @public
 */
export interface PutInstructionNode extends TernaryInstruction_DR_O_O {
    type: "putInstruction";
}


/**
 * @summary 含 RegisterOrIdentifier + DeviceReference + LogicType 操作数的三元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为逻辑类型。
 * @elseif en
 * @summary Base interface for ternary instructions with RegisterOrIdentifier + DeviceReference + LogicType operands
 *
 * @desc First operand is a register or identifier, second is a device reference, third is a logic type.
 *
 * @public
 */
interface TernaryInstruction_RI_DR_LT extends TernaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: DeviceReferenceNode;
    operand3: LogicTypeNode;
}


/**
 * @summary L（读取设备逻辑）指令节点
 *
 * @desc 从目标设备的指定逻辑类型读取数据到寄存器。
 * @elseif en
 * @summary L (Read Device Logic) instruction node
 *
 * @desc Reads data from the target device's specified logic type into the register.
 *
 * @example
 * ```typescript
 * // L r0 console1 Color
 * // 从 console1 读取 Color 数据到 r0
 * ```
 *
 * @public
 */
export interface LInstructionNode extends TernaryInstruction_RI_DR_LT {
    type: "lInstruction";
}


/**
 * @summary 含 RegisterOrIdentifier + DeviceReference + ReagentMode 操作数的三元指令基类
 *
 * @desc 第一个操作数为寄存器或标识符，第二个为设备引用，第三个为试剂模式。
 * @elseif en
 * @summary Base interface for ternary instructions with RegisterOrIdentifier + DeviceReference + ReagentMode operands
 *
 * @desc First operand is a register or identifier, second is a device reference, third is a reagent mode.
 *
 * @public
 */
interface TernaryInstruction_RI_DR_RM extends TernaryInstruction {
    operand1: RegisterOrIdentifierNode;
    operand2: DeviceReferenceNode;
    operand3: ReagentModeNode;
}


/**
 * @summary LR（读取设备库存试剂）指令节点
 *
 * @desc 从目标设备的库存中读取指定试剂的数量到寄存器。
 * @elseif en
 * @summary LR (Read Device Reagent) instruction node
 *
 * @desc Reads the quantity of a specified reagent from the target device's inventory into the register.
 *
 * @example
 * ```typescript
 * // LR r0 tank1 Water
 * // 从 tank1 读取 Water 试剂数量到 r0
 * ```
 *
 * @public
 */
export interface LrInstructionNode extends TernaryInstruction_RI_DR_RM {
    type: "lrInstruction";
}


/**
 * @summary 含 DeviceReference + LogicType + RegisterOrIdentifier 操作数的三元指令基类
 *
 * @desc 第一个操作数为设备引用，第二个为逻辑类型，第三个为寄存器或标识符。
 * @elseif en
 * @summary Base interface for ternary instructions with DeviceReference + LogicType + RegisterOrIdentifier operands
 *
 * @desc First operand is a device reference, second is a logic type, third is a register or identifier.
 *
 * @public
 */
interface TernaryInstruction_DR_LT_RI extends TernaryInstruction {
    operand1: DeviceReferenceNode;
    operand2: LogicTypeNode;
    operand3: RegisterOrIdentifierNode;
}


/**
 * @summary S（设置设备逻辑）指令节点
 *
 * @desc 将寄存器的值设置到目标设备的指定逻辑类型属性。
 * @elseif en
 * @summary S (Set Device Logic) instruction node
 *
 * @desc Sets the value of the register to the specified logic type property of the target device.
 *
 * @example
 * ```typescript
 * // S console1 Color r0
 * // 将 r0 的值设置到 console1 的 Color 属性
 * ```
 *
 * @public
 */
export interface SInstructionNode extends TernaryInstruction_DR_LT_RI {
    type: "sInstruction";
}


/**
 * @summary 含 Operand + LogicType + RegisterOrIdentifier 操作数的三元指令基类
 *
 * @desc 第一个操作数为通用操作数，第二个为逻辑类型，第三个为寄存器或标识符。
 * @elseif en
 * @summary Base interface for ternary instructions with Operand + LogicType + RegisterOrIdentifier operands
 *
 * @desc First operand is a general operand, second is a logic type, third is a register or identifier.
 *
 * @public
 */
interface TernaryInstruction_O_LT_RI extends TernaryInstruction {
    operand1: OperandNode;
    operand2: LogicTypeNode;
    operand3: RegisterOrIdentifierNode;
}


/**
 * @summary SB（设置设备逻辑批量）指令节点
 *
 * @desc 将多个寄存器的值批量设置到目标设备的指定逻辑类型属性。
 * @elseif en
 * @summary SB (Set Device Logic Batch) instruction node
 *
 * @desc Batch sets values from multiple registers to the specified logic type property of the target device.
 *
 * @example
 * ```typescript
 * // SB tank1 Mode r0
 * // 将 r0 的值批量设置到 tank1 的 Mode 属性
 * ```
 *
 * @public
 */
export interface SbInstructionNode extends TernaryInstruction_O_LT_RI {
    type: "sbInstruction";
}


/**
 * @summary 含 DeviceReference + LogicType + Operand 操作数的三元指令基类
 *
 * @desc 第一个操作数为设备引用，第二个为逻辑类型，第三个为通用操作数。
 * @elseif en
 * @summary Base interface for ternary instructions with DeviceReference + LogicType + Operand operands
 *
 * @desc First operand is a device reference, second is a logic type, third is a general operand.
 *
 * @public
 */
interface TernaryInstruction_DR_LT_O extends TernaryInstruction {
    operand1: DeviceReferenceNode;
    operand2: LogicTypeNode;
    operand3: OperandNode;
}


/**
 * @summary BDNVL（Branches on Device No Variant Leak）指令节点
 *
 * @desc 如果设备没有变体泄漏，则跳转到目标地址。
 * @elseif en
 * @summary BDNVL (Branches on Device No Variant Leak) instruction node
 *
 * @desc Jumps to the target address if the device has no variant leak.
 *
 * @example
 * ```typescript
 * // BDNVL tank1 loop
 * // 如果 tank1 没有变体泄漏，跳转到 loop
 * ```
 *
 * @public
 */
export interface BdnvlInstructionNode extends TernaryInstruction_DR_LT_O {
    type: "bdnvlInstruction";
}


/**
 * @summary BDNVS（Branches on Device No Variant Stuck）指令节点
 *
 * @desc 如果设备没有变体卡住，则跳转到目标地址。
 * @elseif en
 * @summary BDNVS (Branches on Device No Variant Stuck) instruction node
 *
 * @desc Jumps to the target address if the device has no variant stuck.
 *
 * @example
 * ```typescript
 * // BDNVS tank1 loop
 * // 如果 tank1 没有变体卡住，跳转到 loop
 * ```
 *
 * @public
 */
export interface BdnvsInstructionNode extends TernaryInstruction_DR_LT_O {
    type: "bdnvsInstruction";
}


/**
 * @summary 含 Operand + Operand + Operand 操作数的三元指令基类
 *
 * @desc 三个操作数都为通用操作数类型。
 * @elseif en
 * @summary Base interface for ternary instructions with Operand + Operand + Operand operands
 *
 * @desc All three operands are general operand types.
 *
 * @public
 */
interface TernaryInstruction_O_O_O extends TernaryInstruction {
    operand1: OperandNode;
    operand2: OperandNode;
    operand3: OperandNode;
}


/**
 * @summary BEQ（Branch if EQual）指令节点
 *
 * @desc 如果第二个操作数等于第三个操作数，则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BEQ (Branch if EQual) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand equals the third operand.
 *
 * @example
 * ```typescript
 * // BEQ loop r0 r1
 * // 如果 r0 == r1，跳转到 loop
 * ```
 *
 * @public
 */
export interface BeqInstructionNode extends TernaryInstruction_O_O_O {
    type: "beqInstruction";
}


/**
 * @summary BEQAL（Branch if EQual And Link）指令节点
 *
 * @desc 如果第二个操作数等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BEQAL (Branch if EQual And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand equals the third operand, and saves the return address.
 *
 * @example
 * ```typescript
 * // BEQAL loop r0 r1
 * // 如果 r0 == r1，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BeqalInstructionNode extends TernaryInstruction_O_O_O {
    type: "beqalInstruction";
}


/**
 * @summary BNE（Branch if Not Equal）指令节点
 *
 * @desc 如果第二个操作数不等于第三个操作数，则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BNE (Branch if Not Equal) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand does not equal the third operand.
 *
 * @example
 * ```typescript
 * // BNE loop r0 r1
 * // 如果 r0 != r1，跳转到 loop
 * ```
 *
 * @public
 */
export interface BneInstructionNode extends TernaryInstruction_O_O_O {
    type: "bneInstruction";
}


/**
 * @summary BNEAL（Branch if Not Equal And Link）指令节点
 *
 * @desc 如果第二个操作数不等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BNEAL (Branch if Not Equal And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand does not equal the third operand, and saves the return address.
 *
 * @example
 * ```typescript
 * // BNEAL loop r0 r1
 * // 如果 r0 != r1，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BnealInstructionNode extends TernaryInstruction_O_O_O {
    type: "bnealInstruction";
}


/**
 * @summary BGE（Branch if Greater than or Equal）指令节点
 *
 * @desc 如果第二个操作数大于等于第三个操作数，则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BGE (Branch if Greater than or Equal) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is greater than or equal to the third operand.
 *
 * @example
 * ```typescript
 * // BGE loop r0 r1
 * // 如果 r0 >= r1，跳转到 loop
 * ```
 *
 * @public
 */
export interface BgeInstructionNode extends TernaryInstruction_O_O_O {
    type: "bgeInstruction";
}


/**
 * @summary BGEAL（Branch if Greater than or Equal And Link）指令节点
 *
 * @desc 如果第二个操作数大于等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BGEAL (Branch if Greater than or Equal And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is greater than or equal to the third operand, and saves the return address.
 *
 * @example
 * ```typescript
 * // BGEAL loop r0 r1
 * // 如果 r0 >= r1，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BgealInstructionNode extends TernaryInstruction_O_O_O {
    type: "bgealInstruction";
}


/**
 * @summary BGT（Branch if Greater Than）指令节点
 *
 * @desc 如果第二个操作数大于第三个操作数，则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BGT (Branch if Greater Than) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is greater than the third operand.
 *
 * @example
 * ```typescript
 * // BGT loop r0 r1
 * // 如果 r0 > r1，跳转到 loop
 * ```
 *
 * @public
 */
export interface BgtInstructionNode extends TernaryInstruction_O_O_O {
    type: "bgtInstruction";
}


/**
 * @summary BGTAL（Branch if Greater Than And Link）指令节点
 *
 * @desc 如果第二个操作数大于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BGTAL (Branch if Greater Than And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is greater than the third operand, and saves the return address.
 *
 * @example
 * ```typescript
 * // BGTAL loop r0 r1
 * // 如果 r0 > r1，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BgtalInstructionNode extends TernaryInstruction_O_O_O {
    type: "bgtalInstruction";
}


/**
 * @summary BLE（Branch if Less than or Equal）指令节点
 *
 * @desc 如果第二个操作数小于等于第三个操作数，则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BLE (Branch if Less than or Equal) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is less than or equal to the third operand.
 *
 * @example
 * ```typescript
 * // BLE loop r0 r1
 * // 如果 r0 <= r1，跳转到 loop
 * ```
 *
 * @public
 */
export interface BleInstructionNode extends TernaryInstruction_O_O_O {
    type: "bleInstruction";
}


/**
 * @summary BLEAL（Branch if Less than or Equal And Link）指令节点
 *
 * @desc 如果第二个操作数小于等于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BLEAL (Branch if Less than or Equal And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is less than or equal to the third operand, and saves the return address.
 *
 * @example
 * ```typescript
 * // BLEAL loop r0 r1
 * // 如果 r0 <= r1，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BlealInstructionNode extends TernaryInstruction_O_O_O {
    type: "blealInstruction";
}


/**
 * @summary BLT（Branch if Less Than）指令节点
 *
 * @desc 如果第二个操作数小于第三个操作数，则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BLT (Branch if Less Than) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is less than the third operand.
 *
 * @example
 * ```typescript
 * // BLT loop r0 r1
 * // 如果 r0 < r1，跳转到 loop
 * ```
 *
 * @public
 */
export interface BltInstructionNode extends TernaryInstruction_O_O_O {
    type: "bltInstruction";
}


/**
 * @summary BLTAL（Branch if Less Than And Link）指令节点
 *
 * @desc 如果第二个操作数小于第三个操作数，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BLTAL (Branch if Less Than And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the second operand is less than the third operand, and saves the return address.
 *
 * @example
 * ```typescript
 * // BLTAL loop r0 r1
 * // 如果 r0 < r1，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BltalInstructionNode extends TernaryInstruction_O_O_O {
    type: "bltalInstruction";
}


/**
 * @summary BAPZ（Branch if And Predicate Zero）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与结果为零，则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BAPZ (Branch if And Predicate Zero) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is zero.
 *
 * @example
 * ```typescript
 * // BAPZ loop r0 r1
 * // 如果 (r0 & r1) == 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BapzInstructionNode extends TernaryInstruction_O_O_O {
    type: "bapzInstruction";
}


/**
 * @summary BAPZAL（Branch if And Predicate Zero And Link）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与结果为零，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BAPZAL (Branch if And Predicate Zero And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BAPZAL loop r0 r1
 * // 如果 (r0 & r1) == 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BapzalInstructionNode extends TernaryInstruction_O_O_O {
    type: "bapzalInstruction";
}


/**
 * @summary BNAZ（Branch if Not And Predicate Zero）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与非结果为零（即按位与非零），则跳转到第一个操作数指定的目标地址。
 * @elseif en
 * @summary BNAZ (Branch if Not And Predicate Zero) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is non-zero.
 *
 * @example
 * ```typescript
 * // BNAZ loop r0 r1
 * // 如果 (r0 & r1) != 0，跳转到 loop
 * ```
 *
 * @public
 */
export interface BnazInstructionNode extends TernaryInstruction_O_O_O {
    type: "bnazInstruction";
}


/**
 * @summary BNAZAL（Branch if Not And Predicate Zero And Link）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与非结果为零，则跳转到第一个操作数指定的目标地址并保存返回地址。
 * @elseif en
 * @summary BNAZAL (Branch if Not And Predicate Zero And Link) instruction node
 *
 * @desc Jumps to the target address specified by the first operand if the bitwise AND of the second and third operands is non-zero, and saves the return address.
 *
 * @example
 * ```typescript
 * // BNAZAL loop r0 r1
 * // 如果 (r0 & r1) != 0，跳转到 loop 并保存返回地址
 * ```
 *
 * @public
 */
export interface BnazalInstructionNode extends TernaryInstruction_O_O_O {
    type: "bnazalInstruction";
}


/**
 * @summary BREQ（Branches Register if EQual）指令节点
 *
 * @desc 如果第二个操作数等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BREQ (Branches Register if EQual) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the second operand equals the third operand.
 *
 * @example
 * ```typescript
 * // BREQ r0 r1 r2
 * // 如果 r1 == r2，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BreqInstructionNode extends TernaryInstruction_O_O_O {
    type: "breqInstruction";
}


/**
 * @summary BRNE（Branches Register if Not Equal）指令节点
 *
 * @desc 如果第二个操作数不等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BRNE (Branches Register if Not Equal) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the second operand does not equal the third operand.
 *
 * @example
 * ```typescript
 * // BRNE r0 r1 r2
 * // 如果 r1 != r2，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrneInstructionNode extends TernaryInstruction_O_O_O {
    type: "brneInstruction";
}


/**
 * @summary BRGE（Branches Register if Greater than or Equal）指令节点
 *
 * @desc 如果第二个操作数大于等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BRGE (Branches Register if Greater than or Equal) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is greater than or equal to the third operand.
 *
 * @example
 * ```typescript
 * // BRGE r0 r1 r2
 * // 如果 r1 >= r2，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrgeInstructionNode extends TernaryInstruction_O_O_O {
    type: "brgeInstruction";
}


/**
 * @summary BRGT（Branches Register if Greater Than）指令节点
 *
 * @desc 如果第二个操作数大于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BRGT (Branches Register if Greater Than) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is greater than the third operand.
 *
 * @example
 * ```typescript
 * // BRGT r0 r1 r2
 * // 如果 r1 > r2，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrgtInstructionNode extends TernaryInstruction_O_O_O {
    type: "brgtInstruction";
}


/**
 * @summary BRLE（Branches Register if Less than or Equal）指令节点
 *
 * @desc 如果第二个操作数小于等于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BRLE (Branches Register if Less than or Equal) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is less than or equal to the third operand.
 *
 * @example
 * ```typescript
 * // BRLE r0 r1 r2
 * // 如果 r1 <= r2，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrleInstructionNode extends TernaryInstruction_O_O_O {
    type: "brleInstruction";
}


/**
 * @summary BRLT（Branches Register if Less Than）指令节点
 *
 * @desc 如果第二个操作数小于第三个操作数，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BRLT (Branches Register if Less Than) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the second operand is less than the third operand.
 *
 * @example
 * ```typescript
 * // BRLT r0 r1 r2
 * // 如果 r1 < r2，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrltInstructionNode extends TernaryInstruction_O_O_O {
    type: "brltInstruction";
}


/**
 * @summary BRAPZ（Branches Register if And Predicate Zero）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与结果为零，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BRAPZ (Branches Register if And Predicate Zero) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the bitwise AND of the second and third operands is zero.
 *
 * @example
 * ```typescript
 * // BRAPZ r0 r1 r2
 * // 如果 (r1 & r2) == 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrapzInstructionNode extends TernaryInstruction_O_O_O {
    type: "brapzInstruction";
}


/**
 * @summary BRNAZ（Branches Register if Not And Predicate Zero）指令节点
 *
 * @desc 如果第二个和第三个操作数的按位与结果非零，则跳转到第二个操作数（作为地址寄存器）指定的目标地址。
 * @elseif en
 * @summary BRNAZ (Branches Register if Not And Predicate Zero) instruction node
 *
 * @desc Jumps to the target address specified by the second operand (as address register) if the bitwise AND of the second and third operands is non-zero.
 *
 * @example
 * ```typescript
 * // BRNAZ r0 r1 r2
 * // 如果 (r1 & r2) != 0，跳转到 r1 存储的地址
 * ```
 *
 * @public
 */
export interface BrnazInstructionNode extends TernaryInstruction_O_O_O {
    type: "brnazInstruction";
}


/**
 * @summary 三元指令联合类型
 *
 * @desc 包含所有三元指令类型的联合，包括可能的错误节点。
 * @elseif en
 * @summary Ternary instruction union type
 *
 * @desc Union of all ternary instruction types, including possible error nodes.
 *
 * @public
 */
export type TernaryInstructionNode =
    | AddInstructionNode
    | Atan2InstructionNode
    | DivInstructionNode
    | MaxInstructionNode
    | MinInstructionNode
    | ModInstructionNode
    | MulInstructionNode
    | PowInstructionNode
    | AndInstructionNode
    | NorInstructionNode
    | OrInstructionNode
    | SlaInstructionNode
    | SllInstructionNode
    | SraInstructionNode
    | SrlInstructionNode
    | XorInstructionNode
    | SapzInstructionNode
    | SnazInstructionNode
    | SeqInstructionNode
    | SneInstructionNode
    | SgeInstructionNode
    | SgtInstructionNode
    | SleInstructionNode
    | SltInstructionNode
    | GetInstructionNode
    | RmapInstructionNode
    | PutInstructionNode
    | LInstructionNode
    | LrInstructionNode
    | SInstructionNode
    | SbInstructionNode
    | BdnvlInstructionNode
    | BdnvsInstructionNode
    | BeqInstructionNode
    | BeqalInstructionNode
    | BneInstructionNode
    | BnealInstructionNode
    | BgeInstructionNode
    | BgealInstructionNode
    | BgtInstructionNode
    | BgtalInstructionNode
    | BleInstructionNode
    | BlealInstructionNode
    | BltInstructionNode
    | BltalInstructionNode
    | BapzInstructionNode
    | BapzalInstructionNode
    | BnazInstructionNode
    | BnazalInstructionNode
    | BreqInstructionNode
    | BrneInstructionNode
    | BrgeInstructionNode
    | BrgtInstructionNode
    | BrleInstructionNode
    | BrltInstructionNode
    | BrapzInstructionNode
    | BrnazInstructionNode
    | ErrorNode;
