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
import { CompletionItem, CompletionItemKind } from "vscode-languageserver";
import type { Optional } from "@ic10/common";
import {
    Device,
    DeviceAnnotation,
    DeviceAnnotationLogic,
    DeviceAnnotationLogicSlot,
    DeviceAnnotationSlot,
    EnumAnnotationValue,
    ErrorNode,
    IdentifierNode,
    OperandType,
    Register,
    Statement
} from "@ic10/compiler";

import { AST, EnumKeyMap, operandToString } from "../../../../utils";
import type { OperandProvider } from "./types";
import { t } from "../../../../locals";

export const provideEnum: OperandProvider = (ctx, opType, prefix) => {
    const res = () => provideGlobalEnum(ctx, opType, prefix);

    if (!ctx.stmt || !ctx.symbols || !ctx.types) return res();

    const device = findPrevDevice(ctx.stmt);
    if (!device) return res();

    const symbol = ctx.symbols.symbols[operandToString(device)];
    if (!symbol || !symbol.typeName) return res();

    const type = ctx.types[symbol.typeName];
    if (
        type &&
        AST.isDeviceAnnotation(type) &&
        (opType === OperandType.LOGIC_PROP || opType === OperandType.LOGIC_SLOT_PROP || opType === OperandType.SLOT_IDX)
    )
        return provideDeviceCompletion(type, opType, prefix);

    return res();
};

const provideGlobalEnum: OperandProvider = (ctx, opType, prefix) => {
    if (!ctx.types) return [];

    const key = EnumKeyMap[opType];

    const type = ctx.types[key];
    if (!type || !AST.isEnumAnnotation(type)) return [];

    return type.values.filter(v => v.name.startsWith(prefix)).map(v => enumItem(v, opType));
};

function provideDeviceCompletion(
    type: DeviceAnnotation,
    opType: OperandType.LOGIC_PROP | OperandType.LOGIC_SLOT_PROP | OperandType.SLOT_IDX,
    prefix: string
): CompletionItem[] {
    const key = (
        {
            [OperandType.LOGIC_PROP]: "logics",
            [OperandType.LOGIC_SLOT_PROP]: "logicSlots",
            [OperandType.SLOT_IDX]: "slots"
        } as const
    )[opType];

    return type[key].filter(l => l.value.startsWith(prefix)).map(enumItem);
}

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

export function enumItem(value: EnumAnnotationValue, opType: OperandType | string): CompletionItem;
export function enumItem(
    item: DeviceAnnotationLogic | DeviceAnnotationSlot | DeviceAnnotationLogicSlot
): CompletionItem;
export function enumItem(
    item: EnumAnnotationValue | DeviceAnnotationLogic | DeviceAnnotationSlot | DeviceAnnotationLogicSlot,
    opType?: OperandType | string
): CompletionItem {
    let key =
        typeof opType === "string"
            ? opType
            : EnumKeyMap[
                  {
                      value: opType!, // 据重载，value时，opType非空
                      logic: OperandType.LOGIC_PROP,
                      "logic-slot": OperandType.LOGIC_SLOT_PROP,
                      slot: OperandType.SLOT_IDX
                  }[item.tag]
              ];

    const detail = Object.values(EnumKeyMap).find(k => k === key)
        ? t(`hover.operandType.${key.charAt(0).toLowerCase() + key.slice(1)}` as any)
        : key;

    return {
        label: item.name,
        kind: CompletionItemKind.Constant,
        insertText: item.name,
        detail: detail,
        labelDetails: {
            detail: `: ${item.value}`,
            description: detail
        },
        data: {
            description: item.desc
        }
    };
}
