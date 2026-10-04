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

import type { BuiltinSymbolInfo, OperandProvider } from "./types";
import { ADDRESSABLE_REGISTERS } from "./register";
import { t } from "../../../../locals";

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

const DEVICES = [...ORDINARY_DEVICES, SELF_REFERENCE_DEVICE];

export const provideDevice: OperandProvider = (ctx, opType, prefix) => {
    // 内置设备
    const items = DEVICES.filter(d => {
        if (!d.value.startsWith(prefix)) return false;

        // 定义模式：已有别名 → 不显示
        return !(ctx.isDefine && ctx.symbolMap?.[d.value]);
    }).map(d => {
        const res = deviceItem(d);
        // 使用模式：已有别名 → 沉底，让别名优先
        if (!ctx.isDefine) {
            const alias = ctx.symbolMap?.[d.value];
            // 两档都带前缀，确保排在别名之后
            // 有别名 → z1 沉底；无别名 → z0 靠前
            res.sortText = alias ? `z1_${d.sort}` : `z0_${d.sort}`;

            if (alias)
                res.labelDetails = {
                    detail: ` = ${alias}`,
                    description: t("hover.operandType.device")
                };
        }

        return res;
    });

    if (prefix && /^dr+$/.test(prefix))
        items.push(
            ...ADDRESSABLE_REGISTERS.map(({ value, sort }): BuiltinSymbolInfo => ({
                value: prefix + value.slice(1), // dr + 0 => dr0；drr + a => drra
                sort,
                data: {
                    description: {
                        nodeName: "Link",
                        paths: ["locals", "builtin"],
                        fields: ["ordinary_devices", "desc"]
                    }
                }
            })).map(deviceItem)
        );

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
