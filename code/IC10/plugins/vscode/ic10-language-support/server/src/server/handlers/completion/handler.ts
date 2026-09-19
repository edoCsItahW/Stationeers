// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file handler.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:23
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { OperandType, Token, TokenCategory, TokenType, TypeTableMap } from "ic10c-node";
import { CompletionItem, CompletionItemKind, Connection } from "vscode-languageserver";

import { AST, DescriptionSolver, end, findRangeTokens, getOperandType } from "../../../utils";
import { Console, debug, lowerBound, traceback } from "common";
import { CompletionProviderContext } from "./providers/types";
import { provideKeyword, provideOperand } from "./providers";
import { combine, RelativeState, State } from "./state";
import { DocumentCache } from "../../cache";
import { locale, t } from "../../../locals";
import { enumItem } from "./providers/enum";


type OnCompletionHandlerType = Parameters<Connection["onCompletion"]>[0];
type OnCompletionResolveHandlerType = Parameters<Connection["onCompletionResolve"]>[0];


export class CompletionHandler {
    constructor(private readonly docCache: DocumentCache) {}

    //    @debug({
    //        message: err => t("server.handler.error", { name: "completion", err: (err as Error).message }),
    //        logger: msg => Console.error(msg, "completion"),
    //        rethrow: false
    //    })
    @traceback()
    handle(
        ...[
            {
                textDocument: { uri },
                context,
                position
            }
        ]: Parameters<OnCompletionHandlerType>
    ): ReturnType<OnCompletionHandlerType> {
        const cache = this.docCache.getCache(uri);

        if (!cache || !cache.ast || !cache.symbols || !context)
            // 无内容则补全关键字
            return provideKeyword({/* provideKeyword不使用context */} as CompletionProviderContext, "");

        // 统一为1-based
        const line = position.line + 1;
        const column = position.character + 1;

        // 当前语句索引
        const stmtIdx = lowerBound(cache.ast.statements, n => n.position.line >= line);

        const ctx: CompletionProviderContext = {
            stmt: cache.ast.statements[stmtIdx]?.position.line === line ? cache.ast.statements[stmtIdx] : undefined,
            symbols: cache.symbols,
            types: cache.types as TypeTableMap,
            getLocale: () => locale.getLocale()
        };

        // 当前行的tokens
        const tokens = cache.tokens.filter(
            t =>
                t.pos.line === line &&
                t.category !== TokenCategory.WHITESPACE &&
                t.category !== TokenCategory.COMMENT &&
                t.type !== TokenType.END
        );

        const { prev: prevIdx, curr: currIdx, next: nextIdx } = findRangeTokens(tokens, column);

        // 光标的前一个token
        const prevToken = tokens[prevIdx];

        // 局部语法上下文解析
        const syntaxCtx = this.detectSyntaxContext(tokens, prevIdx, column, context.triggerCharacter);
        if (syntaxCtx)
            return this.completeSyntax(syntaxCtx, ctx);

        // 前一个token与当前光标间的空格数
        const prevBlocks = prevToken ? column - end(prevToken).column : 0;
        // 操作数索引
        let opIdx = prevIdx; // -1则补keyword(0)，其余补operand${opIdx}

        if (
            prevBlocks > 0 || // 存在空格则应判断为下一个操作数
            opIdx === -1 // 没有前一个token表示位于行首，则加1成0
        )
            opIdx++;

        // 相对位置状态，该形式仅用于简化分支，其中1为特殊情况，使其命中正确的索引
        const rel: RelativeState = [
            [RelativeState.INSIDE_WORD, RelativeState.END_WORD],
            [RelativeState.START_WORD, currIdx > 0 ? RelativeState.INSIDE_WORD : RelativeState.INSIDE_GAP]
        ][prevIdx >= 0 ? Number(prevBlocks > 0 /* 光标前的空格数 */) : 1][
            nextIdx >= 0 ? Number(tokens[nextIdx].pos.column - column > 0 /* 光标后的空格数 */) : 1
        ];

        const state = combine(rel, context.triggerKind);

        // 注: CompletionTriggerKind是动作类型，RelativeState是动作结束后的状态，几乎可以推导出动作前的状态，除非动作前是INSIDE_GAP或END_WORD
        // 虽然CompletionTriggerKind后来发现没有影响逻辑，但保留可以从理论上解释用户的行为意图
        switch (state) {
            // 一个空格说明想结束当前单词并补全下一个词
            // 多个空格说明是在缩进或对齐
            case State.INSIDE_GAP_TRIGGER_CHAR:
            // 仅补全下一个词
            case State.INSIDE_GAP_INVOKED: {
                if (prevBlocks === 1 || state === State.INSIDE_GAP_INVOKED) return this.completeWord(ctx, opIdx);

                break;
            }

            // 从中间补全或结尾补全当前词
            case State.INSIDE_WORD_TRIGGER_INCOMPLETE:
            case State.INSIDE_WORD_INVOKED:
            case State.END_WORD_TRIGGER_INCOMPLETE:
            case State.END_WORD_INVOKED:
                const inside = rel === RelativeState.INSIDE_WORD;
                const token = tokens[inside ? currIdx : prevIdx];
                return this.completeWord(
                    ctx,
                    opIdx,
                    token.lexeme.substring(0, column - 1)
                );

            // 没有明确意图，重新弹出该位置的补全
            case State.START_WORD_INVOKED:
                return this.completeWord(ctx, opIdx);

            // 拆开一个词，不做任何事
            case State.START_WORD_TRIGGER_CHAR:

            // 不可能的状态
            case State.INSIDE_WORD_TRIGGER_CHAR:
            case State.END_WORD_TRIGGER_CHAR:
            case State.INSIDE_GAP_TRIGGER_INCOMPLETE:
            case State.START_WORD_TRIGGER_INCOMPLETE:
                return;
        }
    }

    @debug({
        message: err => t("server.handler.error", { name: "completion resolve", err: (err as Error).message }),
        logger: msg => Console.error(msg, "completion resolve"),
        rethrow: false
    })
    handleResolve(...[params]: Parameters<OnCompletionResolveHandlerType>) {
        if (params.data)
            if (params.data.description)
                params.documentation = DescriptionSolver.solve(params.data.description, locale.getLocale());

        return params;
    }

    private completeWord(ctx: CompletionProviderContext, opIdx: number, prefix: string = "") {
        // 不是行首
        if (opIdx && ctx.stmt) {
            // 是可执行指令，则补全操作数
            if (AST.belongInstruction(ctx.stmt) && getOperandType(ctx.stmt, opIdx) !== undefined)
                return provideOperand(ctx, getOperandType(ctx.stmt, opIdx)!, prefix);

            // 是预处理指令，则不补全第一个操作数（用户自定义标识符），如果是第二个操作数则提供补全
            else if (AST.isAliasDirective(ctx.stmt) && opIdx === 2)
                return provideOperand(ctx, OperandType.REG_OR_DEV, prefix);
            else if (AST.isDefineDirective(ctx.stmt) && opIdx === 2)
                return provideOperand(ctx, OperandType.CONST_NUM, prefix);
        }

        // 是行首，提供关键字补全
        else return provideKeyword(ctx, prefix);
    }

    private detectSyntaxContext(tokens: Token[], prevIdx: number, column: number, char?: string) {
        if (!char) return;

        const prev = tokens[prevIdx];
        if (!prev) return;

        //光标与触发字符间存在空格
        if (end(prev).column !== column) return;

        if (char === ":") {
            const device = tokens[prevIdx - 1];
            if (device && device.type === TokenType.DEVICE)
                return { kind: "pin", device } as const;
        }

        if (char === ".") {
            const name = tokens[prevIdx - 1];
            if (name && name.type === TokenType.IDENTIFIER)
                return { kind: "value", name } as const;
        }
    }

    private completeSyntax(synCtx: Exclude<ReturnType<typeof this.detectSyntaxContext>, undefined>, cmpCtx: CompletionProviderContext): CompletionItem[] {
        switch (synCtx.kind) {
            case "pin":
                return Array.from({ length: 7 }).map((_, i) => ({
                    label: i.toString(),
                    kind: CompletionItemKind.Value,
                    insertText: i.toString(),
                    detail: t("hover.operandType.pin")
                }));
            case "value":
                const type = cmpCtx.types?.[synCtx.name.lexeme];

                if (!type || !AST.isEnumAnnotation(type)) return [];

                return type.values.map(v => enumItem(v, synCtx.name.lexeme));
        }
    }
}
