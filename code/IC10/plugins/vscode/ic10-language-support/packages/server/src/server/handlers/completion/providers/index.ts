// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file index.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:24
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { BasicType, OperandType } from "@ic10/compiler";
import { CompletionItem } from "vscode-languageserver";
import type { CompletionScope } from "@ic10/common";

import { EnumKeyMap, GenericOperandType, SemanticMap } from "../../../../utils";
import type { CompletionProviderContext, OperandProvider } from "./types";
import { NUMBER_CATEGORY_SET, provideIdentifier } from "./identifier";
import { provideSemanticEnum, provideGrammaticalEnum } from "./enum";
import { provideRegister } from "./register";
import { provideKeyword } from "./keyword";
import { provideDevice } from "./device";
import { provideNumber } from "./number";


const COMPLETE_PROVIDERS: Record<GenericOperandType, OperandProvider> = {
    identifier: provideIdentifier,
    enum: provideGrammaticalEnum,
    register: provideRegister,
    device: provideDevice,
    number: provideNumber
};


export function provideOperand(
    ctx: CompletionProviderContext,
    opType: OperandType,
    prefix: string,
    scope?: CompletionScope
): CompletionItem[] {
    const generics = SemanticMap[opType];
    if (!generics) return [];

    // 书写形式对应的提供器，外加语义枚举：枚举值以标识符书写（`Setting`），
    // 因此它由 EnumKeyMap 声明而非 SemanticMap 的书写形式（见 EnumKeyMap 注释）
    const entries: [GenericOperandType, OperandProvider][] = generics.map(g => [g, COMPLETE_PROVIDERS[g]]);

    if (Object.prototype.hasOwnProperty.call(EnumKeyMap, opType)) entries.push(["enum", provideSemanticEnum]);

    // 范围收窄在下面按**每个候选**判定，而不是在这里按提供器族过滤：别名以标识符书写
    // （`alias a r1`），所以寄存器/设备/数字范围都要把"标识符写法但语义相符"的候选一并收进来
    const belongs = (family: GenericOperandType, item: CompletionItem): boolean => {
        if (!scope || scope === "all" || scope === "keyword") return true;
        if (family === scope) return true;

        // 别名：书写形式是标识符，语义由符号表决定
        if (family !== "identifier") return false;

        const symbol = ctx.symbols?.symbols[item.label];

        if (!symbol) return false;

        switch (scope) {
            case "register":
                return symbol.type === BasicType.REGISTER;
            case "device":
                return symbol.type === BasicType.DEVICE;
            case "number":
                return NUMBER_CATEGORY_SET.has(symbol.category);
            default:
                return false;
        }
    };

    type KeyType = `${string}:${string}`;
    const seen = new Set<KeyType>();
    const result: CompletionItem[] = [];

    for (const [family, provider] of entries)
        for (const item of provider(ctx, opType, prefix)) {
            if (!belongs(family, item)) continue;

            const key = `${item.kind}:${item.label}` as const;

            if (seen.has(key)) continue;

            seen.add(key);

            result.push(item);
        }

    return result;
}

export { provideRegister, provideKeyword, provideDevice, provideNumber, provideSemanticEnum };
