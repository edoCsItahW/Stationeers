// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file nullary.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:40
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {ErrorNode, InstructionNode} from "../ast";

// 零元指令（无操作数）

/**
 * @summary 零元指令概述
 *
 * @desc 零元指令是不带任何操作数的指令。
 * 这些指令通常用于控制程序流程或 CPU 状态。
 */

/**
 * @summary HCF（Hang Crisis Failure）指令节点
 *
 * @desc HCF 指令是 IC10 的最高优先级指令。
 * 执行后，CPU 将进入无限循环状态，无法恢复。
 * 通常用于严重错误处理或程序终止。
 *
 * @warning
 * 此指令会导致 CPU 永久停止，应谨慎使用！
 *
 * @example
 * ```typescript
 * // HCF 指令
 * {
 *   "type": "hcfInstruction",
 *   "position": { "line": 0, "column": 0 }
 * }
 * ```
 *
 * @public
 */
export interface HcfInstructionNode extends InstructionNode {
    type: "hcfInstruction";
}


/**
 * @summary YIELD 指令节点
 *
 * @desc YIELD 指令让出 CPU 执行权，允许其他 IC 芯片执行。
 * 用于协作式多任务，避免单个 IC 独占 CPU。
 *
 * @example
 * ```typescript
 * // YIELD 指令
 * {
 *   "type": "yieldInstruction",
 *   "position": { "line": 0, "column": 0 }
 * }
 * ```
 *
 * @public
 */
export interface YieldInstructionNode extends InstructionNode {
    type: "yieldInstruction";
}


/**
 * @summary 零元指令联合类型
 *
 * @desc 包含所有零元指令类型的联合，包括可能的错误节点。
 *
 * @public
 */
export type NullaryInstructionNode =
    | HcfInstructionNode
    | YieldInstructionNode
    | ErrorNode;