// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file linker.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 17:12
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Diagnostic } from "./common";
import { Program } from "./parser";
import { SymbolTable } from "./semantic";


/**
 * @summary 编译单元信息
 *
 * @desc 表示链接过程中每个编译单元的信息，包括路径和诊断信息。
 *
 * @public
 */
export interface UnitInfo {
    /**
     * @summary 源文件路径
     */
    path: string;

    /**
     * @summary 该单元的诊断信息列表
     */
    diagnostics: Diagnostic[];
}


/**
 * @summary 链接器类
 *
 * @desc 用于链接多个 IC10 编译单元，合并它们的符号表并收集所有诊断信息。
 *
 * @elseif en
 * @summary Linker class
 *
 * @desc Used to link multiple IC10 compilation units, merge their symbol tables,
 *       and collect all diagnostics.
 *
 * @example
 * ```typescript
 * // 创建链接器并链接多个编译单元
 * const linker = new ic10.Linker();
 *
 * // 添加源代码字符串
 * linker.addUnit('alias sensor0 console0');
 * linker.addUnit('move r0 r1', 'file.ic10');
 *
 * // 添加已解析的 Program 对象
 * const tokens = ic10.Lexer.tokenize('add r2 r0 r1');
 * const parser = new ic10.Parser(tokens);
 * const program = parser.parse();
 * linker.addUnit(program, 'another.ic10');
 *
 * // 执行链接
 * const symbolTable = linker.link();
 *
 * // 检查诊断信息
 * if (linker.diagnostics.length > 0) {
 *     console.log('诊断信息:', linker.diagnostics);
 * }
 * ```
 *
 * @public
 */
export class Linker {
    /**
     * @summary 构造函数
     *
     * @desc 创建一个新的链接器实例。
     */
    constructor();

    /**
     * @summary 添加编译单元（Program 对象）
     *
     * @param program - 要链接的 Program 对象
     *
     * @desc 将已解析的 AST 程序添加到链接器中。
     */
    addUnit(program: Program): void;

    /**
     * @summary 添加编译单元（源代码字符串）
     *
     * @param source - IC10 源代码字符串
     *
     * @desc 将源代码字符串添加到链接器，内部会自动进行词法分析和语法分析。
     */
    addUnit(source: string): void;

    /**
     * @summary 添加编译单元（带路径）
     *
     * @param program - 要链接的 Program 对象
     * @param path - 源文件路径
     *
     * @desc 将已解析的 AST 程序添加到链接器中，并关联源文件路径。
     */
    addUnit(program: Program, path: string): void;

    /**
     * @summary 添加编译单元（源代码字符串带路径）
     *
     * @param source - IC10 源代码字符串
     * @param path - 源文件路径
     *
     * @desc 将源代码字符串添加到链接器，内部会自动进行词法分析和语法分析，并关联源文件路径。
     */
    addUnit(source: string, path: string): void;

    /**
     * @summary 执行链接操作
     *
     * @returns 合并后的 SymbolTable 对象
     *
     * @desc 对所有已添加的编译单元执行语义分析，合并符号表，并返回最终的符号表。
     */
    link(): SymbolTable;

    /**
     * @summary 诊断列表
     *
     * @desc 链接过程中产生的所有诊断信息（包含错误、警告、提示）。
     *       每个诊断对象包含 level、id、start、end、message 字段。
     */
    get diagnostics(): Diagnostic[];

    /**
     * @summary 编译单元列表
     *
     * @desc 所有已添加的编译单元信息，每个单元包含路径和诊断信息。
     */
    get units(): UnitInfo[];
}