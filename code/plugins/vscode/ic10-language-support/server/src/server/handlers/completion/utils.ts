// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

import type {StatementNode, Token} from "ic10-node-api";
import {TokenType} from "ic10-node-api";

import type {Nullable} from "../../../../../common/types/utils";
import {findMaxByCondition, findMinByCondition, lowerBound} from "../../../../../common/utils";


/**
 * 在语句列表中二分查找光标所在行的语句。
 */
export function findStatementAtPosition(
    statements: StatementNode[],
    line: number
): Nullable<StatementNode> {
    const idx = lowerBound(statements, (item) => item.position.line >= line);
    return (idx >= 0 && idx < statements.length) ? statements[idx] : null;
}

/**
 * 从 tokens 中筛选指定行的 token。
 */
export function tokensOnLine(tokens: Token[], line: number): Token[] {
    return tokens.filter(t => t.pos.line === line);
}

/**
 * 用 token 定位光标前最后一个有意义的单词。
 * 用于 ErrorNode 场景下判断用户正在输入什么。
 */
export function lastTokenBeforeCursor(
    lineTokens: Token[],
    character: number
): Nullable<Token> {
    return lineTokens
        .filter(t => t.pos.column < character)
        .reverse()
        .find(t => t.type !== TokenType.NEWLINE && t.type !== TokenType.END) ?? null;
}

/**
 * 获取光标处所在行的前缀文本（光标前部分）。
 */
export function cursorPrefix(source: string, line: number, character: number): string {
    const lines = source.split("\n");
    const currentLine = lines[line - 1] ?? "";
    return currentLine.substring(0, character - 1);
}

/**
 * 判断光标是否在语句的关键字范围内。
 */
export function isAtKeyword(stmt: StatementNode, character: number): boolean {
    const keywordLen = (stmt as any).keyword?.length ?? 0;
    if (!keywordLen) return false;
    return character >= stmt.position.column
        && character <= stmt.position.column + keywordLen;
}

/**
 * 从 AST 节点获取关键字。
 * 指令节点: 通过 `keyword` 字段
 * alias / define: 硬编码
 */
export function getKeyword(stmt: StatementNode): Nullable<string> {
    if ((stmt as any).keyword) return (stmt as any).keyword;
    if (stmt.type === "AliasDirective") return "alias";
    if (stmt.type === "DefineDirective") return "define";
    return null;
}

/**
 * 确定光标当前在第几个操作数位置。
 * 返回: 0 = 关键字, 1 = operand1, 2 = operand2 ...
 */
export function getOperandIndex(key: string | `operand${number}`) {
    return Number.parseInt(key.replace("operand", ""), 10);
}

export function findCurrentOperand(stmt: StatementNode, character?: string) {
    const values = Object.entries(stmt).filter(([key]) => key.startsWith("operand"));

    const lastErr = findMinByCondition(
        values,
        ([, value]) => value.type === "Error",
        ([k1, ], [k2,]) => getOperandIndex(k1) - getOperandIndex(k2)
    )

    if (character || !lastErr)
        return lastErr;

    return findMaxByCondition(
        values,
        ([k,]) => getOperandIndex(k) < getOperandIndex(lastErr[0]),
        ([k1,], [k2,]) => getOperandIndex(k1) - getOperandIndex(k2)
    );
}

/**
 * 从 AST 节点获取指定操作数位的 OperandType 数值。
 * 优先从 AST 节点的 `type1`/`type2`... 字段读取。
 */
export function getOperandType(stmt: StatementNode, key: string | `operand${number}`): Nullable<number> {
    const operandIndex = getOperandIndex(key);

    if (operandIndex <= 0) return null;

    return (stmt as any)[`type${operandIndex}`] ?? null;
}
