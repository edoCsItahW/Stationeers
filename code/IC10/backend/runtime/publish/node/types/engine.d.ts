// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file engine.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc 执行引擎：导出 {@link Engine}，把编译器产出的程序跑成 tick 序列。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { SymbolTable, Diagnostic } from "@ic10/compiler";
import type { TypeTable } from "@ic10/compiler";
import type { Program } from "@ic10/compiler";
import type { Context } from "./context";
import type { Config } from "./config";


/**
 * @summary IC10 程序执行引擎
 *
 * @desc 把 `@ic10/compiler` 产出的程序、符号表（以及求值枚举操作数所需的类型表）交给运行时，
 *       按 tick 执行：每个 tick 至多执行 `config.maxInstructions` 条语句，`yield` 结束当前 tick，
 *       `sleep` 让后续若干 tick 不执行，`hcf` 停机。
 *
 * @note 无法执行的部分（缺少符号表项的设备别名、越界的动态端口、无法求值的操作数、栈溢出等）
 *       不会抛异常，而是记录到 {@link Engine.diagnostics}，执行继续。
 *
 * @example
 * ```typescript
 * import { Lexer, Parser, Analyser } from '@ic10/compiler';
 * import { Engine } from '@ic10/runtime';
 *
 * const program = new Parser(Lexer.tokenize('move r0 42\nhcf\n')).parse();
 * const analyser = new Analyser();
 * await analyser.visit(program);
 *
 * const engine = new Engine(program, analyser.symbolTable, undefined, analyser.typeTable);
 * engine.runFull();
 *
 * console.log(engine.context.memory.getReg('r0'));   // 42
 * ```
 *
 * @public
 */
export class Engine {

    /**
     * @summary 构造执行引擎
     *
     * @param program - 编译器解析出的程序
     * @param symbols - 语义分析产出的符号表（别名、常量、标签）
     * @param config - 运行时配置，缺省时使用默认值（可选）
     * @param typeTable - 语义分析产出的类型表（`analyser.typeTable`）。枚举常量操作数
     *                    （如 `s d0 Color Color.Green`）的求值依赖它，缺省时这类操作数无法求值。
     *
     * @desc 构造时会拷贝程序、符号表与类型表，并建立一个带芯片设备的设备管理器与一份内存。
     */
    constructor(
        program: Program,
        symbols: SymbolTable,
        config?: Partial<Config>,
        typeTable?: TypeTable
    );

    /**
     * @summary 执行一个 tick
     *
     * @desc 至多执行 `config.maxInstructions` 条语句：遇到 `yield` 提前结束本 tick，遇到 `sleep`
     *       结束本 tick 并进入休眠（休眠期间的 `runTick()` 不执行任何语句），`hcf` 则停机。
     *       调用结束后 tick 计数前进一格，`sleep` 的等待时长按 `config.tickDuration` 换算。
     *
     * @example
     * ```typescript
     * engine.runTick();   // 本 tick 的指令预算用完，或碰到 yield/sleep/hcf
     * engine.runTick();   // 下一个 tick
     * ```
     */
    runTick(): void;

    /**
     * @summary 执行到停机
     *
     * @desc 反复调用 {@link Engine.step} 直到停机。内部有总指令数熔断阈值（默认 100 万条，
     *       用于兜住死循环与无限 `sleep`），达到阈值即停机。
     *
     * @example
     * ```typescript
     * engine.runFull();
     * ```
     */
    runFull(): void;

    /**
     * @summary 单步执行原语：执行恰好一条语句
     *
     * @returns `true` 表示仍可继续执行；`false` 表示已 halt（或执行失败/暂停）
     *
     * @desc `runTick()` / `runFull()` 都基于该原语循环实现，调试器的“逐步骤”也应调用本方法。
     *       注意标签、`alias`、`define`、`#>` 注解块同样占一步（执行它们只是推进 `pc`）。
     *
     * @example
     * ```typescript
     * while (engine.step()) {
     *     console.log(engine.context.pc);   // 每执行一条语句看一次
     * }
     * ```
     */
    step(): boolean;

    /**
     * @summary 执行上下文
     *
     * @returns 本引擎持有的 {@link Context}（内存、设备管理器、配置都在其中）
     */
    get context(): Context;

    /**
     * @summary 运行期诊断
     *
     * @returns 诊断列表，元素形如 `{level, id, start, end, message}`
     *
     * @desc 与编译阶段无关：这里只有执行期问题（无法求值的操作数 `IEM2_1`、栈溢出 `IEM0`、
     *       索引越界 `IEM3`、错误语句 `IEC4`、无效配置 `IEC2_1` 等）。消息语言由
     *       `IC10RuntimeLocal.setLanguage` 控制。
     *
     * @example
     * ```typescript
     * for (const d of engine.diagnostics)
     *     console.log(`${d.level} ${d.id} 第 ${d.start.line} 行：${d.message}`);
     * ```
     */
    get diagnostics(): Diagnostic[];

}
