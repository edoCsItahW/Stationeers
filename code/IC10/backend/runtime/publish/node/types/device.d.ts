// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/12 14:06
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export class Device{
    readLogic(prop: string): number;

    writeLogic(prop: string, value: number): void;

    canReadLogic(prop: string): boolean;

    canWriteLogic(prop: string): boolean;

    readStack(index: number): number;

    writeStack(index: number, value: number): void;

    readSlot(index: number, slot: string): number;

    writeSlot(index: number, slot: string, value: number): void;

    readReagent(mode: number): number;

    queryReagentAmount(reagentHash: number): number;

    getTypeHash(): number;

    getNameHash(): number;

    clearStack(): void;

    tick(): void;
}
