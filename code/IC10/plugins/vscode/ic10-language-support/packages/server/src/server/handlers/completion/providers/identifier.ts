// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file identifier.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:25
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItem, CompletionItemKind } from "vscode-languageserver";
import { BasicType, Symbol, TypeCategory } from "@ic10/compiler";

import type { CompletionProviderContext, OperandProvider } from "./types";
import { SemanticMap } from "../../../../utils";
import { t } from "../../../../locals";

export const provideIdentifier: OperandProvider = (ctx, opType, prefix) => {
    const result: CompletionItem[] = [];

    const types = SemanticMap[opType];

    // 再分派
    for (const type of types)
        switch (type) {
            case "register":
                result.push(
                    ...symbolWhere(
                        ctx,
                        prefix,
                        s => ({
                            detail: t("hover.operandType.register"),
                            labelDetails: { detail: `: ${s.value}`, description: t("hover.operandType.register") }
                        }),
                        s => s.type === BasicType.REGISTER
                    )
                );
                break;
            case "device":
                result.push(
                    ...symbolWhere(
                        ctx,
                        prefix,
                        s => ({
                            detail: t("hover.operandType.device"),
                            labelDetails: {
                                detail: s.value ? `: ${s.value}` : undefined,
                                description: t("hover.operandType.device")
                            }
                        }),
                        s => s.type === BasicType.DEVICE
                    )
                );
                break;
            case "number":
                result.push(
                    ...symbolWhere(
                        ctx,
                        prefix,
                        s => ({
                            detail: t("hover.operandType.number"),
                            labelDetails: {
                                detail: s.value ? `: ${s.value}` : undefined,
                                description: t("hover.operandType.number")
                            }
                        }),
                        s =>
                            s.category in
                            [TypeCategory.NUMBER, TypeCategory.LABEL, TypeCategory.HASH_CALL, TypeCategory.STR_CALL]
                    )
                );
                break;
            default:
                break;
        }

    return result;
};

function symbolWhere(
    ctx: CompletionProviderContext,
    prefix: string,
    itemCallback: (symbol: Symbol) => Partial<CompletionItem>,
    conditionCallback: (symbol: Symbol) => boolean
): CompletionItem[] {
    if (!ctx.symbols) return [];

    return Object.entries(ctx.symbols.symbols)
        .filter(([name, sym]) => name.startsWith(prefix) && conditionCallback(sym))
        .map(([name, sym]) => ({
            label: name,
            kind: CompletionItemKind.Variable,
            insertText: name,
            data: {
                description: sym.desc
            },
            ...itemCallback(sym)
        }));
}
