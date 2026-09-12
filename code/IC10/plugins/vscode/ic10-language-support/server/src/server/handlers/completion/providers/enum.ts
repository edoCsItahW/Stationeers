// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file enum.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:25
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Device, ErrorNode, IdentifierNode, OperandType, Register, Statement } from "ic10c-node";

import { OperandProvider } from "./types";
import { Optional } from "common";

export const provideEnum: OperandProvider = (ctx, opType, prefix) => {
    // TODO: 设备感知
    //    if (ctx.stmt) {
    //        const device = findPrevDevice(ctx.stmt);
    //        if (device)
    //    }

    return [];  // TODO
};

/**
 * @summary 在语句中查找前序设备操作数
 *
 * @summary Find the preceding device operand in a statement
 *
 * @desc 遍历语句节点的 typeN 字段，找到第一个值为 DEV_REF 或 DEV_ALIAS 的
 * 操作数类型字段，然后返回对应的 operandN 节点。用于 completeEnum 的设备感知补全。
 *
 * @desc Iterates over the statement node's `typeN` fields, finds the first
 * field whose value is DEV_REF or DEV_ALIAS, and returns the corresponding
 * `operandN` node. Used by completeEnum for device-aware completion.
 *
 * @param stmt 语句节点 / Statement node
 * @returns 设备/寄存器/标识符节点，或 null / Device, register, identifier node, or null
 * */
function findPrevDevice(stmt: Statement): Optional<Device | Register | IdentifierNode | ErrorNode> {
    // 用 for...in 代替 Object.entries 避免中间数组分配
    for (const attr in stmt) {
        if (!attr.startsWith("type")) continue;
        if ((stmt as any)[attr] === OperandType.DEVICE_REF) {
            const idx = Number.parseInt(attr.replace("type", ""), 10);
            return (stmt as any)[`operand${idx}`];
        }
    }
}
