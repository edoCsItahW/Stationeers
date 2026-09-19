// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com


/**
 * @file device.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:24
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItem, CompletionItemKind } from "vscode-languageserver";

import { BuiltinSymbolInfo, OperandProvider } from "./types";
import { t } from "../../../../locals";
import { TokenType } from "ic10c-node";


const ORDINARY_DEVICES: BuiltinSymbolInfo[] = Array.from({ length: 6 }).map((_, i) => ({
    value: `d${i}`,
    sort: "d" + i.toString().padStart(2, "0"),
    data: {
        description: {
            nodeName: "Link",
            paths: ["locals", "builtin"],
            fields: ["ordinary_devices", "desc"]
        }
    }
}));

const SELF_REFERENCE_DEVICE: BuiltinSymbolInfo = {
    value: "db",
    sort: "db",
    data: {
        description: {
            nodeName: "Link",
            paths: ["locals", "builtin"],
            fields: ["self_reference_device", "desc"]
        }
    }
};

const DEIVCES = [...ORDINARY_DEVICES, SELF_REFERENCE_DEVICE];

export const provideDevice: OperandProvider = (ctx, opType, prefix) => {
    // 内置设备
    const items = DEIVCES.filter(d => d.value.startsWith(prefix)).map(deviceItem);

    // 动态寻址设备
//    if (prefix.length) items.push(...DEIVCES.map(deviceItem));
    // TODO: 动态寻址应该动态的前缀添加到补全项中

    return items;
};

const deviceItem = ({ value, sort, data }: BuiltinSymbolInfo): CompletionItem => ({
    label: value,
    kind: CompletionItemKind.Reference,
    insertText: value,
    detail: t("hover.operandType.device"),
    sortText: sort,
    data
});
