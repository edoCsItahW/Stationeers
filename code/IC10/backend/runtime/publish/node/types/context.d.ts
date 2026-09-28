// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/12 14:49
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { SymbolTable } from "@ic10/compiler";
import type { TypeTable } from "@ic10/compiler";
import type { Program } from "@ic10/compiler";
import type { Manager } from "./manager";
import type { Memory } from "./memory";
import type { Config } from "./config";


export class Context {
    public config: Config;

    /**
     * @param typeTable 语义分析产出的类型表（`analyser.typeTable`）。枚举常量操作数
     *                  （如 `s d0 Color Color.Green`）的求值依赖它，缺省时这类操作数无法求值。
     */
    constructor(
        program: Program,
        symbols: SymbolTable,
        cfg: Config,
        typeTable?: TypeTable
    );

    pc: number;

    get memory(): Memory;

    get manager(): Manager;

    halt(): void;

    get halted(): boolean;

    sleep(seconds: number): void;

    get isSleeping(): boolean;

    getAddr(line: number): number | undefined;

    getLine(addr: number): number | undefined;

}
