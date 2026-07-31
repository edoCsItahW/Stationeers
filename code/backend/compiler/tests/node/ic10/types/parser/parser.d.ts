// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Diagnostic } from "../common";
import { Token } from "../lexer/token";
import { Program } from "./program";


/**
 * @summary 语法分析器类
 *
 * @desc 将 Token 序列解析为 AST（抽象语法树）。
 * 支持调试模式，可以在解析过程中输出调试信息。
 *
 * @elseif en
 * @summary Parser class
 *
 * @desc Parses a sequence of Tokens into an AST (Abstract Syntax Tree).
 * Supports debug mode to output debug information during parsing.
 *
 * @example
 * ```typescript
 * // 创建语法分析器
 * const parser = new ic10.Parser(tokens, true);
 *
 * // 解析 tokens
 * const program = parser.parse();
 * console.log(JSON.parse(program.toJSON()));
 * ```
 *
 * @public
 */
export class Parser {
    /**
     * @summary 构造函数
     *
     * @param tokens - Token 数组
     * @param debug - 是否启用调试模式（可选，默认为 false）
     *
     * @desc 创建语法分析器实例。
     * 调用 {@link parse} 方法执行实际的解析操作。
     */
    constructor(tokens: Token[], debug?: boolean);

    /**
     * @summary 解析 Token 序列
     *
     * @returns 程序节点
     *
     * @desc 执行实际的语法分析，将 Token 序列转换为 AST。
     */
    parse(): Program;

    /**
     * @summary 静态方法：解析 Token 序列
     *
     * @param tokens - Token 数组
     * @param debug - 是否启用调试模式（可选，默认为 false）
     * @returns 程序节点
     *
     * @desc 便捷方法，无需创建 Parser 实例即可解析。
     */
    parsing(tokens: Token[], debug?: boolean): Program;

    /**
     * @summary 诊断列表
     *
     * @desc 语法分析过程中产生的诊断信息（包含错误、警告、提示）。
     *       每个诊断对象包含 level、id、start、end、message 字段。
     */
    get diagnostics(): Diagnostic[];
}