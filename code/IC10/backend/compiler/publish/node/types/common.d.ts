// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file common.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:34
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 位置信息类
 *
 * @desc 表示源代码中的位置，通过行号和列号定位。
 * 词法分析器使用此类记录每个 Token 的起始位置。
 *
 * @note
 * 行号和列号的起始值取决于具体实现，通常从 0 开始。
 *
 * @public
 */
export class Pos {
    /**
     * @summary 行号
     * @desc 源代码中的行索引（从 0 或 1 开始）
     */
    line: number;

    /**
     * @summary 列号
     * @desc 当前行中的字符位置（从 0 或 1 开始）
     */
    column: number;

    /**
     * @summary 相对于源代码起始位置的字节偏移量
     */
    offset: number;

    /**
     * @summary 移动到下一行
     *
     * @desc 将位置推进到下一行的起始处（列号归零，行号加一）。
     * 通常在解析换行符时调用。
     */
    newline(): void;

    /**
     * @summary 移动到下一个字符
     *
     * @desc 将列号加一，表示读取了一个字符。
     * 通常在解析普通字符时调用。
     */
    next(): void;

    /**
     * @summary 移动到指定偏移位置
     *
     * @param index - 目标偏移量（相对于源代码起始位置）
     *
     * @desc 将位置直接设置到指定的字节偏移处。
     * 用于回溯或跳转到特定位置。
     */
    move(index: number): void;
}


/**
 * @summary 诊断信息
 *
 * @desc 语义分析过程中产生的诊断信息，包含级别、ID、位置和消息。
 *       - level: 诊断级别（"error" | "warning" | "info"）
 *       - id: 诊断 ID 字符串（如 "IEA1_2"、"IMP17"）
 *       - start: 起始位置（Pos 对象）
 *       - end: 结束位置（Pos 对象）
 *       - message: 诊断消息文本
 *
 * @public
 */
export interface Diagnostic {
    /**
     * @summary 诊断级别
     * @desc 诊断的严重级别：error（错误）、warning（警告）、info（提示）。
     */
    level: "error" | "warning" | "info";

    /**
     * @summary 诊断 ID
     * @desc 诊断标识符字符串，如 "IEA1_2"、"IMP17"。
     */
    id: string;

    /**
     * @summary 起始位置
     * @desc 诊断信息对应的源代码起始位置。
     */
    start: Pos;

    /**
     * @summary 结束位置
     * @desc 诊断信息对应的源代码结束位置。
     */
    end: Pos;

    /**
     * @summary 诊断消息
     * @desc 诊断信息的文本描述。
     */
    message: string;
}


/**
 * @summary AST 节点在源代码中的位置信息
 *
 * @desc 描述 AST 节点在原始源代码中的位置，用于错误报告、调试和代码导航。
 * 位置信息包括行号和列号，从 0 或 1 开始取决于具体实现。
 *
 * @public
 */
export interface Position {
    /**
     * @summary 行号
     * @desc 源代码中的行索引（从 0 或 1 开始）
     */
    line: number;

    /**
     * @summary 列号
     * @desc 当前行中的字符位置（从 0 或 1 开始）
     */
    column: number;

    /**
    * @summary 字节偏移量
    * @desc 从源代码起始位置到该节点的字节偏移量
    */
    offset: number;
}
