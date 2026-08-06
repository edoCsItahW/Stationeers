// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file astHelper.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 21:33
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Optional } from "type-fest";
import {
    PureExeInstructionNode,
    DefineDirectiveNode,
    AliasDirectiveNode,
    StatementNode,
    TokenCategory,
    OperandNode,
    OperandType,
    TokenType,
    Position,
    Program,
    Token
} from "ic10c-node";

type NodeType = Program["statements"][number] | OperandNode;

type ExtractNodeByType<T extends NodeType["type"]> = Extract<NodeType, { type: T }>;

/**
 * @summary AST 节点访问器，根据节点类型分发到对应的处理函数
 *
 * @summary AST node visitor that dispatches to type-specific handlers
 *
 * @desc 泛型访问器模式实现。传入一个按节点 type 分发的处理器映射和一个节点，
 * 自动调用对应类型的处理函数。适用于遍历 IC10 程序的 AST。
 *
 * @desc Generic visitor pattern. Given a handler map keyed by node type and
 * a node, calls the matching handler. Used for traversing IC10 program ASTs.
 * */
export function visit<T extends NodeType = NodeType, R = any>(
    visitor: { [K in T["type"]]: (node: ExtractNodeByType<K>) => R },
    node: T
): R {
    return visitor[node.type as T["type"]](node as any);
}

/**
 * @summary 操作数节点访问器，专门用于 OperandNode 类型分发
 *
 * @summary Operand-specific visitor that dispatches on OperandNode subtypes
 *
 * @desc 与 visit 类似，但限定于 OperandNode 及其子类型（如 Device、Register、
 * Integer、Float、Identifier 等），提供更精确的类型推导。
 *
 * @desc Similar to visit but scoped to OperandNode and its subtypes
 * (Device, Register, Integer, Float, Identifier, etc.) for precise type inference.
 * */
export function visitOperand<R = any>(
    visitor: { [K in OperandNode["type"]]: (node: Extract<OperandNode, { type: K }>) => R },
    node: OperandNode
): R {
    return visitor[node.type as OperandNode["type"]](node as any);
}

/**
 * @summary 节点类型到处理函数的映射组
 *
 * @summary Mapping from node types to handler functions
 *
 * @desc 将一组 AST 节点 type 映射到对应的处理函数类型。
 * 通常与 visit 函数配合使用以构建类型安全的访问器。
 *
 * @desc Maps a set of AST node types to their handler function types.
 * Typically used with visit() to construct type-safe visitors.
 * */
export type HandleGroup<T extends NodeType["type"] = NodeType["type"], R = any> = {
    [K in T]: (node: ExtractNodeByType<K>) => R;
};

/**
 * @summary 将多个节点类型批量绑定到同一个处理函数
 *
 * @summary Batch-binds multiple node types to a single handler function
 *
 * @desc 便捷工具函数，接收一个类型数组和一个处理函数，生成一个 HandleGroup。
 * 常用于 visit/visitOperand 中多种节点共享同一处理逻辑的场景。
 *
 * @desc Convenience utility that takes a type array and a single handler,
 * producing a HandleGroup. Useful when multiple node types share the same logic.
 * */
export function groupHandlers<T extends NodeType["type"], R = any>(
    types: T[],
    handler: (node: ExtractNodeByType<T>) => R
): HandleGroup<T, R> {
    const result = {} as HandleGroup<T, R>;

    for (const t of types) result[t] = handler;

    return result;
}

/**
 * @summary 获取操作数节点的显示字符长度
 *
 * @summary Gets the display character length of an operand node
 *
 * @desc 根据操作数类型返回其字符串表示的长度（字符数），
 * 用于计算格式化对齐等场景。
 *
 * @desc Returns the character count of an operand's string representation
 * based on its type. Used for formatting/alignment calculations.
 * */
export function operandValueLength(node: OperandNode): number {
    return visitOperand(
        {
            ...groupHandlers(
                ["Device", "Register", "HexNumber", "BinaryNumber", "Identifier"],
                node => node.value.length
            ),
            ...groupHandlers(["Integer", "Float"], node => node.value.toString().length),
            Constant: node => node.keyword.length,
            HashCall: node => node.value.value.length + 8,
            StrCall: node => node.value.value.length + 7,
            Error: node => node.token.lexeme.length
        },
        node
    );
}

/**
 * @summary 将操作数节点转换为其源代码字符串表示
 *
 * @summary Converts an operand node to its source-code string representation
 *
 * @desc 根据操作数类型还原其对应的源代码文本。例如 HashCall 还原为 `HASH("...")`，
 * StrCall 还原为 `STR("...")`，普通标识符和数字直接返回值。
 *
 * @desc Reconstructs the source-code text for an operand node based on its type.
 * E.g., HashCall → `HASH("...")`, StrCall → `STR("...")`, plain identifiers
 * and numbers are returned as-is.
 * */
export function operandToString(node: OperandNode): string {
    return visitOperand(
        {
            ...groupHandlers(["Device", "Register", "HexNumber", "BinaryNumber", "Identifier"], node => node.value),
            ...groupHandlers(["Integer", "Float"], node => node.value.toString()),
            Constant: node => node.keyword,
            HashCall: node => `HASH("${node.value.value}")`,
            StrCall: node => `STR("${node.value.value}")`,
            Error: node => node.token.lexeme
        },
        node
    );
}


export function end(node: OperandNode): Position;
export function end(token: Token): Position;
export function end(item: OperandNode | Token): Position {
    if ("pos" in item)
        return { line: item.pos.line /* 无换行 */, column: item.pos.column + item.lexeme.length };

    return { line: item.position.line /* 无换行 */, column: item.position.column + operandValueLength(item) };
}


export function getOperandType(ins: PureExeInstructionNode, idx: number): Optional<OperandType> {
    return (ins as any)[`type${idx}`];
}

export function isInstruction(stmt: StatementNode): stmt is PureExeInstructionNode {
    return stmt.type.endsWith("Instruction");
}

export function isDirectiveNode(stmt: StatementNode): stmt is AliasDirectiveNode | DefineDirectiveNode {
    return stmt.type.endsWith("Directive");
}

export function findRangeTokens(
    tokens: Token[],
    column: number
): {
    prev: number;
    curr: number;
    next: number;
} {
    const result = { prev: -1, curr: -1, next: -1 };

    tokens.some((token, i) => {
        const idx = (
            token.category === TokenCategory.WHITESPACE
            || token.category === TokenCategory.COMMENT
            || token.type === TokenType.END
        ) ? -1 : i;
        if (end(token).column <= column) result.prev = idx;
        else if (token.pos.column >= column) {
            result.next = idx;
            return true;
        } else result.curr = idx;
    });

    return result;
}
