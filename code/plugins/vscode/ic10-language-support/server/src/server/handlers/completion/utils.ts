// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file utils.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import type { StatementNode } from "ic10-node-api";

import { findMaxByCondition, findMinByCondition, lowerBound, Nullable } from "common";

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
export function findStatementAtPosition(statements: StatementNode[], line: number): Nullable<StatementNode> {
    const idx = lowerBound(statements, item => item.position.line >= line);
    return idx >= 0 && idx < statements.length ? statements[idx] : null;
}

/**
 * @summary 判断光标是否在语句关键字范围内
 *
 * @summary Check if the cursor is within the statement's keyword range
 *
 * @param stmt 语句 AST 节点
 * @param stmt Statement AST node
 * @param character 光标列号（1-based）
 * @param character Cursor column position (1-based)
 *
 * @returns 如果在关键字范围内返回 true
 * @returns True if the cursor is within the keyword range
 */
export function isAtKeyword(stmt: StatementNode, character: number): boolean {
    const keywordLen = (stmt as any).keyword?.length ?? 0;
    if (!keywordLen) return false;
    return character >= stmt.position.column && character <= stmt.position.column + keywordLen;
}

/**
 * @summary 从 AST 语句节点获取关键字字符串
 *
 * @summary Get the keyword string from an AST statement node
 *
 * @param stmt 语句 AST 节点
 * @param stmt Statement AST node
 *
 * @returns 关键字字符串（指令节点返回 keyword 字段，alias/define 硬编码），未找到返回 null
 * @returns Keyword string (keyword field for instructions, hardcoded for alias/define), or null if not found
 *
 * @remarks 指令节点的关键字通过 `keyword` 字段获取；alias 和 define 伪指令的关键字硬编码返回。
 *
 * @remarks Instruction keywords are obtained from the `keyword` field; alias and define directive keywords are hardcoded.
 */
export function getKeyword(stmt: StatementNode): Nullable<string> {
    if ((stmt as any).keyword) return (stmt as any).keyword;
    if (stmt.type === "AliasDirective") return "alias";
    if (stmt.type === "DefineDirective") return "define";
    return null;
}

/**
 * @summary 从 operand 字段名中提取操作数索引
 *
 * @summary Extract the operand index from the operand field name
 *
 * @param key 字段名（如 "operand1"、"operand2"）
 * @param key Field name (e.g. "operand1", "operand2")
 *
 * @returns 数字索引（1 表示第一个操作数，2 表示第二个操作数…）
 * @returns Numeric index (1 for first operand, 2 for second operand, etc.)
 *
 * @remarks 返回 0 = 关键字位置, 1 = operand1, 2 = operand2 ...
 *
 * @remarks Returns 0 = keyword position, 1 = operand1, 2 = operand2 ...
 */
export function getOperandIndex(key: string | `operand${number}`) {
    return Number.parseInt(key.replace("operand", ""), 10);
}

/**
 * @summary 查找当前需要补全的操作数节点
 *
 * @summary Find the current operand node that needs to be completed
 *
 * @param stmt 语句 AST 节点
 * @param stmt Statement AST node
 * @param character 触发字符（空格等）
 * @param character Trigger character (space, etc.)
 *
 * @returns 当前操作数的 [key, value] 对，未找到返回 undefined
 * @returns [key, value] pair of the current operand, or undefined if not found
 *
 * @remarks 空格触发时优先返回最近的 ErrorNode（正在输入的未解析操作数）；
 *  非空格触发时返回最后一个已解析操作数之后的下一个位置。
 *
 * @remarks On space trigger, prefers the nearest ErrorNode (unparsed operand being typed);
 *  otherwise returns the position after the last parsed operand.
 */
export function findCurrentOperand(stmt: StatementNode, character?: string) {
    // 用 for...in 代替 Object.entries + filter 避免中间数组分配
    const values: [string, any][] = [];
    for (const key in stmt) {
        if (key.startsWith("operand")) values.push([key, (stmt as any)[key]]);
    }

    const lastErr = findMinByCondition(
        values,
        ([, value]) => value.type === "Error",
        ([k1], [k2]) => getOperandIndex(k1) - getOperandIndex(k2)
    );

    if (character || !lastErr) return lastErr;

    return findMaxByCondition(
        values,
        ([k]) => getOperandIndex(k) < getOperandIndex(lastErr[0]),
        ([k1], [k2]) => getOperandIndex(k1) - getOperandIndex(k2)
    );
}

/**
 * @summary 从 AST 节点获取指定操作数位的 OperandType 数值
 *
 * @summary Get the OperandType value for a specific operand position from the AST node
 *
 * @param stmt 语句 AST 节点
 * @param stmt Statement AST node
 * @param key 操作数字段名（如 "operand1"）
 * @param key Operand field name (e.g. "operand1")
 *
 * @returns OperandType 数值，无效索引返回 null
 * @returns OperandType value, or null if invalid index
 *
 * @remarks 优先从 AST 节点的 `type1`/`type2`... 字段读取，这些字段在语法分析阶段已确定。
 *
 * @remarks Preferentially reads from the AST node's `type1`/`type2`... fields,
 *  which are determined during the parsing phase.
 */
export function getOperandType(stmt: StatementNode, key: string | `operand${number}`): Nullable<number> {
    const operandIndex = getOperandIndex(key);

    if (operandIndex <= 0) return null;

    return (stmt as any)[`type${operandIndex}`] ?? null;
}
