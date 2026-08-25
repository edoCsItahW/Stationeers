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
import { Diagnostic } from "ic10c-node";

import { DocumentCache } from "../cache";
import { Console, debug } from "common";
import { t } from "../../locals";

type OnHandlerType = Parameters<Languages["diagnostics"]["on"]>[0];

/**
 * @summary 诊断处理器 — 将 IC10 AST 诊断转换为 LSP Diagnostic 格式
 *
 * @summary Diagnostic handler — converts IC10 AST diagnostics into LSP Diagnostic format
 *
 * @desc 从 DocumentCache 中获取当前文档的 AST 诊断信息，将其映射为 VSCode
 *  Languages Server Protocol 的 Diagnostic 对象，包括严重级别转换和坐标映射
 *  （IC10 使用 1-based 坐标，LSP 使用 0-based 坐标）。
 *
 * @desc Retrieves AST diagnostics from the DocumentCache for the current document and
 *  maps them to LSP Diagnostic objects, including severity level translation and
 *  coordinate mapping (IC10 uses 1-based, LSP uses 0-based).
 * */
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

    /**
     * @summary 处理诊断请求 — 从缓存返回当前文档的所有诊断信息
     *
     * @summary Handle diagnostic request — return all diagnostics for the current document from cache
     *
     * @param params LSP 诊断参数，包含 textDocument URI
     * @param params LSP diagnostic parameters, including textDocument URI
     *
     * @returns 包含诊断项列表的 full diagnostic report
     * @returns Full diagnostic report containing the list of diagnostic items
     * */
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
                        start: d.start ? { line: d.start.line - 1, character: d.start.column - 1 } : { line: 0, character: 0 },
                        end: d.end ? { line: d.end.line - 1, character: d.end.column - 1 } : { line: 0, character: 0 }
                    },
                    message: d.message,
                    source: d.id.split("_")[0]
                }))
            };

        return { kind: "full", items: [] };
    }
}
