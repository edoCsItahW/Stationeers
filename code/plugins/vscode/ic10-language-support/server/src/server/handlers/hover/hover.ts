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
