// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file formatting.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/28 09:48
 * @desc IC10 代码格式化器。
 *
 * 架构：流水线模式
 *   [原始文本] → parseSourceLine → [ParsedLine]
 *   → buildFormatUnits (关联 AST) → [FormatUnit]
 *   → groupToSegments (连续同类分组) → [FormatSegment]
 *   → formatSegments (列对齐 + 缩进 + 注释对齐) → [string]
 *   → compressEmptyLines (空行压缩) → [string]
 *   → TextEdit
 *
 * 支持的格式化规则：
 * - 变量格式: 连续 alias/define 按列对齐
 * - 注释格式: 行尾注释同组内对齐，与代码间隔可配置
 * - 缩进格式: 标签定义下的指令统一缩进
 * - 最大空行: 多余连续空行压缩到可配置的最大值
 * - 激进指令格式: 同标签作用域内指令操作数按列对齐
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Connection, Range} from "vscode-languageserver/node";
import {
    AliasDirectiveNode,
    DefineDirectiveNode,
    ErrorNode,
    ExecutableInstructionNode,
    InstructionNode,
    LabelDefNode,
    StatementNode,
    OperandNode
} from "ic10-node-api";

import { operandToString } from "../../utils";
import {DocumentCache} from "../cache";


type OnDocumentFormattingHandlerType = Parameters<Connection["onDocumentFormatting"]>[0];

interface FormatConfig {
    indent: {
        useTab: false;
        width: number;
    } | {
        useTab: true;
    };

    spacesBeforeTrailingComments: number;

    maxEmptyLinesToKeep: number;

    /** 标签前最小空行数：不足则补全至该值，0 为无限制 */
    minEmptyLinesBeforeLabels: number;

    alignConsecutiveStatements: boolean;

    alignTrailingComments: boolean;
}

export const DEFAULT_FORMATTING_CONFIG: FormatConfig = {
    indent: {
        useTab: false,
        width: 4
    },
    spacesBeforeTrailingComments: 2,
    maxEmptyLinesToKeep: 1,
    minEmptyLinesBeforeLabels: 0,
    alignConsecutiveStatements: true,
    alignTrailingComments: true
};

// =========================================================================
// 通用工具函数
// =========================================================================

/**
 * 计算字符串在等宽字体下的视觉宽度。
 * CJK 字符、全角标点等占2列，ASCII 字符占1列。
 */
function visualWidth(str: string): number {
    let width = 0;
    for (const ch of str) {
        const cp = ch.codePointAt(0)!;
        if ((cp >= 0x1100 && cp <= 0x115F) ||
            cp === 0x2329 || cp === 0x232A ||
            (cp >= 0x2E80 && cp <= 0x303E) ||
            (cp >= 0x3040 && cp <= 0x33BF) ||
            (cp >= 0x3400 && cp <= 0x4DBF) ||
            (cp >= 0x4E00 && cp <= 0xA4CF) ||
            (cp >= 0xAC00 && cp <= 0xD7AF) ||
            (cp >= 0xF900 && cp <= 0xFAFF) ||
            (cp >= 0xFE10 && cp <= 0xFE19) ||
            (cp >= 0xFE30 && cp <= 0xFE6F) ||
            (cp >= 0xFF01 && cp <= 0xFF60) ||
            (cp >= 0xFFE0 && cp <= 0xFFE6) ||
            (cp >= 0x20000 && cp <= 0x2FFFD) ||
            (cp >= 0x30000 && cp <= 0x3FFFD)) {
            width += 2;
        } else {
            width += 1;
        }
    }
    return width;
}

// =========================================================================
// 阶段输入类型
// =========================================================================

/** 解析后的源代码行 */
interface ParsedLine {
    index: number;
    text: string;
    code: string;
    comment?: string;
    isCommentOnly: boolean;
    isEmpty: boolean;
}

function parseSourceLine(line: string, index: number): ParsedLine {
    const trimmed = line.trimStart();
    if (trimmed === "") {
        return { index, text: line, code: "", isEmpty: true, isCommentOnly: false };
    }
    if (trimmed.startsWith("#")) {
        return { index, text: line, code: "", comment: trimmed.slice(1), isCommentOnly: true, isEmpty: false };
    }
    let inString = false;
    for (let i = 0; i < line.length; i++) {
        if (line[i] === '"') inString = !inString;
        if (line[i] === '#' && !inString) {
            return {
                index, text: line,
                code: line.substring(0, i).trimEnd(),
                comment: line.substring(i + 1),
                isCommentOnly: false, isEmpty: false
            };
        }
    }
    return { index, text: line, code: line.trimEnd(), isEmpty: false, isCommentOnly: false };
}

// =========================================================================
// 阶段 A — 中间数据结构
// =========================================================================

/**
 * 格式化单元：源代码行与 AST 节点关联后的结果。
 * 每个单元代表一行，由 buildFormatUnits 生成。
 */
type FormatUnit =
    | { readonly kind: "empty" }
    | { readonly kind: "comment"; readonly content: string }
    | { readonly kind: "label"; readonly name: string; readonly suffix?: string }
    | { readonly kind: "alias"; readonly name: string; readonly target: string; readonly comment?: string }
    | { readonly kind: "define"; readonly name: string; readonly value: string; readonly comment?: string }
    | { readonly kind: "instruction"; readonly keyword: string; readonly operands: readonly string[]; readonly comment?: string }
    | { readonly kind: "raw"; readonly text: string };

// =========================================================================
// 阶段 A — buildFormatUnits
// =========================================================================

/** 判断是否为可执行指令节点 */
function isInstructionNode(stmt: StatementNode): stmt is ExecutableInstructionNode {
    return stmt.type.endsWith("Instruction");
}

/**
 * 从指令节点中提取操作数字符串列表（按 operand1..operandN 顺序）。
 * Error 类型操作数的 token.lexeme 为 NEWLINE，需改为从源码行提取实际文本。
 */
function extractOperandStrings(instr: ExecutableInstructionNode, rawLines: string[]): string[] {
    const entries = Object.entries(instr)
        .filter(([k]) => /^operand\d+$/.test(k))
        .sort(([a], [b]) => parseInt(a.slice(7), 10) - parseInt(b.slice(7), 10));

    return entries.map(([, v], idx) => {
        const op = v as OperandNode;
        if (op.type === "Error") {
            return extractErrorOperandSource(op, entries as [string, OperandNode][], idx, rawLines);
        }
        return operandToString(op);
    });
}

/** 从源码行中提取 Error 操作数的实际文本 */
function extractErrorOperandSource(
    errorOp: OperandNode,
    allEntries: [string, OperandNode][],
    errorIdx: number,
    rawLines: string[]
): string {
    const lineNum = errorOp.position.line - 1;
    const line = rawLines[lineNum];
    if (!line) return operandToString(errorOp);

    let startCol: number;
    if (errorIdx > 0) {
        const prevOp = allEntries[errorIdx - 1][1];
        const prevEnd = prevOp.position.column + operandToString(prevOp).length;
        startCol = prevEnd - 1;
    } else {
        startCol = errorOp.position.column - 1;
    }

    const text = line.substring(startCol);
    const hashIdx = text.indexOf("#");
    const content = hashIdx >= 0 ? text.substring(0, hashIdx) : text;
    return content.trim();
}

/**
 * 阶段 A：将源代码行与 AST 关联，生成 FormatUnit 数组。
 *
 * 遍历 parsedLines，利用 stmtMap 判断每行的语句类型，
 * 将 AST 节点信息提取为结构化的 FormatUnit。
 */
function buildFormatUnits(
    parsedLines: ParsedLine[],
    stmtMap: Map<number, StatementNode[]>,
    rawLines: string[]
): FormatUnit[] {
    const units: FormatUnit[] = [];

    for (let i = 0; i < parsedLines.length; i++) {
        const pl = parsedLines[i];
        const stmts = stmtMap.get(i + 1);

        // 空行 / 独立注释行 — 直接映射
        if (pl.isEmpty) {
            units.push({ kind: "empty" });
            continue;
        }
        if (pl.isCommentOnly) {
            units.push({ kind: "comment", content: pl.comment ?? "" });
            continue;
        }

        // 同行多节点：找出主语句（非 Error）和 ErrorNode 词素
        const primaryStmt = stmts?.find(s => s.type !== "Error");
        const errorLexemes = (stmts ?? [])
            .filter(s => s.type === "Error")
            .map(s => (s as ErrorNode).token.lexeme);

        // 纯 ErrorNode 或无法识别 — 保留原文
        if (!primaryStmt) {
            if (stmts && stmts.length > 0) {
                units.push({ kind: "raw", text: errorLexemes.join(" ") });
            } else {
                units.push({ kind: "raw", text: pl.text });
            }
            continue;
        }

        // LabelDef
        if (primaryStmt.type === "LabelDef") {
            const suffix = errorLexemes.length > 0 ? errorLexemes.join(" ") : undefined;
            units.push({ kind: "label", name: (primaryStmt as LabelDefNode).identifier.value, suffix });
            continue;
        }

        // AliasDirective
        if (primaryStmt.type === "AliasDirective") {
            const alias = primaryStmt as AliasDirectiveNode;
            units.push({
                kind: "alias",
                name: alias.identifier.value,
                target: operandToString(alias.registerOrDevice),
                comment: pl.comment
            });
            continue;
        }

        // DefineDirective
        if (primaryStmt.type === "DefineDirective") {
            const define = primaryStmt as DefineDirectiveNode;
            units.push({
                kind: "define",
                name: define.identifier.value,
                value: operandToString(define.number),
                comment: pl.comment
            });
            continue;
        }

        // 可执行指令
        if (isInstructionNode(primaryStmt)) {
            const instr = primaryStmt as InstructionNode;
            const execInstr = primaryStmt as ExecutableInstructionNode;
            units.push({
                kind: "instruction",
                keyword: instr.keyword,
                operands: [...extractOperandStrings(execInstr, rawLines), ...errorLexemes],
                comment: pl.comment
            });
            continue;
        }

        // 兜底
        units.push({ kind: "raw", text: pl.text });
    }

    return units;
}

// =========================================================================
// 阶段 B — groupToSegments
// =========================================================================

type GroupType = "alias" | "define" | "instruction";

/** 格式化组中的一行条目 */
interface GroupEntry {
    keyword: string;
    operandStrings: string[];
    comment?: string;
}

/**
 * 格式化段：要么是一个可对齐的组，要么是独立单元。
 */
type FormatSegment =
    | { readonly kind: "group"; readonly groupType: GroupType; readonly entries: GroupEntry[] }
    | { readonly kind: "unit"; readonly unit: FormatUnit };

/**
 * 将 FormatUnit 转为 GroupEntry（用于格式化和分组）。
 */
function toGroupEntry(u: FormatUnit): GroupEntry {
    if (u.kind === "alias") return { keyword: "alias", operandStrings: [u.name, u.target], comment: u.comment };
    if (u.kind === "define") return { keyword: "define", operandStrings: [u.name, u.value], comment: u.comment };
    if (u.kind === "instruction") return { keyword: u.keyword, operandStrings: [...u.operands], comment: u.comment };
    throw new Error("Not a groupable unit");
}

/**
 * 阶段 B：将连续的同类 FormatUnit 合并为格式化组，其余保持为独立单元。
 *
 * 空行（empty unit）会打断当前分组，并作为独立 unit 段输出。
 */
function groupToSegments(units: readonly FormatUnit[]): FormatSegment[] {
    const segments: FormatSegment[] = [];
    let currentGroup: GroupType | null = null;
    let currentEntries: GroupEntry[] = [];

    const groupKind = (u: FormatUnit): GroupType | null => {
        if (u.kind === "alias") return "alias";
        if (u.kind === "define") return "define";
        if (u.kind === "instruction") return "instruction";
        return null;
    };

    const flushGroup = () => {
        if (currentGroup && currentEntries.length > 0) {
            segments.push({ kind: "group", groupType: currentGroup, entries: currentEntries });
        }
        currentGroup = null;
        currentEntries = [];
    };

    for (const unit of units) {
        const gKind = groupKind(unit);
        if (gKind !== null) {
            if (currentGroup !== gKind) {
                flushGroup();
                currentGroup = gKind;
            }
            currentEntries.push(toGroupEntry(unit));
        } else {
            flushGroup();
            segments.push({ kind: "unit", unit });
        }
    }

    flushGroup();
    return segments;
}

// =========================================================================
// 阶段 C — formatSegments
// =========================================================================

/**
 * 格式化组内的列对齐逻辑。
 * 提取自原 FormatGroup 类，保持与之前一致的格式化行为。
 */
function formatGroup(
    entries: GroupEntry[],
    groupType: GroupType,
    cfg: FormatConfig,
    indent: string
): string[] {
    if (entries.length === 0) return [];

    const minForAlign = groupType === "instruction" ? 3 : 2;
    const alignStmts = cfg.alignConsecutiveStatements && entries.length >= minForAlign;
    const alignComments = cfg.alignTrailingComments && entries.length > 1;

    // 计算各列最大宽度（使用视觉宽度以支持 CJK 字符对齐）
    const colWidths: number[] = [];
    for (const entry of entries) {
        updateColWidth(colWidths, 0, visualWidth(entry.keyword));
        for (let i = 0; i < entry.operandStrings.length; i++) {
            updateColWidth(colWidths, i + 1, visualWidth(entry.operandStrings[i]));
        }
    }

    // 生成每行代码（不含注释），跟踪最大视觉宽度
    const codeLines: string[] = [];
    let maxVisualLen = 0;

    for (const entry of entries) {
        const code = buildCodeLine(entry, colWidths, alignStmts, indent);
        codeLines.push(code);
        const vw = visualWidth(code);
        if (vw > maxVisualLen) maxVisualLen = vw;
    }

    // 生成最终行（含行尾注释对齐）
    const result: string[] = [];
    const commentCol = maxVisualLen + cfg.spacesBeforeTrailingComments;

    for (let i = 0; i < entries.length; i++) {
        let line = codeLines[i];
        if (entries[i].comment !== undefined) {
            if (alignComments) {
                const padding = commentCol - visualWidth(line);
                line += padding > 0 ? " ".repeat(padding) : " ".repeat(cfg.spacesBeforeTrailingComments);
            } else {
                line += " ".repeat(cfg.spacesBeforeTrailingComments);
            }
            line += "#" + entries[i].comment;
        }
        result.push(line);
    }

    return result;
}

function updateColWidth(widths: number[], col: number, val: number): void {
    if (col >= widths.length) {
        widths[col] = val;
    } else if (val > widths[col]) {
        widths[col] = val;
    }
}

function buildCodeLine(
    entry: GroupEntry,
    colWidths: number[],
    align: boolean,
    indent: string
): string {
    let line = indent + entry.keyword;

    if (align && colWidths.length > 0 && colWidths[0] > visualWidth(entry.keyword)) {
        line += " ".repeat(colWidths[0] - visualWidth(entry.keyword));
    }

    for (let i = 0; i < entry.operandStrings.length; i++) {
        line += " " + entry.operandStrings[i];
        if (align && i < entry.operandStrings.length - 1 && i + 1 < colWidths.length) {
            const pad = colWidths[i + 1] - visualWidth(entry.operandStrings[i]);
            if (pad > 0) line += " ".repeat(pad);
        }
    }

    return line;
}

/**
 * 阶段 C：将格式化段转换为输出行数组。
 *
 * 维护标签作用域状态（insideLabel），指令组在标签作用域内自动缩进。
 * alias/define 组会退出标签作用域（不缩进）。
 */
function formatSegments(
    segments: readonly FormatSegment[],
    cfg: FormatConfig,
    indentStr: string
): string[] {
    const output: string[] = [];
    let insideLabel = false;

    for (const seg of segments) {
        if (seg.kind === "unit") {
            const u = seg.unit;
            switch (u.kind) {
                case "empty":
                    output.push("");
                    break;
                case "comment":
                    output.push((insideLabel ? indentStr : "") + "#" + u.content);
                    break;
                case "label": {
                    // 标签前最小空行补齐
                    if (cfg.minEmptyLinesBeforeLabels > 0 && output.length > 0) {
                        let trailingEmpty = 0;
                        for (let j = output.length - 1; j >= 0 && output[j] === ""; j--) {
                            trailingEmpty++;
                        }
                        const pad = cfg.minEmptyLinesBeforeLabels - trailingEmpty;
                        for (let j = 0; j < pad; j++) {
                            output.push("");
                        }
                    }
                    let line = u.name + ":";
                    if (u.suffix) line += " " + u.suffix;
                    output.push(line);
                    insideLabel = true;
                    break;
                }
                case "alias":
                case "define":
                    // alias/define 退出标签作用域（不缩进）
                    insideLabel = false;
                    output.push(
                        formatGroup([toGroupEntry(u)], u.kind, cfg, "")[0]
                    );
                    break;
                case "instruction":
                    output.push(
                        formatGroup([toGroupEntry(u)], u.kind, cfg, insideLabel ? indentStr : "")[0]
                    );
                    break;
                case "raw":
                    output.push((insideLabel ? indentStr : "") + u.text);
                    break;
            }
        } else {
            // 格式化组
            const indent = seg.groupType === "instruction" && insideLabel ? indentStr : "";
            if (seg.groupType === "alias" || seg.groupType === "define") {
                insideLabel = false;
            }
            for (const line of formatGroup(seg.entries, seg.groupType, cfg, indent)) {
                output.push(line);
            }
        }
    }

    return output;
}

// =========================================================================
// 阶段 D — compressEmptyLines
// =========================================================================

/**
 * 阶段 D：压缩连续空行，保留最多 maxKeep 个。
 * 文件末尾的尾随空行也会被去除。
 */
function compressEmptyLines(lines: readonly string[], maxKeep: number): string[] {
    if (maxKeep < 0) return [...lines];

    const result: string[] = [];
    let emptyCount = 0;

    for (const line of lines) {
        if (line === "") {
            emptyCount++;
        } else {
            // 输出缓冲的连续空行（受 maxKeep 限制）
            const keep = Math.min(emptyCount, maxKeep);
            for (let i = 0; i < keep; i++) result.push("");
            emptyCount = 0;
            result.push(line);
        }
    }

    // 文件末尾：如果有内容，保留有限空行；如果整个文件为空，保留
    if (result.length > 0) {
        const keep = Math.min(emptyCount, maxKeep);
        for (let i = 0; i < keep; i++) result.push("");
    }

    return result;
}

// =========================================================================
// 阶段 E — FormattingHandler
// =========================================================================

export class FormattingHandler {
    private readonly cfg: FormatConfig;

    constructor(
        private readonly docCache: DocumentCache,
        config: Partial<FormatConfig> = DEFAULT_FORMATTING_CONFIG
    ) {
        this.cfg = { ...DEFAULT_FORMATTING_CONFIG, ...config };
    }

    handle(...[{textDocument}]: Parameters<OnDocumentFormattingHandlerType>): ReturnType<OnDocumentFormattingHandlerType> {
        const cache = this.docCache.getCache(textDocument.uri);
        if (!cache || !cache.ast || !cache.ast.statements.length) return;

        const source = cache.source;
        const rawLines = source.split("\n");

        // 阶段 A — 构建格式化单元
        const parsedLines = rawLines.map((line, i) => parseSourceLine(line, i));
        const stmtMap = buildStmtMap(cache.ast.statements);
        const units = buildFormatUnits(parsedLines, stmtMap, rawLines);

        // 阶段 B — 分组
        const segments = groupToSegments(units);

        // 阶段 C — 格式化
        const indentStr = this.cfg.indent.useTab ? "\t" : " ".repeat(this.cfg.indent.width);
        const formatted = formatSegments(segments, this.cfg, indentStr);

        // 阶段 D — 压缩空行
        const compressed = compressEmptyLines(formatted, this.cfg.maxEmptyLinesToKeep);

        // 生成 TextEdit
        const newText = compressed.join("\n");
        if (newText === source) return;

        const lineCount = rawLines.length;
        const lastLineLength = rawLines[lineCount - 1].length;

        return [{
            range: Range.create(0, 0, lineCount - 1, lastLineLength),
            newText
        }];
    }
}

/** 建立 1-based 行号 → StatementNode[] 的映射 */
function buildStmtMap(statements: readonly StatementNode[]): Map<number, StatementNode[]> {
    const map = new Map<number, StatementNode[]>();
    for (const stmt of statements) {
        const line = stmt.position.line;
        if (!map.has(line)) map.set(line, []);
        map.get(line)!.push(stmt);
    }
    return map;
}
