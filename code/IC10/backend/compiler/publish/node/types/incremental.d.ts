// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental.d.ts
 * @author edocsitahw
 * @version 1.3
 * @date 2026/09/25
 * @desc 增量编译相关声明：{@link IncLexer}、{@link IncParser} 与把二者串起来的 {@link IncCompiler}，
 *       以及它们的返回结构。目标是让编辑器场景只重新处理改动的行与语句。
 *       三个结果结构都带 `diagnostics`：由各自的缓存汇总，覆盖整份源码，而不只是变化区间。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Diagnostic } from "./common";
import { Token } from "./lexer";
import { Program } from "./parser";


/**
 * @summary 增量词法分析结果
 *
 * @desc {@link IncLexer} 的返回值：既给出完整 token 流，也给出本次重扫涉及的**行区间**，
 *       后者可直接喂给 {@link IncParser.parseInc}。
 *
 * @public
 */
export interface IncLexerResult {
    /** 本次得到的完整 token 流（含换行、注释与结尾的 `END`） */
    tokens: Token[];

    /**
     * 整份源码的词法诊断（全部缓存行，含本次重新扫描的行）
     *
     * @desc 未变化的行沿用其扫描时缓存的诊断，变化的行用本次扫描的诊断，后缀行的诊断随
     *       行号/偏移差值同步平移，因此本字段始终覆盖整份源码。
     */
    diagnostics: Diagnostic[];

    /** 是否走了增量路径；`false` 表示这次是全量词法分析 */
    incremental: boolean;

    /** 本次实际重新词法化的行数（全量时等于源码行数） */
    relexedLines: number;

    /** 变化区间的起始行号（从 1 开始） */
    changedStartLine: number;

    /** 旧源码中变化区间的结束行号（从 1 开始） */
    oldChangedEndLine: number;

    /** 新源码中变化区间的结束行号（从 1 开始） */
    newChangedEndLine: number;
}


/**
 * @summary 增量词法分析器
 *
 * @desc 缓存上一次的行级词法结果，下次只重新词法化发生变化的行。
 *       先用 {@link tokenizeFull} 建立缓存，之后用 {@link tokenizeInc} 提交新源码。
 *
 * @example
 * ```typescript
 * const lexer = new ic10.IncLexer();
 * lexer.tokenizeFull(source);            // 建立缓存，incremental === false
 * const result = lexer.tokenizeInc(source.replace('42', '43'));
 * console.log(result.incremental, result.relexedLines);   // true, 1
 * ```
 *
 * @public
 */
export class IncLexer {
    /** @summary 创建一个空缓存的增量词法分析器 */
    constructor();

    /**
     * @summary 全量词法分析并建立缓存
     *
     * @param source - IC10 源代码
     * @returns 结果，其中 `incremental` 为 `false`
     *
     * @desc 第一次调用必须用它；重复调用会以新的源码重建缓存。
     */
    tokenizeFull(source: string): IncLexerResult;

    /**
     * @summary 增量词法分析
     *
     * @param newSource - 修改后的完整源代码（不是补丁）
     * @returns 结果，其中 `incremental` 为 `true`，`changedStartLine` 指示从哪一行开始变脏
     *
     * @desc 与上次缓存的源码逐行比较，只重扫变化的行；没有缓存时退化为全量。
     */
    tokenizeInc(newSource: string): IncLexerResult;

    /** @summary 是否已存在可用缓存（{@link clear} 后为 `false`） */
    hasCache(): boolean;

    /** @summary 丢弃缓存，下次调用将退化为全量分析 */
    clear(): void;
}


/**
 * @summary 增量语法分析结果
 *
 * @desc {@link IncParser} 的返回值：新的 AST 与本次重解析的语句统计。
 *
 * @public
 */
export interface IncParserResult {
    /** 本次得到的程序 AST（根节点） */
    ast: Program;

    /**
     * 整个程序的语法诊断（复用的前缀语句 + 本次重解析的后缀语句）
     *
     * @desc 前缀语句沿用其上次解析时的诊断，后缀语句用本次解析的诊断；因此本字段覆盖整份程序，
     *       而不只是本次重解析的区间。
     */
    diagnostics: Diagnostic[];

    /** 是否走了增量路径；`false` 表示这次是全量解析 */
    incremental: boolean;

    /** 本次实际重新解析的语句条数 */
    reparsedStmts: number;

    /** 受影响语句区间在语句列表中的起始下标（从 0 开始） */
    affectedStmtStart: number;
}


/**
 * @summary 增量语法分析器
 *
 * @desc 按语句粒度缓存解析结果，只重新解析受改动影响的语句。
 *       入参 token 流通常来自 {@link IncLexer}，且**必须**把它的 `changedStartLine` 一并传给
 *       {@link parseInc}，两者是配套的。
 *
 * @example
 * ```typescript
 * const lexer = new ic10.IncLexer();
 * const tokens = lexer.tokenizeFull(source);
 *
 * const parser = new ic10.IncParser();
 * parser.parseFull(tokens.tokens);                       // 建立缓存
 *
 * const next = lexer.tokenizeInc(modified);
 * const result = parser.parseInc(next.tokens, next.changedStartLine);
 * ```
 *
 * @public
 */
export class IncParser {
    /** @summary 创建一个空缓存的增量语法分析器 */
    constructor();

    /**
     * @summary 全量解析并建立缓存
     *
     * @param tokens - 词法分析得到的 token 流
     * @returns 结果，其中 `incremental` 为 `false`、`affectedStmtStart` 为 `0`
     */
    parseFull(tokens: Token[]): IncParserResult;

    /**
     * @summary 增量解析
     *
     * @param tokens - 新的 token 流
     * @param changedStartLine - 变化起始行号，取自 {@link IncLexerResult.changedStartLine}
     * @returns 结果，其中 `incremental` 为 `true`
     *
     * @desc 依据 `changedStartLine` 推断哪些语句被改动，只重解析这些语句并复用其余部分的 AST。
     */
    parseInc(tokens: Token[], changedStartLine: number): IncParserResult;

    /** @summary 是否已存在可用缓存（{@link clear} 后为 `false`） */
    hasCache(): boolean;

    /** @summary 丢弃缓存，下次调用将退化为全量解析 */
    clear(): void;
}


/**
 * @summary 增量编译结果
 *
 * @desc {@link IncCompiler} 的返回值：一次调用同时给出 token 流、AST 与两个阶段的增量统计。
 *
 * @public
 */
export interface IncCompileResult {
    /** 本次得到的完整 token 流 */
    tokens: Token[];

    /** 本次得到的程序 AST */
    ast: Program;

    /**
     * 整份源码的词法诊断与语法诊断（词法在前，语法在后）
     *
     * @desc 二者都由增量分析器按当前缓存汇总，覆盖整份源码。**不含**语义诊断：
     *       语义分析属于 `Linker`，需要另行全量执行。
     */
    diagnostics: Diagnostic[];

    /** 是否走了增量路径；`false` 表示这次是词法+语法全量 */
    incremental: boolean;

    /** 本次重新词法化的行数 */
    relexedLines: number;

    /** 本次重新解析的语句条数 */
    reparsedStmts: number;
}


/**
 * @summary 增量编译器（词法 + 语法的组合入口）
 *
 * @desc 把 {@link IncLexer} 与 {@link IncParser} 串成一步：内部维护两段缓存，
 *       {@link compileInc} 会自动把词法阶段算出的 `changedStartLine` 转交给语法阶段。
 *
 * @example
 * ```typescript
 * const compiler = new ic10.IncCompiler();
 * compiler.compileFull(source);                  // incremental === false
 *
 * const result = compiler.compileInc(modified);   // incremental === true
 * console.log(result.relexedLines, result.reparsedStmts);   // 例如 1, 2
 * ```
 *
 * @public
 */
export class IncCompiler {
    /** @summary 创建一个空缓存的增量编译器 */
    constructor();

    /**
     * @summary 全量编译并建立缓存
     *
     * @param source - IC10 源代码
     * @returns 结果，其中 `incremental` 为 `false`
     */
    compileFull(source: string): IncCompileResult;

    /**
     * @summary 增量编译
     *
     * @param newSource - 修改后的**完整**源代码（不是补丁）
     * @returns 结果，其中 `incremental` 为 `true`
     */
    compileInc(newSource: string): IncCompileResult;

    /** @summary 是否已存在可用缓存（{@link clear} 后为 `false`） */
    hasCache(): boolean;

    /** @summary 丢弃两段缓存，下次调用将退化为全量编译 */
    clear(): void;
}