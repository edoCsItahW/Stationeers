// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 16:01
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import {Diagnostic} from "../common";
import {Program} from "../parser";
import {SymbolTable} from "./semantic";


/**
 * @summary 静态分析器类
 *
 * @desc 对 IC10 程序进行静态分析，包括：
 * - 符号表构建
 * - 语义检查
 * - 类型检查
 * - 错误检测
 *
 * @elseif en
 * @summary Static analyser class
 *
 * @desc Performs static analysis of IC10 programs, including:
 * - Symbol table construction
 * - Semantic checking
 * - Type checking
 * - Error detection
 *
 * @example
 * ```typescript
 * // 创建分析器并分析程序
 * const analyser = new ic10.Analyser();
 * analyser.visit(program);
 *
 * // 检查是否有诊断信息
 * if (analyser.diagnostics.length > 0) {
 *     console.log('诊断信息:', analyser.diagnostics);
 * }
 *
 * // 获取符号表
 * const symbolTable = analyser.symbolTable;
 * ```
 *
 * @public
 */
export class Analyser {
    /**
     * @summary 符号表
     *
     * @desc 分析过程中构建的符号表，包含所有定义的符号信息。
     */
    get symbolTable(): SymbolTable;

    /**
     * @summary 诊断列表
     *
     * @desc 分析过程中产生的诊断信息（包含错误、警告、提示）。
     *       每个诊断对象包含 level、id、start、end、message 字段。
     */
    get diagnostics(): Diagnostic[];

    /**
     * @summary 静态方法：分析程序
     *
     * @param program - 要分析的 Program 节点
     *
     * @desc 便捷方法，直接分析程序而不需要创建 Analyser 实例。
     */
    static analyse(program: Program): void;

    /**
     * @summary 访问程序节点
     *
     * @param program - 要访问的 Program 节点
     *
     * @desc 执行实际的静态分析，更新符号表和诊断列表。
     */
    visit(program: Program): Promise<void>;
}
