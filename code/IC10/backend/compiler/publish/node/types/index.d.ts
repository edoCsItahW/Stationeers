/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file index.d.ts
 * @summary IC10 编译器 Node.js API 类型定义
 *
 * @desc 本模块提供 IC10 编译器的 Node.js 原生绑定 API 类型定义。
 * IC10 是一种用于 Stationeers 游戏的汇编式编程语言，用于控制游戏中的计算机和设备。
 *
 * API 分为两个主要层次：
 * 1. **运行时绑定**：通过 `ObjectWrap` 包装的 C++ 类（如 {@link IC10CompilerLocal}、{@link Pos}、{@link Token}、{@link Lexer}、{@link Parser}、{@link Program}、{@link Analyser}、{@link Linker} 等）
 * 2. **数据序列化**：通过 `toJSON()` 序列化的 AST 节点对象（如各种 {@link ASTNode} 子接口）
 *
 * AST 节点（如指令、表达式）通过 `toJSON()` 方法序列化为普通的 JavaScript 对象，
 * 可直接用于静态分析、代码转换或调试可视化。
 *
 * @example
 * ```typescript
 * import * as ic10 from 'ic10c-node';
 *
 * // 词法分析
 * const tokens = ic10.Lexer.tokenize('move r0 r1');
 *
 * // 语法分析
 * const parser = new ic10.Parser(tokens);
 * const program = parser.parse();
 *
 * // 访问 AST
 * const json = JSON.parse(program.toJSON());
 * console.log(json.statements[0].type); // "moveInstruction"
 * ```
 *
 * @public
 */
export * from "./incremental";
export * from "./semantic";
export * from "./parser";
export * from "./common";
export * from "./linker";
export * from "./locale";
export * from "./lexer";


/**
 * @summary 标准库数据
 *
 * @desc 描述随包分发的 IC10 标准库（`src/stdLib.ic`）：`content` 为标准库源码文本。
 *       该接口目前只用于描述数据形状，运行时的导出表里没有对应成员。
 *
 * @public
 */
export interface StdLibData {
    /** 标准库源码内容 */
    content: string;
}