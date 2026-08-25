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
import { SymbolTable, Diagnostic } from "ic10c-node";
import { Program } from "ic10c-node";
import { Context } from "./context";
import { Config } from "./config";


export class Engine {

    constructor(program: Program, symbols: SymbolTable, config?: Partial<Config>);

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
