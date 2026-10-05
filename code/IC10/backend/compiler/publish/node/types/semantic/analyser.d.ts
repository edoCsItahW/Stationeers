// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 语义分析器 {@link Analyser} 的类型声明：把 AST 走一遍，产出符号表、类型表与诊断。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import {SymbolTable} from "./semantic";
import {TypeTable} from "./types";
import {Diagnostic} from "../common";
import {Program} from "../parser";


/**
 * @summary 静态分析器类
 *
 * @desc 对 IC10 程序进行静态分析，包括：
 * - 符号表构建
 * - 语义检查
 * - 类型检查
 * - 错误检测
 *
 * @remarks
 * **English:** performs static analysis of an IC10 program — symbol table construction, semantic and
 * type checking, and error detection.
 *
 * @example
 * ```typescript
 * // 创建分析器并分析程序（visit 是异步的，必须 await）
 * const analyser = new ic10.Analyser();
 * await analyser.visit(program);
 *
 * // 检查是否有诊断信息
 * if (analyser.diagnostics.length > 0) {
 *     console.log('诊断信息:', analyser.diagnostics);
 * }
 *
 * // 获取符号表与类型表
 * const symbolTable = analyser.symbolTable;
 * const typeTable = analyser.typeTable;
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
     * @summary 类型表
     *
     * @desc 分析过程中构建的类型表，包含所有自定义类型（设备类型和枚举类型）。
     */
    get typeTable(): TypeTable;

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
     * @returns 分析完成后 resolve 的 Promise（**不返回分析结果**）
     *
     * @desc 便捷方法，直接分析程序而不需要创建 Analyser 实例。
     *
     * @warning 它内部创建的分析器实例在返回时即被丢弃，因此**拿不到符号表、类型表与诊断** ——
     *          需要结果时请改用 `new Analyser()` + `await analyser.visit(program)`。
     *
     * @example
     * ```typescript
     * await ic10.Analyser.analyse(program);   // 只跑分析，不要结果
     * ```
     */
    static analyse(program: Program): Promise<void>;

    /**
     * @summary 访问程序节点
     *
     * @param program - 要访问的 Program 节点
     * @returns 分析完成后 resolve 的 Promise
     *
     * @desc 执行实际的静态分析，更新符号表、类型表与诊断列表。
     *
     * @note **必须 `await`**：符号前向引用会让分析在内部挂起、稍后恢复，
     *       不等待就读取 `symbolTable` / `diagnostics` 会拿到尚未填充的结果。
     */
    visit(program: Program): Promise<void>;
}
