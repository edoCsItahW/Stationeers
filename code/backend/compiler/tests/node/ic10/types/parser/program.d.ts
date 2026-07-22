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
import {Pos} from "../common";
import {LabelDefNode, PreprocessorDirectiveNode} from "./ast";
import {
    NullaryInstructionNode,
    UnaryInstructionNode,
    BinaryInstructionNode,
    TernaryInstructionNode,
    QuaternaryInstructionNode, QuinaryInstructionNode, SenaryInstructionNode
} from "./instructions";


/**
 * @summary 可执行指令联合类型
 *
 * @desc 包含所有可执行指令类型的联合，包括零元、一元、二元、三元、四元、五元和六元指令。
 * 这是 IC10 程序中所有可执行指令的完整集合。
 *
 * @elseif en
 * @summary Executable instruction union type
 *
 * @desc Union of all executable instruction types, including nullary, unary, binary, ternary, quaternary, quinary, and senary instructions.
 * This is the complete set of all executable instructions in IC10 programs.
 *
 * @public
 */
export type ExecutableInstructionNode =
    | NullaryInstructionNode
    | UnaryInstructionNode
    | BinaryInstructionNode
    | TernaryInstructionNode
    | QuaternaryInstructionNode
    | QuinaryInstructionNode
    | SenaryInstructionNode;

/**
 * @summary 语句联合类型
 *
 * @desc 包含 IC10 程序中所有语句类型的联合：
 * - 可执行指令（{@link ExecutableInstructionNode}）
 * - 标签定义（{@link LabelDefNode}）
 * - 预处理指令（{@link PreprocessorDirectiveNode}）
 *
 * @elseif en
 * @summary Statement union type
 *
 * @desc Union of all statement types in IC10 programs:
 * - Executable instructions ({@link ExecutableInstructionNode})
 * - Label definitions ({@link LabelDefNode})
 * - Preprocessor directives ({@link PreprocessorDirectiveNode})
 *
 * @public
 */
export type StatementNode =
    | ExecutableInstructionNode
    | LabelDefNode
    | PreprocessorDirectiveNode;


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
    nodeName: string;

    /**
     * @summary 程序语句列表
     * @desc 包含程序中的所有语句（指令、标签、预处理指令等）
     */
    statements: StatementNode[];

    /**
     * @summary 结束位置
     * @desc 程序在源代码中的结束位置
     */
    end: Pos;

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