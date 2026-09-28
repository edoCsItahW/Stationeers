// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file engine.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/12 14:46
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { SymbolTable, Diagnostic } from "@ic10/compiler";
import type { TypeTable } from "@ic10/compiler";
import type { Program } from "@ic10/compiler";
import type { Context } from "./context";
import type { Config } from "./config";


export class Engine {

    /**
     * @param typeTable 语义分析产出的类型表（`analyser.typeTable`）。枚举常量操作数
     *                  （如 `s d0 Color Color.Green`）的求值依赖它，缺省时这类操作数无法求值。
     */
    constructor(
        program: Program,
        symbols: SymbolTable,
        config?: Partial<Config>,
        typeTable?: TypeTable
    );

    runTick(): void;

    runFull(): void;

    /**
     * 单步执行原语：执行恰好一条语句。
     * @returns true 表示仍可继续执行；false 表示已 halt（或执行失败/暂停）。
     */
    step(): boolean;

    get context(): Context;

    get diagnostics(): Diagnostic[];

}
