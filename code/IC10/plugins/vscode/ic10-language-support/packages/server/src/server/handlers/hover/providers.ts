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

import { instructions as localsInstructions } from "@ic10/metadata/locals";
import { instructions as stdInstructions } from "@ic10/metadata/std";
import { Nullable, Optional, pascalToSnake } from "@ic10/common";
import type { Hover } from "vscode-languageserver/node";
import { hashValue, strValue } from "@ic10/runtime";
import { resources } from "../../../locals";
import {
    PureExeInstructionNode,
    DefineDirectiveNode,
    AliasDirectiveNode,
    TypeOfNodeEntry,
    IdentifierNode,
    LabelDefNode,
    TypeCategory,
    OperandType,
    StringNode,
    TypeOfNode,
    TokenType,
    ErrorNode,
    Statement,
    BasicType,
    Operand
} from "@ic10/compiler";

import { formatBasicType, formatType, isInsideNode } from "./utils";
import type { HoverContext, IHoverProvider } from "./types";
import { SettingsManager } from "../../services";
import { s } from "../../../style";
import {
    renderMarkdownCard,
    renderSvgCard,
    operandValueLength,
    DescriptionSolver,
    operandToString,
    locateOperand,
    TypeHintTags,
    EnumKeyMap,
    type HoverBadge,
    type HoverCard,
    AST,
    end
} from "../../../utils";

/** 图标的语义种类（决定外圈/填充取哪一组颜色与大写字面） */
type BadgeKind = "device" | "register" | "alias" | "constant" | "label" | "enum" | "number" | "instruction" | "tag";

/** 主题取色的路径（`s()` 的入参类型） */
type StylePath = Parameters<typeof s>[0];

/**
 * @summary 操作数种类在 `hover.operandType.*` 里的候选键
 *
 * @summary Candidate keys of an operand kind inside `hover.operandType.*`
 *
 * @desc 静态设备取内部设备节点（`d0` 是平凡设备而不是静态设备）；数字/字符串/宏这类字面量统一归到
 * `number`；词条有 snake_case（`ordinary_device`）与 camelCase（`slotIdx`）两种写法，依次尝试。
 *
 * @desc A static device takes its inner device node (`d0` is an ordinary device, not a static one);
 *       number/string/macro literals all map to `number`; entries come in both snake_case
 *       (`ordinary_device`) and camelCase (`slotIdx`), so both are tried in order.
 * */
function operandKindKeys(operand: Operand): string[] {
    const nodeName = operand.nodeName === "StaticDevice" ? operand.device.nodeName : operand.nodeName;

    if (/Number|Macro|String|Integer|Float/.test(nodeName)) return ["number"];

    const snake = pascalToSnake(nodeName);
    const camel = snake.replace(/_(\w)/g, (_, char: string) => char.toUpperCase());

    return camel === snake ? [snake] : [snake, camel];
}

/**
 * @summary 悬停提供器基类 — 负责把卡片按设置渲染成悬停内容
 *
 * @summary Base hover provider — renders a card into hover content according to the setting
 *
 * @desc 提供器只负责**内容**：按模板组织成 {@link HoverCard}（视觉显示物 + 类型、表达式、字段），
 * 由基类按 `ic10.hoverRenderer` 选用渲染器——`svg` 出一张等宽、按列对齐、会自动换行的图，
 * `markdown` 出与其同内容的原生 Markdown。内容只写一次，两种渲染器不会再各自漂移。
 *
 * @desc A provider only produces **content**: a {@link HoverCard} organised by the template (subject
 * plus type, expression, fields). The base class picks the renderer from `ic10.hoverRenderer`: `svg`
 * yields a monospace image aligned by column and wrapped automatically, while `markdown` yields the
 * equivalent native Markdown. The content is written once, so the two renderers cannot drift apart.
 * */
abstract class HoverProvider implements IHoverProvider {
    constructor(protected settingMgr: SettingsManager) {}

    abstract canHandle(node: Statement): boolean;

    abstract provideHover(node: Statement, ctx: HoverContext): Nullable<Hover>;

    provideErrorHover(error: ErrorNode, ctx: HoverContext): Hover {
        return {
            contents: {
                kind: "markdown",
                value: `(${ctx.t("hover.operandType.error")}) ${error.message}`
            }
        };
    }

    /** @summary 按设置渲染卡片 / @summary Render the card according to the setting */
    protected render(card: HoverCard): string {
        return this.settingMgr.hoverRenderer === "markdown"
            ? renderMarkdownCard(card)
            : renderSvgCard(card, { maxWidth: this.settingMgr.hoverMaxWidth });
    }

    /**
     * @summary 渲染卡片并包成悬停结果
     *
     * @summary Render the card and wrap it as a hover result
     *
     * @desc 末尾自动补上当前文件路径（小一号的脚注）；没有路径信息时就省略这一行。
     *
     * @desc The current file path is appended as a smaller footnote; the line is omitted when no path is
     *       available.
     * */
    protected card(card: HoverCard, ctx?: HoverContext): Hover {
        const full = ctx?.path ? { ...card, footer: [{ text: ctx.path }] } : card;

        return { contents: { kind: "markdown", value: this.render(full) } };
    }

    /**
     * @summary 视觉显示物的图标：类型色外圈 + 标识符色填充 + 大写字面
     *
     * @summary The icon of the visual subject: a type-colored ring, an identifier-colored fill and an uppercase letter
     *
     * @desc 每种语义各有一组取色：外圈取该类型的**类型色**、填充取该类型**标识符的颜色**，字面是该
     * 类型的首字母缩写。主题里没有区分的（枚举、数值、指令）就退化成同色填充。
     *
     * @desc Every semantic kind has its own pairing: the ring takes that type's **type color**, the fill
     *       takes the **identifier color** of the same type, and the letter is its initial. Kinds the
     *       theme does not separate (enum, number, instruction) fall back to a solid fill.
     * */
    protected badge(kind: BadgeKind): HoverBadge {
        /** 每一行是：大写字面、填充色（标识符色）、外圈色（类型色） */
        const table: Record<BadgeKind, [abbr: string, fill: StylePath, ring?: StylePath]> = {
            device: ["D", "hover.device.identifier", "hover.device.type"],
            register: ["R", "hover.register.identifier", "hover.register.type"],
            alias: ["A", "hover.aliasDirective.identifier", "hover.aliasDirective.type"],
            constant: ["C", "hover.number.identifier", "hover.number.type"],
            label: ["L", "hover.labelDef.identifier", "hover.labelDef.type"],
            enum: ["E", "hover.constant.type"],
            number: ["N", "hover.number.type"],
            instruction: ["I", "common.text"],
            tag: ["T", "common.text"]
        };

        const [abbr, fill, ring] = table[kind];

        // 取色表是按种类索引的，路径不是字面量类型，这里按字符串取值
        const fillColor = s(fill as any) as unknown as string | undefined;
        const ringColor = ring ? (s(ring as any) as unknown as string | undefined) : undefined;

        return { abbr, fill: fillColor || "#888888", ring: ringColor || undefined };
    }

    /**
     * @summary 操作数的类型名（本地化）
     *
     * @summary The localized type name of an operand
     *
     * @desc 静态设备取**内部设备节点**的类型名（`d0` 是「平凡设备」而非「静态设备」），与操作数悬停
     * 保持一致；找不到对应词条时退回节点名。
     *
     * @desc A static device takes the type name of its **inner device node** (`d0` is an "origin device",
     *       not a "static device"), matching the operand hover; an unknown entry falls back to the node
     *       name.
     * */
    protected operandTypeName(operand: Operand, ctx: HoverContext): string {
        for (const kind of operandKindKeys(operand)) {
            const key = `hover.operandType.${kind}`;
            const localized = ctx.t(key as any);

            if (localized !== key) return localized;
        }

        // 找不到词条就退回节点名（正常情况下词条齐全）
        return operand.nodeName;
    }

    /**
     * @summary 操作数类型名的英文说法（英文界面/注解里的叫法）
     *
     * @summary The English name of an operand's type (as the English UI and annotations call it)
     *
     * @desc 行 2 的 `: 英文类型名` 用：设备是 `Origin device`、寄存器是 `General Purpose Register`、
     * 字面量是 `Number`。裸操作数没有注解类型，英文类型名就是它的种类名。
     *
     * @desc Used by the `: English type` slot of line 2: a device is an `Origin device`, a register a
     *       `General Purpose Register`, a literal a `Number`. A bare operand has no annotated type, so
     *       its kind name is the English type name.
     * */
    protected englishOperandTypeName(operand: Operand): Optional<string> {
        const table = (resources["en-us"] as any)?.hover?.operandType as Record<string, string> | undefined;

        for (const kind of operandKindKeys(operand)) if (table?.[kind]) return table[kind];

        return undefined;
    }

    /**
     * @summary 指令类悬停的卡片（关键字、`alias`、`define` 共用）
     *
     * @summary The card of an instruction-like hover (shared by a keyword, `alias` and `define`)
     *
     * @desc 行 1 是指令图标 + 关键字 + "指令"，行 2 是签名，字段只留详情——类型与签名分别由行 1 与
     * 行 2 承担，不需要重复成字段。
     *
     * @desc Line 1 is the instruction icon plus the keyword plus "Instruction", line 2 is the signature,
     *       and only the details remain as a field, since lines 1 and 2 already carry the type and the
     *       signature.
     * */
    protected instructionCard(keyword: string, ctx: HoverContext, statement?: string): HoverCard {
        const key = keyword as keyof typeof stdInstructions;
        const ins = stdInstructions[key];

        if (!ins)
            return {
                badge: this.badge("instruction"),
                display: [{ text: keyword, bold: true }]
            };

        return {
            badge: this.badge("instruction"),
            display: [{ text: keyword, bold: true }],
            type: [{ text: ctx.t("hover.instruction.type") }],
            expression: [{ text: statement ?? ins.signature }],
            fields: [
                {
                    label: ctx.t("hover.instruction.details"),
                    value: [{ text: localsInstructions[key]?.desc?.[ctx.getLocale()] ?? "---" }]
                }
            ]
        };
    }
}

abstract class HoverOperand extends HoverProvider {
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
    provideOperandHover(operand: Operand, ctx: HoverContext): Nullable<Hover> {
        if (!isInsideNode(operand.position.column, operandValueLength(operand), ctx.character)) return { contents: [] };

        if (operand.nodeName === "Error") return this.provideErrorHover(operand, ctx);

        let badge: BadgeKind;
        let color: string;
        let computed: Optional<string>; // `= 值`：计算值（宏的哈希值、十六进制字面量的十进制值）

        switch (operand.nodeName) {
            case "GeneralPurposeRegister":
            case "AddressRegister":
            case "StackPointerRegister":
            case "DynamicRegister":
                badge = "register";
                color = s("hover.register.identifier");
                break;
            case "DynamicDevice":
            case "StaticDevice":
                badge = "device";
                color = s("hover.device.identifier");
                break;
            case "Enum":
                badge = "enum";
                color = s("hover.constant.type");
                break;
            case "HashMacro":
            case "StrMacro":
                badge = "number";
                color = s("hover.number.identifier");

                if (AST.isString(operand.value))
                    computed = (AST.isHashMacro(operand) ? hashValue : strValue)(operand.value.value.replace(/"/g, "")).toString();
                break;
            case "HexNumber":
            case "BinaryNumber":
                // 十六进制/二进制字面量的十进制值
                badge = "number";
                color = s("hover.number.identifier");
                computed = (
                    operand.nodeName === "HexNumber"
                        ? Number.parseInt(operand.value.slice(1), 16)
                        : Number.parseInt(operand.value.slice(1), 2)
                ).toString();
                break;
            default:
                badge = "number";
                color = s("hover.number.identifier");
        }

        const text = operandToString(operand);
        const englishType = this.englishOperandTypeName(operand);

        // 行 1：图标 + 本地化类型名；行 2：`操作数: 英文类型名 = 值`
        return this.card(
            {
                badge: this.badge(badge),
                type: [{ text: this.operandTypeName(operand, ctx) }],
                expression: [{ text, color }],
                data: englishType ? [{ text: englishType }] : undefined,
                computed: computed ? [{ text: computed }] : undefined
            },
            ctx
        );
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
export class LabelDefHoverProvider extends HoverProvider {
    constructor(settingMgr: SettingsManager) {
        super(settingMgr);
    }

    canHandle(node: Statement): boolean {
        return AST.isLabelDef(node);
    }

    provideHover(node: Statement, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as LabelDefNode;
        if (!isInsideNode(stmt.position.column, stmt.identifier.value.length + 1, ctx.character)) return null;

        // 行 2 冒号后：标签的值是行号，符号表里记的类型名（通常是 integer）
        const symbol = ctx.symbols?.symbols[stmt.identifier.value];
        const englishType = symbol ? (symbol.typeName ?? formatBasicType(symbol.type) ?? "") : "";

        return this.card(
            {
                // 行 1：图标 + 本地化的"标签"
                badge: this.badge("label"),
                type: [{ text: ctx.t("hover.labelDef.type") }],
                syntax: [{ text: ctx.t("hover.labelDef.type") }],
                expression: [{ text: stmt.identifier.value, color: s("hover.labelDef.identifier") }],
                data: englishType ? [{ text: englishType }] : undefined,
                computed: [{ text: stmt.position.line.toString() }]
            },
            ctx
        );
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

    canHandle(node: Statement): boolean {
        return AST.isAliasDirective(node);
    }

    provideHover(node: Statement, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as AliasDirectiveNode; // AliasDirectiveNode
        if (ctx.character > stmt.registerOrDevice.position.column + operandValueLength(stmt.registerOrDevice))
            return null;

        // 第一个关键字：整条指令的说明
        if (ctx.character < stmt.position.column + 5) return this.card(this.instructionCard("alias", ctx), ctx);

        // 第三个操作数：交给操作数悬停
        if (ctx.character >= stmt.registerOrDevice.position.column || !AST.isIdentifier(stmt.identifier))
            return this.provideOperandHover(stmt.registerOrDevice, ctx);

        // 第二个标识符：别名本身。行 1 是图标 + 本地化的类型名（不重复变量名），
        // 行 2 是 `(别名) 变量名: 英文类型名 = 指向的操作数`
        const isDevice = AST.isStaticDevice(stmt.registerOrDevice) || AST.isDynamicDevice(stmt.registerOrDevice);

        const typeOfNodeMap: Map<string, TypeOfNodeEntry> = new Map(Object.entries(TypeOfNode));
        const basicType = typeOfNodeMap.get(stmt.registerOrDevice.nodeName)?.kind;

        // 行 1：本地化的类型名（平凡设备/通用寄存器…）
        const typeName = this.operandTypeName(stmt.registerOrDevice, ctx);

        // 行 2 冒号后：注解里声明的英文类型名（如 `Sensor`），没有就退回基础类型名
        const englishType =
            (ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) ??
            (basicType ? formatBasicType(basicType) : "");

        const desc = stmt.typeHint?.desc ? DescriptionSolver.solve(stmt.typeHint.desc, ctx.getLocale()) : undefined;

        return this.card(
            {
                badge: this.badge(isDevice ? "device" : "register"),
                type: [{ text: typeName }],
                syntax: [{ text: ctx.t("hover.aliasDirective.type") }],
                expression: [{ text: stmt.identifier.value }],
                data: englishType ? [{ text: englishType }] : undefined,
                computed: [{ text: operandToString(stmt.registerOrDevice) }],
                fields: desc ? [{ label: ctx.t("hover.common.description"), value: [{ text: desc }] }] : []
            },
            ctx
        );
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

    canHandle(node: Statement): boolean {
        return AST.isDefineDirective(node);
    }

    provideHover(node: Statement, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as DefineDirectiveNode;
        if (ctx.character > stmt.operand.position.column + operandValueLength(stmt.operand)) return null;

        // 悬停keyword：整条指令的说明
        if (ctx.character < stmt.position.column + 6) return this.card(this.instructionCard("define", ctx), ctx);

        // 悬停自定义标识符之外的位置：复用操作数悬停
        if (ctx.character >= stmt.operand.position.column || !AST.isIdentifier(stmt.identifier))
            return this.provideOperandHover(stmt.operand, ctx);

        // 行 2 冒号后：注解/推断出的英文类型名（如 `integer`）
        const englishType = (ctx.symbols ? formatType(stmt.identifier, ctx.symbols) : null) || stmt.operand.nodeName;

        // `= 值` 给的是**字面量本身**；HASH/STR 的数值是运行时算出来的，单独作为"值"字段放在下方
        const literal = operandToString(stmt.operand);
        let value: Optional<string>;

        switch (stmt.operand.nodeName) {
            case "HashMacro":
            case "StrMacro":
                value = AST.isString(stmt.operand.value)
                    ? (AST.isHashMacro(stmt.operand) ? hashValue : strValue)(
                          stmt.operand.value.value.replace(/"/g, "")
                      ).toString()
                    : undefined;
                break;
        }

        // 描述可能是 `@desc` 的链接写法，统一用 DescriptionSolver 解析成文本
        const description = stmt.typeHint?.desc
            ? DescriptionSolver.solve(stmt.typeHint.desc, ctx.getLocale())
            : undefined;

        return this.card(
            {
                badge: this.badge("constant"),
                // 行 1 只给图标 + 本地化的类型名，变量名放到行 2
                type: [{ text: this.operandTypeName(stmt.operand, ctx) }],
                syntax: [{ text: ctx.t("hover.defineDirective.type") }],
                expression: [{ text: stmt.identifier.value }],
                data: [{ text: englishType }],
                computed: [{ text: literal }],
                fields: [
                    ...(value ? [{ label: ctx.t("hover.common.value"), value: [{ text: value }] }] : []),
                    ...(description
                        ? [{ label: ctx.t("hover.common.description"), value: [{ text: description }] }]
                        : [])
                ]
            },
            ctx
        );
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

    canHandle(node: Statement): boolean {
        return AST.belongInstruction(node);
    }

    provideHover(node: Statement, ctx: HoverContext): Nullable<Hover> {
        const stmt = node as PureExeInstructionNode;
        const location = locateOperand(stmt, ctx.character);

        // 命中操作数的槽位与节点；光标位于操作数之后的空隙时沿用最近的前一个操作数
        const hit = location.operand ? { slot: location.slot, operand: location.operand } : location.previous;

        if (!hit) return this.provideKeywordHover(stmt.keyword, stmt, ctx);

        if (AST.isIdentifier(hit.operand))
            return this.provideIdentifierHover(
                hit.operand,
                ctx,
                hit.slot > 0 ? (stmt[`type${hit.slot}` as keyof typeof stmt] as unknown as OperandType) : undefined
            );

        return this.provideOperandHover(hit.operand, ctx);
    }

    private provideKeywordHover(keyword: string, stmt: PureExeInstructionNode, ctx: HoverContext): Nullable<Hover> {
        if (!isInsideNode(stmt.position.column, stmt.keyword.length, ctx.character)) return null;

        if (!(keyword in stdInstructions)) return null;

        // 指令悬停的表达式用签名，字段给出类型与详情
        return this.card(this.instructionCard(keyword, ctx), ctx);
    }

    private provideIdentifierHover(
        identifier: IdentifierNode,
        ctx: HoverContext,
        opType?: OperandType
    ): Nullable<Hover> {
        if (!isInsideNode(identifier.position.column, identifier.value.length, ctx.character)) return { contents: [] };

        const symbol = ctx.symbols?.symbols[identifier.value];
        if (!symbol) {
            // 是内置枚举：行 1 只给图标 + 本地化的"枚举"，行 2 是 `(槽位语义) 成员名: 取值`
            if (opType && Object.prototype.hasOwnProperty.call(EnumKeyMap, opType)) {
                const key = EnumKeyMap[opType];
                const type = ctx.types?.[key];
                const member = type && AST.isEnumAnnotation(type) ? type.values.find(v => v.name === identifier.value) : undefined;

                const desc = member?.desc ? DescriptionSolver.solve(member.desc, ctx.getLocale()) : undefined;
                const syntaxKey = `hover.operandType.${key.charAt(0).toLowerCase() + key.slice(1)}`;

                return this.card(
                    {
                        badge: this.badge("enum"),
                        type: [{ text: ctx.t("hover.operandType.enum") }],
                        syntax: [{ text: ctx.t(syntaxKey as any) }],
                        expression: [{ text: identifier.value, color: s("hover.constant.type") }],
                        // `: 枚举类型名` + `= 成员取值`
                        data: [{ text: key }],
                        computed: member?.value ? [{ text: member.value }] : undefined,
                        fields: desc ? [{ label: ctx.t("hover.common.description"), value: [{ text: desc }] }] : []
                    },
                    ctx
                );
            }

            return this.card(
                {
                    display: [{ text: identifier.value }],
                    type: [{ text: ctx.t("hover.common.identifier") }]
                },
                ctx
            );
        }

        let badge: BadgeKind = "constant";
        let color = "";
        let syntax = "";
        let kindName = ctx.t("hover.operandType.number");
        let value: Optional<string>;
        let literalValue = symbol.value;

        switch (symbol.type) {
            case BasicType.DEVICE:
            case BasicType.REGISTER:
                badge = symbol.type === BasicType.DEVICE ? "device" : "register";
                kindName =
                    symbol.type === BasicType.DEVICE
                        ? ctx.t("hover.operandType.device")
                        : ctx.t("hover.operandType.register");
                color =
                    symbol.type === BasicType.DEVICE ? s("hover.device.identifier") : s("hover.register.identifier");
                break;
            case BasicType.INTEGER:
            case BasicType.FLOAT:
                badge = "constant";
                kindName = ctx.t("hover.operandType.number");
                color = s("hover.number.identifier");
                break;
        }

        switch (symbol.category) {
            case TypeCategory.LABEL:
                badge = "label";
                syntax = ctx.t("hover.labelDef.type");
                // 标签的类型名与语法类型都是"标签"：行 1 给本地化类型名，行 2 照旧
                kindName = ctx.t("hover.labelDef.type");
                color = s("hover.labelDef.identifier");
                break;
            case TypeCategory.HASH_CALL:
            case TypeCategory.STR_CALL:
                // 宏常量仍是一个常量，与 define 悬停的图标保持一致
                badge = "constant";
                syntax = ctx.t("hover.defineDirective.type");
                kindName = ctx.t("hover.operandType.number");
                color = s("hover.number.identifier");

                const result = symbol.value ? /"(?<value>\w+?)"/.exec(symbol.value) : undefined;
                if (result && result.groups) {
                    // HASH/STR 的数值是运行时算出来的：`= ` 处仍给字面量，数值单独作为"值"字段
                    value = (
                        symbol.category === TypeCategory.HASH_CALL
                            ? hashValue(result.groups.value)
                            : strValue(result.groups.value)
                    ).toString();

                    literalValue = operandToString({
                        nodeName: symbol.category === TypeCategory.HASH_CALL ? "HashMacro" : "StrMacro",
                        value: { nodeName: "String", value: `"${result.groups.value}"` } as StringNode
                    } as Operand);
                }
                break;
        }

        if (!syntax)
            switch (symbol.type) {
                case BasicType.DEVICE:
                case BasicType.REGISTER:
                    syntax = ctx.t("hover.aliasDirective.type");
                    break;
                case BasicType.INTEGER:
                case BasicType.FLOAT:
                    syntax = ctx.t("hover.defineDirective.type");
                    break;
            }

        // 行 1：图标 + 本地化的类型名（不含变量名）；行 2：`(语法类型) 变量名: 英文类型名 = 字面量`；
        // 字段：HASH/STR 的运行时数值、@desc 描述
        const typeName = symbol.typeName ?? formatBasicType(symbol.type) ?? "";
        const description =
            symbol.desc && !AST.isError(symbol.desc)
                ? DescriptionSolver.solve(symbol.desc, ctx.getLocale())
                : undefined;

        return this.card(
            {
                badge: this.badge(badge),
                type: kindName ? [{ text: kindName }] : undefined,
                syntax: syntax ? [{ text: syntax }] : undefined,
                expression: [{ text: symbol.name, color }],
                data: typeName ? [{ text: typeName }] : undefined,
                computed: symbol.value ? [{ text: literalValue ?? "" }] : undefined,
                fields: [
                    ...(value ? [{ label: ctx.t("hover.common.value"), value: [{ text: value }] }] : []),
                    ...(description
                        ? [{ label: ctx.t("hover.common.description"), value: [{ text: description }] }]
                        : [])
                ]
            },
            ctx
        );
    }
}

// ==================== HintTag Provider ====================

/**
 * @summary 类型提示标签悬停提供器 — 为 `#:` 提示里的 `@标签` 生成说明
 *
 * @summary Type-hint tag hover provider — documents the `@tags` of a `#:` hint
 *
 * @desc 提示里的每个标签（`@type` / `@desc` / `@builtin` / `@default`）都只由词法 token 承载，
 * AST 上只有位置而没有标签名，因此按光标所在列在 token 里定位标签；命中受支持的标签时显示它的
 * 用法说明。其余位置返回 null，交回语句自己的悬停提供器。
 *
 * @desc Every tag of a hint (`@type` / `@desc` / `@builtin` / `@default`) exists only as a lexical
 * token: the AST carries positions but no tag names, so the tag under the cursor is located among the
 * tokens and, when it is a supported tag, its usage is shown. Any other position returns null so the
 * statement's own hover provider takes over.
 * */
export class HintTagHoverProvider extends HoverProvider {
    constructor(settingMgr: SettingsManager) {
        super(settingMgr);
    }

    canHandle(node: Statement): boolean {
        return (AST.isAliasDirective(node) || AST.isDefineDirective(node)) && !!node.typeHint;
    }

    provideHover(node: Statement, ctx: HoverContext): Nullable<Hover> {
        const hint = (node as AliasDirectiveNode | DefineDirectiveNode).typeHint;

        if (!hint) return null;

        const token = ctx.tokens?.find(
            t =>
                t.pos.line === ctx.line &&
                t.type === TokenType.TAG &&
                t.pos.column >= hint.position.column &&
                t.pos.column <= ctx.character &&
                ctx.character <= end(t).column
        );

        if (!token) return null;

        // 提示只认这四个标签（`#>` 块注解是另一套，不在这里）
        if (!(TypeHintTags as readonly string[]).includes(token.lexeme)) return null;

        // 标签本身作为显示物，用法说明作为描述字段
        return this.card(
            {
                badge: this.badge("tag"),
                display: [{ text: token.lexeme, bold: true }],
                fields: [
                    {
                        label: ctx.t("hover.common.description"),
                        value: [{ text: ctx.t(`hover.hintTag.${token.lexeme.slice(1)}` as any) }]
                    }
                ]
            },
            ctx
        );
    }
}
