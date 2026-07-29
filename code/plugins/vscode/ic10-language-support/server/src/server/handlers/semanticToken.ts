// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semanticToken.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/23 12:37
 * @desc IC10 语义令牌处理器
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Languages } from "vscode-languageserver";
import {
    AliasDirectiveNode,
    BasicType,
    ConstantNode,
    DefineDirectiveNode,
    ErrorNode,
    ExecutableInstructionNode,
    HashCallNode,
    IdentifierNode,
    LabelDefNode,
    OperandNode,
    OperandType,
    Position,
    Program,
    StatementNode,
    StrCallNode,
    SymbolMap,
    TypeCategory,
    TypeOfNode
} from "ic10-node-api";

import { Console, debug, upperBound } from "common";
import { DocumentCache } from "../cache";
import { t } from "../../locals/locale";

type OnHandlerType = Parameters<Languages["semanticTokens"]["on"]>[0];
type OnRangeHandlerType = Parameters<Languages["semanticTokens"]["onRange"]>[0];

/**
 * 语义令牌类型枚举，顺序对应 LSP legend 中的 tokenTypes 顺序。
 * 使用 IC10 自定义类型避免与 VS Code 标准类型混淆，并可通过 package.json 中的
 * semanticTokenColors 精确控制颜色。
 *
 * @see https://code.visualstudio.com/api/language-extensions/semantic-highlight-guide
 */
export enum TokenLegend {
    /** 指令关键字（move, add, sub 等）和预处理指令（alias, define） */
    Keyword = 0,
    /** 寄存器（r0, r1, ra, sp 等） */
    Register,
    RegisterIdentifier,
    /** 函数/宏调用（hash, str） */
    Macro,
    /** 设备引用（d0, d1, db 等） */
    Device,
    DeviceIdentifier,
    /** 数字字面量（整数、浮点、十六进制、二进制） */
    Number,
    NumberIdentifier,
    /** 字符串字面量 */
    String,
    Constant,
    /** 注释 */
    Comment,
    /** 注解/类型提示（#: @type） */
    Decorator,
    /** 标识符（标签名、别名、define 常量名） */
    Label,
    LabelIdentifier,
    Unknown
}

/**
 * 语义令牌修饰符枚举，顺序对应 LSP legend 中的 tokenModifiers 顺序。
 */
export enum TokenModifier {
    /** 声明（alias、define 引入的新符号） */
    Declaration = 0,
    /** 只读（数学常量：nan, pi, tau 等） */
    Readonly
}

export const TOKEN_TYPES = Object.keys(TokenLegend).filter(k => isNaN(Number(k)));

export const TOKEN_MODIFIERS = Object.keys(TokenModifier).filter(k => isNaN(Number(k)));

interface SemanticToken {
    /**
     * @summary 行偏移量
     * @desc 相对于前一个行号的偏移量
     * @remarks 必须大于等于0
     * */
    line: number;

    /**
     * @summary 起始位置偏移量
     * @desc 相对于该行起始位置的字符偏移量
     * @remarks 同一行必须 ≥ 0，新行可 ≥ 0
     * */
    start: number;

    /**
     * @summary 令牌长度
     * @desc 令牌的字符长度
     * */
    length: number;

    /**
     * @summary 令牌类型
     * @desc 令牌类型在 legend.types 中的索引
     * @remarks 从0开始
     * */
    type: TokenLegend;

    /**
     * @summary 修饰符位
     * @desc 修饰符的位掩码
     * @remarks 每个 bit 对应 legend.modifiers 中的一个修饰符,并且无论是语法修饰符还是语义修饰符都可以
     * @example 0b01
     * */
    modifier: TokenModifier;
}

interface HandlerContext {
    prev: Position;
    table: SymbolMap;
}

export class SemanticTokenHandler {
    constructor(private readonly docCache: DocumentCache) {}

    @debug({
        message: err => t("server.handler.error", { name: "semantic token", err: (err as Error).message }),
        logger: msg => Console.error(msg, "semantic token"),
        rethrow: false
    })
    handle(...[params]: Parameters<OnHandlerType>): ReturnType<OnHandlerType> {
        try {
            const cache = this.docCache.getCache(params.textDocument.uri);

            if (!cache || !cache.ast || !cache.symbols) return { data: [] };

            const context: HandlerContext = {
                prev: { line: 1, column: 1 },
                table: cache.symbols
            };

            return { data: this.visitProgram(cache.ast, context) };
        } catch (error) {}
    }

    @debug({
        message: err => t("server.handler.error", { name: "semantic token range", err: (err as Error).message }),
        logger: msg => Console.error(msg, "semantic token range"),
        rethrow: false
    })
    handleRange(
        ...[
            {
                textDocument,
                range: { start, end }
            }
        ]: Parameters<OnRangeHandlerType>
    ): ReturnType<OnRangeHandlerType> {
        try {
            const cache = this.docCache.getCache(textDocument.uri);

            if (!cache || !cache.ast || !cache.symbols) return { data: [] };

            const compPos = (a: Position, b: Position): number => a.line - b.line || a.column - b.column;

            const lastLE = (target: Position) =>
                upperBound(cache.ast!.statements, stmt => compPos(stmt.position, target) <= 0);

            let startIdx = lastLE({ line: start.line + 1, column: start.character + 1 });
            let endIdx = lastLE({ line: end.line + 1, column: end.character + 1 });

            if (startIdx === -1) startIdx = 0;
            if (endIdx === -1) endIdx = 0;

            if (startIdx > endIdx) {
                startIdx = 0;
                endIdx = 0;
            }

            const rangs = cache.ast.statements.slice(startIdx, endIdx + 1);

            const context: HandlerContext = {
                prev: { line: 1, column: 1 },
                table: cache.symbols
            };

            return {
                data: rangs.flatMap(n => {
                    const tks = this.visitStatement(n, context);

                    return tks.flatMap(t => [t.line, t.start, t.length, t.type, t.modifier]);
                })
            };
        } catch (error) {
            console.error("[IC10 LSP] - Error: (range)", error);
        }
    }

    private visitProgram(node: Program, context: HandlerContext): number[] {
        return node.statements.flatMap(n => {
            const tks = this.visitStatement(n, context);

            return tks.flatMap(t => [t.line, t.start, t.length, t.type, t.modifier]);
        });
    }

    private visitStatement(node: StatementNode, context: HandlerContext): SemanticToken[] {
        if (this.isInstructionType(node)) return this.visitInstruction(node, context);

        const mthName = `visit${node.type}` as const;

        if (mthName in this) return (this as any)[mthName](node, context);

        Console.warning(`Unknown statement type: ${node.type}`, "SemanticToken");
        return [];
    }

    private visitInstruction(
        node: Exclude<ExecutableInstructionNode, ErrorNode>,
        context: HandlerContext
    ): SemanticToken[] {
        const result: SemanticToken[] = [];

        const gap = this.getGap(context, node.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: node.keyword.length,
            type: TokenLegend.Keyword,
            modifier: 0
        });

        Object.entries(node).forEach(([key, value]) => {
            if (key.startsWith("operand")) {
                const typeKey = key.replace("operand", "type");

                result.push(...this.handleOperand(value, context, (node as any)[typeKey]));
            }
        });

        return result;
    }

    private visitLabelDef(node: LabelDefNode, context: HandlerContext): SemanticToken[] {
        const gap = this.getGap(context, node.position);

        return [
            {
                line: gap.line,
                start: gap.column,
                length: node.identifier.value.length,
                type: TokenLegend.Label,
                modifier: 0
            }
        ];
    }

    private visitAliasDirective(node: AliasDirectiveNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, node.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: 5,
            type: TokenLegend.Keyword,
            modifier: 0
        });

        if (node.identifier.value) {
            gap = this.getGap(context, node.identifier.position);

            result.push({
                line: gap.line,
                start: gap.column,
                length: node.identifier.value.length,
                type:
                    node.registerOrDevice.type === "Register"
                        ? TokenLegend.RegisterIdentifier
                        : node.registerOrDevice.type == "Device"
                          ? TokenLegend.DeviceIdentifier
                          : TokenLegend.Unknown,
                modifier: this.modifierBits(TokenModifier.Declaration)
            });
        }

        result.push(...this.handleOperand(node.registerOrDevice, context));

        return result;
    }

    private visitDefineDirective(node: DefineDirectiveNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, node.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: 5,
            type: TokenLegend.Keyword,
            modifier: 0
        });

        if (node.identifier.value) {
            gap = this.getGap(context, node.identifier.position);

            result.push({
                line: gap.line,
                start: gap.column,
                length: node.identifier.value.length,
                type: TokenLegend.NumberIdentifier,
                modifier: this.modifierBits(TokenModifier.Declaration)
            });
        }

        result.push(...this.handleOperand(node.number, context));

        return result;
    }

    private visitError(node: ErrorNode, context: HandlerContext): SemanticToken[] {
        return this.handleError(node, context);
    }

    private handleError(node: ErrorNode, context: HandlerContext): SemanticToken[] {
        const gap = this.getGap(context, node.position);

        return [
            {
                line: gap.line,
                start: gap.column,
                length: node.token.lexeme.length,
                type: TokenLegend.Unknown,
                modifier: 0
            }
        ];
    }

    private handleOperand(node: OperandNode, context: HandlerContext, operandType?: OperandType): SemanticToken[] {
        // Identifier 需要 operandType 参数，优先处理
        if (node.type === "Identifier") return [this.handleIdentifier(node, context, operandType)];

        // 派发到专用 handler（HashCall / StrCall / Constant 等），
        // 避免被下面泛化的 "value" in node 检查误匹配
        const mthName = `handle${node.type}` as const;

        if (mthName in this) {
            const result = (this as any)[mthName](node, context);

            return Array.isArray(result) ? result : [result];
        }

        if ("value" in node) {
            const gap = this.getGap(context, node.position);

            const type = TypeOfNode[node.type];

            return [
                {
                    line: gap.line,
                    start: gap.column,
                    length: typeof node.value === "string" ? node.value.length : node.value.toString().length,
                    type: this.toLegend(type.kind, type.category),
                    modifier: 0
                }
            ];
        }

        Console.warning(`Unknown operand type: ${node.type}`, "SemanticToken");
        return [];
    }

    private handleConstant(node: ConstantNode, context: HandlerContext): SemanticToken {
        const gap = this.getGap(context, node.position);

        return {
            line: gap.line,
            start: gap.column,
            length: node.keyword.length,
            type: TokenLegend.Constant,
            modifier: 0
        };
    }

    private handleIdentifier(node: IdentifierNode, context: HandlerContext, operandType?: OperandType): SemanticToken {
        const gap = this.getGap(context, node.position);

        const symbol = context.table[node.value];

        const type = symbol
            ? this.toLegend(symbol.type, symbol.category, true)
            : this.operandTypeToLegend(operandType!);

        return {
            line: gap.line,
            start: gap.column,
            length: node.value.length,
            type,
            modifier: 0
        };
    }

    private handleStrCall(node: StrCallNode, context: HandlerContext): SemanticToken[] {
        const result: SemanticToken[] = [];

        let gap = this.getGap(context, node.position);

        const type = TypeOfNode[node.type];

        result.push({
            line: gap.line,
            start: gap.column,
            length: 3,
            type: this.toLegend(type.kind, type.category),
            modifier: 0
        });

        gap = this.getGap(context, node.value.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: node.value.value.length,
            type: TokenLegend.String,
            modifier: 0
        });

        return result;
    }

    private handleHashCall(node: HashCallNode, context: HandlerContext): SemanticToken[] {
        if (!node.value || !node.value.position || node.value.value == null) {
            Console.warning("handleHashCall: missing value data", "SemanticToken");
            return [];
        }

        const result: SemanticToken[] = [];

        let gap = this.getGap(context, node.position);

        const type = TypeOfNode[node.type];

        result.push({
            line: gap.line,
            start: gap.column,
            length: 4,
            type: this.toLegend(type.kind, type.category),
            modifier: 0
        });

        gap = this.getGap(context, node.value.position);

        result.push({
            line: gap.line,
            start: gap.column,
            length: node.value.value.length,
            type: TokenLegend.String,
            modifier: 0
        });

        return result;
    }

    private modifierBits(...modifiers: TokenModifier[]): number {
        let bits = 0;
        for (const m of modifiers) bits |= 1 << m;

        return bits;
    }

    private toLegend(type: BasicType, category: TypeCategory, isIdentifier?: boolean): TokenLegend {
        switch (category) {
            case TypeCategory.LABEL:
                return isIdentifier ? TokenLegend.LabelIdentifier : TokenLegend.Label;
            case TypeCategory.NUMBER:
            case TypeCategory.CONSTANT:
                return isIdentifier ? TokenLegend.NumberIdentifier : TokenLegend.Number;
            case TypeCategory.HASH_CALL:
            case TypeCategory.STR_CALL:
                return TokenLegend.Macro;
        }

        switch (type) {
            case BasicType.DEVICE:
                return isIdentifier ? TokenLegend.DeviceIdentifier : TokenLegend.Device;
            case BasicType.REGISTER:
                return isIdentifier ? TokenLegend.RegisterIdentifier : TokenLegend.Register;
            case BasicType.STRING:
                return TokenLegend.String;
        }

        return TokenLegend.Unknown;
    }

    private operandTypeToLegend(operandType: OperandType): TokenLegend {
        switch (operandType) {
            case OperandType.LOGIC_TYPE:
            case OperandType.LOGIC_SLOT:
            case OperandType.REAGENT_MODE:
            case OperandType.BATCH_MODE:
            case OperandType.SLOT_IDX:
                return TokenLegend.Constant;
            case OperandType.JUMP_TARGET:
                return TokenLegend.Label;
            default:
                return TokenLegend.Unknown;
        }
    }

    private getGap(context: HandlerContext, pos: Position): Position {
        const lineGap = pos.line - context.prev.line;

        const result = { line: lineGap, column: lineGap <= 0 ? pos.column - context.prev.column : pos.column - 1 };

        context.prev = pos;

        return result;
    }

    private isInstructionType(node: StatementNode): node is Exclude<ExecutableInstructionNode, ErrorNode> {
        return node.type.endsWith("Instruction");
    }
}
