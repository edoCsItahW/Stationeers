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
import { Console, debug, lowerBound, type CompletionScope, type Optional } from "@ic10/common";
import { CompletionItem, CompletionItemKind, Connection } from "vscode-languageserver";
import { OperandType, TokenCategory, TokenType, TypeTableMap } from "@ic10/compiler";

import {
    DescriptionSolver,
    findRangeTokens,
    OperandLocation,
    getOperandType,
    locateOperand,
    end,
    AST
} from "../../../utils";
import { CompletionProviderContext } from "./providers/types";
import { provideKeyword, provideOperand } from "./providers";
import { combine, RelativeState, State } from "./state";
import { DocumentCache } from "../../cache";
import { locale, t } from "../../../locals";
import { enumItem } from "./providers/enum";


type OnCompletionHandlerType = Parameters<Connection["onCompletion"]>[0];
type OnCompletionResolveHandlerType = Parameters<Connection["onCompletionResolve"]>[0];

export class CompletionHandler {
    /**
     * 各文档的补全范围：绑定到"按键后第一次补全所在的行与操作数槽位"（见 {@link setScope}），
     * 行或槽位一变就失效——填完这个操作数、换行、或把该行内容删掉都会自动清除
     * */
    private scopes: Map<string, { scope: CompletionScope; line: number; slot?: number }> = new Map();

    constructor(private readonly docCache: DocumentCache) {}

    /**
     * @summary 设置补全范围
     *
     * @summary Set the completion scope
     *
     * @desc 客户端在用户按下"收窄补全范围"的按键时调用。范围并不按"一次请求"或"整行"生效，而是
     * **绑定到按键后第一次补全所在的操作数槽位**：在该槽位内继续打字/反复请求都保持收窄，光标移到
     * 别的操作数、别的行，或该行的内容被改动导致槽位变化时自动失效，因此不会粘到下一处补全。
     *
     * @desc Called by the client when the user presses a "narrow the scope" key. The scope neither
     * expires after one request nor lasts the whole line: it **binds to the operand slot of the first
     * completion after the key press**. Further typing and repeated requests in that slot stay
     * narrowed; moving to another operand, another line, or editing the line in a way that changes the
     * slot expires it, so it never sticks to the next completion.
     *
     * @param uri - 文档 URI
     * @param uri - Document URI
     * @param scope - 期望的操作数类别；`all` 表示清空
     * @param scope - The expected operand kind; `all` clears it
     * @param line - 按键时光标所在行（1-based）
     * @param line - The cursor line when the key was pressed (1-based)
     * */
    setScope(uri: string, scope: CompletionScope, line: number) {
        if (scope === "all") this.scopes.delete(uri);
        else this.scopes.set(uri, { scope, line });
    }

    @debug({
        message: err => t("server.handler.error", { name: "completion", err: (err as Error).message }),
        logger: msg => Console.error(msg, "completion"),
        rethrow: false
    })
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

        // 统一为1-based（下面的范围判定也要用）
        const line = position.line + 1;
        const column = position.character + 1;

        if (!cache || !cache.ast || !cache.symbols || !context)
            // 无内容则补全关键字
            return provideKeyword({/* provideKeyword不使用context */} as CompletionProviderContext, "");

        // 当前语句索引
        const stmtIdx = lowerBound(cache.ast.statements, n => n.position.line >= line);

        const ctx: CompletionProviderContext = {
            stmt: cache.ast.statements[stmtIdx]?.position.line === line ? cache.ast.statements[stmtIdx] : undefined,
            symbols: cache.symbols,
            types: cache.types as TypeTableMap,
            isDefine: false,
            symbolMap: cache.symbols
                ? Object.fromEntries(
                      Object.entries(cache.symbols.symbols)
                          .filter(([, s]) => s.value)
                          .map(([, s]) => [s.value as string, s.name] as const)
                  )
                : undefined,
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

        // 操作数槽位与子段：由AST定位，行内token序号不等于操作数序号（如 `d0:1`、`Foo.Bar`）
        const location = ctx.stmt ? locateOperand(ctx.stmt, column) : undefined;

        // 多token操作数的子段（引脚 `d0:1`、枚举值 `Foo.Bar`）：与触发字符无关，Ctrl+Space同样生效
        if (location?.segment) return this.completeSegment(location, ctx);

        // 操作数槽位，0表示位于语句头部（关键字/标签名）
        const slot = location?.slot ?? 0;

        // 补全范围：先在"按键后第一次补全"上落定槽位，之后只在该行该槽位内生效；
        // 槽位或行一变（填完这个操作数、换行、该行内容被改动）即清除，避免粘到下一处补全
        const stored = this.scopes.get(uri);

        let scope: Optional<CompletionScope>;

        if (stored?.line === line) {
            stored.slot ??= slot;

            scope = stored.slot === slot ? stored.scope : undefined;

            if (!scope) this.scopes.delete(uri);
        } else if (stored) this.scopes.delete(uri);

        // 前一个token与当前光标间的空格数
        const prevBlocks = prevToken ? column - end(prevToken).column : 0;

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
                if (prevBlocks === 1 || state === State.INSIDE_GAP_INVOKED)
                    return this.completeWord(ctx, slot, "", scope);

                break;
            }

            // 从中间补全或结尾补全当前词
            case State.INSIDE_WORD_TRIGGER_INCOMPLETE:
            case State.INSIDE_WORD_INVOKED:
            case State.END_WORD_TRIGGER_INCOMPLETE:
            case State.END_WORD_INVOKED:
                const inside = rel === RelativeState.INSIDE_WORD;
                const token = tokens[inside ? currIdx : prevIdx];
                return this.completeWord(ctx, slot, token.lexeme.substring(0, column - 1), scope);

            // 没有明确意图，重新弹出该位置的补全
            case State.START_WORD_INVOKED:
                return this.completeWord(ctx, slot, "", scope);

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

    /**
     * @summary 补全一个词（语句头部或某个操作数槽位）
     *
     * @summary Complete one word (the statement head or an operand slot)
     *
     * @param ctx - 补全上下文
     * @param ctx - Completion context
     * @param slot - 操作数槽位，0 表示语句头部
     * @param slot - Operand slot; 0 means the statement head
     * @param prefix - 已输入的前缀
     * @param prefix - The prefix already typed
     * @param scope - 一次性补全范围（见 {@link setScope}）：限定候选的书写形式族
     * @param scope - One-shot completion scope (see {@link setScope}): restricts the writing forms
     * */
    private completeWord(ctx: CompletionProviderContext, slot: number, prefix: string = "", scope?: CompletionScope) {
        // 不是语句头部
        if (slot && ctx.stmt) {
            // 是可执行指令，则补全操作数
            if (AST.belongInstruction(ctx.stmt) && getOperandType(ctx.stmt, slot) !== undefined)
                return provideOperand(ctx, getOperandType(ctx.stmt, slot)!, prefix, scope);

            // 是预处理指令，则不补全第一个操作数（用户自定义标识符），如果是第二个操作数则提供补全
            ctx.isDefine = true;
            if (AST.isAliasDirective(ctx.stmt) && slot === 2)
                return provideOperand(ctx, OperandType.REG_OR_DEV, prefix, scope);
            if (AST.isDefineDirective(ctx.stmt) && slot === 2)
                return provideOperand(ctx, OperandType.CONST_NUM, prefix, scope);
        }

        // 是语句头部，提供关键字补全：头部的候选本来就只有关键字，范围（除关键字本身）在此无意义
        else return provideKeyword(ctx, prefix);
    }

    /**
     * @summary 补全多 token 操作数内部的子段
     *
     * @summary Complete a sub-segment inside a multi-token operand
     *
     * @desc 静态设备的引脚 `d0:1` 提供 0-6 的引脚号；枚举 `Foo.Bar` 提供点号前枚举类型
     * 所声明的成员。两类子段均与触发字符无关，因此 `Ctrl+Space` 与触发字符输入行为一致。
     *
     * @desc The pin of a static device (`d0:1`) offers pin numbers 0-6; an enum (`Foo.Bar`)
     * offers the members declared by the enum type before the dot. Both sub-segments are
     * trigger-character independent, so `Ctrl+Space` behaves like typing the trigger character.
     * */
    private completeSegment(location: OperandLocation, ctx: CompletionProviderContext): CompletionItem[] {
        switch (location.segment) {
            case "pin":
                return Array.from({ length: 7 }).map((_, i) => ({
                    label: i.toString(),
                    kind: CompletionItemKind.Value,
                    insertText: i.toString(),
                    detail: t("hover.operandType.pin")
                }));

            case "enumValue": {
                const operand = location.operand;

                if (!operand || !AST.isEnum(operand)) return [];

                // 枚举类型名（`Foo.Bar` 的 `Foo`）：其注解声明了可补全的成员
                const name = operand.name;
                if (!AST.isIdentifier(name)) return [];

                const type = ctx.types?.[name.value];

                if (!type || !AST.isEnumAnnotation(type)) return [];

                return type.values.map(v => enumItem(v, name.value));
            }

            default:
                return [];
        }
    }
}
