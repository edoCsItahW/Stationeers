// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file memory.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc 寄存器文件与栈内存：导出 {@link Memory} 与 {@link MemoryInfo}，由 `Context` 持有。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Config } from "./config";


/**
 * @summary 寄存器文件与栈内存
 *
 * @desc 保存 IC10 程序运行期的全部可变状态：以**名称**为键的寄存器表（`r0`–`r15`、`ra`、`sp`
 *       之外的任意名字都可作为键）和一块定长栈。
 *
 * @note 通常通过 `engine.context.memory` 取得，此时它与运行时的诊断上报器相连，溢出／越界会记录
 *       诊断；自行 `new Memory(cfg)` 得到的实例与本运行时无关，不会产生诊断。
 *
 * @example
 * ```typescript
 * const mem = engine.context.memory;
 *
 * mem.setReg('r0', 42);
 * mem.push(1);
 * mem.push(2);
 * console.log(mem.pop());     // 2
 * console.log(mem.getStack(0));  // 1（索引 0 为栈顶）
 * ```
 *
 * @public
 */
export class Memory {
    /**
     * @summary 当前配置
     *
     * @desc 与 `Context` 共享同一份配置对象；`maxStackSize` 在构造时决定栈的长度。
     */
    public config: Config;

    /**
     * @summary 栈指针
     *
     * @desc 下一个 `push` 的落点，也等于当前栈内元素个数。IC10 源码中的 `sp` 专用寄存器由执行器
     *       映射到这个字段（而不是寄存器表里名为 `sp` 的条目）。
     */
    public sp: number;

    /**
     * @summary 构造一块内存
     *
     * @param cfg - 运行时配置，决定栈的容量
     *
     * @desc 绑定层内部在 `Context` 构造时调用；一般不需要直接构造。
     */
    constructor(cfg: Config);

    /**
     * @summary 序列化内存状态
     *
     * @returns JSON 字符串，形状见 {@link MemoryInfo}
     *
     * @desc 包含全部寄存器、整个栈数组与栈指针，便于快照或调试。
     *
     * @example
     * ```typescript
     * const info: MemoryInfo = JSON.parse(mem.toJSON());
     * console.log(info.registers.r0, info.sp);
     * ```
     */
    toJSON(): string;

    /**
     * @summary 读取寄存器
     *
     * @param name - 寄存器名，如 `'r0'`、`'ra'`、`'sp'`
     *
     * @returns 寄存器当前值；从未写过的名字返回 `0`
     *
     * @desc 寄存器表是稀疏的：任何名字都合法，缺省值为 `0`。注意读取会为该名字建项（以 `0` 填充），
     *       因此读过的名字同样会出现在 {@link Memory.toJSON} 的结果里。
     */
    getReg(name: string): number;

    /**
     * @summary 写入寄存器
     *
     * @param name - 寄存器名，如 `'r0'`、`'ra'`
     * @param value - 待写入的值
     *
     * @note 寄存器表只按名字索引，`'sp'` 在这里同样是一个普通键；要改栈指针请写 {@link Memory.sp}
     *       或使用 `push`/`pop`（IC10 源码中的 `sp` 寄存器由执行器映射到栈指针）。
     */
    setReg(name: string, value: number): void;

    /**
     * @summary 读取栈内元素（相对索引）
     *
     * @param index - 相对栈顶的索引，`0` 为栈顶，`1` 为栈顶下方一个元素，依此类推
     *
     * @returns 该位置的元素值
     *
     * @desc 与 `pop`/`peek` 的视角一致：索引从栈顶向下数，而非数组下标。
     *
     * @example
     * ```typescript
     * mem.push(10);
     * mem.push(20);
     * mem.getStack(0);   // 20
     * mem.getStack(1);   // 10
     * ```
     */
    getStack(index: number): number;

    /**
     * @summary 写入栈内元素（相对索引）
     *
     * @param index - 相对栈顶的索引，`0` 为栈顶
     * @param value - 待写入的值
     *
     * @desc 只改动元素内容，不改变栈指针；越界索引会访问到栈数组之外，请控制在 `[0, sp)` 内。
     */
    setStack(index: number, value: number): void;

    /**
     * @summary 压栈
     *
     * @param value - 待压入的值
     *
     * @desc 写入 `sp` 指向的位置并递增 `sp`。超过 `maxStackSize` 时报告 `IEM0`（栈溢出）。
     */
    push(value: number): void;

    /**
     * @summary 出栈
     *
     * @returns 弹出的值
     *
     * @desc 先递减 `sp` 再读取该位置。栈为空时报告 `IEM1`（栈下溢）。
     */
    pop(): number;

    /**
     * @summary 查看栈顶
     *
     * @returns 栈顶元素的值（不出栈）
     *
     * @desc 对应 IC10 的 `peek` 指令：读取 `sp - 1` 处的元素。栈为空时报告 `IEM0`。
     */
    peek(): number;

    /**
     * @summary 按绝对索引写入栈
     *
     * @param index - 栈数组的**绝对**下标（`0` 为栈底），与 {@link Memory.setStack} 的相对索引不同
     * @param value - 待写入的值
     *
     * @desc 对应宿主侧的调试写入；越界（`index >= maxStackSize`）时报告 `IEM3`（索引越界）。
     *       IC10 源码中的 `poke` 指令由执行器换算成相对栈顶的地址，不直接使用本方法。
     */
    poke(index: number, value: number): void;

}

/**
 * @summary {@link Memory.toJSON} 的结果
 *
 * @desc 寄存器表以名字为键（被读写过的名字都会出现，读取未写过的名字会以 `0` 建项），栈为从栈底
 *       到栈顶的完整数组。
 *
 * @note `registers` 的键类型只声明了常用的 `r0`–`r15`；实际内容还可能包含 `ra` 以及宿主自行
 *       使用的其它名字。
 *
 * @public
 */
export interface MemoryInfo {
    /** 寄存器表：名字到数值的映射 */
    registers: { [K in `r${0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15}`]: number; };

    /** 整个栈数组（长度为 `maxStackSize`，未使用的部分为 `0`） */
    stack: number[];

    /** 栈指针 */
    sp: number;
}
