// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file providers.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import type { CompletionItem } from "vscode-languageserver/node";
import { CompletionItemKind, CompletionTriggerKind, InsertTextFormat } from "vscode-languageserver/node";
import {
    AliasDirectiveNode,
    BasicType,
    DefineDirectiveNode,
    DeviceNode,
    DeviceType,
    ErrorNode,
    IdentifierNode,
    OperandType,
    RegisterNode,
    StatementNode,
    TypeCategory
} from "ic10-node-api";

import { findCurrentOperand, getKeyword, getOperandType, isAtKeyword } from "./utils";
import type { CompletionContext, CompletionData, CompletionProvider } from "./types";
import { INS_META_MAP } from "../../../mateData";
import { RadixTree } from "../../../utils";
import { t } from "../../../locals/locale";
import { Optional } from "common";

// ── 内置补全数据 ──

const REGISTERS = Array.from({ length: 16 }).map((_, i) => ({
    value: `r${i}`,
    sort: "r" + String(i).padStart(2, "0")
}));

const DEVICE_REFS = Array.from({ length: 6 }).map((_, i) => ({
    value: `d${i}`,
    sort: "d" + String(i).padStart(2, "0")
}));

const insTree = RadixTree.fromMap(INS_META_MAP);

// ── 辅助 ──

function label(text: string): string {
    return text.toLowerCase();
}

function filterByPrefix(items: { value: string; sort: string }[], prefix: string) {
    const p = prefix.toLowerCase();
    return items.filter(({ value }) => value.toLowerCase().startsWith(p));
}

// ═══════════════════════════════════════════════════════════════
//  KeywordProvider — 字母触发时补全指令关键字
// ═══════════════════════════════════════════════════════════════

/**
 * @summary 关键字补全提供器 — 在字母触发时补全 IC10 指令关键字
 *
 * @summary Keyword completion provider — completes IC10 instruction keywords on letter trigger
 *
 * @desc 处理三种触发方式：显式字母触发（TriggerCharacter）、补全列表中继续输入
 *  （TriggerForIncompleteCompletions）以及手动调用（Invoked，仅在关键字位置提供）。
 *  使用 RadixTree 进行前缀匹配以提高性能。
 *
 * @desc Handles three trigger kinds: explicit letter trigger (TriggerCharacter),
 *  continued typing in completion list (TriggerForIncompleteCompletions),
 *  and manual invocation (Invoked, only at keyword positions).
 *  Uses RadixTree for prefix matching to improve performance.
 * */
export class KeywordCompletionProvider implements CompletionProvider {
    canHandle(ctx: CompletionContext): boolean {
        // 显式字母触发 — 始终提供关键字补全
        if (ctx.triggerKind === CompletionTriggerKind.TriggerCharacter && ctx.triggerCharacter !== " ") {
            return true;
        }

        // 补全列表显示中继续输入 — 始终提供关键字补全
        if (ctx.triggerKind === CompletionTriggerKind.TriggerForIncompleteCompletions) return true;

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
        if (ctx.stmt?.type === "Error") if (ctx.token) prefix = ctx.token.lexeme;

        return insTree
            .entriesWithPrefix(prefix)
            .filter(([, v]) => v.type === "Instruction")
            .map(([key, value]) => ({
                label: key,
                kind: CompletionItemKind.Keyword,
                insertText: key,
                detail: value.signature,
                data: { local: ctx.getLocale(), name: key, key: "Instruction" } satisfies CompletionData
            }));
    }
}

// ═══════════════════════════════════════════════════════════════
//  OperandProvider — 空格触发时根据 AST typeN 补全操作数
// ═══════════════════════════════════════════════════════════════

/**
 * @summary 操作数补全提供器 — 在空格触发时根据 AST 类型字段补全操作数
 *
 * @summary Operand completion provider — completes operands on space trigger based on AST type fields
 *
 * @desc 当用户输入空格后，读取当前指令的 AST typeN 字段确定操作数类型，
 *  然后补全对应的寄存器、设备引用、枚举值、跳转标签等。
 *  支持根据前置设备节点过滤 LogicType/LogicSlot/BatchMode 等枚举值。
 *
 * @desc After a space input, reads the current instruction's AST typeN fields to
 *  determine operand type, then completes corresponding registers, device references,
 *  enum values, jump targets, etc. Supports filtering LogicType/LogicSlot/BatchMode
 *  enums based on the preceding device node.
 * */
export class OperandCompletionProvider implements CompletionProvider {
    canHandle(ctx: CompletionContext): boolean {
        return ctx.triggerCharacter === " ";
    }

    provide(ctx: CompletionContext): CompletionItem[] {
        if (!ctx.stmt || ctx.stmt.type === "Error") return [];

        const keyword = getKeyword(ctx.stmt);
        const opItem = findCurrentOperand(ctx.stmt, ctx.triggerCharacter);

        if (!opItem) return [];

        const [key] = opItem;

        const opType = getOperandType(ctx.stmt, key);

        if (!keyword) return [];

        // 根据 AST 的 typeN 字段分发
        if (opType === null || opType === undefined) return [];

        return this.completeForOperandType(ctx, opType);
    }

    private completeForOperandType(ctx: CompletionContext, opType: number): CompletionItem[] {
        switch (opType) {
            case OperandType.REG_IDENT:
            case OperandType.REG_NUM: {
                // 寄存器 + 数字（具体补数字留给用户自行输入）
                const matching = filterByPrefix(REGISTERS, ctx.prefix);
                const builtin = matching.map(({ value, sort }) => ({
                    label: value,
                    kind: CompletionItemKind.Variable,
                    insertText: value,
                    detail: t("hover.operandType.register"),
                    sortText: sort
                }));

                // 用户定义的别名（从符号表，单次 for...in 避免 Object.entries 中间数组分配）
                const userAliases: CompletionItem[] = [];
                if (ctx.symbols) {
                    for (const name in ctx.symbols) {
                        const sym = ctx.symbols[name];
                        if (
                            (sym.type === BasicType.REGISTER ||
                                ((opType as OperandType) === OperandType.REG_NUM &&
                                    sym.category === TypeCategory.NUMBER)) &&
                            name.startsWith(ctx.prefix)
                        )
                            userAliases.push({
                                label: name,
                                kind: CompletionItemKind.Variable,
                                insertText: name,
                                detail: t("hover.operandType.register"),
                                labelDetails: {
                                    detail: `: ${sym.value}`,
                                    description: t("hover.operandType.register")
                                },
                                documentation: sym.desc
                            });
                    }
                }

                return [...builtin, ...userAliases];
            }

            case OperandType.DEV_ALIAS:
            case OperandType.DEV_REF: {
                // 设备引用: d0-d5, db  + 用户别名
                const builtin = filterByPrefix(DEVICE_REFS, ctx.prefix).map(({ value, sort }) => ({
                    label: value,
                    kind: CompletionItemKind.Reference,
                    insertText: value,
                    detail: t("hover.operandType.device"),
                    sortText: sort
                }));
                // 用户定义的别名（从符号表，单次 for...in 避免 Object.entries 中间数组分配）
                const userAliases: CompletionItem[] = [];
                if (ctx.symbols) {
                    for (const name in ctx.symbols) {
                        const sym = ctx.symbols[name];
                        if (sym.type === BasicType.DEVICE && name.startsWith(ctx.prefix))
                            userAliases.push({
                                label: name,
                                kind: CompletionItemKind.Reference,
                                insertText: name,
                                detail: t("hover.operandType.device"),
                                labelDetails: { detail: `: ${sym.value}`, description: t("hover.operandType.device") },
                                documentation: sym.desc
                            });
                    }
                }
                return [...builtin, ...userAliases];
            }

            case OperandType.LOGIC_TYPE:
            case OperandType.LOGIC_SLOT:
            case OperandType.REAGENT_MODE:
            case OperandType.BATCH_MODE:
            case OperandType.SLOT_IDX:
                return this.enumCompletions(ctx, opType);

            case OperandType.JUMP_TARGET: {
                // 标签（从符号表，单次 for...in 避免链式中间数组分配）
                const result: CompletionItem[] = [];
                if (ctx.symbols) {
                    const lowerPrefix = label(ctx.prefix);
                    for (const name in ctx.symbols) {
                        const sym = ctx.symbols[name];
                        if (sym.category === TypeCategory.LABEL && label(name).startsWith(lowerPrefix))
                            result.push({
                                label: name,
                                kind: CompletionItemKind.Variable,
                                insertText: name,
                                detail: t("hover.labelDef.type")
                            });
                    }
                }
                return result;
            }

            default:
                return [];
        }
    }

    private enumCompletions(ctx: CompletionContext, opType: OperandType): CompletionItem[] {
        const device = this.findPrevDevice(ctx.stmt!);

        if (device)
            switch (device.type) {
                case "Device":

                case "Identifier":
                    const symbol = ctx.symbols![device.value];

                    if (symbol && symbol.typeName) {
                        const type = ctx.types![symbol.typeName];

                        if (type && type.type === "device" && opType !== OperandType.REAGENT_MODE)
                            return this.deviceCompletions(ctx, type, opType);
                    }
            }

        switch (opType) {
            case OperandType.LOGIC_SLOT: {
                const type = ctx.types!["LogicSlotType"];

                if (type.type === "enum")
                    return type.values.map(entry => ({
                        label: entry.name,
                        kind: CompletionItemKind.Constant,
                        insertText: entry.name,
                        detail: t("hover.operandType.logicSlotType"),
                        labelDetails: {
                            detail: `: ${entry.value}`,
                            description: t("hover.operandType.logicSlotType")
                        },
                        data: {
                            local: ctx.getLocale(),
                            name: entry.name,
                            key: "LogicSlotType"
                        } satisfies CompletionData
                    }));

                break;
            }
            case OperandType.BATCH_MODE: {
                const type = ctx.types!["BatchMode"];

                if (type.type === "enum")
                    return type.values.map(entry => ({
                        label: entry.name,
                        kind: CompletionItemKind.Constant,
                        insertText: entry.name,
                        detail: t("hover.operandType.batchMode"),
                        labelDetails: {
                            detail: `: ${entry.value}`,
                            description: t("hover.operandType.batchMode")
                        },
                        data: { local: ctx.getLocale(), name: entry.name, key: "BatchMode" } satisfies CompletionData
                    }));

                break;
            }
            case OperandType.LOGIC_TYPE: {
                const type = ctx.types!["LogicType"];

                if (type.type === "enum")
                    return type.values.map(entry => ({
                        label: entry.name,
                        kind: CompletionItemKind.Constant,
                        insertText: entry.name,
                        detail: t("hover.operandType.logicType"),
                        labelDetails: {
                            detail: `: ${entry.value}`,
                            description: t("hover.operandType.logicType")
                        },
                        data: { local: ctx.getLocale(), name: entry.name, key: "LogicType" } satisfies CompletionData
                    }));

                break;
            }
            case OperandType.REAGENT_MODE: {
                const type = ctx.types!["ReagentMode"];

                if (type.type === "enum")
                    return type.values.map(entry => ({
                        label: entry.name,
                        kind: CompletionItemKind.Constant,
                        insertText: entry.name,
                        detail: t("hover.operandType.reagentMode"),
                        labelDetails: {
                            detail: `: ${entry.value}`,
                            description: t("hover.operandType.reagentMode")
                        },
                        data: { local: ctx.getLocale(), name: entry.name, key: "ReagentMode" } satisfies CompletionData
                    }));

                break;
            }
        }

        return [];
    }

    private deviceCompletions(ctx: CompletionContext, type: DeviceType, opType: OperandType): CompletionItem[] {
        switch (opType) {
            case OperandType.LOGIC_SLOT:
                return type.logicSlots.map(s => ({
                    label: s,
                    kind: CompletionItemKind.Constant,
                    insertText: s,
                    detail: t("hover.operandType.logicSlotType"),
                    data: { local: ctx.getLocale(), name: s, key: "LogicSlotType" } satisfies CompletionData
                }));

            case OperandType.LOGIC_TYPE:
                return type.logics.map(l => ({
                    label: l.name,
                    kind: CompletionItemKind.Constant,
                    insertText: l.name,
                    detail:
                        t("hover.operandType.logicType") +
                        `(${t(`completion.access.${l.access.toLowerCase()}` as any)})`,
                    data: { local: ctx.getLocale(), name: l.name, key: "LogicSlotType" } satisfies CompletionData
                }));

            case OperandType.BATCH_MODE:
                return type.modes.map(m => ({
                    label: m.index,
                    kind: CompletionItemKind.Constant,
                    insertText: m.index,
                    detail: t("hover.operandType.batchMode"),
                    data: { local: ctx.getLocale(), name: m.index, key: "LogicSlotType" } satisfies CompletionData
                }));

            case OperandType.SLOT_IDX:
                return type.slots.map(s => ({
                    label: s.index,
                    kind: CompletionItemKind.Constant,
                    insertText: s.index,
                    detail: t("hover.operandType.slotIdx") + `(${t(`completion.direction.${s.direction}`) as any})`,
                    data: { local: ctx.getLocale(), name: s.index, key: "LogicSlotType" } satisfies CompletionData
                }));
        }

        return [];
    }

    private findPrevDevice(stmt: StatementNode): Optional<DeviceNode | RegisterNode | IdentifierNode | ErrorNode> {
        // 用 for...in 代替 Object.entries 避免中间数组分配
        for (const k in stmt) {
            if (!k.startsWith("type")) continue;
            const v = (stmt as any)[k];
            if (v === OperandType.DEV_REF || v === OperandType.DEV_ALIAS) {
                const idx = Number.parseInt(k.replace("type", ""), 10);
                return (stmt as any)[`operand${idx}`];
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════
//  DirectiveProvider — alias / define 专用的补全
// ═══════════════════════════════════════════════════════════════

/**
 * @summary 伪指令补全提供器 — 为 alias 和 define 语句提供专用补全
 *
 * @summary Directive completion provider — provides specialized completions for alias and define statements
 *
 * @desc 处理 alias 和 define 伪指令的补全需求：
 *  - alias: 补全寄存器引用（r0-r15, ra, sp）和设备引用（d0-d5, db）
 *  - define: 补全 % 二进制数、$ 哈希数、HASH() 和 STR() 函数调用
 *
 * @desc Handles completion for alias and define directives:
 *  - alias: completes register references (r0-r15, ra, sp) and device references (d0-d5, db)
 *  - define: completes % binary numbers, $ hash numbers, HASH(), and STR() function calls
 * */
export class DirectiveCompletionProvider implements CompletionProvider {
    canHandle(ctx: CompletionContext): boolean {
        const t = ctx.stmt?.type;
        return t === "AliasDirective" || t === "DefineDirective";
    }

    provide(ctx: CompletionContext): CompletionItem[] {
        if (!ctx.stmt) return [];

        if (ctx.stmt.type === "AliasDirective") return this.completeAlias(ctx);
        if (ctx.stmt.type === "DefineDirective") return this.completeDefine(ctx);

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

        if (stmt.identifier.type === "Identifier" && stmt.number.type === "Error") return this.completeDefineValue(ctx);

        return [];
    }

    // ── 辅助 ────────────────────────────────────────────────

    /** alias xxx_ → 寄存器 + 设备引用 */
    private completeRegOrDev(ctx: CompletionContext): CompletionItem[] {
        const regItems: CompletionItem[] = filterByPrefix(REGISTERS, ctx.prefix).map(({ value, sort }) => ({
            label: value,
            kind: CompletionItemKind.Variable,
            insertText: value,
            detail: t("hover.operandType.register"),
            sortText: sort
        }));
        const devItems = filterByPrefix(DEVICE_REFS, ctx.prefix).map(({ value, sort }) => ({
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
        const meta = {
            "%": {
                value: INS_META_MAP.get("binary_number")!,
                name: "binary_number"
            },
            $: {
                value: INS_META_MAP.get("hash_number")!,
                name: "hash_number"
            },
            HASH: {
                value: INS_META_MAP.get("hash")!,
                name: "hash"
            },
            STR: {
                value: INS_META_MAP.get("str")!,
                name: "str"
            }
        };

        const items: CompletionItem[] = [
            { label: "%", kind: CompletionItemKind.Operator, insertText: "%" },
            { label: "$", kind: CompletionItemKind.Operator, insertText: "$" },
            {
                label: "HASH",
                kind: CompletionItemKind.Function,
                insertText: 'HASH("$0")',
                insertTextFormat: InsertTextFormat.Snippet
            },
            {
                label: "STR",
                kind: CompletionItemKind.Function,
                insertText: 'STR("$0")',
                insertTextFormat: InsertTextFormat.Snippet
            }
        ];

        return items
            .filter(i => i.label.toLowerCase().startsWith(ctx.prefix))
            .map(i => {
                const metaData = meta[i.label as keyof typeof meta];

                return {
                    ...i,
                    detail: metaData.value.signature,
                    data: { local: ctx.getLocale(), name: metaData.name, key: "Instruction" } satisfies CompletionData
                };
            });
    }
}
