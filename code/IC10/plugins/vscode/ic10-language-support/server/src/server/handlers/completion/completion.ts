// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file completion.ts
 * @author edocsitahw
 * @version 2.0
 * @date 2026/07/30 20:00
 * @desc 代码补全处理器。基于光标相对位置（RelativeState）与触发类型
 *       （CompletionTriggerKind）的组合状态机分发补全请求，
 *       通过 RadixTree 前缀匹配提供指令关键字补全，
 *       通过 AST 操作数类型（OperandType）提供操作数补全。
 * @desc Completion handler. Dispatches completion requests via a combined state
 *       machine of cursor RelativeState and CompletionTriggerKind. Provides
 *       keyword completion via RadixTree prefix matching and operand completion
 *       based on AST OperandType.
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
    TokenCategory,
    RegisterNode,
    TypeCategory,
    TypeTableMap,
    OperandType,
    DeviceNode,
    DeviceType,
    SymbolMap,
    BasicType,
    ErrorNode,
    TokenType
} from "ic10c-node";

import { ENUMS_LOCAL_MAP, INS_LOCAL_MAP, INS_META_MAP, LOGIC_LOCAL_MAP, LOGIC_SLOT_LOCAL_MAP } from "../../../mateData";
import { end, getOperandType, isInstruction, RadixTree, findRangeTokens } from "../../../utils";
import { Console, debug, lowerBound, Optional } from "common";
import { DocumentCache } from "../../cache";
import { locale, t } from "../../../locals";

type OnCompletionHandlerType = Parameters<Connection["onCompletion"]>[0];
type OnCompletionResolveHandlerType = Parameters<Connection["onCompletionResolve"]>[0];

/**
 * @summary 光标相对于 token 的位置状态
 *
 * @summary Cursor position state relative to surrounding tokens
 *
 * @desc 描述光标在当前行中相对于已有 token 的位置关系，用于补全状态机决策。
 * 由 handle() 方法根据 findRangeTokens 返回的 prev/curr/next 索引推导得出。
 *
 * @desc Describes the cursor's positional relationship to existing tokens on the
 * current line, used by the completion state machine for dispatch decisions.
 * Derived in handle() from the prev/curr/next indices returned by findRangeTokens.
 * */
enum RelativeState {
    /** @summary 位于一个单词中间 / Inside a word (cursor splits a token) */
    INSIDE_WORD = 0,
    /** @summary 位于一个单词的结束位置 / At the end of a word */
    END_WORD,
    /** @summary 位于一个单词的起始位置 / At the start of a new word */
    START_WORD,
    /** @summary 位于两个单词的空隙处 / In a gap between words */
    INSIDE_GAP
}

const REL_SHIFT = 2;
const TRIG_MASK = 0x03;

/**
 * @summary 将 RelativeState 与 CompletionTriggerKind 打包为单一状态值
 *
 * @summary Pack RelativeState and CompletionTriggerKind into a single state value
 *
 * @desc 使用位运算将两个枚举组合：高 2 位存 RelativeState，低 2 位存 CompletionTriggerKind。
 * 组合后的值用于 State 枚举的 switch 分发。
 *
 * @desc Packs two enums via bit operations: upper 2 bits for RelativeState,
 * lower 2 bits for CompletionTriggerKind. The combined value drives the
 * State enum's switch dispatch.
 * */
function combine(rel: RelativeState, trig: CompletionTriggerKind) {
    return (rel << 2) | trig;
}

/**
 * @summary 从组合状态值中拆分出 RelativeState 与 CompletionTriggerKind
 *
 * @summary Unpack RelativeState and CompletionTriggerKind from a combined state value
 *
 * @desc combine 的逆操作，用于在 switch 分支内按需提取原始分量。
 *
 * @desc Inverse of combine(), used within switch branches to extract
 * original components on demand.
 * */
function decompose(state: State): [RelativeState, CompletionTriggerKind] {
    return [state >> REL_SHIFT, (state & TRIG_MASK) as CompletionTriggerKind];
}

/**
 * @summary 补全状态枚举（RelativeState × CompletionTriggerKind 的笛卡尔积）
 *
 * @summary Completion state enum (Cartesian product of RelativeState × CompletionTriggerKind)
 *
 * @desc 每个 State 值由 combine() 将 RelativeState（光标位置）与
 * CompletionTriggerKind（触发方式）打包而成，handle() 方法据此用 switch
 * 分发到对应的补全逻辑分支。共 4×3=12 种状态。
 *
 * @desc Each State value is packed by combine() from RelativeState (cursor position)
 * and CompletionTriggerKind (trigger type). The handle() method uses a switch
 * to dispatch to the corresponding completion logic branch. 4×3=12 states total.
 * */
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

/**
 * @summary 补全上下文，贯穿各补全方法的数据载体
 *
 * @summary Completion context, the data carrier shared across completion methods
 *
 * @desc 封装 AST 语句节点、符号表、类型表和语言环境，由 handle() 构建，
 * 传递给 completeWord → completeKeyword / completeOperand 等方法链。
 *
 * @desc Encapsulates the AST statement node, symbol table, type table, and locale.
 * Built by handle() and passed down through completeWord → completeKeyword /
 * completeOperand method chain.
 * */
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

/**
 * @summary 内置符号信息（寄存器 / 设备引用）
 *
 * @summary Builtin symbol info (registers / device references)
 *
 * @desc 用于 REGISTERS 和 DEVICE_REFS 静态数组的元素类型，
 * value 为补全标签，sort 为排序键。
 *
 * @desc Element type for the REGISTERS and DEVICE_REFS static arrays.
 * `value` is the completion label, `sort` is the sort key.
 * */
export interface BuiltinSymbolInfo {
    value: string;
    sort: string;
}

/**
 * @summary LSP 代码补全处理器
 *
 * @summary LSP completion handler
 *
 * @desc 统一处理关键字补全与操作数补全。核心流程：
 * 1. handle() 根据光标位置和触发类型计算组合状态（State）
 * 2. 通过 switch 分发到 completeWord()
 * 3. completeWord() 根据 opIdx 判断补全关键字还是操作数
 * 4. 操作数补全进一步根据 OperandType 分发到寄存器/设备/枚举/标签等
 *
 * @desc Unified handler for keyword and operand completion. Core flow:
 * 1. handle() computes a combined State from cursor position and trigger kind
 * 2. Dispatches to completeWord() via a switch
 * 3. completeWord() decides keyword vs. operand based on opIdx
 * 4. Operand completion further dispatches by OperandType to
 *    register / device / enum / label etc.
 * */
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

    /**
     * @summary 处理 LSP onCompletion 请求
     *
     * @summary Handle LSP onCompletion request
     *
     * @desc 核心分发逻辑。通过 findRangeTokens 定位光标周围的 token，
     * 推导 RelativeState（INSIDE_WORD / END_WORD / START_WORD / INSIDE_GAP），
     * 再与 CompletionTriggerKind 组合成 State，用 switch 分发补全：
     * - INSIDE_GAP + TRIGGER_CHAR/INVOKED：补全下一个词
     * - INSIDE_WORD/END_WORD + INVOKED/TRIGGER_INCOMPLETE：补全当前词
     * - START_WORD + INVOKED：补全当前位置
     * - 其余状态不补全
     *
     * @desc Core dispatch logic. Uses findRangeTokens to locate tokens around the
     * cursor, derives RelativeState, combines it with CompletionTriggerKind into
     * State, and dispatches completion via a switch:
     * - INSIDE_GAP + TRIGGER_CHAR/INVOKED: complete next word
     * - INSIDE_WORD/END_WORD + INVOKED/TRIGGER_INCOMPLETE: complete current word
     * - START_WORD + INVOKED: complete at position
     * - Other states: no completion
     * */
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

        if (!cache || !cache.ast || !cache.symbols || !context)
            // 无内容则补全关键字
            return this.completeKeyword(
                {
                    getLocale: () => locale.getLocale()
                } as CompletionContext,
                ""
            );

        // 统一为1-based
        const line = position.line + 1;
        const column = position.character + 1;

        // 当前语句索引
        const stmtIdx = lowerBound(cache.ast.statements, n => n.position.line >= line);

        const ctx: CompletionContext = {
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

        // 前一个token与当前光标间的空格数
        const prevBlocks = prevToken ? column - end(prevToken).column : 0;
        // 操作数索引
        let opIdx = prevIdx; // -1则补keyword(0)，其余补operand${opIdx}

        if (
            prevBlocks > 0 ||  // 存在空格则应判断为下一个操作数
            opIdx === -1       // 没有前一个token表示位于行首，则加1成0
        )
            opIdx++;

        // 相对位置状态，该形式仅用于简化分支，其中1为特殊情况，使其命中正确的索引
        const rel: RelativeState = [
            [RelativeState.INSIDE_WORD, RelativeState.END_WORD],
            [RelativeState.START_WORD , currIdx > 0 ? RelativeState.INSIDE_WORD : RelativeState.INSIDE_GAP]
        ][prevIdx >= 0 ? Number(prevBlocks > 0 /* 光标前的空格数 */) : 1][
            nextIdx >= 0 ? Number(tokens[nextIdx].pos.column - column > 0 /* 光标后的空格数 */) : 1
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

    /**
     * @summary 处理 LSP onCompletionResolve 请求
     *
     * @summary Handle LSP onCompletionResolve request
     *
     * @desc 根据补全项 data 中的 key 字段，从对应的本地化映射表
     * （INS_LOCAL_MAP / LOGIC_LOCAL_MAP / LOGIC_SLOT_LOCAL_MAP / ENUMS_LOCAL_MAP）
     * 查找文档并填充到 params.documentation。
     *
     * @desc Resolves completion item documentation. Based on the `key` field in
     * `data`, looks up the corresponding localization map
     * (INS_LOCAL_MAP / LOGIC_LOCAL_MAP / LOGIC_SLOT_LOCAL_MAP / ENUMS_LOCAL_MAP)
     * and populates params.documentation.
     * */
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

    /**
     * @summary 补全分发核心：根据操作数索引选择关键字或操作数补全
     *
     * @summary Completion dispatch core: select keyword or operand completion by opIdx
     *
     * @desc opIdx 为 0 或无语句时补全指令关键字；opIdx > 0 时根据语句类型
     * 分发到对应操作数补全（指令用 getOperandType，alias 用 completeRegOrDev，
     * define 用 completeNumber）。
     *
     * @desc When opIdx is 0 or no statement exists, completes instruction keywords;
     * when opIdx > 0, dispatches to operand completion based on statement type
     * (instructions use getOperandType, alias uses completeRegOrDev,
     * define uses completeNumber).
     *
     * @param ctx 补全上下文 / Completion context
     * @param opIdx 操作数索引（0=关键字，1+=操作数）/ Operand index (0=keyword, 1+=operand)
     * @param prefix 已输入的前缀 / Already-typed prefix
     * */
    private completeWord(ctx: CompletionContext, opIdx: number, prefix: string = "") {
        // 不是行首
        if (opIdx && ctx.stmt) {
            // 是可执行指令，则补全操作数
            if (isInstruction(ctx.stmt) && getOperandType(ctx.stmt, opIdx) !== undefined)
                return this.completeOperand(ctx, getOperandType(ctx.stmt, opIdx)!, prefix);

            // 是预处理指令，则不补全第一个操作数（用户自定义标识符），如果是第二个操作数则提供补全
            else if (ctx.stmt.type === "AliasDirective" && opIdx === 2) return this.completeRegOrDev(ctx, prefix);
            else if (ctx.stmt.type === "DefineDirective" && opIdx === 2) return this.completeNumber(ctx, prefix);
        }

        // 是行首，提供关键字补全
        else return this.completeKeyword(ctx, prefix);
    }

    /**
     * @summary 指令关键字补全
     *
     * @summary Instruction keyword completion
     *
     * @desc 通过 RadixTree 前缀匹配查找所有以 prefix 开头的指令，
     * 过滤出 type === "Instruction" 的条目并构造 CompletionItem。
     *
     * @desc Finds all instructions starting with `prefix` via RadixTree prefix
     * matching, filters for type === "Instruction", and builds CompletionItems.
     *
     * @param ctx 补全上下文 / Completion context
     * @param prefix 已输入的前缀 / Already-typed prefix
     * @returns 补全项数组 / Array of completion items
     * */
    private completeKeyword(ctx: CompletionContext, prefix: string): CompletionItem[] {
        return CompletionHandler.insTree
            .entriesWithPrefix(prefix)
            // 过滤掉不是指令的元数据
            .filter(([, v]) => v.type === "Instruction")
            .map(([key, value]) => ({
                label: key,
                kind: CompletionItemKind.Keyword,
                insertText: key,
                detail: value.signature,
                data: { local: ctx.getLocale(), name: key, key: "Instruction" } satisfies CompletionData
            }));
    }

    /**
     * @summary 操作数补全
     *
     * @summary Operand completion
     *
     * @desc 根据 OperandType 分发到具体补全逻辑：
     * - REG_IDENT / REG_NUM：寄存器 + 用户别名（+ 数字）
     * - DEV_ALIAS / DEV_REF：设备引用 + 用户别名
     * - LOGIC_TYPE / LOGIC_SLOT / REAGENT_MODE / BATCH_MODE / SLOT_IDX：枚举补全
     * - JUMP_TARGET：标签补全
     *
     * @desc Dispatches to specific completion logic based on OperandType:
     * - REG_IDENT / REG_NUM: registers + user aliases (+ numbers)
     * - DEV_ALIAS / DEV_REF: device references + user aliases
     * - LOGIC_TYPE / LOGIC_SLOT / REAGENT_MODE / BATCH_MODE / SLOT_IDX: enum completion
     * - JUMP_TARGET: label completion
     *
     * @param ctx 补全上下文 / Completion context
     * @param opType 操作数类型 / Operand type
     * @param prefix 已输入的前缀 / Already-typed prefix
     * @returns 补全项数组 / Array of completion items
     * */
    private completeOperand(ctx: CompletionContext, opType: OperandType, prefix: string): CompletionItem[] {
        switch (opType) {
            case OperandType.REG_IDENT:  // 寄存器类型
            case OperandType.REG_NUM: {  // 寄存器或数值类型
                // 内置寄存器
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
                            (sym.type === BasicType.REGISTER || // 标识符的值为寄存器类型
                                ((opType as OperandType) === OperandType.REG_NUM && // 如果操作数期望register or number类型，则还提供类型数字类型的标识符
                                    (sym.category === TypeCategory.NUMBER || sym.category === TypeCategory.LABEL))) &&
                            name.startsWith(prefix)
                        ) {
                            const detail =
                                sym.category === TypeCategory.LABEL
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

                // 如果操作数期望register or number类型，则还提供类型数字类型的补全
                if (opType === OperandType.REG_NUM)
                    result.push(...this.completeNumber(ctx, prefix), ...this.completeConstant(ctx, prefix));

                return result;
            }

            case OperandType.DEV_ALIAS:  // 设备别名类型
            case OperandType.DEV_REF: {  // 设备引用类型
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

    /**
     * @summary 枚举补全（设备感知）
     *
     * @summary Enum completion (device-aware)
     *
     * @desc 先通过 findPrevDevice 查找语句中前序的设备操作数，若找到则尝试
     * 从符号表解析设备类型并调用 deviceCompletions 提供设备特定的枚举值。
     * 若无法解析设备类型，则回退到全局类型表中的枚举定义。
     *
     * @desc First finds the preceding device operand via findPrevDevice. If found,
     * attempts to resolve the device type from the symbol table and calls
     * deviceCompletions for device-specific enum values. Falls back to global
     * enum definitions from the type table if the device type cannot be resolved.
     *
     * @param ctx 补全上下文 / Completion context
     * @param opType 操作数类型 / Operand type
     * @returns 补全项数组 / Array of completion items
     * */
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

    /**
     * @summary 设备特定的枚举补全
     *
     * @summary Device-specific enum completion
     *
     * @desc 根据设备类型定义提供特定的 logicSlot / logic / batchMode / slotIndex 值。
     * 由 completeEnum 在成功解析设备类型后调用。
     *
     * @desc Provides device-specific logicSlot / logic / batchMode / slotIndex values
     * based on the device type definition. Called by completeEnum after successfully
     * resolving the device type.
     *
     * @param ctx 补全上下文 / Completion context
     * @param type 设备类型定义 / Device type definition
     * @param opType 操作数类型 / Operand type
     * @returns 补全项数组 / Array of completion items
     * */
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

    /**
     * @summary 寄存器或设备引用补全（用于 alias 指令）
     *
     * @summary Register or device reference completion (for alias directive)
     *
     * @desc 合并寄存器（r0-r15, ra, sp）和设备引用（d0-d5, db）的补全项，
     * 按前缀过滤。专用于 alias 指令的第三个操作数。
     *
     * @desc Merges register (r0-r15, ra, sp) and device reference (d0-d5, db)
     * completion items, filtered by prefix. Dedicated to the third operand of
     * the alias directive.
     *
     * @param ctx 补全上下文 / Completion context
     * @param prefix 已输入的前缀 / Already-typed prefix
     * @returns 补全项数组 / Array of completion items
     * */
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

    /**
     * @summary 数字/特殊符号补全（用于 define 指令和 REG_NUM 操作数）
     *
     * @summary Number / special symbol completion (for define directive and REG_NUM operands)
     *
     * @desc 提供二进制前缀（%）、哈希前缀（$）、HASH() 和 STR() 函数模板的补全。
     * 每个补全项关联对应的指令元数据以支持 resolve 阶段的文档查询。
     *
     * @desc Provides completion for binary prefix (%), hash prefix ($),
     * HASH() and STR() function templates. Each item is linked to its
     * instruction metadata for documentation resolution.
     *
     * @param ctx 补全上下文 / Completion context
     * @param prefix 已输入的前缀 / Already-typed prefix
     * @returns 补全项数组 / Array of completion items
     * */
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

    private completeConstant(ctx: CompletionContext, prefix: string): CompletionItem[] {
        return CompletionHandler.insTree
            .entriesWithPrefix(prefix)
            .filter(([, v]) => v.type === "Constant")
            .map(([k, v]) => ({
                label: k,
                kind: CompletionItemKind.Constant,
                insertText: k,
                detail: v.signature,
                data: { local: ctx.getLocale(), name: k, key: "Instruction" } satisfies CompletionData
            }));
    }

    /**
     * @summary 在语句中查找前序设备操作数
     *
     * @summary Find the preceding device operand in a statement
     *
     * @desc 遍历语句节点的 typeN 字段，找到第一个值为 DEV_REF 或 DEV_ALIAS 的
     * 操作数类型字段，然后返回对应的 operandN 节点。用于 completeEnum 的设备感知补全。
     *
     * @desc Iterates over the statement node's `typeN` fields, finds the first
     * field whose value is DEV_REF or DEV_ALIAS, and returns the corresponding
     * `operandN` node. Used by completeEnum for device-aware completion.
     *
     * @param stmt 语句节点 / Statement node
     * @returns 设备/寄存器/标识符节点，或 null / Device, register, identifier node, or null
     * */
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

    /**
     * @summary 按前缀过滤内置符号列表
     *
     * @summary Filter builtin symbol list by prefix
     *
     * @param items 内置符号数组 / Builtin symbol array
     * @param prefix 前缀 / Prefix
     * @returns 匹配的符号数组 / Filtered symbol array
     * */
    private filterByPrefix(items: BuiltinSymbolInfo[], prefix: string) {
        return items.filter(({ value }) => value.startsWith(prefix));
    }
}
