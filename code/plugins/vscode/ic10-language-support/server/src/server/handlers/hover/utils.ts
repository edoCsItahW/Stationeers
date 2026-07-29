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
 * Find the statement at the given line using binary search.
 */
export function findStatementAtPosition(statements: StatementNode[], line: number): Nullable<StatementNode> {
    const idx = lowerBound(statements, item => item.position.line >= line);
    return idx >= 0 && idx < statements.length ? statements[idx] : null;
}

/**
 * Check if the cursor character position falls within a node's range.
 * @param col The column position of the node (1-based).
 * @param length The text length of the node.
 */
export function isInsideNode(col: number, length: number, character: number): boolean {
    return character >= col && character <= col + length;
}

/**
 * Find the operand at a given character position in an instruction, or return the keyword string.
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
 * Format an operand node into a readable string representation.
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
 * Format a BasicType enum value to its lowercase string name.
 */
export function formatBasicType(type: BasicType): string {
    return getEnumName(BasicType, type)!.toLowerCase();
}

/**
 * Resolve the human-readable type name for an identifier from the symbol table.
 */
export function formatType(identifier: IdentifierNode, symbols: SymbolMap): Optional<string> {
    const symbol = symbols[identifier.value];
    if (symbol) {
        if (symbol.typeName) return symbol.typeName;
        return formatBasicType(symbol.type);
    }
}

/**
 * Type guard: check if a statement node is an executable instruction (excluding ErrorNode).
 */
export function isInstruction(node: StatementNode): node is PureExeInstructionNode {
    return node.type.endsWith("Instruction");
}

/**
 * Shared hover provider for operand nodes (used by directive and instruction providers).
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
