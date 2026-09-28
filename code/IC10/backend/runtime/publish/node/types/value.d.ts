// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file value.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc 操作数取值的工具函数：实现 `HASH` / `STR` 宏与内建常量的求值规则，供宿主或调试器
 *       在不执行程序的前提下核对编译结果。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 计算 `HASH("...")` 的结果
 *
 * @param value - 参与哈希的文本（源码 `HASH("...")` 中引号内的内容）
 *
 * @returns 该文本的 32 位 CRC32 哈希，作为有符号 32 位整数返回
 *
 * @desc 与游戏一致地使用 CRC32，并把结果解释为 `int32`。设备／名称／试剂哈希以及逻辑属性哈希
 *       都走这一条路径，因此宿主可以用它预先算出与运行时一致的数字。
 *
 * @example
 * ```typescript
 * import { hashValue } from '@ic10/runtime';
 *
 * hashValue('StructureDoor');   // 与源码中 HASH("StructureDoor") 的结果完全一致
 * ```
 *
 * @public
 */
export function hashValue(value: string): number;

/**
 * @summary 计算 `STR("...")` 的结果
 *
 * @param str - 参与打包的文本（最长 8 个字节）
 *
 * @returns 把文本按小端序逐字节填入 64 位后的数值
 *
 * @desc `STR` 不是哈希：它把最多 8 个字符的字节按小端序打包成一个 64 位数，用于在游戏显示面板上
 *       绘制短文本。超过 8 个字节的部分不参与打包。
 *
 * @example
 * ```typescript
 * import { strValue } from '@ic10/runtime';
 *
 * strValue('AB');   // 0x4241 = 16961
 * ```
 *
 * @public
 */
export function strValue(str: string): number;

/**
 * @summary 查询内建常量的值
 *
 * @param keyword - 常量名，如 `'pi'`、`'epsilon'`、`'rgas'`
 *
 * @returns 对应数值；常量名未知时返回 `undefined`
 *
 * @desc 覆盖 `rgas`、`deg2rad`、`tau`、`epsilon`、`nan`、`pinf`、`ninf`、`pi`、`rad2deg`。
 *       源码中的这些常量由编译器的标准库 `define <名称> "<内建名>" #: @builtin` 声明，并在
 *       语法阶段就被替换为浮点字面量，所以本函数主要用于宿主侧核对数值。
 *
 * @example
 * ```typescript
 * import { constantValue } from '@ic10/runtime';
 *
 * constantValue('pi');      // 3.14159265358979
 * constantValue('unknown'); // undefined
 * ```
 *
 * @public
 */
export function constantValue(keyword: string): number | undefined;

/**
 * @summary 近零容差常量
 *
 * @desc 等于最小正次正规数（`Number.MIN_VALUE`）乘以 8，对应游戏中 `epsilon` 的定义。
 *       `sap` / `sna` / `bap` / `bna` 等近似比较指令用它判定“约等于”。
 *
 * @example
 * ```typescript
 * import { EPSILON_TIMES_8 } from '@ic10/runtime';
 *
 * EPSILON_TIMES_8;   // 3.952525166729972e-323（= Number.MIN_VALUE * 8）
 * ```
 *
 * @public
 */
export const EPSILON_TIMES_8: number;
