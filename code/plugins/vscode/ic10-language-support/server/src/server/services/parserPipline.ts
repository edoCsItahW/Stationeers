/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file parserPipline.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/29 17:03
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Lexer, Parser, Linker, IncLexer, IncParser, type SymbolMap, Diagnostic} from "ic10-node-api";
import stdLib from "ic10-node-api/static/stdLib.ic.json"
import { createHash } from "node:crypto";

import { DocCacheValue } from "../cache";


export interface ParseResult extends DocCacheValue {
    changed: boolean;
}


export class ParserPipline {
    /** 增量词法分析器（内部维护缓存，用于增量解析） */
    private incLexer = new IncLexer();
    /** 增量语法分析器（内部维护缓存，用于增量解析） */
    private incParser = new IncParser();

    /**
     * 过滤 IME 组合态可能触发词法分析器崩溃的字符。
     *
     * 中文输入法的拼音分隔符 `'`（如 `shui'beng`）被 IC10 词法分析器
     * 误认为单引号字符串界定符，组合态下必然产生未闭合的「字符串」，
     * 导致 Lexer 进入死循环 / 内存溢出。
     *
     * IC10 语法不使用单引号，替换为 `_` 保持 1:1 字符宽度，
     * 不影响后续解析和位置计算。
     *
     * @param code 原始源代码
     * @returns 安全替换后的代码
     */
    private sanitizeIME(code: string): string {
        return code.replace(/'/g, '_');
    }

    public async parse(code: string, cache?: DocCacheValue): Promise<ParseResult> {
        const noop: ParseResult = {
            changed: false,
            source: code,
            tokens: cache?.tokens ?? [],
            ast: cache?.ast!,
            diagnostics: cache?.diagnostics ?? [],
            symbols: cache?.symbols ?? null,
            hash: cache?.hash ?? ""
        };

        if (!code.trim().length) return noop;

        const hash = createHash("md5").update(code).digest("hex");
        if (cache?.hash === hash) return noop;

        const diagnostics: Diagnostic[] = []

        // IME 输入法拼音分隔符 ' 会触发词法分析器崩溃，替换为 _ 后再词法分析
        // 原始 code 保留用于 hash 和 source，确保位置不变
        const sanitized = this.sanitizeIME(code);

        const lexer = new Lexer(sanitized);
        const tokens = lexer.scan();

        diagnostics.push(...lexer.diagnostics);

        const parser = new Parser(tokens);
        const ast = parser.parse();

        diagnostics.push(...parser.diagnostics);

        const linker = new Linker();

        linker.addUnit(stdLib.content);
        linker.addUnit(ast);

        const symbolJson = linker.link().toJSON();

        diagnostics.push(...linker.diagnostics);

        return { changed: true, source: code, tokens, ast, diagnostics, symbols: JSON.parse(symbolJson) as SymbolMap, hash };
    }

    /**
     * 增量解析接口。利用 {@link IncLexer} 和 {@link IncParser} 的增量能力，
     * 仅重新分析发生变化的行和受影响的语句，避免全量重建 AST。
     *
     * 首次调用时内部缓为空，会自动通过 `tokenizeFull` / `parseFull` 建立缓存。
     * 后续调用使用 `tokenizeInc` / `parseInc` 实现增量更新。
     *
     * @note IncLexer / IncParser 不暴露 diagnostics，增量路径仅包含 Linker 诊断信息。
     *
     * @param code  当前文档全文
     * @param cache 文档缓存（用于哈希比对和回退）
     * @returns 解析结果，`changed: true` 表示 AST 有变化
     */
    public async parseInc(code: string, cache?: DocCacheValue): Promise<ParseResult> {
        console.log("[IC10 LSP] parseInc called, hasCache:", this.incLexer.hasCache());

        const noop: ParseResult = {
            changed: false,
            source: code,
            tokens: cache?.tokens ?? [],
            ast: cache?.ast!,
            diagnostics: cache?.diagnostics ?? [],
            symbols: cache?.symbols ?? null,
            hash: cache?.hash ?? ""
        };

        if (!code.trim().length) return noop;

        const hash = createHash("md5").update(code).digest("hex");
        if (cache?.hash === hash) return noop;

        const diagnostics: Diagnostic[] = [];

        // 首次调用 / 缓存失效：全量词法+语法分析以建立增量基准
        if (!this.incLexer.hasCache() || !this.incParser.hasCache()) {
            console.log("[IC10 LSP] parseInc: seeding incremental cache (tokenizeFull + parseFull)");

            const sanitized = this.sanitizeIME(code);
            const lexResult = this.incLexer.tokenizeFull(sanitized);
            const parseResult = this.incParser.parseFull(lexResult.tokens);

            const linker = new Linker();
            linker.addUnit(stdLib.content);
            linker.addUnit(parseResult.ast);

            const symbolJson = linker.link().toJSON();
            diagnostics.push(...linker.diagnostics);

            console.log("[IC10 LSP] parseInc: cache seeded, statements:", parseResult.ast.statements.length);
            return {
                changed: true,
                source: code,
                tokens: lexResult.tokens,
                ast: parseResult.ast,
                diagnostics,
                symbols: JSON.parse(symbolJson) as SymbolMap,
                hash
            };
        }

        // 增量词法分析
        console.log("[IC10 LSP] parseInc: tokenizeInc start");
        const sanitized = this.sanitizeIME(code);
        const lexResult = this.incLexer.tokenizeInc(sanitized);
        console.log("[IC10 LSP] parseInc: tokenizeInc done, incremental:", lexResult.incremental,
            "changedStartLine:", lexResult.changedStartLine);

        // 增量语法分析
        console.log("[IC10 LSP] parseInc: parseInc start");
        const parseResult = this.incParser.parseInc(lexResult.tokens, lexResult.changedStartLine);
        console.log("[IC10 LSP] parseInc: parseInc done, incremental:", parseResult.incremental,
            "statements:", parseResult.ast.statements.length);

        // 链接器（全量执行，Linker 不支持增量）
        const linker = new Linker();
        linker.addUnit(stdLib.content);
        linker.addUnit(parseResult.ast);

        const symbolJson = linker.link().toJSON();
        diagnostics.push(...linker.diagnostics);

        console.log("[IC10 LSP] parseInc: done, diagnostics:", diagnostics.length);
        return {
            changed: true,
            source: code,
            tokens: lexResult.tokens,
            ast: parseResult.ast,
            diagnostics,
            symbols: JSON.parse(symbolJson) as SymbolMap,
            hash
        };
    }

}
