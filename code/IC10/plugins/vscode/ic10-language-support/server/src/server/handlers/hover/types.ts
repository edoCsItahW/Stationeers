/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file types.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import type { StatementNode, SymbolMap } from "ic10c-node";
import type { Hover } from "vscode-languageserver/node";
import { t } from "../../../locals"

import { Nullable } from "common";

/**
 * @summary 悬停上下文 — 传递给每个悬停提供器的上下文信息
 *
 * @summary Hover context — context information passed to each hover provider
 *
 * @desc 包含当前光标位置、符号表、语句列表以及国际化辅助函数，供
 *  各个 HoverProvider 在生成悬停内容时使用。
 *
 * @desc Contains the current cursor position, symbol table, statement list,
 *  and i18n helper functions for use by each HoverProvider when generating hover content.
 * */
export interface HoverContext {
    /** 当前行号（1-based） */
    /** Current line number (1-based) */
    line: number;
    /** 当前列号（1-based） */
    /** Current column number (1-based) */
    character: number;
    /** 符号表 */
    /** Symbol table */
    symbols?: SymbolMap;
    /** 文档中的所有语句 */
    /** All statements in the document */
    statements: StatementNode[];
    /** 获取当前语言环境 */
    /** Get the current locale */
    getLocale(): "zh-hans" | "en-us";
    /** 国际化翻译函数 */
    /** Internationalization translation function */
    t: typeof t
}

/**
 * @summary 悬停提供器接口 — 为特定语句类型生成悬停内容
 *
 * @summary Hover provider interface — generates hover content for a specific statement type
 *
 * @desc 每个实现类负责判断是否能处理给定语句节点，并在光标位于相关位置时
 *  生成 Hover 内容。返回 null 表示光标不在可悬停范围内。
 *
 * @desc Each implementation determines whether it can handle a given statement node
 *  and generates Hover content when the cursor is at a relevant position.
 *  Returns null if the cursor is not within a hoverable range.
 * */
export interface IHoverProvider {
    /**
     * @summary 判断此提供器是否能处理给定语句
     *
     * @summary Whether this provider can handle the given statement
     *
     * @param node AST 语句节点
     * @param node AST statement node
     *
     * @returns 如果能处理返回 true
     * @returns True if this provider can handle the node
     * */
    canHandle(node: StatementNode): boolean;
    /**
     * @summary 生成悬停内容
     *
     * @summary Generate hover content
     *
     * @param node AST 语句节点
     * @param node AST statement node
     * @param ctx 悬停上下文
     * @param ctx Hover context
     *
     * @returns 悬停内容，若光标不在相关位置则返回 null
     * @returns Hover content, or null if the cursor is not on a relevant position
     * */
    provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover>;
}
