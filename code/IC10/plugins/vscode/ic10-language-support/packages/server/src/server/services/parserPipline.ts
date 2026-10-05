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
import { Lexer, Parser, Linker, IncLexer, IncParser, Diagnostic } from "@ic10/compiler";
import { Console, debug } from "@ic10/common";
import { createHash } from "node:crypto";
import { readFileSync } from "node:fs";

import { DocCacheValue } from "../cache";
import { t } from "../../locals";


const STAND_LIB = readFileSync(require.resolve("@ic10/compiler/src/stdLib.ic"), "utf-8");

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
        const noop: ParseResult = Object.assign({
            hash: "", tokens: [], diagnostics: [], source: code, changed: false
        }, cache);

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

        linker.addUnit(STAND_LIB);
        linker.addUnit(ast);

        const table = linker.link();

        const typesJson = linker.typeTable.toJSON();

        diagnostics.push(...linker.diagnostics);

        return {
            changed: true,
            source: code,
            tokens,
            ast,
            diagnostics,
            symbolTable: table,
            symbols: JSON.parse(table.toJSON()),
            types: JSON.parse(typesJson),
            hash
        };
    }

    /**
     * @summary 增量解析 IC10 源代码
     *
     * @summary Incremental parse of IC10 source code
     *
     * @desc 优先使用增量解析管线（IncLexer → IncParser → Linker）：词法与语法阶段只重新分析
     * 变化的行与语句，语义阶段（Linker）不支持增量，始终全量执行。当内部增量缓存失效时
     * （如首次调用），自动回退到全量解析以建立基准。内置 MD5 缓存检查与 `parse` 一致。
     * 适用于用户在编辑器中连续编辑大文件的场景，可大幅减少 CPU 开销。
     *
     * @desc Preferentially uses the incremental parsing pipeline
     * (IncLexer → IncParser → Linker): the lexical and syntactic phases re-analyze only the changed
     * lines and statements, while the semantic phase (Linker) does not support incremental operation
     * and always runs in full. When internal incremental caches are invalid (e.g., first call), it
     * automatically falls back to full parse to establish a baseline. MD5 cache check is consistent
     * with `parse`. Ideal for continuous editing of large files in the editor, significantly
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
     * @note 诊断由词法、语法（均来自增量结果的缓存汇总）与语义（每次全量链接产生）三段构成，
     *       三者都覆盖整份源码，因此编辑任意一行都会刷新该行的全部诊断——包括语义诊断
     *       （如把 `Setting` 改成不存在的逻辑属性后应立即报 IWA15_1）。
     * @note Diagnostics consist of three parts — lexical and syntactic (aggregated from the
     *       incremental caches) plus semantic (produced by a full link on every call) — all of which
     *       cover the whole source. Editing any line therefore refreshes every diagnostic of that
     *       line, semantic ones included (e.g. replacing `Setting` with a nonexistent logic property
     *       must report IWA15_1 immediately).
     * @note Linker 不支持增量操作，因此链接阶段仍需全量执行。
     * @note The Linker does not support incremental operation, so the linking phase always runs in full.
     */
    @debug({
        message: err => t("server.handler.error", { name: "inc parser", err: (err as Error).message }),
        logger: msg => Console.error(msg, "inc parser"),
        rethrow: false
    })
    public async parseInc(code: string, cache?: DocCacheValue): Promise<ParseResult> {
        const noop: ParseResult = Object.assign(
            {
                hash: "",
                tokens: [],
                diagnostics: [],
                source: code,
                changed: false
            },
            cache
        );

        if (!code.trim().length) return noop;

        const hash = createHash("md5").update(code).digest("hex");
        if (cache?.hash === hash) return noop;

        const diagnostics: Diagnostic[] = [];

        // 首次调用 / 缓存失效：全量词法+语法分析以建立增量基准
        if (!this.incLexer.hasCache() || !this.incParser.hasCache()) {
            const lexResult = this.incLexer.tokenizeFull(code);
            const parseResult = this.incParser.parseFull(lexResult.tokens);

            this.assertIncrementalDiagnostics(lexResult, parseResult);

            // 词法与语法诊断随增量结果一并给出，且已覆盖整份源码
            diagnostics.push(...lexResult.diagnostics, ...parseResult.diagnostics);

            const linker = new Linker();
            linker.addUnit(STAND_LIB);
            linker.addUnit(parseResult.ast);

            const table = linker.link();
            const typesJson = linker.typeTable.toJSON();

            // 语义诊断由 Linker 产生（Linker 只支持全量分析）
            diagnostics.push(...linker.diagnostics);

            return {
                changed: true,
                source: code,
                tokens: lexResult.tokens,
                ast: parseResult.ast,
                symbolTable: table,
                diagnostics,
                symbols: JSON.parse(table.toJSON()),
                types: JSON.parse(typesJson),
                hash
            };
        }

        // 增量词法分析
        const lexResult = this.incLexer.tokenizeInc(code);

        // 增量语法分析
        const parseResult = this.incParser.parseInc(lexResult.tokens, lexResult.changedStartLine);

        this.assertIncrementalDiagnostics(lexResult, parseResult);

        // 词法与语法诊断随增量结果一并给出：未变化的行沿用其缓存时的诊断，
        // 变化的行用本次诊断，因此无需再复用上一次的 diagnostics
        diagnostics.push(...lexResult.diagnostics, ...parseResult.diagnostics);

        // 链接器：Linker 不支持增量操作，且语义诊断（未定义标识符、操作数类型、
        // 标准库枚举属性等）只能由它产生。跳过它只能复用陈旧的诊断，编辑器会因此
        // 停留在旧文本的错误上（例如把合法属性改成不存在的属性后没有任何提示），
        // 所以这里始终全量执行。
        const linker = new Linker();

        linker.addUnit(STAND_LIB);
        linker.addUnit(parseResult.ast);

        const table = linker.link();

        diagnostics.push(...linker.diagnostics);

        return {
            changed: true,
            source: code,
            ast: parseResult.ast,
            tokens: lexResult.tokens,
            symbols: JSON.parse(table.toJSON()),
            symbolTable: table,
            diagnostics,
            types: JSON.parse(linker.typeTable.toJSON()),
            hash
        };
    }

    /**
     * @internal 校验增量结果携带了诊断
     *
     * @desc 增量结果上的 `diagnostics` 是新增的编译器能力：原生模块缺失该字段说明
     * node_modules 里的 `@ic10/compiler` 比类型声明旧（常见的「本地编译好了但没更新依赖」）。
     * 此时若继续展开只会抛出难懂的 `undefined is not iterable`，因此显式抛错：调用方会回退到
     * 全量解析（诊断依然正确），并打印一条可定位的日志。
     *
     * @internal Check that incremental results carry diagnostics
     *
     * @desc `diagnostics` on incremental results is a newer compiler capability: a missing field means
     * the `@ic10/compiler` native module in node_modules is older than its type declarations (the usual
     * "built locally but never updated the dependency"). Spreading it would only raise an obscure
     * `undefined is not iterable`, so fail explicitly instead: the caller falls back to a full parse
     * (diagnostics stay correct) and logs something actionable.
     * */
    private assertIncrementalDiagnostics(...results: { diagnostics?: Diagnostic[] }[]) {
        if (results.some(result => !result.diagnostics)) throw new Error(t("server.parser.info.LIE3"));
    }
}
