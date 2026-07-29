// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import type { CompletionTriggerKind, CompletionItem } from "vscode-languageserver/node";
import type {StatementNode, SymbolMap, Token, TypeTableMap} from "ic10-node-api";

import type { DocCacheValue } from "../../cache";
import type {Nullable, Optional} from "common";


/**
 * @summary 补全上下文 — 传递给每个 CompletionProvider 的上下文信息
 *
 * @summary Completion context — context information passed to each CompletionProvider
 *
 * @desc 包含光标位置、触发方式、当前行的 token 和 AST 语句、文档缓存、
 *  符号表、类型表、已输入前缀以及语言环境等完整上下文，供各补全提供器
 *  判断是否能处理并生成补全项。
 *
 * @desc Contains the full context including cursor position, trigger kind, current line
 *  token and AST statement, document cache, symbol table, type table, input prefix,
 *  and locale for each completion provider to determine eligibility and generate items.
 * */
export interface CompletionContext {
    /** 1-based 行号 */
    /** 1-based line number */
    line: number;
    /** 1-based 列号 */
    /** 1-based column number */
    character: number;
    /** 此次补全的触发方式 */
    /** Trigger kind for this completion */
    triggerKind: CompletionTriggerKind;
    /** 触发字符（如有） */
    /** Trigger character (if any) */
    triggerCharacter: string | undefined;
    /** 光标所在行的所有 token */
    /** Token at the cursor line */
    token: Optional<Token>;
    /** 当前行的语句节点（可能为 null / ErrorNode） */
    /** Statement node of the current line (may be null or ErrorNode) */
    stmt: Nullable<StatementNode>;
    /** 文档完整缓存 */
    /** Full document cache */
    cache: DocCacheValue;
    /** 符号表 */
    /** Symbol table */
    symbols: Nullable<SymbolMap>;
    /** 类型表 */
    /** Type table */
    types: Nullable<TypeTableMap>;
    /** 光标左边已输入的文本（当前行光标前部分） */
    /** Text already typed to the left of the cursor (prefix of current line) */
    prefix: string;
    /** 获取当前语言 */
    /** Get current locale */
    getLocale(): "zh-hans" | "en-us";
}

/**
 * @summary 补全数据 — 传递给 onCompletionResolve 用于延迟加载文档
 *
 * @summary Completion data — passed to onCompletionResolve for lazy-loading documentation
 *
 * @desc 用于在补全项被选中时异步加载详细的文档描述。
 *
 * @desc Used to asynchronously load detailed documentation when a completion item is selected.
 * */
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
 * @summary 补全提供器接口 — 为特定上下文生成补全项列表
 *
 * @summary Completion provider interface — generates completion item lists for specific contexts
 *
 * @desc 每个实现类负责判断是否能处理当前补全上下文，并生成对应的
 *  CompletionItem 列表。第一个能处理的提供器独占响应。
 *
 * @desc Each implementation determines whether it can handle the current completion
 *  context and generates the corresponding CompletionItem list.
 *  The first provider that can handle it takes exclusive response.
 * */
export interface CompletionProvider {
    /**
     * @summary 判断此提供器是否能处理当前上下文
     *
     * @summary Whether this provider can handle the current context
     *
     * @param ctx 补全上下文
     * @param ctx Completion context
     *
     * @returns 如果能处理返回 true
     * @returns True if this provider can handle the context
     * */
    canHandle(ctx: CompletionContext): boolean;
    /**
     * @summary 生成补全项列表
     *
     * @summary Generate completion item list
     *
     * @param ctx 补全上下文
     * @param ctx Completion context
     *
     * @returns 补全项数组
     * @returns Array of completion items
     * */
    provide(ctx: CompletionContext): CompletionItem[];
}
