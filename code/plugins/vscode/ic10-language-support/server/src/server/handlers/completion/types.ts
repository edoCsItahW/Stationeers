// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

import type { CompletionItem } from "vscode-languageserver/node";
import type { CompletionTriggerKind } from "vscode-languageserver/node";
import type { StatementNode, SymbolMap, Token } from "ic10-node-api";

import type { DocCacheValue } from "../../cache";
import type {Nullable, Optional} from "../../../../../common/types/utils";

/** 补全场景分类 */
export type CompletionScenario =
    | "keyword"       // 正在输入关键字（字母触发）
    | "operand"       // 关键字后补全操作数（空格触发）
    | "directive";    // alias / define 等指令特有上下文

/** 传递给每个 Provider 的上下文 */
export interface CompletionContext {
    /** 1-based 行号 */
    line: number;
    /** 1-based 列号 */
    character: number;
    /** 此次补全的触发方式 */
    triggerKind: CompletionTriggerKind;
    /** 触发字符（如有） */
    triggerCharacter: string | undefined;
    /** 光标所在行的所有 token */
    token: Optional<Token>;
    /** 当前行的语句节点（可能为 null / ErrorNode） */
    stmt: Nullable<StatementNode>;
    /** 文档完整缓存 */
    cache: DocCacheValue;
    /** 符号表 */
    symbols: Nullable<SymbolMap>;
    /** 光标左边已输入的文本（当前行光标前部分） */
    prefix: string;
    /** 当前语言 */
    getLocale(): "zh-hans" | "en-us";
}

/** 补全项生成器接口 */
export interface CompletionProvider {
    /** 是否能处理当前上下文 */
    canHandle(ctx: CompletionContext): boolean;
    /** 生成补全项列表 */
    provide(ctx: CompletionContext): CompletionItem[];
}
