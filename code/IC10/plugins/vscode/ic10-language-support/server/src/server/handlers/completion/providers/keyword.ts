// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file keyword.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 20:16
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItem, CompletionItemKind } from "vscode-languageserver";

import { INS_META_MAP } from "../../../../mateData";
import { CompletionProviderContext } from "./types";
import { DescriptionSolver, RadixTree } from "../../../../utils";


const INSTRUCTIONS_META_TREE = RadixTree.fromMap(INS_META_MAP);

export function provideKeyword(ctx: CompletionProviderContext, prefix: string): CompletionItem[] {
    return (
        INSTRUCTIONS_META_TREE.entriesWithPrefix(prefix)
            // 过滤掉不是指令的元数据
            .filter(([, v]) => v.type === "Instruction")
            .map(([key, value]) => ({
                label: key,
                kind: CompletionItemKind.Keyword,
                insertText: key,
                detail: value.signature,
                data: { description: DescriptionSolver.parse(value.desc) }
            }))
    );
}
