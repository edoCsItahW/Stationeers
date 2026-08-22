/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file documentCache.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/27 21:40
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Token, Program, type Diagnostic, type SymbolMap, type TypeTableMap, SymbolTable } from "ic10c-node";

import { Optional, Nullable } from "common";

/**
 * @summary 单个文档的缓存数据结构
 *
 * @summary Cache data structure for a single document
 *
 * @desc 存储单个 IC10 文档解析后的完整信息，包括原始源代码、词法令牌、AST、
 * 语义诊断、符号表和类型表。hash 字段用于增量更新检测——内容未变化时跳过重新解析。
 *
 * @desc Stores the complete parsed information of a single IC10 document, including
 * original source code, lexical tokens, AST, semantic diagnostics, symbol table, and
 * type table. The hash field enables incremental update detection — re-parsing is
 * skipped when the content has not changed.
 * */
export interface DocCacheValue {
    source: string;
    tokens: Token[];
    ast?: Program;
    diagnostics: Diagnostic[];
    symbols?: SymbolMap;
    symbolTable?: SymbolTable;
    types?: TypeTableMap;
    hash: string;
}

/**
 * @summary 文档级缓存管理器
 *
 * @summary Document-level cache manager
 *
 * @desc 以文档 URI 为键管理所有已打开 IC10 文档的解析结果缓存。提供文档初始化、
 * 缓存查询、hash 失效和解析后更新等方法，支持 LSP 服务器各处理器高效获取
 * 文档的 AST、tokens、symbols 等信息。基于 hash 的增量更新避免重复解析。
 *
 * @desc Manages parsed result caches for all open IC10 documents, keyed by document URI.
 * Provides methods for document initialization, cache retrieval, hash invalidation,
 * and post-parse updates, enabling LSP server handlers to efficiently access document
 * ASTs, tokens, symbols, etc. Hash-based incremental updates avoid redundant re-parsing.
 * */
export class DocumentCache {
    private caches: Map<string, DocCacheValue> = new Map();

    initDocument(uri: string): void {
        this.caches.set(uri, {
            source: "",
            tokens: [],
            diagnostics: [],
            hash: ""
        });
    }

    getCache(uri: string): Optional<DocCacheValue> {
        return this.caches.get(uri);
    }

    invalidateHash(uri: string): void {
        const cache = this.caches.get(uri);
        if (cache) cache.hash = "";
    }

    updateAfterParse(uri: string, result: DocCacheValue) {
        const cache = this.caches.get(uri);

        if (!cache) return;

        Object.assign(cache, result);
    }
}
