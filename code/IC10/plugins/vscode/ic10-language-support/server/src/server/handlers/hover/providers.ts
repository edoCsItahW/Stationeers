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
} from "ic10c-node";
import {  } from "ic10r-node";

import { INS_LOCAL_MAP, INS_META_MAP } from "../../../mateData";
import type { HoverContext, IHoverProvider } from "./types";
import { operandValueLength } from "../../../utils";
import svgBuilder from "../../../utils/svgBuilder";
import type { Nullable, Optional } from "common";
import { SettingsManager } from "../../services";
import { t } from "../../../locals";
import { s } from "../../../style";
import { formatBasicType, formatOperand, isInstruction, isInsideNode, findOperand, formatType } from "./utils";

type HoverRendererKey = SettingsManager["hoverRenderer"];

interface HoverAdapter<T extends string | void = string | void> {
    supplement: (...args: any[]) => T;
    render: () => T extends string ? void : string;
}

abstract class HoverProvider<R extends HoverRendererKey> implements IHoverProvider {
    protected buffer: string = "";
    private adapter = {
        svg: new SvgAdapter(),
        markdown: new MarkdownAdapter()
    } satisfies { [K in HoverRendererKey]: HoverAdapter; };

    constructor(protected settingMgr: SettingsManager) {}

    abstract canHandle(node: StatementNode): boolean;

    abstract provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover>;

    supplement(args: {
        [K in R]: Parameters<(typeof this.adapter)[K]["supplement"]>;
    }): void {
        const result = (this.adapter[this.settingMgr.hoverRenderer] as any).supplement(...args[this.settingMgr.hoverRenderer as R]);

        if (result) this.buffer += result;
    }

    renderer(): string {
        let result = this.adapter[this.settingMgr.hoverRenderer].render();

        result ||= this.buffer;

        this.buffer = "";

        return result;
    }
}

class SvgAdapter implements HoverAdapter {
    supplement(...[segments]: Parameters<typeof svgBuilder.addSegments> | Parameters<typeof svgBuilder.addSegment>) {
        if (Array.isArray(segments)) svgBuilder.addSegments(segments);
        else svgBuilder.addSegment(segments);
    }

    render(): string {
        return svgBuilder.build();
    }
}

class MarkdownAdapter implements HoverAdapter {
    supplement(content: string): string {
        return content;
    }

    render() {}
}

abstract class HoverOperand extends HoverProvider<HoverRendererKey> {
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
    provideOperandHover(operand: OperandNode, ctx: HoverContext): Nullable<Hover> {
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

        this.supplement({
            svg: [
                [
                    { text: `(${type}) ` },
                    { text: formatOperand(operand) },
                    { text: ": " },
                    { text: operand.type.toLowerCase() }
                ]
            ],
            markdown: [`(${type}) ${formatOperand(operand)}: ${operand.type.toLowerCase()}`]
        });

        return {
            contents: {
                kind: "markdown",
                value: this.renderer()
            }
        };
    }
}

// ==================== LabelDef Provider ====================

/**
 * @summary 标签定义悬停提供器 — 为 LabelDef 语句生成悬停提示
 *
 * @summary LabelDef hover provider — generates hover tooltips for LabelDef statements
 *
 * @desc 当光标悬停在标签定义行时，显示标签名及其所在行号。
 *
 * @desc When hovering over a label definition line, displays the label name and its line number.
 * */
export class LabelDefHoverProvider extends HoverProvider<HoverRendererKey> {
    constructor(settingMgr: SettingsManager) {
        super(settingMgr);
    }

    canHandle(node: StatementNode): boolean {
        return node.type === "LabelDef";
    }

    provideHover(node: StatementNode, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as LabelDefNode;
        if (!isInsideNode(stmt.position.column, stmt.identifier.value.length + 1, ctx.character)) return null;

        this.supplement({
            svg: [
                [
                    { text: `(${ctx.t("hover.labelDef.type")}) ` },
                    { text: stmt.identifier.value, bold: true, color: s("hover.labelDef.identifier") },
                    { text: ` = ${stmt.position.line}` }
                ]
            ],
            markdown: [`(${ctx.t("hover.labelDef.type")}) **${stmt.identifier.value}** = ${stmt.position.line}`]
        });

        return {
            contents: {
                kind: "markdown",
                value: this.renderer()
            }
        };
    }
}

// ==================== AliasDirective Provider ====================

/**
 * @summary Alias 指令悬停提供器 — 为 alias 语句生成悬停提示
 *
 * @summary Alias directive hover provider — generates hover tooltips for alias statements
 *
 * @desc 识别光标在 alias 关键字、标识符或操作数上的位置，分别显示
 *  指令签名与描述、别名类型信息或操作数值。
 *
 * @desc Identifies the cursor position on alias keyword, identifier, or operand,
 *  displaying instruction signature/description, alias type info, or operand value respectively.
 * */
export class AliasDirectiveHoverProvider extends HoverOperand {
    constructor(settingMgr: SettingsManager) {
        super(settingMgr);
    }

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
            this.supplement({
                svg: [
                    [
                        { text: `(${ctx.t("hover.aliasDirective.type")}) ` },
                        { text: stmt.identifier.value, bold: true, color: s("hover.aliasDirective.identifier") },
                        { text: ": " }
                    ]
                ],
                markdown: [`(${ctx.t("hover.aliasDirective.type")}) **${stmt.identifier.value}**: `]
            });
        // 第三个操作数
        else return this.provideOperandHover(stmt.registerOrDevice, ctx);

        const typeOfNodeMap: Map<string, TypeOfNodeEntry> = new Map(Object.entries(TypeOfNode));
        const type = typeOfNodeMap.get(stmt.registerOrDevice.type)?.kind;

        if (type)
            this.supplement({
                svg: [{ text: formatBasicType(type), color: s("hover.aliasDirective.type") }],
                markdown: [formatBasicType(type)]
            });
        else
            this.supplement({
                svg: [
                    {
                        text: (ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) ?? "",
                        color: s("hover.aliasDirective.type")
                    }
                ],
                markdown: [(ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) ?? ""]
            });

        this.supplement({
            svg: [{ text: ` = ${formatOperand(stmt.registerOrDevice)}` }],
            markdown: [` = ${formatOperand(stmt.registerOrDevice)}`]
        })

        return {
            contents: {
                kind: "markdown",
                value: `${this.renderer()}  \n${stmt.desc ? "---\n" + `**${ctx.t("hover.common.description")}**: ${stmt.desc}` : ""}`
            }
        };
    }
}

// ==================== DefineDirective Provider ====================

/**
 * @summary Define 指令悬停提供器 — 为 define 语句生成悬停提示
 *
 * @summary Define directive hover provider — generates hover tooltips for define statements
 *
 * @desc 识别光标在 define 关键字、标识符或数值上的位置，分别显示
 *  指令签名与描述、常量类型信息或具体数值。
 *
 * @desc Identifies the cursor position on define keyword, identifier, or value,
 *  displaying instruction signature/description, constant type info, or the value itself.
 * */
export class DefineDirectiveHoverProvider extends HoverOperand {
    constructor(settingMgr: SettingsManager) {
        super(settingMgr);
    }

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
            this.supplement({
                svg: [
                    [
                        { text: `(${ctx.t("hover.defineDirective.type")}) ` },
                        { text: stmt.identifier.value, bold: true, color: s("hover.defineDirective.identifier") },
                        { text: ": " },
                        {
                            text: (ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) || stmt.number.type,
                            color: s("hover.defineDirective.type")
                        },
                        { text: ` = ${formatOperand(stmt.number)}` }
                    ]
                ],
                markdown: [
                    `(${ctx.t("hover.defineDirective.type")}) **${stmt.identifier.value}**: ${(ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) || stmt.number.type} = ${formatOperand(stmt.number)}`
                ]
            });
        else return this.provideOperandHover(stmt.number, ctx);

        return {
            contents: {
                kind: "markdown",
                value: `${this.renderer()}\n${stmt.desc ? "---\n" + `**${ctx.t("hover.common.description")}**: ${stmt.desc}` : ""}`
            }
        };
    }
}

// ==================== Instruction Provider ====================

/**
 * @summary 指令悬停提供器 — 为可执行指令语句生成悬停提示
 *
 * @summary Instruction hover provider — generates hover tooltips for executable instruction statements
 *
 * @desc 处理各类 IC10 可执行指令（以 "Instruction" 结尾的语句类型）。
 *  对关键字位置显示签名与描述，对标识符操作数从符号表解析类型并显式其定义信息，
 *  对其他操作数委托给 provideOperandHover。
 *
 * @desc Handles various IC10 executable instructions (statement types ending with "Instruction").
 *  For keyword positions, displays signature and description; for identifier operands,
 *  resolves types from the symbol table and shows definition info;
 *  other operands are delegated to provideOperandHover.
 * */
export class InstructionHoverProvider extends HoverOperand {
    constructor(settingMgr: SettingsManager) {
        super(settingMgr);
    }

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

        return this.provideOperandHover(result, ctx);
    }

    private provideKeywordHover(keyword: string, stmt: PureExeInstructionNode, ctx: HoverContext): Nullable<Hover> {
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

                this.supplement({
                    svg: [
                        [
                            { text: `(${text ?? "unknown"}) ` },
                            { text: ident.value, color: s("hover.contant.identifier") },
                            { text: ": " },
                            { text: tp, color: s("hover.contant.type") }
                        ]
                    ],
                    markdown: [`(${text ?? "unknown"}) ${ident.value}: ${tp}`]
                });

                return { contents: { kind: "markdown", value: this.renderer() } };
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

        if (prefix.length)
            this.supplement({
                svg: [{ text: prefix }],
                markdown: [prefix]
            });

        this.supplement({
            svg: [
                [
                    { text: symbol.name, bold: true, color },
                    { text: ": " },
                    { text: symbol.typeName ?? formatBasicType(symbol.type) }
                ]
            ],
            markdown: [`**${symbol.name}**: ${symbol.typeName ?? formatBasicType(symbol.type)}`]
        });

        if (symbol.value)
            this.supplement({
                svg: [{ text: ` = ${symbol.value}` }],
                markdown: [` = ${symbol.value}`]
            });

        const descPart = symbol.desc ? `  \n**${ctx.t("hover.common.description")}**: ${symbol.desc}` : "";

        return {
            contents: {
                kind: "markdown",
                value: this.renderer() + descPart
            }
        };
    }
}
