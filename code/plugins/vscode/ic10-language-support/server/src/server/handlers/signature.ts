// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file signature.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/27 22:47
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { ErrorNode, ExecutableInstructionNode, StatementNode } from "ic10-node-api";
import { Connection } from "vscode-languageserver";

import { findCurrentOperand, getOperandIndex } from "./completion/utils";
import { debug, lowerBound, Optional, Console } from "common";
import { INS_META_MAP, INS_LOCAL_MAP } from "../../mateData";
import { locale, t } from "../../locals/locale";
import { DocumentCache } from "../cache";

type OnSignatureHelpHandlerType = Parameters<Connection["onSignatureHelp"]>[0];

export class SignatureHandler {
    constructor(private readonly docCache: DocumentCache) {}

    @debug({
        message: err => t("server.handler.error", { name: "signature", err: (err as Error).message }),
        logger: msg => Console.error(msg, "signature"),
        rethrow: false
    })
    handle(
        ...[
            {
                textDocument,
                context: ctx,
                position: { line, character }
            }
        ]: Parameters<OnSignatureHelpHandlerType>
    ): ReturnType<OnSignatureHelpHandlerType> {
        const cache = this.docCache.getCache(textDocument.uri);

        if (!cache || !cache.ast || !cache.symbols) return;

        const L = line + 1;
        const C = character + 1;

        const stmtIdx = lowerBound(cache.ast.statements, stmt => stmt.position.line >= L);
        const stmt = cache.ast.statements[stmtIdx];

        if (!stmt) return;

        let keyword: Optional<string> = undefined;

        if (this.isInstruction(stmt)) keyword = stmt.keyword;
        else if (stmt.type === "AliasDirective") keyword = "alias";
        else if (stmt.type === "DefineDirective") keyword = "define";

        if (!keyword) return;

        const doc = INS_META_MAP.get(keyword)!.signature;
        const local = INS_LOCAL_MAP.get(keyword)!;

        if (this.isInstruction(stmt)) {
            if (stmt.keyword && ctx && ctx.triggerCharacter) {
                const operand = findCurrentOperand(stmt, ctx.triggerCharacter);

                if (!operand) return;

                const idx = getOperandIndex(operand[0]);

                return {
                    signatures: [
                        {
                            label: doc,
                            documentation: local["desc"][locale.getLocale()],
                            parameters: this.getOperandPositions(doc).map(item => ({
                                label: item
                            })),
                            activeParameter: idx - 1
                        }
                    ]
                };
            }
        } else if (stmt.type === "AliasDirective" || stmt.type === "DefineDirective") {
            if (ctx && ctx.triggerCharacter) {
                let idx = 0;

                if (stmt.identifier.type === "Identifier") idx = 1;

                return {
                    signatures: [
                        {
                            label: doc,
                            documentation: local["desc"][locale.getLocale()],
                            parameters: this.getOperandPositions(doc).map(item => ({
                                label: item
                            })),
                            activeParameter: idx
                        }
                    ]
                };
            }
        }
    }

    private getOperandPositions(input: string): [number, number][] {
        const regex = /\S+/g;
        const tokens: { value: string; start: number; end: number }[] = [];
        let match: RegExpExecArray | null;

        while ((match = regex.exec(input)) !== null)
            tokens.push({ value: match[0], start: match.index, end: match.index + match[0].length });

        // 若只有 keyword 或完全为空，则返回空数组
        if (tokens.length < 2) return [];

        // 跳过第一个 token（keyword），返回后续所有 token 的位置
        return tokens.slice(1).map(t => [t.start, t.end]);
    }

    private isInstruction(stmt: StatementNode): stmt is Exclude<ExecutableInstructionNode, ErrorNode> {
        return stmt.type.endsWith("Instruction");
    }
}
