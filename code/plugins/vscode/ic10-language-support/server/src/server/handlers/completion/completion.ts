// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file completion.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/26 11:58
 * @desc Completion handler — dispatches to typed providers based on trigger context.
 *       Operand types are read directly from AST node's `type1`/`type2`... fields.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {
    CompletionTriggerKind,
    CompletionItemKind,
    InsertTextFormat,
    CompletionItem,
    Connection
} from "vscode-languageserver";
import {
    IdentifierNode,
    StatementNode,
    RegisterNode,
    TypeCategory,
    TypeTableMap,
    OperandType,
    DeviceNode,
    DeviceType,
    SymbolMap,
    BasicType,
    ErrorNode
} from "ic10-node-api";

import { ENUMS_LOCAL_MAP, INS_LOCAL_MAP, INS_META_MAP, LOGIC_LOCAL_MAP, LOGIC_SLOT_LOCAL_MAP } from "../../../mateData";
import { end, getOperandType, isInstruction, RadixTree, findRangeTokens } from "../../../utils";
import { Console, debug, lowerBound, Optional } from "common";
import { DocumentCache } from "../../cache";
import { locale, t } from "../../../locals";


type OnCompletionHandlerType = Parameters<Connection["onCompletion"]>[0];
type OnCompletionResolveHandlerType = Parameters<Connection["onCompletionResolve"]>[0];

enum RelativeState {
    /** @summary 位于一个单词中间 */
    INSIDE_WORD = 0,
    /** @summary 位于一个单词的结束位置 */
    END_WORD,
    /** @summary 位于一个单词的起始位置 */
    START_WORD,
    /** @summary 位于两个单词的空隙处 */
    INSIDE_GAP
}

const REL_SHIFT = 2;
const TRIG_MASK = 0x03;

function combine(rel: RelativeState, trig: CompletionTriggerKind) {
    return (rel << 2) | trig;
}

function decompose(state: State): [RelativeState, CompletionTriggerKind] {
    return [state >> REL_SHIFT, (state & TRIG_MASK) as CompletionTriggerKind];
}

enum State {
    // 1. INSIDE_WORD (0)
    INSIDE_WORD_INVOKED = combine(RelativeState.INSIDE_WORD, CompletionTriggerKind.Invoked),
    INSIDE_WORD_TRIGGER_CHAR = combine(RelativeState.INSIDE_WORD, CompletionTriggerKind.TriggerCharacter),
    INSIDE_WORD_TRIGGER_INCOMPLETE = combine(
        RelativeState.INSIDE_WORD,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    ),

    // 2. END_WORD (1)
    END_WORD_INVOKED = combine(RelativeState.END_WORD, CompletionTriggerKind.Invoked),
    END_WORD_TRIGGER_CHAR = combine(RelativeState.END_WORD, CompletionTriggerKind.TriggerCharacter),
    END_WORD_TRIGGER_INCOMPLETE = combine(
        RelativeState.END_WORD,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    ),

    // 3. START_WORD (2)
    START_WORD_INVOKED = combine(RelativeState.START_WORD, CompletionTriggerKind.Invoked),
    START_WORD_TRIGGER_CHAR = combine(RelativeState.START_WORD, CompletionTriggerKind.TriggerCharacter),
    START_WORD_TRIGGER_INCOMPLETE = combine(
        RelativeState.START_WORD,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    ),

    // 4. INSIDE_GAP (3)
    INSIDE_GAP_INVOKED = combine(RelativeState.INSIDE_GAP, CompletionTriggerKind.Invoked),
    INSIDE_GAP_TRIGGER_CHAR = combine(RelativeState.INSIDE_GAP, CompletionTriggerKind.TriggerCharacter),
    INSIDE_GAP_TRIGGER_INCOMPLETE = combine(
        RelativeState.INSIDE_GAP,
        CompletionTriggerKind.TriggerForIncompleteCompletions
    )
}

interface CompletionContext {
    stmt: Optional<StatementNode>;
    symbols: Optional<SymbolMap>;
    types: Optional<TypeTableMap>;

    getLocale(): ReturnType<typeof locale.getLocale>;
}

export interface CompletionData {
    /** 当前语言环境 */
    /** Current locale */
    local: ReturnType<CompletionContext["getLocale"]>;
    /** 补全项名称 */
    /** Completion item name */
    name: string;
    /** 补全项分类键 */
    /** Completion item category key */
    key: "Instruction" | "LogicType" | "LogicSlotType" | "BatchMode" | "ReagentMode";
}

export interface BuiltinSymbolInfo {
    value: string;
    sort: string;
}

export class CompletionHandler {
    private static insTree = RadixTree.fromMap(INS_META_MAP);
    private static REGISTERS = Array.from({ length: 16 }).map((_, i) => ({
        value: `r${i}`,
        sort: "r" + String(i).padStart(2, "0")
    }));
    private static DEVICE_REFS = Array.from({ length: 6 }).map((_, i) => ({
        value: `d${i}`,
        sort: "d" + String(i).padStart(2, "0")
    }));

    constructor(private readonly docCache: DocumentCache) {}

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

        if (!cache || !cache.ast || !cache.symbols || !context) return;

        const line = position.line + 1;
        const column = position.character + 1;
        const stmtIdx = lowerBound(cache.ast.statements, n => n.position.line >= line);
        const ctx: CompletionContext = {
            stmt: cache.ast.statements[stmtIdx]?.position.line === line ? cache.ast.statements[stmtIdx] : undefined,
            symbols: cache.symbols,
            types: cache.types as TypeTableMap,
            getLocale: () => locale.getLocale()
        };

        const tokens = cache.tokens.filter(t => t.pos.line === line);
        const { prev: prevIdx, curr: currIdx, next: nextIdx } = findRangeTokens(tokens, column);

        const prevToken = tokens[prevIdx];

        const prevBlocks = prevToken ? column - end(prevToken).column : 0;
        let opIdx = prevIdx; // -1则补keyword(0)，其余补operand${opIdx}

        if (prevBlocks > 0 || opIdx === -1) opIdx++;

        const rel: RelativeState = [
            [RelativeState.INSIDE_WORD, RelativeState.END_WORD],
            [RelativeState.START_WORD, currIdx > 0 ? RelativeState.INSIDE_WORD : RelativeState.INSIDE_GAP]
        ][prevIdx >= 0 ? Number(prevBlocks > 0) : 1][
            nextIdx >= 0 ? Number(tokens[nextIdx].pos.column - column > 0) : 1
        ];

        const state = combine(rel, context.triggerKind);

        // 注: CompletionTriggerKind是动作类型，RelativeState是动作结束后的状态，几乎可以推导出动作前的状态，除非动作前是INSIDE_GAP或END_WORD
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
                const inside = decompose(state)[0] === RelativeState.INSIDE_WORD;
                return this.completeWord(
                    ctx,
                    opIdx,
                    tokens[inside ? currIdx : prevIdx].lexeme.substring(0, column - 1)
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
    handleResolve(...[params]: Parameters<OnCompletionResolveHandlerType>): ReturnType<OnCompletionResolveHandlerType> {
        const data: Optional<CompletionData> = params.data;

        if (data) {
            switch (data.key) {
                case "Instruction": {
                    const doc = INS_LOCAL_MAP.get(data.name)!;

                    params.documentation = (doc["desc"] as any)[data.local];
                    break;
                }
                case "LogicType": {
                    const doc = LOGIC_LOCAL_MAP.get(data.name)!;

                    if (doc?.["desc"] && data.local in doc["desc"]) params.documentation = doc["desc"][data.local];
                    else params.documentation = t("completion.common.noDesc");

                    break;
                }
                case "LogicSlotType": {
                    const doc = LOGIC_SLOT_LOCAL_MAP.get(data.name)!;

                    if (doc?.["desc"] && data.local in doc["desc"]) params.documentation = doc["desc"][data.local];
                    else params.documentation = t("completion.common.noDesc");

                    break;
                }
                case "BatchMode":
                case "ReagentMode": {
                    const map = ENUMS_LOCAL_MAP.get(data.key)!.enums;

                    const doc = (map as any)[data.name];

                    if (doc?.["desc"] && data.local in doc["desc"]) params.documentation = doc["desc"][data.local];
                    else params.documentation = t("completion.common.noDesc");

                    break;
                }
            }
        }

        return params;
    }

    private completeWord(ctx: CompletionContext, opIdx: number, prefix: string = "") {
        if (opIdx && ctx.stmt) {
            if (isInstruction(ctx.stmt) && getOperandType(ctx.stmt, opIdx) !== undefined)
                return this.completeOperand(ctx, getOperandType(ctx.stmt, opIdx)!, prefix);
            else if (ctx.stmt.type === "AliasDirective" && opIdx === 2) return this.completeRegOrDev(ctx, prefix);
            else if (ctx.stmt.type === "DefineDirective" && opIdx === 2) return this.completeNumber(ctx, prefix);
        } else return this.completeKeyword(ctx, prefix);
    }

    private completeKeyword(ctx: CompletionContext, prefix: string): CompletionItem[] {
        return CompletionHandler.insTree
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

    private completeOperand(ctx: CompletionContext, opType: OperandType, prefix: string): CompletionItem[] {
        switch (opType) {
            case OperandType.REG_IDENT:
            case OperandType.REG_NUM: {
                // 寄存器 + 数字（具体补数字留给用户自行输入）
                const matching = this.filterByPrefix(CompletionHandler.REGISTERS, prefix);
                const builtin: CompletionItem[] = matching.map(({ value, sort }) => ({
                    label: value,
                    kind: CompletionItemKind.Variable,
                    insertText: value,
                    detail: t("hover.operandType.register"),
                    sortText: sort
                }));

                // 用户定义的别名（从符号表，单次 for...in 避免 Object.entries 中间数组分配）
                const userAliases: CompletionItem[] = [];
                if (ctx.symbols)
                    for (const name in ctx.symbols) {
                        const sym = ctx.symbols[name];
                        if (
                            (sym.type === BasicType.REGISTER ||
                                ((opType as OperandType) === OperandType.REG_NUM &&
                                    (sym.category === TypeCategory.NUMBER || sym.category === TypeCategory.LABEL))) &&
                            name.startsWith(prefix)
                        ){
                            const detail = sym.category === TypeCategory.LABEL
                                ? t("hover.labelDef.type")
                                : sym.category === TypeCategory.NUMBER
                                    ? t("hover.operandType.number")
                                    : t("hover.operandType.register");
                            userAliases.push({
                                label: name,
                                kind: CompletionItemKind.Variable,
                                insertText: name,
                                detail,
                                labelDetails: {
                                    detail: `: ${sym.value}`,
                                    description: detail
                                },
                                documentation: sym.desc
                            });
                        }
                    }

                const result = [...builtin, ...userAliases];

                if (opType === OperandType.REG_NUM) result.push(...this.completeNumber(ctx, prefix));

                return result;
            }

            case OperandType.DEV_ALIAS:
            case OperandType.DEV_REF: {
                // 设备引用: d0-d5, db  + 用户别名
                const builtin = this.filterByPrefix(CompletionHandler.DEVICE_REFS, prefix).map(({ value, sort }) => ({
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
                        if (sym.type === BasicType.DEVICE && name.startsWith(prefix))
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
                return this.completeEnum(ctx, opType);

            case OperandType.JUMP_TARGET: {
                // 标签（从符号表，单次 for...in 避免链式中间数组分配）
                const result: CompletionItem[] = [];
                if (ctx.symbols) {
                    for (const name in ctx.symbols) {
                        const sym = ctx.symbols[name];
                        if (sym.category === TypeCategory.LABEL && name.startsWith(prefix))
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

    private completeEnum(ctx: CompletionContext, opType: OperandType): CompletionItem[] {
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

    private completeRegOrDev(ctx: CompletionContext, prefix: string): CompletionItem[] {
        const regItems: CompletionItem[] = this.filterByPrefix(CompletionHandler.REGISTERS, prefix).map(
            ({ value, sort }) => ({
                label: value,
                kind: CompletionItemKind.Variable,
                insertText: value,
                detail: t("hover.operandType.register"),
                sortText: sort
            })
        );
        const devItems = this.filterByPrefix(CompletionHandler.DEVICE_REFS, prefix).map(({ value, sort }) => ({
            label: value,
            kind: CompletionItemKind.Reference,
            insertText: value,
            detail: t("hover.operandType.device"),
            sortText: sort
        }));

        return [...regItems, ...devItems];
    }

    private completeNumber(ctx: CompletionContext, prefix: string): CompletionItem[] {
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
            .filter(i => i.label.toLowerCase().startsWith(prefix))
            .map(i => {
                const metaData = meta[i.label as keyof typeof meta];

                return {
                    ...i,
                    detail: metaData.value.signature,
                    data: { local: ctx.getLocale(), name: metaData.name, key: "Instruction" } satisfies CompletionData
                };
            });
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

    private filterByPrefix(items: BuiltinSymbolInfo[], prefix: string) {
        return items.filter(({ value }) => value.startsWith(prefix));
    }

}
