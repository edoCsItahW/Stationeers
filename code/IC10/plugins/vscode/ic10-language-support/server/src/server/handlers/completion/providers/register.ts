// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file register.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:24
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItemKind, CompletionItem } from "vscode-languageserver";

import { BuiltinSymbolInfo, OperandProvider } from "./types";
import { t } from "../../../../locals";


const GENERAL_PURPOSE_REGISTERS: BuiltinSymbolInfo[] = Array.from({ length: 16 }).map((_, i) => ({
    value: `r${i}`,
    sort: "r" + i.toString().padStart(2, "0"),
    data: {
        description: {
            nodeName: "Link",
            paths: ["locals", "builtin"],
            fields: ["general_purpose_registers", "desc"]
        }
    }
}));

const ADDRESS_REGISTER: BuiltinSymbolInfo = {
    value: "ra",
    sort: "ra",
    data: {
        description: {
            nodeName: "Link",
            paths: ["locals", "builtin"],
            fields: ["address_register", "desc"]
        }
    }
};

const STACK_POINTER_REGISTER: BuiltinSymbolInfo = {
    value: "sp",
    sort: "sp",
    data: {
        description: {
            nodeName: "Link",
            paths: ["locals", "builtin"],
            fields: ["stack_pointer_register", "desc"]
        }
    }
};

const ADDRESSABLE_REGISTERS = [...GENERAL_PURPOSE_REGISTERS, ADDRESS_REGISTER];

const REGISTERS = [...ADDRESSABLE_REGISTERS, STACK_POINTER_REGISTER];

export const provideRegister: OperandProvider = (ctx, opType, prefix) => {
    // 内置寄存器
    const items = REGISTERS.filter(r => r.value.startsWith(prefix)).map(registerItem);

    // 动态寻址寄存器
    //    if (prefix.length) items.push(...ADDRESSABLE_REGISTERS.map(registerItem));
    // TODO: 动态寻址应该动态的前缀添加到补全项中

    return items;
};


const registerItem = ({ value, sort, data }: BuiltinSymbolInfo): CompletionItem => ({
    label: value,
    kind: CompletionItemKind.Variable,
    insertText: value,
    detail: t("hover.operandType.register"),
    sortText: sort,
    data
});
