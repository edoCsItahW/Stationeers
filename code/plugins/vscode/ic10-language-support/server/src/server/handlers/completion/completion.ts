// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file completion.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/26 11:58
 * @desc Completion handler — dispatches to typed providers based on trigger context.
 *       Operand types are read directly from AST node's `type1`/`type2`... fields.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Connection } from "vscode-languageserver/node";

import type { CompletionContext, CompletionProvider } from "./types";
import { INS_LOCAL_MAP } from "../../../mateData";
import { locale } from "../../../locals/locale";
import { DocumentCache } from "../../cache";
import {
    KeywordCompletionProvider,
    OperandCompletionProvider,
    DirectiveCompletionProvider,
} from "./providers";
import {
    findStatementAtPosition,
    tokensOnLine,
    cursorPrefix,
} from "./utils";


type OnCompletionHandlerType = Parameters<Connection["onCompletion"]>[0];
type OnCompletionResolveHandlerType = Parameters<Connection["onCompletionResolve"]>[0];


export class CompletionHandler {
    private readonly providers: CompletionProvider[];

    constructor(private readonly docCache: DocumentCache) {
        // 注册顺序决定优先级：Directive > Operand > Keyword
        this.providers = [
            new DirectiveCompletionProvider(),
            new OperandCompletionProvider(),
            new KeywordCompletionProvider(),
        ];

    }

    handle(...[{ textDocument, position: { line, character }, context }]: Parameters<OnCompletionHandlerType>): ReturnType<OnCompletionHandlerType> {

        // 转换为 1-based 坐标
        const L = line + 1;
        const C = character + 1;

        const cache = this.docCache.getCache(textDocument.uri);
        if (!cache?.ast || !cache.tokens) return [];

        const stmt  = findStatementAtPosition(cache.ast.statements, L);

        const token = cache.tokens.filter(t => t.pos.line === line).reverse().find(t => t.pos.column < character);
        const prefix = context?.triggerCharacter ? "" : token?.lexeme ?? "";

        const ctx: CompletionContext = {
            line: L,
            character: C,
            triggerKind: context?.triggerKind ?? 1,
            triggerCharacter: context?.triggerCharacter,
            token,
            stmt,
            cache,
            symbols: cache.symbols,
            prefix,
            getLocale: () => locale.getLocale(),
        };

        // 遍历 provider，第一个 canHandle 的独占响应（包括空结果）
        for (const provider of this.providers)
            if (provider.canHandle(ctx))
                return provider.provide(ctx);

        return [];
    }

    handleResolve(...[params]: Parameters<OnCompletionResolveHandlerType>): ReturnType<OnCompletionResolveHandlerType> {
        const key = params.data?.name;
        const local = params.data?.local;

        if (key && local && INS_LOCAL_MAP.has(key)) {
            const doc = INS_LOCAL_MAP.get(key)!;

            params.documentation = (doc["desc"] as any)[local];
        }

        return params;
    }
}
