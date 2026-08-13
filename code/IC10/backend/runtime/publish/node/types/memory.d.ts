// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file memory.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/12 14:51
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Config } from "./config";


export class Memory {

    public sp: number;

    constructor(public config: Config);

    toJSON(): string;

    getReg(name: string): number;

    setReg(name: string, value: number): void;

    getStack(index: number): number;

    setStack(index: number, value: number): void;

    push(value: number): void;

    pop(): number;

    peek(): number;

    poke(index: number, value: number): number;

}

export interface MemoryInfo {
    registers: { [K in `r${0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15}`]: number; };
    stack: number[];
    sp: number;
}
