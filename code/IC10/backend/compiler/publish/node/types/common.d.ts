// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file common.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 各模块共用的基础声明：位置 {@link Pos}、诊断 {@link Diagnostic}、位置字面量
 *       {@link Position} 与工具命名空间 {@link IC10Utils}。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 位置信息类
 *
 * @desc 表示源代码中的位置，通过行号和列号定位。
 * 词法分析器使用此类记录每个 Token 的起始位置。
 *
 * @note 行号与列号均**从 1 开始**：`new Pos()` 的初始值为 `line = 1, column = 1, offset = 0`。
 *
 * @warning 三个字段都是**只读**的（原生侧只暴露 getter）。在严格模式下赋值会抛
 *          `TypeError: Cannot set property line of #<Pos> which has only a getter`，
 *          非严格模式下则被静默忽略 —— 要移动位置请用 {@link next} / {@link newline} /
 *          {@link move}。
 *
 * @public
 */
export class Pos {
    /**
     * @summary 行号（从 1 开始）
     */
    readonly line: number;

    /**
     * @summary 列号（从 1 开始）
     */
    readonly column: number;

    /**
     * @summary 相对于源代码起始位置的字节偏移量（从 0 开始）
     */
    readonly offset: number;

    /**
     * @summary 移动到下一行
     *
     * @desc 行号加一、列号归 1、偏移量加一。通常在解析换行符时调用。
     */
    newline(): void;

    /**
     * @summary 移动到下一个字符
     *
     * @desc 列号与偏移量各加一。通常在读取一个普通字符时调用。
     */
    next(): void;

    /**
     * @summary 向后移动指定距离
     *
     * @param index - 相对移动量（不是绝对偏移量）
     *
     * @desc **相对**前进：偏移量与列号各加 `index`。例如偏移为 2、列为 1 时调用 `move(11)`，
     *       得到偏移 13、列 12。需要跳到某个绝对位置时，请自行计算差值。
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
    start: Pos | undefined;

    /**
     * @summary 结束位置
     * @desc 诊断信息对应的源代码结束位置。
     */
    end: Pos | undefined;

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


/**
 * @summary 工具类型命名空间
 *
 * @desc 放置供本包类型声明共用的辅助类型，目前只有可选值别名 {@link Optional}。
 *
 * @public
 */
export namespace IC10Utils {

    /**
     * @summary 可选值别名
     *
     * @desc 等价于 `T | undefined`。原生侧访问器在字段缺失时返回 `undefined` 而非 `null`，
     *       因此声明里统一用这个别名而不是可选属性记号。
     */
    export type Optional<T> = T | undefined;

}
