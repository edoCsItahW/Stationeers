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
import { Connection, Range } from "vscode-languageserver/node";
import { parse as parseYaml } from "yaml";
import * as path from "path";
import * as fs from "fs";
import {
    ExecutableInstructionNode,
    PureExeInstructionNode,
    DefineDirectiveNode,
    AliasDirectiveNode,
    StatementNode,
    LabelDefNode,
    OperandNode,
    ErrorNode
} from "ic10c-node";

import { operandToString } from "../../utils";
import { DocumentCache } from "../cache";
import { Console, debug } from "common";
import { t } from "../../locals";


type OnDocumentFormattingHandlerType = Parameters<Connection["onDocumentFormatting"]>[0];

interface FormatConfig {
    indent:
        | {
              useTab: false;
              width: number;
          }
        | {
              useTab: true;
          };

    spacesBeforeTrailingComments: number;

    maxEmptyLinesToKeep: number;

    /** 标签前最小空行数：不足则补全至该值，0 为无限制 */
    minEmptyLinesBeforeLabels: number;

    alignConsecutiveStatements: boolean;

    alignTrailingComments: boolean;
}

/**
 * @summary IC10 格式化器的默认配置
 *
 * @summary Default configuration for the IC10 formatter
 *
 * @desc 定义所有格式化选项的硬编码默认值：使用 4 空格缩进、行尾注释前保留 2 空格、
 * 最多保留 1 个连续空行、标签前不额外补齐空行、启用连续语句和行尾注释对齐。
 * 这些默认值会被插件设置和项目配置文件 (.ic.json / .ic.yml / .ic.yaml) 覆盖。
 *
 * @desc Hardcoded defaults for all formatting options: 4-space indentation, 2 spaces
 * before trailing comments, at most 1 consecutive empty line, 0 minimum empty lines
 * before labels, consecutive statement and trailing comment alignment enabled.
 * These defaults can be overridden by plugin settings and project config files
 * (.ic.json / .ic.yml / .ic.yaml).
 * */
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
        if (
            (cp >= 0x1100 && cp <= 0x115f) ||
            cp === 0x2329 ||
            cp === 0x232a ||
            (cp >= 0x2e80 && cp <= 0x303e) ||
            (cp >= 0x3040 && cp <= 0x33bf) ||
            (cp >= 0x3400 && cp <= 0x4dbf) ||
            (cp >= 0x4e00 && cp <= 0xa4cf) ||
            (cp >= 0xac00 && cp <= 0xd7af) ||
            (cp >= 0xf900 && cp <= 0xfaff) ||
            (cp >= 0xfe10 && cp <= 0xfe19) ||
            (cp >= 0xfe30 && cp <= 0xfe6f) ||
            (cp >= 0xff01 && cp <= 0xff60) ||
            (cp >= 0xffe0 && cp <= 0xffe6) ||
            (cp >= 0x20000 && cp <= 0x2fffd) ||
            (cp >= 0x30000 && cp <= 0x3fffd)
        )
            width += 2;
        else width += 1;
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
    if (trimmed === "") return { index, text: line, code: "", isEmpty: true, isCommentOnly: false };

    if (trimmed.startsWith("#"))
        return { index, text: line, code: "", comment: trimmed.slice(1), isCommentOnly: true, isEmpty: false };

    let inString = false;
    for (let i = 0; i < line.length; i++) {
        if (line[i] === '"') inString = !inString;
        if (line[i] === "#" && !inString)
            return {
                index,
                text: line,
                code: line.substring(0, i).trimEnd(),
                comment: line.substring(i + 1),
                isCommentOnly: false,
                isEmpty: false
            };
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
    | {
          readonly kind: "instruction";
          readonly keyword: string;
          readonly operands: readonly string[];
          readonly comment?: string;
      }
    | { readonly kind: "raw"; readonly text: string };

// =========================================================================
// 阶段 A — buildFormatUnits
// =========================================================================

/** 判断是否为可执行指令节点 */
function isInstructionNode(stmt: StatementNode): stmt is PureExeInstructionNode {
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
        if (op.type === "Error")
            return extractErrorOperandSource(op, entries as [string, OperandNode][], idx, rawLines);

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
    } else startCol = errorOp.position.column - 1;

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
        const errorLexemes = (stmts ?? []).filter(s => s.type === "Error").map(s => (s as ErrorNode).token.lexeme);

        // 纯 ErrorNode 或无法识别 — 保留原文
        if (!primaryStmt) {
            if (stmts && stmts.length > 0) units.push({ kind: "raw", text: errorLexemes.join(" ") });
            else units.push({ kind: "raw", text: pl.text });

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
            // 行内存在错误语句（语法不完整，如非法符号/多余 token）：保留原文，
            // 避免把错误词素（可能含 NEWLINE）当作操作数格式化导致行错乱
            if (errorLexemes.length > 0) {
                units.push({ kind: "raw", text: pl.text });
                continue;
            }

            units.push({
                kind: "instruction",
                keyword: primaryStmt.keyword,
                operands: extractOperandStrings(primaryStmt, rawLines),
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
        if (currentGroup && currentEntries.length > 0)
            segments.push({ kind: "group", groupType: currentGroup, entries: currentEntries });

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
function formatGroup(entries: GroupEntry[], groupType: GroupType, cfg: FormatConfig, indent: string): string[] {
    if (entries.length === 0) return [];

    const minForAlign = groupType === "instruction" ? 3 : 2;
    const alignStmts = cfg.alignConsecutiveStatements && entries.length >= minForAlign;
    const alignComments = cfg.alignTrailingComments && entries.length > 1;

    // 计算各列最大宽度（使用视觉宽度以支持 CJK 字符对齐）
    const colWidths: number[] = [];
    for (const entry of entries) {
        updateColWidth(colWidths, 0, visualWidth(entry.keyword));
        for (let i = 0; i < entry.operandStrings.length; i++)
            updateColWidth(colWidths, i + 1, visualWidth(entry.operandStrings[i]));
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
            } else line += " ".repeat(cfg.spacesBeforeTrailingComments);

            line += "#" + entries[i].comment;
        }

        result.push(line);
    }

    return result;
}

function updateColWidth(widths: number[], col: number, val: number): void {
    if (col >= widths.length) widths[col] = val;
    else if (val > widths[col]) widths[col] = val;
}

function buildCodeLine(entry: GroupEntry, colWidths: number[], align: boolean, indent: string): string {
    let line = indent + entry.keyword;

    if (align && colWidths.length > 0 && colWidths[0] > visualWidth(entry.keyword))
        line += " ".repeat(colWidths[0] - visualWidth(entry.keyword));

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
function formatSegments(segments: readonly FormatSegment[], cfg: FormatConfig, indentStr: string): string[] {
    const output: string[] = [];
    let insideLabel = false;

    for (const seg of segments)
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
                    output.push(formatGroup([toGroupEntry(u)], u.kind, cfg, "")[0]);
                    break;
                case "instruction":
                    output.push(formatGroup([toGroupEntry(u)], u.kind, cfg, insideLabel ? indentStr : "")[0]);
                    break;
                case "raw":
                    output.push((insideLabel ? indentStr : "") + u.text);
                    break;
            }
        } else {
            // 格式化组
            const indent = seg.groupType === "instruction" && insideLabel ? indentStr : "";
            if (seg.groupType === "alias" || seg.groupType === "define") insideLabel = false;

            for (const line of formatGroup(seg.entries, seg.groupType, cfg, indent)) output.push(line);
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
        if (line === "") emptyCount++;
        else {
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
// 配置文件加载
// =========================================================================

/** 配置文件缓存条目 */
interface ConfigFileCache {
    filePath: string;
    mtimeMs: number;
    config: Partial<FormatConfig>;
}

const CONFIG_FILE_NAMES = [".ic.json", ".ic.yml", ".ic.yaml"] as const;

/** 将原始解析结果转换为 FormatConfig 格式 */
function normalizeRawConfig(raw: Record<string, unknown>): Partial<FormatConfig> {
    const result: Partial<FormatConfig> = {};

    for (const [key, value] of Object.entries(raw)) {
        switch (key) {
            case "indent":
                if (typeof value === "object" && value !== null) {
                    const v = value as Record<string, unknown>;
                    result.indent =
                        v.useTab === true
                            ? { useTab: true }
                            : { useTab: false, width: typeof v.width === "number" ? v.width : 4 };
                }
                break;
            case "spacesBeforeTrailingComments":
                if (typeof value === "number") result.spacesBeforeTrailingComments = value;
                break;
            case "maxEmptyLinesToKeep":
                if (typeof value === "number") result.maxEmptyLinesToKeep = value;
                break;
            case "minEmptyLinesBeforeLabels":
                if (typeof value === "number") result.minEmptyLinesBeforeLabels = value;
                break;
            case "alignConsecutiveStatements":
                if (typeof value === "boolean") result.alignConsecutiveStatements = value;
                break;
            case "alignTrailingComments":
                if (typeof value === "boolean") result.alignTrailingComments = value;
                break;
        }
    }

    return result;
}

// =========================================================================
// 阶段 E — 标签前空行补齐
// =========================================================================

/**
 * 阶段 E：确保每个标签前至少有 minKeep 个空行。
 * 在 compressEmptyLines 之后执行，保证补齐不被压缩。
 */
function padEmptyLinesBeforeLabels(lines: readonly string[], minKeep: number): string[] {
    if (minKeep <= 0) return [...lines];

    const result: string[] = [];
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        const isLabel = line !== "" && !line.startsWith("#") && /^\w+:/.test(line);

        if (isLabel && result.length > 0) {
            let trailingEmpty = 0;
            for (let j = result.length - 1; j >= 0 && result[j] === ""; j--) trailingEmpty++;

            const pad = minKeep - trailingEmpty;
            for (let j = 0; j < pad; j++) result.push("");
        }
        result.push(line);
    }
    return result;
}

// =========================================================================
// 阶段 F — FormattingHandler
// =========================================================================

/**
 * @summary IC10 代码格式化处理器
 *
 * @summary IC10 code formatting handler
 *
 * @desc 采用流水线架构将 IC10 源代码格式化为规范样式。处理流程：
 *   [原始文本] → parseSourceLine → [ParsedLine]
 *   → buildFormatUnits (关联 AST) → [FormatUnit]
 *   → groupToSegments (连续同类分组) → [FormatSegment]
 *   → formatSegments (列对齐 + 缩进 + 注释对齐) → [string]
 *   → compressEmptyLines (空行压缩) → [string]
 *   → padEmptyLinesBeforeLabels (标签前空行补齐) → [string]
 *   → TextEdit
 *
 * 配置加载优先级（惰性）：
 * 1. 项目根目录下的 .ic.json / .ic.yml / .ic.yaml 配置文件
 * 2. 构造时传入的 config（来自插件设置）
 * 3. DEFAULT_FORMATTING_CONFIG 硬编码默认值
 *
 * @desc Implements a pipeline architecture to format IC10 source code into canonical style.
 * Pipeline: raw text → parse → build format units → group → format (align + indent) →
 * compress empty lines → pad before labels → TextEdit.
 *
 * Config loading priority (lazy):
 * 1. .ic.json / .ic.yml / .ic.yaml in the project root directory
 * 2. Injected config (from plugin settings)
 * 3. DEFAULT_FORMATTING_CONFIG hardcoded defaults
 * */
export class FormattingHandler {
    private readonly pluginConfigProvider: () => Partial<FormatConfig>;
    private readonly projectRootDirProvider: () => string | undefined;
    private readonly onConfigError?: (message: string) => void;
    private configCache: ConfigFileCache | null = null;

    constructor(
        private readonly docCache: DocumentCache,
        options: {
            pluginConfigProvider?: () => Partial<FormatConfig>;
            projectRootDirProvider: () => string | undefined;
            onConfigError?: (message: string) => void;
        }
    ) {
        this.pluginConfigProvider = options.pluginConfigProvider ?? (() => ({}));
        this.projectRootDirProvider = options.projectRootDirProvider;
        this.onConfigError = options.onConfigError;
    }

    /** 获取基础配置（默认值合并当前插件设置） */
    private getBaseConfig(): FormatConfig {
        return { ...DEFAULT_FORMATTING_CONFIG, ...this.pluginConfigProvider() };
    }

    @debug({
        message: err => t("server.handler.error", { name: "formatting", err: (err as Error).message }),
        logger: msg => Console.error(msg, "formatting"),
        rethrow: false
    })
    handle(
        ...[{ textDocument }]: Parameters<OnDocumentFormattingHandlerType>
    ): ReturnType<OnDocumentFormattingHandlerType> {
        const cache = this.docCache.getCache(textDocument.uri);
        if (!cache || !cache.ast || !cache.ast.statements.length) return;
        // 惰性检查项目配置文件
        const cfg = this.resolveConfig(textDocument.uri);

        const source = cache.source;
        const rawLines = source.split("\n");

        // 阶段 A — 构建格式化单元
        const parsedLines = rawLines.map((line, i) => parseSourceLine(line, i));
        const stmtMap = buildStmtMap(cache.ast.statements);
        const units = buildFormatUnits(parsedLines, stmtMap, rawLines);

        // 阶段 B — 分组
        const segments = groupToSegments(units);

        // 阶段 C — 格式化
        const indentStr = cfg.indent.useTab ? "\t" : " ".repeat(cfg.indent.width);
        const formatted = formatSegments(segments, cfg, indentStr);

        // 阶段 D — 压缩空行
        const compressed = compressEmptyLines(formatted, cfg.maxEmptyLinesToKeep);

        // 阶段 E — 标签前空行补齐
        const padded = padEmptyLinesBeforeLabels(compressed, cfg.minEmptyLinesBeforeLabels);

        // 生成 TextEdit
        const newText = padded.join("\n");
        if (newText === source) return;

        const lineCount = rawLines.length;
        const lastLineLength = rawLines[lineCount - 1].length;

        return [
            {
                range: Range.create(0, 0, lineCount - 1, lastLineLength),
                newText
            }
        ];
    }

    /**
     * 惰性解析格式化配置。
     *
     * 按优先级检查项目根目录下的 .ic.json / .ic.yml / .ic.yaml 文件，
     * 利用 mtime 缓存避免重复读取。若找不到配置文件或读取失败，
     * 回退到构造时传入的插件配置。
     *
     * @param docUri - 文档 URI，当 projectRootDirProvider 返回 undefined 时用于 fallback
     */
    private resolveConfig(docUri: string): FormatConfig {
        let projectRootDir = this.projectRootDirProvider();

        if (!projectRootDir) projectRootDir = this.deriveProjectRoot(docUri);

        const baseCfg = this.getBaseConfig();

        if (!projectRootDir) return baseCfg;

        for (const fileName of CONFIG_FILE_NAMES) {
            const filePath = path.join(projectRootDir, fileName);
            let stat: fs.Stats;
            try {
                stat = fs.statSync(filePath);
            } catch {
                continue; // 文件不存在，尝试下一个
            }
            // 缓存命中：文件未修改则直接返回合并结果
            if (
                this.configCache &&
                this.configCache.filePath === filePath &&
                this.configCache.mtimeMs === stat.mtimeMs
            ) {
                return { ...baseCfg, ...this.configCache.config };
            }

            // 读取并解析配置文件
            try {
                const content = fs.readFileSync(filePath, "utf-8");
                const loaded = fileName.endsWith(".json") ? JSON.parse(content) : parseYaml(content);
                const partial = normalizeRawConfig(loaded);

                this.configCache = { filePath, mtimeMs: stat.mtimeMs, config: partial };
                return { ...baseCfg, ...partial };
            } catch (err) {
                this.notifyConfigError(t("formatting.parseError", { filename: fileName, err: (err as Error).message }));
                this.configCache = null;
                return baseCfg;
            }
        }

        // 未找到任何配置文件，使用插件配置
        this.configCache = null;
        return baseCfg;
    }

    /**
     * 从文档 URI 推导项目根目录作为 fallback。
     * 向上遍历目录树，查找包含 .ic.json / .ic.yml / .ic.yaml 的目录；
     * 若找不到，返回文档所在目录。
     */
    private deriveProjectRoot(docUri: string): string | undefined {
        const fsPath = docUri.startsWith("file://") ? docUri.slice("file://".length).replace("%3A", ":") : docUri;
        let dir = path.dirname(fsPath);

        // 向上最多查找 5 层
        for (let i = 0; i < 5; i++) {
            for (const name of CONFIG_FILE_NAMES) {
                try {
                    fs.statSync(path.join(dir, name));
                    return dir;
                } catch {
                    /* 不存在，继续 */
                }
            }
            const parent = path.dirname(dir);
            if (parent === dir) break;
            dir = parent;
        }

        // 回退到文档所在目录
        return path.dirname(fsPath);
    }

    /** 向客户端发送配置加载错误消息 */
    private notifyConfigError(message: string): void {
        this.onConfigError?.(`[IC10 Formatting] ${message}`);
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
