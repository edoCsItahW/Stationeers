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
import { OperandType, Token } from "ic10c-node";

import { GenericOperandType, SemanticMap } from "../../../../utils";
import { CompletionProviderContext, OperandProvider } from "./types";
import { provideIdentifier } from "./identifier";
import { provideRegister } from "./register";
import { provideKeyword } from "./keyword";
import { provideDevice } from "./device";
import { provideNumber } from "./number";
import { provideEnum } from "./enum";


const COMPLETE_PROVIDERS: Record<GenericOperandType, OperandProvider> = {
    register: provideRegister,
    device: provideDevice,
    identifier: provideIdentifier,
    number: provideNumber,
    enum: provideEnum
};


export function provideOperand(ctx: CompletionProviderContext, opType: OperandType, prefix: string, preToken?: Token): CompletionItem[] {
    const generics = SemanticMap[opType];
    if (!generics) return [];

    type KeyType = `${string}:${string}`;
    const seen = new Set<KeyType>();
    const result: CompletionItem[] = [];

    for (const g of generics)
        for (const item of COMPLETE_PROVIDERS[g](ctx, opType, prefix)) {
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
    provideEnum
}
