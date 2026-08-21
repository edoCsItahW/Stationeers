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
import { SymbolTable } from "ic10c-node";
import type { Config } from "./config";
import { Program } from "ic10c-node";
import { Manager } from "./manager";
import { Memory } from "./memory";


export class Context {
    public config: Config;

    constructor(program: Program, symbols: SymbolTable, cfg: Config);

    pc: number;

    get memory(): Memory;

    get manager(): Manager;

    halt(): void;

    get halted(): boolean;

    sleep(seconds: number): void;

    get isSleeping(): boolean;

}
