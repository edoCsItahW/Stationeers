// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file number.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:25
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItem, CompletionItemKind, InsertTextFormat } from "vscode-languageserver";
import { instructions } from "@ic10/metadata/std";

import { DescriptionSolver } from "../../../../utils";
import type { OperandProvider } from "./types";


type SpecialNumberKey = "%" | "$" | "HASH" | "STR";

const NUMBER_META = {
    "%": {
        value: instructions["binary_number"],
        name: "binary_number"
    },
    $: {
        value: instructions["hash_number"],
        name: "hash_number"
    },
    HASH: {
        value: instructions["hash"],
        name: "hash"
    },
    STR: {
        value: instructions["str"],
        name: "str"
    }
} satisfies Record<SpecialNumberKey, any>;

interface Shortcut extends CompletionItem {
    label: SpecialNumberKey;
}

const NUMBER_SHORTCUTS: Shortcut[] = [
    { label: "%", kind: CompletionItemKind.Operator, insertText: "%" },
    { label: "$", kind: CompletionItemKind.Operator, insertText: "$" },
    {
        label: "HASH",
        kind: CompletionItemKind.Function,
        insertText: 'HASH("$0")',
        insertTextFormat: InsertTextFormat.Snippet
    },
    {
        label: "STR",
        kind: CompletionItemKind.Function,
        insertText: 'STR("$0")',
        insertTextFormat: InsertTextFormat.Snippet
    }
];


export const provideNumber: OperandProvider = (ctx, opType, prefix) => {
    return NUMBER_SHORTCUTS.filter(i => i.label.startsWith(prefix))
        .map(i => {
            const meta = NUMBER_META[i.label];

            return {
                ...i,
                detail: meta.value.signature,
                data: {
                    description: DescriptionSolver.parse(meta.value.desc)
                }
            };
        });
};

