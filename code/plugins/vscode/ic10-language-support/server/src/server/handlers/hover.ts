/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file hover.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/27 21:19
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Connection} from "vscode-languageserver/node";
import {BasicType, TypeOfNode} from "ic10-node-api";
import type {
    OperandNode,
    IdentifierNode,
    SymbolMap,
    TypeOfNodeEntry,
    ExecutableInstructionNode,
    ErrorNode
} from "ic10-node-api";

import {lowerBound, getEnumName} from "../../../../common/utils";
import {type Optional} from "../../../../common/types/utils";
import {INS_META_MAP, INS_LOCAL_MAP} from "../../mateData";
import svgBuilder from "../../utils/svgBuilder";
import {t, locale} from "../../locals/locale";
import {DocumentCache} from "../cache";
import {s} from "../../style";


type OnHoverHandlerType = Parameters<Connection["onHover"]>[0];


export class HoverHandler {
    constructor(private docCache: DocumentCache) {
    }

    public formatOperand(op: OperandNode): string {
        switch (op.type) {
            case "Error":
                return op.message;
            case "Constant":
                return op.keyword;
            case "HashCall":
                return `HASH(${op.value.value})`;
            case "StrCall":
                return `STR(${op.value.value})`;
            case "Identifier":
                return op.value;
            default:
                return String(op.value);
        }
    }

    private formatBasicType(type: BasicType): string {
        return getEnumName(BasicType, type)!.toLowerCase();
    }

    private formatType(identifier: IdentifierNode, symbols: SymbolMap): Optional<string> {
        const symbol = symbols[identifier.value];
        if (symbol) {
            if (symbol.typeName)
                return symbol.typeName;

            return this.formatBasicType(symbol.type);
        }
    };

    private findOperand(node: Exclude<ExecutableInstructionNode, ErrorNode>, a: number): OperandNode | string {
        let maxCol = -Infinity;
        let foundOperand: OperandNode | null = null;

        // 遍历对象所有键
        for (const key in node)
            // 只处理以 "operand" 开头的属性，并且存在 position.column
            if (key.startsWith('operand')) {
                const operand = (node as any)[key] as OperandNode | undefined;
                if (operand && operand.position && typeof operand.position.column === 'number') {
                    const col = operand.position.column;
                    if (col <= a && col > maxCol) {
                        maxCol = col;
                        foundOperand = operand;
                    }
                }
            }

        // 如果找到，返回 operand 对象；否则返回 keyword
        return foundOperand !== null ? foundOperand : node.keyword;
    }

    public handle(...[{position, textDocument}]: Parameters<OnHoverHandlerType>): ReturnType<OnHoverHandlerType> {
        const line = position.line + 1;
        const character = position.character + 1;
        const cache = this.docCache.getCache(textDocument.uri);

        const typeOfNodeMap: Map<string, TypeOfNodeEntry> = new Map(Object.entries(TypeOfNode));

        if (!cache?.ast) return {contents: []};

        const idx = lowerBound(cache.ast.statements, (item) => item.position.line >= line);
        if (idx < 0 || idx >= cache.ast.statements.length) return {contents: []};

        const stmt = cache.ast.statements[idx];

        if (((stmt): stmt is Exclude<ExecutableInstructionNode, ErrorNode> => stmt.type.endsWith("Instruction"))(stmt)) {
            const result = this.findOperand(stmt, character);
            if (typeof result === "string") {
                const ins = INS_META_MAP.get(result);
                if (ins) {
                    const desc = INS_LOCAL_MAP.get(result)?.desc?.[locale.getLocale()];

                    return {
                        contents: {
                            kind: "markdown",
                            value: `**${t("hover.instruction.signature")}**: _${ins.signature}_  \n` +
                                `**${t("hover.instruction.type")}**: _${ins.type}_  \n` +
                                `**${t("hover.instruction.details")}**: _${desc ?? '---'}_`
                        }
                    }
                }
            } else if (result.type === "Identifier") {
                const symbol = cache.symbols?.[result.value];

                if (symbol) {
                    svgBuilder.addSegments([
                        {text: symbol.name},
                        {text: ": " + (symbol.typeName ?? this.formatBasicType(symbol.type))}
                    ]);

                    if (symbol.value)
                        svgBuilder.addSegment({text: ` = ${symbol.value}`});

                    return {
                        contents: {
                            kind: "markdown",
                            value: svgBuilder.build() + (symbol.desc ? `  \n${symbol.desc}` : "")
                        }
                    }
                }

                return {
                    contents: {
                        kind: "markdown",
                        value: result.value
                    }
                }
            } else return {
                contents: {
                    kind: "markdown",
                    value: this.formatOperand(result)
                }
            }

        } else
            switch (stmt.type) {
                case "LabelDef":
                    svgBuilder.addSegments([
                        {text: `(${t("hover.labelDef.type")}) `},
                        {text: stmt.identifier.value, bold: true, color: s("hover.labelDef.identifier")},
                        {text: ` = ${stmt.position.line}`}
                    ]);
                    return {
                        contents: {
                            kind: "markdown",
                            value: svgBuilder.build()
                        }
                    };
                case "AliasDirective":
                    svgBuilder.addSegments([
                        {text: `(${t("hover.aliasDirective.type")}) `},
                        {text: stmt.identifier.value, bold: true, color: s("hover.aliasDirective.identifier")},
                        {text: ": "}
                    ]);

                    const type = typeOfNodeMap.get(stmt.registerOrDevice.type)?.kind;

                    if (type)
                        svgBuilder.addSegment({
                            text: this.formatBasicType(type),
                            color: s("hover.aliasDirective.type")
                        })
                    else
                        svgBuilder.addSegment({
                            text: (cache.symbols ? this.formatType(stmt.identifier, cache.symbols) : null) ?? "",
                            color: s("hover.aliasDirective.type")
                        });

                    svgBuilder.addSegment({text: ` = ${this.formatOperand(stmt.registerOrDevice)}`});

                    return {
                        contents: {
                            kind: "markdown",
                            value: `${svgBuilder.build()}\n${stmt.desc ? "---\n" + stmt.desc : ""}`
                        }
                    };
                case "DefineDirective":
                    svgBuilder.addSegments([
                        {text: `(${t("hover.defineDirective.type")}) `},
                        {text: stmt.identifier.value, bold: true, color: s("hover.defineDirective.identifier")},
                        {text: ": "},
                        {
                            text: (cache.symbols ? this.formatType(stmt.identifier, cache.symbols) : null)
                                || stmt.number.type, color: s("hover.aliasDirective.type")
                        },
                        {text: ` = ${this.formatOperand(stmt.number)}`}
                    ]);

                    return {
                        contents: {
                            kind: "markdown",
                            value: `${svgBuilder.build()}\n${stmt.desc ? "---\n" + stmt.desc : ""}`
                        }
                    };
                default:
                    break;
            }

        return {contents: "xxx"};
    }
}
