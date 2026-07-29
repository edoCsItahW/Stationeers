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
import { StatementNode, PureExeInstructionNode } from "ic10-node-api";
import { Connection } from "vscode-languageserver";

import { findCurrentOperand, getOperandIndex } from "./completion/utils";
import { debug, lowerBound, Optional, Console } from "common";
import { INS_META_MAP, INS_LOCAL_MAP } from "../../mateData";
import { locale, t } from "../../locals/locale";
import { DocumentCache } from "../cache";

type OnSignatureHelpHandlerType = Parameters<Connection["onSignatureHelp"]>[0];

/**
 * @summary IC10 函数签名帮助处理器
 *
 * @summary IC10 function signature help handler
 *
 * @desc 响应用户输入的触发字符（如空格、逗号）提供指令和预处理指令的函数签名帮助。
 * 根据当前 AST 上下文识别指令关键字或预处理指令（alias/define），
 * 从元数据中检索对应的签名信息和本地化描述，标记当前活跃参数，
 * 并计算各操作数在签名中的位置范围。
 *
 * @desc Provides function signature help for instructions and preprocessor directives
 * when trigger characters (e.g., space, comma) are entered. Identifies the instruction
 * keyword or preprocessor directive (alias/define) from the current AST context,
 * retrieves the corresponding signature and localized description from metadata,
 * marks the active parameter, and computes operand position ranges within the signature.
 * */
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

    private isInstruction(stmt: StatementNode): stmt is PureExeInstructionNode {
        return stmt.type.endsWith("Instruction");
    }
}
