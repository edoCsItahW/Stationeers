// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file lexer.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 词法分析器 {@link Lexer} 的类型声明：把 IC10 源码切分为 Token 序列。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Token} from "./token";
import {Diagnostic} from "../common";


/**
 * @summary 词法分析器类
 *
 * @desc 将 IC10 源代码字符串分解为 Token 序列。
 * 支持调试模式，可保留注释 Token。
 *
 * @public
 */
export class Lexer {
    /**
     * @summary 构造函数
     *
     * @param source - 待分析的 IC10 源代码字符串
     * @param debug - 预留参数，**当前不改变任何行为**
     *
     * @desc 创建词法分析器实例，但不立即执行分析。
     * 调用 {@link scan} 方法执行实际的词法分析。
     *
     * @remarks
     * `debug` 会被传给 C++ 侧的同名标志，而该标志目前在编译器核心中没有任何读取点，因此打开它
     * 既不会保留也不会过滤注释 —— 注释本来就**始终**出现在 {@link scan} 的结果里（类别为
     * `TokenCategory.COMMENT`），由语法分析器负责跳过。
     *
     * @example
     * ```typescript
     * const lexer = new ic10.Lexer('move r0 r1');
     * ```
     */
    constructor(source: string, debug?: boolean);

    /**
     * @summary 静态方法：直接对源代码进行词法分析
     *
     * @param source - IC10 源代码字符串
     * @returns Token 数组
     *
     * @desc 便捷方法，无需创建 Lexer 实例即可获取 Token 序列。
     *
     * @example
     * ```typescript
     * const tokens = ic10.Lexer.tokenize('add r0 r1 r2');
     * ```
     */
    static tokenize(source: string): Token[];

    /**
     * @summary 实例方法：对构造时传入的源代码进行词法分析
     *
     * @returns Token 数组
     *
     * @desc 执行实际的词法分析，将源代码分解为 Token 序列。
     * 结果包含换行（`TokenCategory.WHITESPACE`）、注释（`TokenCategory.COMMENT`）与结尾的
     * `END`，即**未经筛选的完整 token 流**。
     *
     * @example
     * ```typescript
     * const lexer = new ic10.Lexer('move r0 r1');
     * const tokens = lexer.scan();
     * ```
     */
    scan(): Token[];

    /**
     * @summary 诊断列表
     *
     * @desc 词法分析过程中产生的诊断信息（包含错误、警告、提示）。
     *       每个诊断对象包含 level、id、start、end、message 字段。
     */
    get diagnostics(): Diagnostic[];
}