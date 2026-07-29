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
import { Lexer, Parser, Linker, IncLexer, IncParser, type SymbolMap, Diagnostic } from "ic10-node-api";
import stdLib from "ic10-node-api/static/stdLib.ic.json";
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

    private sanitizeIME(code: string): string {
        return code.replace(/'/g, "_");
    }

    public async parse(code: string, cache?: DocCacheValue): Promise<ParseResult> {
        const noop: ParseResult = {
            changed: false,
            source: code,
            tokens: cache?.tokens ?? [],
            ast: cache?.ast!,
            diagnostics: cache?.diagnostics ?? [],
            symbols: cache?.symbols ?? null,
            types: cache?.types ?? null,
            hash: cache?.hash ?? ""
        };

        if (!code.trim().length) return noop;

        const hash = createHash("md5").update(code).digest("hex");
        if (cache?.hash === hash) return noop;

        const diagnostics: Diagnostic[] = [];

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

        const typesJson = linker.typeTable.toJSON();

        diagnostics.push(...linker.diagnostics);

        return {
            changed: true,
            source: code,
            tokens,
            ast,
            diagnostics,
            symbols: JSON.parse(symbolJson),
            types: JSON.parse(typesJson),
            hash
        };
    }

    public async parseInc(code: string, cache?: DocCacheValue): Promise<ParseResult> {
        const noop: ParseResult = {
            changed: false,
            source: code,
            tokens: cache?.tokens ?? [],
            ast: cache?.ast!,
            diagnostics: cache?.diagnostics ?? [],
            symbols: cache?.symbols ?? null,
            types: cache?.types ?? null,
            hash: cache?.hash ?? ""
        };

        if (!code.trim().length) return noop;

        const hash = createHash("md5").update(code).digest("hex");
        if (cache?.hash === hash) return noop;

        const diagnostics: Diagnostic[] = [];

        // 首次调用 / 缓存失效：全量词法+语法分析以建立增量基准
        if (!this.incLexer.hasCache() || !this.incParser.hasCache()) {
            const sanitized = this.sanitizeIME(code);
            const lexResult = this.incLexer.tokenizeFull(sanitized);
            const parseResult = this.incParser.parseFull(lexResult.tokens);

            const linker = new Linker();
            linker.addUnit(stdLib.content);
            linker.addUnit(parseResult.ast);

            const symbolJson = linker.link().toJSON();
            const typesJson = linker.typeTable.toJSON();

            diagnostics.push(...linker.diagnostics);

            return {
                changed: true,
                source: code,
                tokens: lexResult.tokens,
                ast: parseResult.ast,
                diagnostics,
                symbols: JSON.parse(symbolJson),
                types: JSON.parse(typesJson),
                hash
            };
        }

        // 增量词法分析
        const sanitized = this.sanitizeIME(code);
        const lexResult = this.incLexer.tokenizeInc(sanitized);

        // 增量语法分析
        const parseResult = this.incParser.parseInc(lexResult.tokens, lexResult.changedStartLine);

        // 链接器（全量执行，Linker 不支持增量）
        const linker = new Linker();

        linker.addUnit(stdLib.content);
        linker.addUnit(parseResult.ast);

        const symbolJson = linker.link().toJSON();
        const typesJson = linker.typeTable.toJSON();

        diagnostics.push(...linker.diagnostics);

        return {
            changed: true,
            source: code,
            tokens: lexResult.tokens,
            ast: parseResult.ast,
            diagnostics,
            symbols: JSON.parse(symbolJson),
            types: JSON.parse(typesJson),
            hash
        };
    }
}
