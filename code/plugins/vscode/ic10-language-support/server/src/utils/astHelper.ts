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
import { Program, OperandNode } from "ic10-node-api";

type NodeType = Program["statements"][number] | OperandNode;

type ExtractNodeByType<T extends NodeType["type"]> = Extract<NodeType, { type: T }>;

export function visit<T extends NodeType = NodeType, R = any>(
    visitor: { [K in T["type"]]: (node: ExtractNodeByType<K>) => R },
    node: T
): R {
    return visitor[node.type as T["type"]](node as any);
}

export function visitOperand<R = any>(
    visitor: { [K in OperandNode["type"]]: (node: Extract<OperandNode, { type: K }>) => R },
    node: OperandNode
): R {
    return visitor[node.type as OperandNode["type"]](node as any);
}

export type HandleGroup<T extends NodeType["type"] = NodeType["type"], R = any> = {
    [K in T]: (node: ExtractNodeByType<K>) => R;
};

export function groupHandlers<T extends NodeType["type"], R = any>(
    types: T[],
    handler: (node: ExtractNodeByType<T>) => R
): HandleGroup<T, R> {
    const result = {} as HandleGroup<T, R>;

    for (const t of types) result[t] = handler;

    return result;
}

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
