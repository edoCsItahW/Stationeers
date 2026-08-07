// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file lexer.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:35
 * @desc
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
     * @param debug - 是否保留注释（可选，默认为 false）
     *
     * @desc 创建词法分析器实例，但不立即执行分析。
     * 调用 {@link scan} 方法执行实际的词法分析。
     *
     * @example
     * ```typescript
     * const lexer = new ic10.Lexer('move r0 r1', true);
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
     * 如果构造时设置了 debug=true，注释也会作为 Token 保留。
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