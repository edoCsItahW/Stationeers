// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 语法分析器 {@link Parser} 的类型声明：把 Token 序列解析为 AST。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Diagnostic } from "../common";
import { Token } from "../lexer/token";
import { Program } from "./program";


/**
 * @summary 语法分析器类
 *
 * @desc 将 Token 序列解析为 AST（抽象语法树）。
 *
 * @remarks
 * **English:** parses a token sequence into an AST (abstract syntax tree).
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
     * @param debug - 预留参数，**当前不改变任何行为**
     *
     * @desc 创建语法分析器实例。
     * 调用 {@link parse} 方法执行实际的解析操作。
     *
     * @remarks `debug` 会传入 C++ 侧同名标志，而该标志目前在编译器核心中没有任何读取点，
     *          因此打开它不会产生任何调试输出。
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
     * @param debug - 预留参数，**当前不改变任何行为**
     * @returns 程序节点
     *
     * @desc 便捷方法，无需创建 Parser 实例即可解析。
     *
     * @note 它内部创建的 Parser 实例会连同其诊断一起丢弃；需要 `diagnostics` 时请自行
     *       `new Parser(tokens)` 再调用 {@link parse}。
     */
    static parsing(tokens: Token[], debug?: boolean): Program;

    /**
     * @summary 诊断列表
     *
     * @desc 语法分析过程中产生的诊断信息（包含错误、警告、提示）。
     *       每个诊断对象包含 level、id、start、end、message 字段。
     */
    get diagnostics(): Diagnostic[];
}