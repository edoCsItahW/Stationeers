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

import {CompletionItemKind, CompletionTriggerKind, InsertTextFormat} from "vscode-languageserver/node";
import type {CompletionItem} from "vscode-languageserver/node";
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

import {findCurrentOperand, getKeyword, getOperandIndex, getOperandType, isAtKeyword} from "./utils";
import type {CompletionContext, CompletionData, CompletionProvider} from "./types";
import {Optional} from "../../../../../common/types/utils";
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
                data: {local: ctx.getLocale(), name: key, key: "Instruction"} satisfies CompletionData
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

        return this.completeForOperandType(ctx, opType);
    }

    private completeForOperandType(
        ctx: CompletionContext,
        opType: number
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
                            labelDetails: { detail: `: ${symbol.value}`, description: t("hover.operandType.register") },
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
                            labelDetails: { detail: `: ${sym.value}`, description: t("hover.operandType.device") },
                            documentation: sym.desc
                        }))
                    : [];
                return [...builtin, ...userAliases];
            }

            case OperandType.LOGIC_TYPE:
            case OperandType.LOGIC_SLOT:
            case OperandType.REAGENT_MODE:
            case OperandType.BATCH_MODE:
            case OperandType.SLOT_IDX:
                return this.enumCompletions(ctx, opType);

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
                        data: { local: ctx.getLocale(), name: entry.name, key: "LogicSlotType" } satisfies CompletionData
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
                    detail: t("hover.operandType.logicType") + `(${t(`completion.access.${l.access.toLowerCase()}` as any)})`,
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
        const deviceItem = Object.entries(stmt).find(([, v]) => v === OperandType.DEV_REF || v === OperandType.DEV_ALIAS);

        if (!deviceItem) return deviceItem;

        return (stmt as any)[`operand${getOperandIndex(deviceItem[0])}`];
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
        const meta = {
            "%": {
                value: INS_META_MAP.get("binary_number")!,
                name: "binary_number"
            },
            "$": {
                value: INS_META_MAP.get("hash_number")!,
                name: "hash_number"
            },
            "HASH": {
                value: INS_META_MAP.get("hash")!,
                name: "hash"
            },
            "STR": {
                value: INS_META_MAP.get("str")!,
                name: "str"
            }
        }

        const items: CompletionItem[] = [
            {label: "%", kind: CompletionItemKind.Operator, insertText: "%"},
            {label: "$", kind: CompletionItemKind.Operator, insertText: "$"},
            {
                label: "HASH",
                kind: CompletionItemKind.Function,
                insertText: "HASH(\"$0\")",
                insertTextFormat: InsertTextFormat.Snippet
            },
            {
                label: "STR",
                kind: CompletionItemKind.Function,
                insertText: "STR(\"$0\")",
                insertTextFormat: InsertTextFormat.Snippet
            }
        ];

        return items.filter(i => i.label.toLowerCase().startsWith(ctx.prefix)).map(i => {
            const metaData = meta[i.label as keyof typeof meta];

            return {
                ...i,
                detail: metaData.value.signature,
                data: { local: ctx.getLocale(), name: metaData.name, key: "Instruction" } satisfies CompletionData
            };
        });
    }
}
