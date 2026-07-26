// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

import type {CompletionItem} from "vscode-languageserver/node";
import {CompletionItemKind, CompletionTriggerKind, InsertTextFormat} from "vscode-languageserver/node";
import {AliasDirectiveNode, BasicType, DefineDirectiveNode, OperandType, TypeCategory} from "ic10-node-api";

import {findCurrentOperand, getKeyword, getOperandType, isAtKeyword} from "./utils";
import type {CompletionContext, CompletionProvider} from "./types";
import {INS_META_MAP} from "../../../mateData";
import {RadixTree} from "../../../utils/utils";
import {t} from "../../../locals/locale";

// ── 内置补全数据 ──

const REGISTERS = [...Array.from({length: 16}).map((_, i) => ({
    value: `r${i}`,
    sort: "r" + String(i).padStart(2, '0')
})), {value: "ra", sort: "ra"}, {value: "sp", sort: "sp"}];

const DEVICE_REFS = [...Array.from({length: 6}).map((_, i) => ({
    value: `d${i}`,
    sort: "d" + String(i).padStart(2, '0')
})), {value: "db", sort: "db"}];


const insTree = RadixTree.fromMap(INS_META_MAP);

// ── 辅助 ──

function label(text: string): string {
    return text.toLowerCase();
}

function filterByPrefix(items: { value: string, sort: string; }[], prefix: string) {
    const p = prefix.toLowerCase();
    return items.filter(({value}) => value.toLowerCase().startsWith(p));
}

function toItem(label: string, kind: CompletionItemKind, detail?: string): CompletionItem {
    return {label, kind, insertText: label, detail};
}

// ═══════════════════════════════════════════════════════════════
//  KeywordProvider — 字母触发时补全指令关键字
// ═══════════════════════════════════════════════════════════════

export class KeywordCompletionProvider implements CompletionProvider {
    canHandle(ctx: CompletionContext): boolean {
        // 显式字母触发 — 始终提供关键字补全
        if (ctx.triggerKind === CompletionTriggerKind.TriggerCharacter
            && ctx.triggerCharacter !== " ") {
            return true;
        }

        // 补全列表显示中继续输入 — 始终提供关键字补全
        if (ctx.triggerKind === CompletionTriggerKind.TriggerForIncompleteCompletions)
            return true;

        // 手动调用（Ctrl+Space 或补全被关闭后继续输入）
        // 仅在光标处于「关键字位置」时提供补全，避免干扰操作数补全
        if (ctx.triggerKind === CompletionTriggerKind.Invoked) {
            const t = ctx.stmt?.type;
            if (!t || t === "Error") return true;
            return isAtKeyword(ctx.stmt!, ctx.character);
        }

        return false;
    }

    provide(ctx: CompletionContext): CompletionItem[] {
        let prefix = ctx.prefix;

        // 对于 ErrorNode，尝试用 token 精确定位用户正在输入的单词
        if (ctx.stmt?.type === "Error")
            if (ctx.token) prefix = ctx.token.lexeme;

        return insTree.keysWithPrefix(prefix).map(key => [key, insTree.search(key)!] as const)
            .filter(([_, v]) => v.type === "Instruction")
            .map(([key, value]) => ({
            label: key,
            kind: CompletionItemKind.Keyword,
            insertText: key,
            detail: value.signature,
            data: {local: ctx.getLocale(), name: key}
        }));
    }
}


// ═══════════════════════════════════════════════════════════════
//  OperandProvider — 空格触发时根据 AST typeN 补全操作数
// ═══════════════════════════════════════════════════════════════

export class OperandCompletionProvider implements CompletionProvider {
    canHandle(ctx: CompletionContext): boolean {
        return ctx.triggerCharacter === " ";
    }

    provide(ctx: CompletionContext): CompletionItem[] {
        if (!ctx.stmt || ctx.stmt.type === "Error") return [];

        const keyword = getKeyword(ctx.stmt);
        const opItem = findCurrentOperand(ctx.stmt, ctx.triggerCharacter);

        if (!opItem) return [];

        const [key, value] = opItem;

        const opType = getOperandType(ctx.stmt, key);

        if (!keyword) return [];

        // 根据 AST 的 typeN 字段分发
        if (opType === null || opType === undefined) return [];

        return this.completeForOperandType(opType, ctx);
    }

    private completeForOperandType(
        opType: number,
        ctx: CompletionContext
    ): CompletionItem[] {
        switch (opType) {
            case OperandType.REG_IDENT:
            case OperandType.REG_NUM: {
                // 寄存器 + 数字（具体补数字留给用户自行输入）
                const matching = filterByPrefix(REGISTERS, ctx.prefix);
                const builtin = matching.map(({value, sort}) => ({
                    label: value,
                    kind: CompletionItemKind.Variable,
                    insertText: value,
                    detail: t("hover.operandType.register"),
                    sortText: sort
                }));

                const userAliases: CompletionItem[] = ctx.symbols ?
                    Object.entries(ctx.symbols).filter(([name, sym]) => sym.type === BasicType.REGISTER && name.startsWith(ctx.prefix))
                        .map(([name, symbol]) => ({
                            label: name,
                            kind: CompletionItemKind.Variable,
                            insertText: name,
                            detail: t("hover.operandType.register"),
                            documentation: symbol.desc
                        })) : [];

                return [...builtin, ...userAliases];
            }

            case OperandType.DEV_ALIAS:
            case OperandType.DEV_REF: {
                // 设备引用: d0-d5, db  + 用户别名
                const builtin = filterByPrefix(DEVICE_REFS, ctx.prefix).map(
                    ({value, sort}) => ({
                        label: value,
                        kind: CompletionItemKind.Reference,
                        insertText: value,
                        detail: t("hover.operandType.device"),
                        sortText: sort
                    })
                );
                // 用户定义的别名（从符号表）

                const userAliases: CompletionItem[] = ctx.symbols
                    ? Object.entries(ctx.symbols)
                        .filter(([name, sym]) => sym.type === BasicType.DEVICE && name.startsWith(ctx.prefix)) // BasicType.DEVICE = 4
                        .map(([name, sym]) => ({
                            label: name,
                            kind: CompletionItemKind.Reference,
                            insertText: name,
                            detail: t("hover.operandType.device"),
                            documentation: sym.desc
                        }))
                    : [];
                return [...builtin, ...userAliases];
            }

            case OperandType.LOGIC_TYPE:
                return this.enumCompletions("LogicType", ctx);

            case OperandType.LOGIC_SLOT:
                return this.enumCompletions("LogicSlotType", ctx);

            case OperandType.REAGENT_MODE:
                return this.enumCompletions("ReagentMode", ctx);

            case OperandType.BATCH_MODE:
                return this.enumCompletions("BatchMode", ctx);

            case OperandType.JUMP_TARGET: {
                // 标签（从符号表）
                const prefix = ctx.prefix;
                return ctx.symbols
                    ? Object.entries(ctx.symbols)
                        .filter(([, sym]) => sym.category === TypeCategory.LABEL) // TypeCategory.LABEL = 0
                        .map(([name]) => name)
                        .filter(n => label(n).startsWith(label(prefix)))
                        .map(l => ({
                            label: l,
                            kind: CompletionItemKind.Variable,
                            insertText: l,
                            detail: t("hover.labelDef.type")
                        }))
                    : [];
            }

            case OperandType.SLOT_IDX:
                // 槽索引通常为 0 到槽数-1，不补全
                return [];

            default:
                return [];
        }
    }

    // TODO: 从 enums.json 按枚举类型名取枚举值
    private enumCompletions(_enumTypeName: string, ctx: CompletionContext): CompletionItem[] {
        const prefix = ctx.prefix;
        // 示例桩：接入 enums.json 数据即可
        return [];
    }
}


// ═══════════════════════════════════════════════════════════════
//  DirectiveProvider — alias / define 专用的补全
// ═══════════════════════════════════════════════════════════════

export class DirectiveCompletionProvider implements CompletionProvider {
    canHandle(ctx: CompletionContext): boolean {
        const t = ctx.stmt?.type;
        return t === "AliasDirective" || t === "DefineDirective";
    }

    provide(ctx: CompletionContext): CompletionItem[] {
        if (!ctx.stmt) return [];

        if (ctx.stmt.type === "AliasDirective")
            return this.completeAlias(ctx);
        if (ctx.stmt.type === "DefineDirective")
            return this.completeDefine(ctx);

        return [];
    }

    // ── alias ────────────────────────────────────────────────

    private completeAlias(ctx: CompletionContext): CompletionItem[] {
        const stmt = ctx.stmt! as AliasDirectiveNode;
        const keywordCol = stmt.position.column;

        // alias关键字区 → 不补全
        if (ctx.character <= keywordCol + 5) return [];

        if (stmt.identifier.type === "Identifier" && stmt.registerOrDevice.type === "Error")
            return this.completeRegOrDev(ctx);

        return [];
    }

    // ── define ───────────────────────────────────────────────

    private completeDefine(ctx: CompletionContext): CompletionItem[] {
        const stmt = ctx.stmt! as DefineDirectiveNode;
        const keywordCol = stmt.position.column;

        // define关键字区 → 不补全
        if (ctx.character <= keywordCol + 6) return [];

        if (stmt.identifier.type === "Identifier" && stmt.number.type === "Error")
            return this.completeDefineValue(ctx);

        return [];
    }

    // ── 辅助 ────────────────────────────────────────────────

    /** alias xxx_ → 寄存器 + 设备引用 */
    private completeRegOrDev(ctx: CompletionContext): CompletionItem[] {
        const regItems: CompletionItem[] = filterByPrefix(REGISTERS, ctx.prefix)
            .map(({value, sort}) => ({
                label: value,
                kind: CompletionItemKind.Variable,
                insertText: value,
                detail: t("hover.operandType.register"),
                sortText: sort
            }));
        const devItems = filterByPrefix(DEVICE_REFS, ctx.prefix)
            .map(({value, sort}) => ({
                label: value,
                kind: CompletionItemKind.Reference,
                insertText: value,
                detail: t("hover.operandType.device"),
                sortText: sort
            }));

        return [...regItems, ...devItems];
    }

    /** define xxx_ → %, $, HASH(), STR() */
    private completeDefineValue(ctx: CompletionContext): CompletionItem[] {
        const items: CompletionItem[] = [
            // TODO:
            {label: "%", kind: CompletionItemKind.Snippet, detail: "寄存器引用", insertText: "%"},
            {label: "$", kind: CompletionItemKind.Snippet, detail: "设备引用", insertText: "$"},
            {
                label: "HASH",
                kind: CompletionItemKind.Function,
                detail: "HASH()",
                insertText: "HASH(\"$0\")",
                insertTextFormat: InsertTextFormat.Snippet
            },
            {
                label: "STR",
                kind: CompletionItemKind.Function,
                detail: "STR()",
                insertText: "STR(\"$0\")",
                insertTextFormat: InsertTextFormat.Snippet
            }
        ];
        return items.filter(i => i.label.toLowerCase().startsWith(ctx.prefix));
    }
}
