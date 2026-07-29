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

import { KeywordCompletionProvider, OperandCompletionProvider, DirectiveCompletionProvider } from "./providers";
import { ENUMS_LOCAL_MAP, INS_LOCAL_MAP, LOGIC_LOCAL_MAP, LOGIC_SLOT_LOCAL_MAP } from "../../../mateData";
import type { CompletionContext, CompletionData, CompletionProvider } from "./types";
import { locale, t } from "../../../locals/locale";
import { findStatementAtPosition } from "./utils";
import { Optional, Console, debug } from "common";
import { DocumentCache } from "../../cache";
import { Token } from "ic10-node-api";


type OnCompletionHandlerType = Parameters<Connection["onCompletion"]>[0];
type OnCompletionResolveHandlerType = Parameters<Connection["onCompletionResolve"]>[0];

/**
 * @summary 补全处理器 — 根据触发上下文将请求分发给对应的 CompletionProvider
 *
 * @summary Completion handler — dispatches requests to the appropriate CompletionProvider based on trigger context
 *
 * @desc 管理 Keyword、Operand、Directive 三个补全提供器，按优先级遍历，
 *  第一个 canHandle 返回 true 的提供器独占响应。同时负责补全项选中后的
 *  文档异步加载（handleResolve）。
 *
 * @desc Manages three completion providers (Keyword, Operand, Directive), iterating
 *  by priority. The first provider whose canHandle returns true takes exclusive response.
 *  Also handles async documentation loading when a completion item is selected (handleResolve).
 * */
export class CompletionHandler {
    private readonly providers: CompletionProvider[];

    constructor(private readonly docCache: DocumentCache) {
        // 注册顺序决定优先级：Directive > Operand > Keyword
        this.providers = [
            new DirectiveCompletionProvider(),
            new OperandCompletionProvider(),
            new KeywordCompletionProvider()
        ];
    }

    /**
     * @summary 处理补全请求 — 构建上下文并分发给各补全提供器
     *
     * @summary Handle completion request — build context and dispatch to completion providers
     *
     * @param params LSP completion 参数，包含 textDocument、position 和 context
     * @param params LSP completion parameters, including textDocument, position, and context
     *
     * @returns 补全项列表，无可用的补全项则返回空数组
     * @returns List of completion items, or empty array if none available
     * */
    @debug({
        message: err => t("server.handler.error", { name: "completion", err: (err as Error).message }),
        logger: msg => Console.error(msg, "completion"),
        rethrow: false
    })
    handle(
        ...[
            {
                textDocument,
                position: { line, character },
                context
            }
        ]: Parameters<OnCompletionHandlerType>
    ): ReturnType<OnCompletionHandlerType> {
        // 转换为 1-based 坐标
        const L = line + 1;
        const C = character + 1;

        const cache = this.docCache.getCache(textDocument.uri);
        if (!cache?.ast || !cache.tokens) return [];

        const stmt = findStatementAtPosition(cache.ast.statements, L);

        // 反向查找光标前最近的同行动词，避免 filter+reverse+find 的三趟扫描
        let token: Optional<Token>;
        for (let i = cache.tokens.length - 1; i >= 0; i--) {
            const t = cache.tokens[i];
            if (t.pos.line === line && t.pos.column < character) {
                token = t;
                break;
            }
        }
        const prefix = context?.triggerCharacter ? "" : (token?.lexeme ?? "");

        const ctx: CompletionContext = {
            line: L,
            character: C,
            triggerKind: context?.triggerKind ?? 1,
            triggerCharacter: context?.triggerCharacter,
            token,
            stmt,
            cache,
            symbols: cache.symbols,
            types: cache.types,
            prefix,
            getLocale: () => locale.getLocale()
        };

        // 遍历 provider，第一个 canHandle 的独占响应（包括空结果）
        for (const provider of this.providers) if (provider.canHandle(ctx)) return provider.provide(ctx);

        return [];
    }

    /**
     * @summary 处理补全项解析 — 为选中的补全项异步加载详细文档
     *
     * @summary Handle completion resolve — asynchronously load detailed documentation for the selected item
     *
     * @param params 待解析的 CompletionItem，包含 data 字段
     * @param params CompletionItem to resolve, containing a data field
     *
     * @returns 补充了 documentation 字段的补全项
     * @returns Completion item with documentation field populated
     *
     * @remarks 根据 data.key 从 INS_LOCAL_MAP、LOGIC_LOCAL_MAP、
     *  LOGIC_SLOT_LOCAL_MAP 或 ENUMS_LOCAL_MAP 中查找对应语言版本的描述。
     *
     * @remarks Based on data.key, looks up the locale-appropriate description from
     *  INS_LOCAL_MAP, LOGIC_LOCAL_MAP, LOGIC_SLOT_LOCAL_MAP, or ENUMS_LOCAL_MAP.
     * */
    handleResolve(...[params]: Parameters<OnCompletionResolveHandlerType>): ReturnType<OnCompletionResolveHandlerType> {
        const data: Optional<CompletionData> = params.data;

        if (data) {
            switch (data.key) {
                case "Instruction": {
                    const doc = INS_LOCAL_MAP.get(data.name)!;

                    params.documentation = (doc["desc"] as any)[data.local];
                    break;
                }
                case "LogicType": {
                    const doc = LOGIC_LOCAL_MAP.get(data.name)!;

                    if (doc?.["desc"] && data.local in doc["desc"]) params.documentation = doc["desc"][data.local];
                    else params.documentation = t("completion.common.noDesc");

                    break;
                }
                case "LogicSlotType": {
                    const doc = LOGIC_SLOT_LOCAL_MAP.get(data.name)!;

                    if (doc?.["desc"] && data.local in doc["desc"]) params.documentation = doc["desc"][data.local];
                    else params.documentation = t("completion.common.noDesc");

                    break;
                }
                case "BatchMode":
                case "ReagentMode": {
                    const map = ENUMS_LOCAL_MAP.get(data.key)!.enums;

                    const doc = (map as any)[data.name];

                    if (doc?.["desc"] && data.local in doc["desc"]) params.documentation = doc["desc"][data.local];
                    else params.documentation = t("completion.common.noDesc");

                    break;
                }
            }
        }

        return params;
    }
}
