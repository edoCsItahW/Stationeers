// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file config.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/28
 * @desc 运行时配置：导出 {@link Config}，由 `Engine` / `Context` / `Memory` 的构造函数消费。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 运行时配置
 *
 * @desc 控制执行循环的节奏与内存规模。构造 `Engine` 时只需给出关心的字段（声明中为
 *       `Partial<Config>`），其余字段保持默认值。
 *
 * @note 未被列出的 C++ 侧配置项（`maxTotalInstructions` 总指令数熔断阈值、
 *       `allowErrorStatements` 是否把错误语句降级为警告）**尚未暴露给绑定层**，始终取默认值。
 *
 * @example
 * ```typescript
 * import { Engine } from '@ic10/runtime';
 *
 * const engine = new Engine(program, analyser.symbolTable, {
 *     tickDuration: 0.5,   // 每 tick 0.5 秒（游戏默认）
 *     maxInstructions: 128 // 每 tick 最多 128 条指令
 * });
 * ```
 *
 * @public
 */
export interface Config {
    /**
     * @summary 单个 tick 代表的游戏内时长（秒）
     *
     * @desc `sleep n` 会换算成 `n / tickDuration` 个 tick 来等待，因此该值同时决定了休眠的
     *       精度。必须为正数：小于等于 0 时 `sleep` 会报告 `IEC2_1`（无效配置参数）。
     *
     * @default 0.5
     */
    tickDuration: number;

    /**
     * @summary 单个 tick 内允许执行的最大指令数
     *
     * @desc `runTick()` 每调用一次最多执行这么多条语句（标签、`alias`/`define` 等非指令语句同样
     *       计入），用于模拟 IC10 芯片的每 tick 指令预算。
     *
     * @default 128
     */
    maxInstructions: number;

    /**
     * @summary 栈的最大容量（以 double 元素计）
     *
     * @desc 超过该容量 `push` 会报告 `IEM0`（栈溢出），`poke` 越界会报告 `IEM3`（索引越界）。
     *
     * @default 512
     */
    maxStackSize: number;

    /**
     * @summary 是否严格求值
     *
     * @desc 启用时，无法求值的操作数（例如尚未赋值的设备逻辑字段、未知的 `define` 哈希常量）
     *       会报告 `IEM2_1`（无法求值）并中止本次 tick；关闭时这些操作数按 `0` 参与运算，
     *       便于在调试器中观察程序行为。
     *
     * @default true
     */
    strictEvaluation: boolean;
}
