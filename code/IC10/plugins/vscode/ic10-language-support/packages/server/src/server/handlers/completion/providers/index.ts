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
import { CompletionItem } from "vscode-languageserver";
import { OperandType, Token } from "@ic10/compiler";

import { EnumKeyMap, GenericOperandType, SemanticMap } from "../../../../utils";
import type { CompletionProviderContext, OperandProvider } from "./types";
import { provideSemanticEnum, provideGrammaticalEnum } from "./enum";
import { provideIdentifier } from "./identifier";
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


export function provideOperand(ctx: CompletionProviderContext, opType: OperandType, prefix: string): CompletionItem[] {
    const generics = SemanticMap[opType];
    if (!generics) return [];

    // 书写形式对应的提供器，外加语义枚举：枚举值以标识符书写（`Setting`），
    // 因此它由 EnumKeyMap 声明而非 SemanticMap 的书写形式（见 EnumKeyMap 注释）
    const providers: OperandProvider[] = generics.map(g => COMPLETE_PROVIDERS[g]);

    if (Object.prototype.hasOwnProperty.call(EnumKeyMap, opType)) providers.push(provideSemanticEnum);

    type KeyType = `${string}:${string}`;
    const seen = new Set<KeyType>();
    const result: CompletionItem[] = [];

    for (const provider of providers)
        for (const item of provider(ctx, opType, prefix)) {
            const key = `${item.kind}:${item.label}` as const;

            if (seen.has(key)) continue;

            seen.add(key);

            result.push(item);
        }

    return result;
}


export {
    provideRegister,
    provideKeyword,
    provideDevice,
    provideNumber,
    provideSemanticEnum
}
