// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file program.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc AST 根节点 {@link Program} 与语句联合类型 {@link Statement} 的类型声明。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {LabelDefNode, PreprocessorDirective, ExecutableInstruction, TypeAnnotation, Errorable, ErrorNode} from "./ast";
import {Pos} from "../common";


/**
 * @summary 可执行指令节点（排除 `ErrorNode`）
 *
 * @desc 从 {@link ExecutableInstruction} 中剔除错误节点后的联合类型，
 *       用于「确定拿到了一个真正可执行的指令」的场景。
 *
 * @public
 */
export type PureExeInstructionNode = Exclude<ExecutableInstruction, ErrorNode>;


/**
 * @summary 语句联合类型
 *
 * @desc 一条语句可能是可执行指令、标签定义、预处理指令（`alias` / `define`）或类型注解，
 *       任何一项解析失败时都会退化为 `ErrorNode`。
 *
 * @public
 */
export type Statement = Errorable<ExecutableInstruction
    | LabelDefNode
    | PreprocessorDirective
    | TypeAnnotation>;


/**
 * @summary 程序节点类
 *
 * @desc 表示 IC10 程序的根节点。
 * 包含程序的所有语句和结束位置信息。
 *
 * @remarks
 * **English:** the root node of an IC10 program — every statement plus the end position.
 *
 * @example
 * ```typescript
 * // 解析 IC10 代码
 * const parser = new ic10.Parser(tokens);
 * const program = parser.parse();
 *
 * // 访问程序语句
 * console.log(`语句数量: ${program.statements.length}`);
 * ```
 *
 * @public
 */
export class Program {
    /**
     * @summary 节点名称（静态常量）
     * @desc 固定为 `"Program"`。注意它是**静态**成员：`Program.nodeName`，不是实例属性。
     */
    static readonly nodeName: "Program";

    /**
     * @summary 程序语句列表
     * @desc 包含程序中的所有语句（指令、标签、预处理指令等）
     */
    get statements(): Statement[];

    /**
     * @summary 结束位置
     * @desc 程序在源代码中的结束位置
     */
    get end(): Pos;

    /**
     * @summary 返回可读字符串表示
     *
     * @returns 格式化的程序字符串
     *
     * @desc 用于调试目的，返回人类可读的程序表示。
     */
    toString(): string;

    /**
     * @summary 返回 JSON 字符串表示
     *
     * @returns JSON 格式的程序表示
     *
     * @desc 返回包含 nodeName、statements 和 end 的 JSON 对象字符串。
     */
    toJSON(): string;
}