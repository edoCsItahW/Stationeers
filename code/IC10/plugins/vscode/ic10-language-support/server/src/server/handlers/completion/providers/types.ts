// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:27
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { OperandType, Statement, SymbolMap, TypeTableMap } from "ic10c-node";
import { CompletionItem } from "vscode-languageserver";

import { locale } from "../../../../locals";
import type { Optional } from "common";


export interface CompletionProviderContext {
    stmt: Optional<Statement>;
    symbols: Optional<SymbolMap>;
    types: Optional<TypeTableMap>;
    getLocale(): ReturnType<typeof locale.getLocale>;
}

export type OperandProvider = (ctx: CompletionProviderContext, opType: OperandType, prefix: string) => CompletionItem[];

export interface BuiltinSymbolInfo {
    value: string;
    sort: string;
    data: {
        description: {
            nodeName: "Link";
            paths: string[];
            fields: string[];
        };
    };
}
