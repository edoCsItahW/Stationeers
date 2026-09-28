// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc 执行上下文：导出 {@link Context}，持有程序、符号表、类型表、内存、设备管理器与执行状态。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { SymbolTable } from "@ic10/compiler";
import type { TypeTable } from "@ic10/compiler";
import type { Program } from "@ic10/compiler";
import type { Manager } from "./manager";
import type { Memory } from "./memory";
import type { Config } from "./config";


/**
 * @summary 执行上下文
 *
 * @desc `Engine` 内部的执行状态：程序计数器、内存（寄存器文件与栈）、设备管理器、配置与休眠状态。
 *       通常通过 `engine.context` 取得，而不是自行构造。
 *
 * @note 程序、符号表与类型表在构造时**按值拷贝**，因此构造之后修改 `Program` 不会影响已创建的
 *       上下文。
 *
 * @example
 * ```typescript
 * const ctx = engine.context;
 *
 * ctx.pc = 0;                       // 回到程序开头
 * ctx.memory.setReg('r0', 42);
 * ctx.manager.getDevice('db');
 * ```
 *
 * @public
 */
export class Context {
    /**
     * @summary 当前配置
     *
     * @desc 与 `Engine`、`Memory` 共享的配置对象；修改后对后续 tick 生效（例如调整
     *       `maxInstructions`）。
     */
    public config: Config;

    /**
     * @summary 构造执行上下文
     *
     * @param program - 编译器解析出的程序
     * @param symbols - 语义分析产出的符号表（别名、常量、标签）
     * @param cfg - 运行时配置
     * @param typeTable - 语义分析产出的类型表（`analyser.typeTable`）。枚举常量操作数
     *                    （如 `s d0 Color Color.Green`）的求值依赖它，缺省时这类操作数无法求值。
     *
     * @desc 绑定层在 `new Engine(...)` 内部调用；直接构造可用于只做单步调试而不启动执行循环的
     *       场景。
     */
    constructor(
        program: Program,
        symbols: SymbolTable,
        cfg: Config,
        typeTable?: TypeTable
    );

    /**
     * @summary 程序计数器
     *
     * @desc 当前语句在 `program.statements` 中的下标（不是源码行号）。可写，用于跳转或回退。
     */
    pc: number;

    /**
     * @summary 内存（寄存器文件与栈）
     *
     * @returns 本上下文持有的 {@link Memory}（绑定层返回同一实例的引用）
     */
    get memory(): Memory;

    /**
     * @summary 设备管理器
     *
     * @returns 本上下文持有的 {@link Manager}（绑定层返回同一实例的引用）
     */
    get manager(): Manager;

    /**
     * @summary 停机
     *
     * @desc 置停机标志，此后 `Engine.step()` / `runTick()` / `runFull()` 都立即返回。IC10 的
     *       `hcf` 指令即调用本方法。
     */
    halt(): void;

    /**
     * @summary 是否已停机
     *
     * @returns 停机标志
     */
    get halted(): boolean;

    /**
     * @summary 休眠指定时长
     *
     * @param seconds - 休眠的秒数，按 `config.tickDuration` 换算成 tick 数
     *
     * @desc 供 `sleep` 指令与宿主调试使用。`tickDuration <= 0` 时报告 `IEC2_1`（无效配置参数）。
     */
    sleep(seconds: number): void;

    /**
     * @summary 是否处于休眠中
     *
     * @returns 休眠尚未结束则为 `true`
     *
     * @desc 休眠期间 `runTick()` 不执行任何语句，直到 `Engine` 推进的 tick 数越过休眠终点。
     */
    get isSleeping(): boolean;

    /**
     * @summary 源码行号 → 语句下标
     *
     * @param line - 源码行号（从 `1` 开始）
     *
     * @returns 该行第一条语句的下标；该行没有语句时返回 `undefined`
     *
     * @desc 跳转目标在符号表里以行号存放，执行器据此换算成 {@link Context.pc}。同一行有多条语句时
     *       取第一条。
     */
    getAddr(line: number): number | undefined;

    /**
     * @summary 语句下标 → 源码行号
     *
     * @param addr - 语句下标（`pc`）
     *
     * @returns 该语句所在的源码行号；该下标不是所在行的首条语句（或越界）时返回 `undefined`
     *
     * @desc {@link Context.getAddr} 的反查，用于把 `pc` 映射回源码位置。行号只记录“该行第一条语句”
     *       的下标，因此只有落在行首的语句才查得到。
     */
    getLine(addr: number): number | undefined;

}
