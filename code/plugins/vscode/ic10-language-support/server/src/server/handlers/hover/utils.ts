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

import type { Hover } from "vscode-languageserver/node";
import { BasicType } from "ic10-node-api";
import type {
    PureExeInstructionNode,
    IdentifierNode,
    StatementNode,
    OperandNode,
    SymbolMap,
} from "ic10-node-api";

import { lowerBound, getEnumName, type Nullable, type Optional } from "common";
import { groupHandlers, visit, operandValueLength } from "../../../utils";
import svgBuilder from "../../../utils/svgBuilder";
import type { HoverContext } from "./types";

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
 * @summary 在指令中根据光标位置查找对应的操作数或关键字
 *
 * @summary Find the operand or keyword at a given character position in an instruction
 *
 * @param node 可执行指令 AST 节点
 * @param node Pure executable instruction AST node
 * @param a 光标列号（1-based）
 * @param a Cursor column position (1-based)
 *
 * @returns 找到的操作数节点（或关键字字符串）及其索引
 * @returns The found operand node (or keyword string) and its index
 */
export function findOperand(
    node: PureExeInstructionNode,
    a: number
): { result: OperandNode | string; index: number } {
    let maxCol = -Infinity;
    let foundOperand: Nullable<OperandNode> = null;
    let index = -1;

    for (const key in node)
        if (key.startsWith("operand")) {
            const operand: Optional<OperandNode> = (node as any)[key];
            if (operand && operand.position && typeof operand.position.column === "number") {
                const col = operand.position.column;
                if (col <= a && col > maxCol) {
                    maxCol = col;
                    foundOperand = operand;
                    index = Number(key.replace("operand", ""));
                }
            }
        }

    return {
        result: foundOperand !== null ? foundOperand : node.keyword,
        index
    };
}

/**
 * @summary 将操作数节点格式化为可读的字符串表示
 *
 * @summary Format an operand node into a readable string representation
 *
 * @param op 操作数 AST 节点
 * @param op Operand AST node
 *
 * @returns 格式化后的字符串（如 HASH("value")、寄存器名、数字等）
 * @returns Formatted string (e.g. HASH("value"), register name, number, etc.)
 */
export function formatOperand(op: OperandNode): string {
    return visit(
        {
            Error: node => node.message,
            Constant: node => node.keyword,
            HashCall: node => `HASH("${node.value.value}")`,
            StrCall: node => `STR("${node.value.value}")`,
            ...groupHandlers(["BinaryNumber", "HexNumber", "Identifier", "Register", "Device"], node => node.value),
            ...groupHandlers(["Integer", "Float"], node => node.value.toString())
        },
        op
    );
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
    const symbol = symbols[identifier.value];
    if (symbol) {
        if (symbol.typeName) return symbol.typeName;
        return formatBasicType(symbol.type);
    }
}

/**
 * @summary 类型守卫：判断语句节点是否为可执行指令（排除 ErrorNode）
 *
 * @summary Type guard: check if a statement node is an executable instruction (excluding ErrorNode)
 *
 * @param node 语句 AST 节点
 * @param node Statement AST node
 *
 * @returns 如果节点类型以 "Instruction" 结尾返回 true
 * @returns True if the node type ends with "Instruction"
 */
export function isInstruction(node: StatementNode): node is PureExeInstructionNode {
    return node.type.endsWith("Instruction");
}

/**
 * @summary 操作数悬停提供器 — 为指令和伪指令中的操作数节点生成悬停内容
 *
 * @summary Operand hover provider — generates hover content for operand nodes in instructions and directives
 *
 * @desc 供指令和伪指令悬停提供器共享使用。根据操作数类型（Device、Register、
 *  Constant、Error 等）生成对应的 SVG 格式悬停提示。
 *
 * @desc Shared by instruction and directive hover providers. Generates SVG-formatted
 *  hover tooltips based on operand type (Device, Register, Constant, Error, etc.).
 *
 * @param operand 操作数 AST 节点
 * @param operand Operand AST node
 * @param ctx 悬停上下文（包含光标位置和国际化函数）
 * @param ctx Hover context (includes cursor position and i18n functions)
 *
 * @returns 悬停内容，如果光标不在操作数范围内返回空 contents
 * @returns Hover content, or empty contents if cursor is not within the operand range
 */
export function provideOperandHover(operand: OperandNode, ctx: HoverContext): Hover | null {
    if (!isInsideNode(operand.position.column, operandValueLength(operand), ctx.character)) return { contents: [] };

    let type: string;
    switch (operand.type) {
        case "Device":
        case "Register":
        case "Constant":
        case "Error":
            type = ctx.t(`hover.operandType.${operand.type.toLowerCase()}` as any);
            break;
        default:
            type = ctx.t("hover.operandType.number");
    }

    svgBuilder.addSegments([
        { text: `(${type}) ` },
        { text: formatOperand(operand) },
        { text: ": " },
        { text: operand.type.toLowerCase() }
    ]);

    return {
        contents: {
            kind: "markdown",
            value: svgBuilder.build()
        }
    };
}
