/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file utils.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import { lowerBound, getEnumName, type Nullable, type Optional } from "@ic10/common";
import type { IdentifierNode, SymbolMap, Statement } from "@ic10/compiler";
import { BasicType } from "@ic10/compiler";



/**
 * @summary 通过二分查找定位指定行的语句
 *
 * @summary Find the statement at the given line using binary search
 *
 * @param statements 已排序的语句列表
 * @param statements Sorted list of statements
 * @param line 目标行号（1-based）
 * @param line Target line number (1-based)
 *
 * @returns 匹配的语句节点，未找到返回 null
 * @returns The matching statement node, or null if not found
 */
export function findStatementAtPosition(statements: Statement[], line: number): Nullable<Statement> {
    const idx = lowerBound(statements, item => item.position.line >= line);
    return idx >= 0 && idx < statements.length ? statements[idx] : null;
}

/**
 * @summary 判断光标列位置是否落在某个节点的字符范围内
 *
 * @summary Check if the cursor column position falls within a node's character range
 *
 * @param col 节点的起始列号（1-based）
 * @param col The column position of the node (1-based)
 * @param length 节点的文本长度
 * @param length The text length of the node
 * @param character 光标的列号（1-based）
 * @param character The cursor column position (1-based)
 *
 * @returns 如果在范围内返回 true
 * @returns True if within range
 */
export function isInsideNode(col: number, length: number, character: number): boolean {
    return character >= col && character <= col + length;
}

/**
 * @summary 将 BasicType 枚举值格式化为小写字符串名称
 *
 * @summary Format a BasicType enum value to its lowercase string name
 *
 * @param type BasicType 枚举值
 * @param type BasicType enum value
 *
 * @returns 小写的类型名称字符串
 * @returns Lowercase type name string
 */
export function formatBasicType(type: BasicType): string {
    return getEnumName(BasicType, type)!.toLowerCase();
}

/**
 * @summary 从符号表中解析标识符的可读类型名称
 *
 * @summary Resolve the human-readable type name for an identifier from the symbol table
 *
 * @param identifier 标识符 AST 节点
 * @param identifier Identifier AST node
 * @param symbols 符号表
 * @param symbols Symbol table
 *
 * @returns 类型名称（优先使用 typeName，回退到 BasicType 名称），未找到返回 undefined
 * @returns Type name (prefers typeName, falls back to BasicType name), or undefined if not found
 */
export function formatType(identifier: IdentifierNode, symbols: SymbolMap): Optional<string> {
    const symbol = symbols.symbols[identifier.value];
    if (symbol) {
        if (symbol.typeName) return symbol.typeName;
        return formatBasicType(symbol.type);
    }
}
