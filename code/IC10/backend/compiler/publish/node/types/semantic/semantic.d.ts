// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semantic.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 17:13
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {BasicType} from "./types";
import {TypeCategory} from "./types";


/**
 * @summary 符号表类
 *
 * @desc 存储 IC10 程序中的符号信息（如标签、别名定义等）。
 * 用于静态分析和代码验证。
 *
 * @elseif en
 * @summary Symbol table class
 *
 * @desc Stores symbol information in IC10 programs (such as labels, alias definitions, etc.).
 * Used for static analysis and code validation.
 *
 * @example
 * ```typescript
 * // 执行静态分析
 * ic10.Analyser.analyse(program);
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
    /** 符号值（可选） */
    value?: string;
    /** 描述信息（可选） */
    desc?: string;
}


export interface SymbolMap {
    [key: string]: Symbol;
}
