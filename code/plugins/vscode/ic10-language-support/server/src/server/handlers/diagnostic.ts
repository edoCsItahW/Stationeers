// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file diagnostic.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Languages, DiagnosticSeverity } from "vscode-languageserver";
import { Diagnostic } from "ic10-node-api";

import { DocumentCache } from "../cache";
import { t } from "../../locals/locale";
import { Console, debug } from "common";

type OnHandlerType = Parameters<Languages["diagnostics"]["on"]>[0];

export class DiagnosticHandler {
    constructor(private readonly docCache: DocumentCache) {}

    private levelTrans(level: Diagnostic["level"]): DiagnosticSeverity {
        switch (level) {
            case "info":
                return DiagnosticSeverity.Information;
            case "warning":
                return DiagnosticSeverity.Warning;
            case "error":
                return DiagnosticSeverity.Error;
            default:
                return DiagnosticSeverity.Hint;
        }
    }

    @debug({
        message: err => t("server.handler.error", { name: "diagnostic", err: (err as Error).message }),
        logger: msg => Console.error(msg, "diagnostic"),
        rethrow: false
    })
    handle(...[params]: Parameters<OnHandlerType>): ReturnType<OnHandlerType> {
        const cache = this.docCache.getCache(params.textDocument.uri);

        if (cache)
            return {
                kind: "full",
                items: cache.diagnostics.map(d => ({
                    severity: this.levelTrans(d.level),
                    range: {
                        start: { line: d.start.line - 1, character: d.start.column - 1 },
                        end: { line: d.end.line - 1, character: d.end.column - 1 }
                    },
                    message: d.message,
                    source: d.id.split("_")[0]
                }))
            };

        return { kind: "full", items: [] };
    }
}
