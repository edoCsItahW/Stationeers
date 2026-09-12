// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file program.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:52
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {LabelDefNode, PreprocessorDirective, ExecutableInstruction, TypeAnnotation, Errorable, ErrorNode} from "./ast";
import {Pos} from "../common";


export type PureExeInstructionNode = Exclude<ExecutableInstruction, ErrorNode>;


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
 * @elseif en
 * @summary Program node class
 *
 * @desc Represents the root node of an IC10 program.
 * Contains all statements of the program and end position information.
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
     * @summary 节点名称
     * @desc 通常为 "Program"
     */
    get nodeName(): "Program";

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