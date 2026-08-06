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
import { Lexer, Parser, Linker, IncLexer, IncParser, Diagnostic, StatementNode } from "ic10c_node";
import stdLib from "ic10c-node/static/stdLib.ic.json";
import { createHash } from "node:crypto";

import { DocCacheValue } from "../cache";
import { Console, debug } from "common";
import { t } from "../../locals/locale";

/** 会改变符号表的声明式语句类型集合 */
const DECLARATIVE_TYPES = new Set(["AliasDirective", "DefineDirective", "LabelDef"]);

/**
 * @summary 检查语句范围内是否包含声明式节点
 *
 * @summary Check if statement range contains declarative nodes
 *
 * @desc 遍历指定范围内的语句，检测是否存在会改变符号表的声明式节点
 * （AliasDirective / DefineDirective / LabelDef）。
 * 若不存在声明式节点，则可跳过 Linker 并复用缓存的符号表和类型表。
 *
 * @desc Scans statements in the given range to detect declarative nodes
 * (AliasDirective / DefineDirective / LabelDef) that modify the symbol table.
 * If no declarative nodes found, the Linker can be skipped and cached symbol/type
 * tables can be reused.
 *
 * @param statements - 语句列表
 * @param statements - Statement list
 * @param startIdx - 检查起始索引
 * @param startIdx - Start index for checking
 * @returns 若包含声明式节点返回 true
 * @returns true if declarative nodes are present
 */
function hasDeclarativeChanges(statements: StatementNode[], startIdx: number): boolean {
    for (let i = startIdx; i < statements.length; i++) {
        if (DECLARATIVE_TYPES.has(statements[i].type)) return true;
    }
    return false;
}

/**
 * @summary 解析结果，继承缓存值并附加变更标记
 *
 * @summary Parse result, extends cache value with a change flag
 *
 * @desc 封装 IC10 代码解析后的完整产物，包括 tokens、AST、诊断信息、符号表、
 * 类型表和内容哈希。`changed` 字段用于指示本次解析是否产生了新的结果，
 * 避免对未修改的代码进行无意义的后续处理（如诊断刷新、语义高亮刷新）。
 *
 * @desc Wraps the complete output of IC10 code parsing, including tokens, AST,
 * diagnostics, symbol table, type table, and content hash. The `changed` field
 * indicates whether this parse produced new results, avoiding pointless downstream
 * processing (e.g., diagnostic refresh, semantic token refresh) for unchanged code.
 */
export interface ParseResult extends DocCacheValue {
    /** 本次解析是否产生了变更（与缓存相比） */
    changed: boolean;
}

/**
 * @class ParserPipline
 *
 * @summary IC10 代码解析流水线，编排词法分析、语法分析、符号链接的全量/增量流程
 *
 * @summary IC10 code parsing pipeline that orchestrates full/incremental lexing,
 * parsing, and symbol linking
 *
 * @desc 作为语言服务核心的解析入口，封装了 Lexer → Parser → Linker 的完整流水线。
 * 提供 `parse`（全量）和 `parseInc`（增量）两种模式。增量模式通过维护内部
 * IncLexer/IncParser 缓存来实现在大文件中仅重新分析变更区域，显著降低编辑延迟。
 * 两种模式均内置了基于 MD5 的缓存校验，避免对未修改代码重复解析。
 *
 * @desc Serves as the central parsing entry point for the language server,
 * encapsulating the full Lexer → Parser → Linker pipeline. Provides `parse`
 * (full) and `parseInc` (incremental) modes. The incremental mode maintains
 * internal IncLexer/IncParser caches to re-analyze only changed regions in
 * large files, significantly reducing editing latency. Both modes include
 * MD5-based cache validation to avoid re-parsing unchanged code.
 *
 * @remarks Linker 不支持增量操作，因此在增量模式下链接阶段仍需全量执行。
 *
 * @remarks The Linker does not support incremental operation, so the linking
 * phase always runs in full even in incremental mode.
 */
export class ParserPipline {
    /** 增量词法分析器（内部维护缓存，用于增量解析） */
    private incLexer = new IncLexer();
    /** 增量语法分析器（内部维护缓存，用于增量解析） */
    private incParser = new IncParser();

    /**
     * @summary 全量解析 IC10 源代码
     *
     * @summary Full parse of IC10 source code
     *
     * @desc 对输入的完整源代码执行 Lexer → Parser → Linker 全量流水线。
     * 内置 MD5 缓存检查：若代码未变更则返回 `changed: false` 的 noop 结果。
     * 适用于文件首次打开、增量解析失败回退等场景。
     *
     * @desc Executes the full Lexer → Parser → Linker pipeline on the input
     * source code. Includes MD5 cache check: returns a noop result with
     * `changed: false` if the code hasn't changed. Suitable for initial file
     * open, incremental parse fallback, etc.
     *
     * @param code - 待解析的完整 IC10 源代码
     * @param code - Complete IC10 source code to parse
     * @param cache - 上一次解析的缓存结果，用于 MD5 比对跳过高开销的重复解析
     * @param cache - Previous parse cache result, used for MD5 comparison to skip expensive re-parsing
     * @returns 解析结果，包含 tokens、AST、诊断、符号表、类型表和变更标记
     * @returns Parse result with tokens, AST, diagnostics, symbols, types, and change flag
     *
     * @throws 解析错误由 @debug 装饰器捕获并记录，不会向上层抛出
     * @throws Parse errors are caught and logged by the @debug decorator, not thrown upward
     */
    @debug({
        message: err => t("server.handler.error", { name: "parser", err: (err as Error).message }),
        logger: msg => Console.error(msg, "parser"),
        rethrow: false
    })
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

        const lexer = new Lexer(code);
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

    /**
     * @summary 增量解析 IC10 源代码
     *
     * @summary Incremental parse of IC10 source code
     *
     * @desc 优先使用增量解析管线（IncLexer → IncParser → Linker），仅对自上次
     * 解析以来发生变化的代码区域重新分析。当内部增量缓存失效时（如首次调用），
     * 自动回退到全量解析以建立基准。内置 MD5 缓存检查与 `parse` 一致。
     * 适用于用户在编辑器中连续编辑大文件的场景，可大幅减少 CPU 开销。
     *
     * @desc Preferentially uses the incremental parsing pipeline
     * (IncLexer → IncParser → Linker), re-analyzing only the code regions that
     * have changed since the last parse. When internal incremental caches are
     * invalid (e.g., first call), automatically falls back to full parse to
     * establish a baseline. MD5 cache check is consistent with `parse`.
     * Ideal for continuous editing of large files in the editor, significantly
     * reducing CPU overhead.
     *
     * @param code - 待解析的完整（可能局部变更的）IC10 源代码
     * @param code - Complete (possibly partially changed) IC10 source code to parse
     * @param cache - 上一次解析的缓存结果，用于 MD5 比对
     * @param cache - Previous parse cache result, used for MD5 comparison
     * @returns 解析结果，包含 tokens、AST、诊断、符号表、类型表和变更标记
     * @returns Parse result with tokens, AST, diagnostics, symbols, types, and change flag
     *
     * @throws 解析错误由 @debug 装饰器捕获并记录，不会向上层抛出
     * @throws Parse errors are caught and logged by the @debug decorator, not thrown upward
     *
     * @note Linker 不支持增量操作，因此链接阶段仍需全量执行。
     * @note The Linker does not support incremental operation, so the linking phase always runs in full.
     */
    @debug({
        message: err => t("server.handler.error", { name: "inc parser", err: (err as Error).message }),
        logger: msg => Console.error(msg, "inc parser"),
        rethrow: false
    })
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
            const lexResult = this.incLexer.tokenizeFull(code);
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
        const lexResult = this.incLexer.tokenizeInc(code);

        // 增量语法分析
        const parseResult = this.incParser.parseInc(lexResult.tokens, lexResult.changedStartLine);

        // 检测受影响范围内是否包含声明式语句（AliasDirective / DefineDirective / LabelDef）
        // 若没有声明式变更，可以跳过 Linker 和 JSON 序列化/反序列化，复用缓存的符号表和类型表
        const canSkipLinker =
            cache?.symbols != null
            && cache?.types != null
            && !hasDeclarativeChanges(parseResult.ast.statements, parseResult.affectedStmtStart);

        let symbols = cache?.symbols ?? null;
        let types = cache?.types ?? null;

        if (canSkipLinker)
            // 跳过 Linker，复用缓存的符号表、类型表和诊断信息
            diagnostics.push(...(cache?.diagnostics ?? []));
        else {
            // 链接器（全量执行，Linker 不支持增量）
            const linker = new Linker();

            linker.addUnit(stdLib.content);
            linker.addUnit(parseResult.ast);

            const symbolJson = linker.link().toJSON();
            const typesJson = linker.typeTable.toJSON();

            symbols = JSON.parse(symbolJson);
            types = JSON.parse(typesJson);

            diagnostics.push(...linker.diagnostics);
        }

        return {
            changed: true,
            source: code,
            tokens: lexResult.tokens,
            ast: parseResult.ast,
            diagnostics,
            symbols,
            types,
            hash
        };
    }
}
