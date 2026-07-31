// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file hover.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/25
 * @desc Hover handler — dispatches to typed providers for each statement kind.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 */

import type { Connection } from "vscode-languageserver/node";

import type { HoverContext, HoverProvider } from "./types";
import { t, locale } from "../../../locals/locale";
import { findStatementAtPosition } from "./utils";
import { DocumentCache } from "../../cache";
import { Console, debug } from "common";
import {
    DefineDirectiveHoverProvider,
    AliasDirectiveHoverProvider,
    InstructionHoverProvider,
    LabelDefHoverProvider
} from "./providers";

type OnHoverHandlerType = Parameters<Connection["onHover"]>[0];

/**
 * @summary 悬停处理器 — 根据语句类型将请求分发给对应的 HoverProvider
 *
 * @summary Hover handler — dispatches requests to the appropriate HoverProvider based on statement type
 *
 * @desc 管理一组 HoverProvider（LabelDef、AliasDirective、DefineDirective、Instruction），
 *  遍历查找能处理当前语句的提供器，由其生成 Markdown 格式的悬停提示。
 *
 * @desc Manages a set of HoverProviders (LabelDef, AliasDirective, DefineDirective, Instruction),
 *  iterates to find the one that can handle the current statement, and generates Markdown hover tooltips.
 * */
export class HoverHandler {
    private providers: HoverProvider[];

    constructor(private docCache: DocumentCache) {
        this.providers = [
            new LabelDefHoverProvider(),
            new AliasDirectiveHoverProvider(),
            new DefineDirectiveHoverProvider(),
            new InstructionHoverProvider()
        ];
    }

    /**
     * @summary 处理悬停事件 — 定位光标所在语句并生成悬停提示
     *
     * @summary Handle hover event — locate the statement at cursor and generate hover tooltip
     *
     * @param params LSP hover 参数，包含 textDocument URI 和光标位置
     * @param params LSP hover parameters, including textDocument URI and cursor position
     *
     * @returns 悬停内容（Markdown 格式），若无相关内容返回空
     * @returns Hover content (Markdown format), or empty if nothing relevant
     * */
    @debug({
        message: err => t("server.handler.error", { name: "hover", err: (err as Error).message }),
        logger: msg => Console.error(msg, "hover"),
        rethrow: false
    })
    public handle(...[{ position, textDocument }]: Parameters<OnHoverHandlerType>): ReturnType<OnHoverHandlerType> {
        const line = position.line + 1;
        const character = position.character + 1;
        const cache = this.docCache.getCache(textDocument.uri);

        if (!cache?.ast) return { contents: [] };

        const stmt = findStatementAtPosition(cache.ast.statements, line);
        if (!stmt) return { contents: [] };

        const ctx: HoverContext = {
            line,
            character,
            symbols: cache.symbols,
            statements: cache.ast.statements,
            getLocale: () => locale.getLocale(),
            t: (key, ...args) => t(key as any, ...args)
        };

        for (const provider of this.providers)
            if (provider.canHandle(stmt)) {
                const result = provider.provideHover(stmt, ctx);
                if (result) return result;
            }

        return { contents: [] };
    }
}
