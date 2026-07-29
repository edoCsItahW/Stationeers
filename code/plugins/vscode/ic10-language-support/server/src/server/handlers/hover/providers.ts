/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file providers.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/25 22:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

import type { Hover } from "vscode-languageserver/node";
import {
    PureExeInstructionNode,
    DefineDirectiveNode,
    AliasDirectiveNode,
    TypeOfNodeEntry,
    IdentifierNode,
    StatementNode,
    LabelDefNode,
    TypeCategory,
    OperandNode,
    OperandType,
    TypeOfNode,
    BasicType
} from "ic10-node-api";

import { INS_LOCAL_MAP, INS_META_MAP } from "../../../mateData";
import type { HoverContext, HoverProvider } from "./types";
import { operandValueLength } from "../../../utils";
import svgBuilder from "../../../utils/svgBuilder";
import { t } from "../../../locals/locale";
import { s } from "../../../style";
import { Nullable } from "common";
import {
    provideOperandHover,
    formatBasicType,
    formatOperand,
    isInstruction,
    isInsideNode,
    findOperand,
    formatType
} from "./utils";

// ==================== LabelDef Provider ====================

export class LabelDefHoverProvider implements HoverProvider {
    canHandle(node: StatementNode): boolean {
        return node.type === "LabelDef";
    }

    provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as LabelDefNode;
        if (!isInsideNode(stmt.position.column, stmt.identifier.value.length + 1, ctx.character)) return null;

        svgBuilder.addSegments([
            { text: `(${ctx.t("hover.labelDef.type")}) ` },
            { text: stmt.identifier.value, bold: true, color: s("hover.labelDef.identifier") },
            { text: ` = ${stmt.position.line}` }
        ]);

        return {
            contents: {
                kind: "markdown",
                value: svgBuilder.build()
            }
        };
    }
}

// ==================== AliasDirective Provider ====================

export class AliasDirectiveHoverProvider implements HoverProvider {
    canHandle(node: StatementNode): boolean {
        return node.type === "AliasDirective";
    }

    provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as AliasDirectiveNode; // AliasDirectiveNode
        if (ctx.character > stmt.registerOrDevice.position.column + operandValueLength(stmt.registerOrDevice))
            return null;

        // 第一个关键字
        if (ctx.character < stmt.position.column + 5) {
            const ins = INS_META_MAP.get("alias");
            if (!ins) return null;

            const desc = INS_LOCAL_MAP.get("alias")?.desc?.[ctx.getLocale()];

            return {
                contents: {
                    kind: "markdown",
                    value:
                        `**${ctx.t("hover.instruction.signature")}**: _${ins.signature}_  \n` +
                        `**${ctx.t("hover.instruction.type")}**: _${ins.type}_  \n` +
                        `**${ctx.t("hover.instruction.details")}**: _${desc ?? "---"}_`
                }
            };
        }
        // 第二个标识符
        else if (ctx.character < stmt.registerOrDevice.position.column)
            svgBuilder.addSegments([
                { text: `(${ctx.t("hover.aliasDirective.type")}) ` },
                { text: stmt.identifier.value, bold: true, color: s("hover.aliasDirective.identifier") },
                { text: ": " }
            ]);
        // 第三个操作数
        else return provideOperandHover(stmt.registerOrDevice, ctx);

        const typeOfNodeMap: Map<string, TypeOfNodeEntry> = new Map(Object.entries(TypeOfNode));
        const type = typeOfNodeMap.get(stmt.registerOrDevice.type)?.kind;

        if (type)
            svgBuilder.addSegment({
                text: formatBasicType(type),
                color: s("hover.aliasDirective.type")
            });
        else
            svgBuilder.addSegment({
                text: (ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) ?? "",
                color: s("hover.aliasDirective.type")
            });

        svgBuilder.addSegment({ text: ` = ${formatOperand(stmt.registerOrDevice)}` });

        return {
            contents: {
                kind: "markdown",
                value: `${svgBuilder.build()}  \n${stmt.desc ? "---\n" + `**${ctx.t("hover.common.description")}**: ${stmt.desc}` : ""}`
            }
        };
    }
}

// ==================== DefineDirective Provider ====================

export class DefineDirectiveHoverProvider implements HoverProvider {
    canHandle(node: StatementNode): boolean {
        return node.type === "DefineDirective";
    }

    provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as DefineDirectiveNode;
        if (ctx.character > stmt.number.position.column + operandValueLength(stmt.number)) return null;

        if (ctx.character < stmt.position.column + 6) {
            const ins = INS_META_MAP.get("define");
            if (!ins) return null;

            const desc = INS_LOCAL_MAP.get("define")?.desc?.[ctx.getLocale()];

            return {
                contents: {
                    kind: "markdown",
                    value:
                        `**${ctx.t("hover.instruction.signature")}**: _${ins.signature}_  \n` +
                        `**${ctx.t("hover.instruction.type")}**: _${ins.type}_  \n` +
                        `**${ctx.t("hover.instruction.details")}**: _${desc ?? "---"}_`
                }
            };
        } else if (ctx.character < stmt.number.position.column)
            svgBuilder.addSegments([
                { text: `(${ctx.t("hover.defineDirective.type")}) ` },
                { text: stmt.identifier.value, bold: true, color: s("hover.defineDirective.identifier") },
                { text: ": " },
                {
                    text: (ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) || stmt.number.type,
                    color: s("hover.defineDirective.type")
                },
                { text: ` = ${formatOperand(stmt.number)}` }
            ]);
        else return provideOperandHover(stmt.number, ctx);

        return {
            contents: {
                kind: "markdown",
                value: `${svgBuilder.build()}\n${stmt.desc ? "---\n" + `**${ctx.t("hover.common.description")}**: ${stmt.desc}` : ""}`
            }
        };
    }
}

// ==================== Instruction Provider ====================

export class InstructionHoverProvider implements HoverProvider {
    canHandle(node: StatementNode): boolean {
        return isInstruction(node);
    }

    provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as PureExeInstructionNode;
        const { result, index } = findOperand(stmt, ctx.character);

        if (typeof result === "string") return this.provideKeywordHover(result, stmt, ctx);

        if (result.type === "Identifier")
            return this.provideIdentifierHover(
                result,
                ctx,
                index > 0 ? (stmt[`type${index}` as keyof typeof stmt] as unknown as OperandType) : undefined
            );

        return provideOperandHover(result, ctx);
    }

    private provideKeywordHover(
        keyword: string,
        stmt: PureExeInstructionNode,
        ctx: HoverContext
    ): Nullable<Hover> {
        if (!isInsideNode(stmt.position.column, stmt.keyword.length, ctx.character)) return null;

        const ins = INS_META_MAP.get(keyword);
        if (!ins) return null;

        const desc = INS_LOCAL_MAP.get(keyword)?.desc?.[ctx.getLocale()];

        return {
            contents: {
                kind: "markdown",
                value:
                    `**${ctx.t("hover.instruction.signature")}**: _${ins.signature}_  \n` +
                    `**${ctx.t("hover.instruction.type")}**: _${ins.type}_  \n` +
                    `**${ctx.t("hover.instruction.details")}**: _${desc ?? "---"}_`
            }
        };
    }

    private provideIdentifierHover(operand: OperandNode, ctx: HoverContext, type?: OperandType): Nullable<Hover> {
        const ident = operand as IdentifierNode;
        if (!isInsideNode(ident.position.column, ident.value.length, ctx.character)) return { contents: [] };

        const symbol = ctx.symbols?.[ident.value];
        if (!symbol) {
            if (type) {
                let text: string = "";
                let tp: string = "";

                switch (type) {
                    case OperandType.BATCH_MODE:
                        text = t("hover.operandType.batchMode");
                        tp = "BatchMode";
                        break;
                    case OperandType.LOGIC_SLOT:
                        text = t("hover.operandType.logicSlotType");
                        tp = "LogicSlotType";
                        break;
                    case OperandType.LOGIC_TYPE:
                        text = t("hover.operandType.logicType");
                        tp = "LogicType";
                        break;
                    case OperandType.REAGENT_MODE:
                        text = t("hover.operandType.reagentMode");
                        tp = "ReagentMode";
                        break;
                    case OperandType.SLOT_IDX:
                        text = t("hover.operandType.slotIdx");
                        tp = "SlotIndex";
                        break;
                }

                svgBuilder.addSegments([
                    { text: `(${text ?? "unknown"}) ` },
                    { text: ident.value, color: s("hover.contant.identifier") },
                    { text: ": " },
                    { text: tp, color: s("hover.contant.type") }
                ]);

                return { contents: { kind: "markdown", value: svgBuilder.build() } };
            }

            return { contents: { kind: "markdown", value: ident.value } };
        }

        let prefix = "";
        let color = "";

        switch (symbol.type) {
            case BasicType.DEVICE:
            case BasicType.REGISTER:
                prefix = `(${ctx.t("hover.aliasDirective.type")}) `;
                color = s("hover.aliasDirective.identifier");
                break;
            case BasicType.INTEGER:
            case BasicType.FLOAT:
                prefix = `(${ctx.t("hover.defineDirective.type")}) `;
                color = s("hover.defineDirective.identifier");
                break;
        }

        switch (symbol.category) {
            case TypeCategory.LABEL:
                prefix = `(${ctx.t("hover.labelDef.type")}) `;
                color = s("hover.labelDef.identifier");
                break;
        }

        if (prefix.length) svgBuilder.addSegment({ text: prefix });

        svgBuilder.addSegments([
            { text: symbol.name, bold: true, color },
            { text: ": " },
            { text: symbol.typeName ?? formatBasicType(symbol.type) }
        ]);

        if (symbol.value) svgBuilder.addSegment({ text: ` = ${symbol.value}` });

        const descPart = symbol.desc ? `  \n**${ctx.t("hover.common.description")}**: ${symbol.desc}` : "";

        return {
            contents: {
                kind: "markdown",
                value: svgBuilder.build() + descPart
            }
        };
    }
}
