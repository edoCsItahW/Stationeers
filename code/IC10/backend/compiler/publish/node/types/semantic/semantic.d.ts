// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semantic.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 符号表 {@link SymbolTable} 与符号 {@link Symbol} 的类型声明。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {BasicType, TypeCategory} from "./types";
import {Description} from "../parser";


/**
 * @summary 符号表类
 *
 * @desc 存储 IC10 程序中的符号信息（如标签、别名定义等）。
 * 用于静态分析和代码验证。
 *
 * @remarks
 * **English:** stores the symbols of an IC10 program (labels, alias and constant definitions, …) and is
 * the basis of static analysis and validation.
 *
 * @example
 * ```typescript
 * // 执行静态分析（visit 是异步的，必须 await）
 * const analyser = new ic10.Analyser();
 * await analyser.visit(program);
 *
 * // 获取符号表
 * const symbolTable = analyser.symbolTable;
 * console.log(JSON.parse(symbolTable.toJSON()));
 * ```
 *
 * @public
 */
export class SymbolTable {
    /**
     * @summary 返回 JSON 字符串表示
     *
     * @returns JSON 格式的符号表表示
     *
     * @desc 返回包含所有符号信息的 JSON 对象字符串。
     */
    toJSON(): string;
}


/**
 * @summary 符号信息
 *
 * @desc 表示符号表中的单个符号，由 `SymbolTable.toJSON()` 序列化输出。
 *
 * @public
 */
export interface Symbol {
    /** 符号名称 */
    name: string;
    /** 基本类型（BasicType 枚举值） */
    type: BasicType;
    /** 类型类别（TypeCategory 枚举值） */
    category: TypeCategory;
    /** 类型名称（可选，如设备类型名） */
    typeName?: string;
    /** 符号值（可选；标签为行号，别名为其目标，常量为字面量） */
    value?: string;
    /** 描述信息（可选，来自 `#: @desc` 类型提示，链接或文本） */
    desc?: Description;
    /** 是否由 `#: @builtin` 类型提示标记为内建常量 */
    builtin: boolean;
}


/**
 * @summary 符号表 JSON 结构
 *
 * @desc {@link SymbolTable.toJSON} 的输出形状：用户符号与内建符号分开存放。
 *       内建符号目前是设备端口 `d0`-`d5`。
 *
 * @public
 */
export interface SymbolMap {
    /** 源码中定义的符号，以符号名为键 */
    symbols: {
        [key: string]: Symbol;
    };
    /** 编译器内建符号（如 `d0`-`d5`），以名称为键 */
    builtinSymbols: {
        [key: string]: Symbol;
    };
}
