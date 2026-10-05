/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file hoverCard.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/02
 * @desc 悬停卡片：内容模型 + 两种渲染器（SVG 图片 / 原生 Markdown）
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { s } from "../style";

/**
 * @summary 卡片里的一段带样式文本
 *
 * @summary One styled run of text inside a card
 *
 * @desc 颜色/粗体/斜体/下划线都可选，缺省即继承卡片默认样式。
 *
 * @desc Color, bold, italic and underline are all optional; whatever is omitted inherits the card
 *       defaults.
 * */
export interface TextRun {
    text: string;
    color?: string;
    bold?: boolean;
    italic?: boolean;
    underline?: boolean;
}

/**
 * @summary 视觉显示物的图标：类型色外圈 + 标识符色填充 + 大写字面
 *
 * @summary The icon of the visual subject: a type-colored ring, an identifier-colored fill and an uppercase letter
 *
 * @desc 类似 JetBrains 的图标：外圈用**类型色**、内部填充用**该类型标识符的颜色**，中间是一个稍小的
 *       大写字面（`R` 寄存器、`D` 设备、`A` 别名、`L` 标签、`C` 常量、`N` 数值、`E` 枚举、`I` 指令、
 *       `T` 标签提示）。字面颜色按填充色的明度自动取黑或白，保证可读。
 *
 * @desc A JetBrains-like icon: the ring uses the **type color**, the fill uses the **identifier color of
 *       that type**, and a slightly smaller uppercase letter sits in the middle (`R` register, `D`
 *       device, `A` alias, `L` label, `C` constant, `N` number, `E` enum, `I` instruction, `T` hint tag).
 *       The letter takes black or white automatically from the fill's luminance so it stays readable.
 * */
export interface HoverBadge {
    /** @summary 圈内的大写字面 / @summary The uppercase letter inside the ring */
    abbr: string;

    /** @summary 外圈颜色（类型色）/ @summary Ring color (the type color) */
    ring?: string;

    /** @summary 内部填充色（该类型标识符的颜色）/ @summary Fill color (the identifier color of that type) */
    fill: string;
}

/**
 * @summary 卡片的一行字段
 *
 * @summary One field line of a card
 *
 * @desc 模板里的 `<字段>: <内容>`：所有字段的**冒号后一列**对齐到最长字段之后，内容过长时在该列
 *       换行并保持缩进。
 *
 * @desc One `<field>: <content>` line of the template: the column right after the colon is aligned
 *       past the longest field, and longer content wraps at that column keeping the indent.
 * */
export interface HoverField {
    /** @summary 字段名（如"签名""类型""详情"），渲染时自动补冒号 / @summary Field name; a colon is added when rendering */
    label: string;

    /** @summary 字段内容 / @summary Field content */
    value: TextRun[];

    /** @summary 内容是否为代码（Markdown 渲染器会用行内代码包裹；SVG 本就等宽）/ @summary Whether the content is code (Markdown wraps it in an inline code span) */
    code?: boolean;
}

/**
 * @summary 一张悬停卡片
 *
 * @summary A hover card
 *
 * @desc 结构即模板，方括号表示可缺省：
 * ```text
 * [<图标> <视觉显示物> <类型>]
 * [(<语法类型>)] <表达式> [: <纯数据>] [= <计算值>]
 * <hr>
 * <字段>:          <内容>
 * <最长的字段>:    <内容>
 * [<当前文件路径，小一号>]
 * ```
 * 只有 `display` 时就是一行紧凑提示；没有 `fields` 时连分隔线一起省掉。
 *
 * @desc The structure is the template, with brackets marking the optional parts:
 * ```text
 * [<icon> <subject> <type>]
 * [(<syntax kind>)] <expression> [: <raw data>] [= <computed value>]
 * <hr>
 * <field>:         <content>
 * <longest field>: <content>
 * [<current file path, one size down>]
 * ```
 * A lone `display` yields a compact one-line hint, and missing `fields` skip the rule as well.
 * */
export interface HoverCard {
    /** @summary 视觉显示物的图标 / @summary The icon of the visual subject */
    badge?: HoverBadge;

    /** @summary 视觉显示物（标识符、指令关键字、字面量……），自带颜色 / @summary The visual subject (identifier, keyword, literal, …), colored by the caller */
    display?: TextRun[];

    /** @summary 类型（如"寄存器""普通设备"），显示在显示物之后 / @summary The type (e.g. "register"), shown after the subject */
    type?: TextRun[];

    /** @summary 语法类型（如"别名""标签""常量"），渲染时带圆括号 / @summary The syntax kind (e.g. "alias", "label"); rendered in parentheses */
    syntax?: TextRun[];

    /** @summary 表达式（符号名、签名或字面量本身）/ @summary The expression (a symbol name, a signature or the literal itself) */
    expression?: TextRun[];

    /** @summary 纯数据（如枚举成员的取值），渲染成 `: 数据` / @summary Raw data (e.g. an enum member's value), rendered as `: data` */
    data?: TextRun[];

    /** @summary 计算值（宏常量的哈希值、标签所在行……），渲染成 `= 值` / @summary A computed value (a macro constant's hash, a label's line, …), rendered as `= value` */
    computed?: TextRun[];

    /** @summary 分隔线之后的字段 / @summary The fields after the rule */
    fields?: HoverField[];

    /** @summary 末尾小一号的脚注（当前文件路径等）/ @summary A smaller footnote at the end (the current file path, …) */
    footer?: TextRun[];
}

/**
 * @summary 卡片渲染选项
 *
 * @summary Card rendering options
 * */
export interface HoverCardOptions {
    /**
     * @summary 内容区最大宽度（px）
     *
     * @summary Maximum content width in pixels
     *
     * @desc 超过该宽度的行在断点处换行，卡片也不会比它更宽。
     *
     * @desc A line wider than this wraps at a break opportunity, and the card never grows past it.
     * */
    maxWidth?: number;

    /** @summary 字号（px）/ @summary Font size in pixels */
    fontSize?: number;

    /** @summary 内边距（px）/ @summary Inner padding in pixels */
    padding?: number;
}

/** @summary 内容区默认最大宽度 / @summary Default maximum content width */
export const DEFAULT_HOVER_MAX_WIDTH = 560;

/**
 * @summary 等宽字体下一个**半角**字符的宽度（以 em 计）
 *
 * @summary Advance width of one **half-width** character in a monospace font (in em)
 *
 * @desc 卡片统一用等宽字体：常见等宽字体的字符前进宽度是 0.6em（CJK 恰好占两格 = 1.2em），于是
 *       "第几列"可以精确换算成像素，列对齐不依赖浏览器测量，也不会因为字体不同而错位。
 *
 * @desc Cards use a monospace font throughout: the advance width of a common monospace font is 0.6em
 *       (a CJK glyph takes exactly two cells, i.e. 1.2em), so a column index converts to pixels
 *       exactly — alignment needs no browser measurement and cannot drift between fonts.
 * */
const CELL_WIDTH_EM = 0.6;

/** @summary 图标边长（px）/ @summary Side length of the icon in pixels */
const BADGE_SIZE = 16;

/** @summary 图标与文字之间的间距（px）/ @summary Gap between the icon and the text in pixels */
const BADGE_GAP = 6;

/** @summary 脚注字号相对正文字号的缩放 / @summary Footnote font size relative to the body font size */
const FOOTER_SCALE = 0.85;

/** @summary 占两格的码点区间（CJK、全角、假名、谚文、emoji…）/ @summary Double-width code point ranges */
const WIDE_RANGES: readonly (readonly [number, number])[] = [
    [0x1100, 0x115f], // 谚文字母
    [0x2e80, 0x303e], // CJK 部首扩展 ~ CJK 符号
    [0x3041, 0x33ff], // 假名、注音、CJK 兼容
    [0x3400, 0x4dbf], // CJK 扩展 A
    [0x4e00, 0x9fff], // CJK 基本区
    [0xa000, 0xa4cf], // 彝文
    [0xac00, 0xd7a3], // 谚文音节
    [0xf900, 0xfaff], // CJK 兼容表意
    [0xfe10, 0xfe19], // 竖排标点
    [0xfe30, 0xfe6f], // CJK 兼容形式
    [0xff00, 0xff60], // 全角形式
    [0xffe0, 0xffe6], // 全角符号
    [0x1f300, 0x1faff], // emoji
    [0x20000, 0x3fffd] // CJK 扩展 B 及以后
];

/** @summary 占零格的码点区间（组合记号、变体选择符、零宽字符）/ @summary Zero-width code point ranges */
const ZERO_RANGES: readonly (readonly [number, number])[] = [
    [0x0300, 0x036f], // 组合变音记号
    [0x200b, 0x200f], // 零宽空格、方向控制
    [0xfe00, 0xfe0f] // 变体选择符
];

/** 断行处的字符：在它**之后**可以断行 */
const BREAK_CHARS = new Set([" ", "-", "/", ",", "·", "，", "。", "、"]);

/** @summary 一个码点占几格（0 / 1 / 2）/ @summary Cells taken by one code point (0 / 1 / 2) */
function charCells(codePoint: number): number {
    for (const [from, to] of ZERO_RANGES) if (codePoint >= from && codePoint <= to) return 0;

    for (const [from, to] of WIDE_RANGES) if (codePoint >= from && codePoint <= to) return 2;

    return 1;
}

/** @summary 文本占几格 / @summary Cells taken by a text */
export function textCells(text: string): number {
    let cells = 0;

    for (const ch of text) cells += charCells(ch.codePointAt(0)!);

    return cells;
}

/** @summary 文本宽度（px）/ @summary Text width in pixels */
export function textWidth(text: string, fontSize: number): number {
    return textCells(text) * fontSize * CELL_WIDTH_EM;
}

/** @summary 片段拼起来的纯文本 / @summary Plain text of the concatenated runs */
export function runsText(runs: readonly TextRun[]): string {
    return runs.map(run => run.text).join("");
}

/** @summary 片段拼起来的宽度（px）/ @summary Width of the concatenated runs in pixels */
function runsWidth(runs: readonly TextRun[], fontSize: number): number {
    return textWidth(runsText(runs), fontSize);
}

/** 换行时逐个字符处理，需记住每个字符来自哪个片段，断行后仍按片段着色 */
interface CharPiece {
    run: TextRun;
    ch: string;
    cells: number;
}

/** @summary 两个片段是否同风格 / @summary Whether two runs share the same style */
function sameStyle(a: TextRun, b: TextRun): boolean {
    return (
        a.color === b.color &&
        !!a.bold === !!b.bold &&
        !!a.italic === !!b.italic &&
        !!a.underline === !!b.underline
    );
}

/** @summary 合并相邻同风格片段（逐字符处理后避免 `tspan` 碎片化）/ @summary Merge adjacent runs of the same style */
function mergeChars(pieces: readonly CharPiece[]): TextRun[] {
    const merged: TextRun[] = [];

    for (const piece of pieces) {
        const last = merged[merged.length - 1];

        if (last && sameStyle(last, piece.run)) last.text += piece.ch;
        else merged.push({ ...piece.run, text: piece.ch });
    }

    return merged;
}

/** @summary 该字符之后是否可断行 / @summary Whether a line may break after this character */
function breakAfter(piece: CharPiece): boolean {
    // 两格字符之间可断（CJK 逐字断行），空格/连字符/逗号之后可断
    return piece.cells === 2 || BREAK_CHARS.has(piece.ch);
}

/**
 * @summary 换行：把片段切成若干行，每行宽度不超过 `limit`
 *
 * @summary Wrap the runs into lines no wider than `limit`
 *
 * @desc 断点取空格/连字符/逗号之后，以及任何**两格字符之间**（CJK 逐字可断）；找不到断点时硬断，
 *       因此一行绝不会超宽。两种断法都不丢字符，断行后各片段仍是完整的一段同风格文本。
 *
 * @desc Break opportunities are after a space, hyphen or comma, and between any two double-width
 *       characters (CJK breaks per character). Without an opportunity the line breaks hard, so a line
 *       can never exceed the limit; neither break drops characters, and each line still holds whole
 *       styled runs.
 *
 * @param runs 待换行的片段
 * @param runs The runs to wrap
 * @param limit 每行最大宽度（px）；非正数表示不换行
 * @param limit Maximum width per line in pixels; a non-positive value disables wrapping
 * @param fontSize 字号（px）
 * @param fontSize Font size in pixels
 *
 * @returns 每行的片段（至少一行；`runs` 为空时返回空数组）
 * @returns The runs of each line (at least one line; empty when `runs` is empty)
 * */
export function wrapRuns(runs: readonly TextRun[], limit: number, fontSize: number): TextRun[][] {
    const pieces: CharPiece[] = [];

    for (const run of runs)
        for (const ch of run.text) pieces.push({ run, ch, cells: charCells(ch.codePointAt(0)!) });

    if (!pieces.length) return [];

    const cellsPerLine = limit / (fontSize * CELL_WIDTH_EM);

    if (!(cellsPerLine > 0)) return [mergeChars(pieces)];

    const lines: TextRun[][] = [];
    let line: CharPiece[] = [];
    let cells = 0;
    let breakIndex = -1; // 当前行内可断行的位置（字符个数）

    /** 把当前行从 `from` 处切开，剩余字符留到下一行 */
    const cut = (from: number) => {
        const head = line.slice(0, from);
        const tail = line.slice(from);

        lines.push(mergeChars(head));

        line = tail;
        cells = tail.reduce((sum, piece) => sum + piece.cells, 0);
        breakIndex = -1;

        tail.forEach((piece, index) => {
            if (breakAfter(piece)) breakIndex = index + 1;
        });
    };

    for (const piece of pieces) {
        // 放不下：优先在最近的断点处断，没有断点就硬断
        if (cells + piece.cells > cellsPerLine && line.length) cut(breakIndex > 0 ? breakIndex : line.length);

        line.push(piece);
        cells += piece.cells;

        if (breakAfter(piece)) breakIndex = line.length;
    }

    if (line.length) lines.push(mergeChars(line));

    return lines;
}

/** @summary 转义 XML 文本 / @summary Escape text for XML */
function escapeXml(text: string): string {
    return text
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&apos;");
}

/** @summary 转义 Markdown 图片 alt 里会破坏语法的字符 / @summary Escape the characters that break an image alt */
function escapeAlt(text: string): string {
    return text.replace(/[[\]()!]/g, ch => `\\${ch}`).replace(/\r?\n/g, " ");
}

/**
 * @summary 把片段渲染成一行 `<tspan>`
 *
 * @summary Render the runs of one line as `<tspan>` elements
 *
 * @desc 只有首个 `<tspan>` 带 `x`，其余交给文本流接续：即使实际字体比估算略宽/略窄，整行也只是整体
 *       平移，不会互相重叠或被撑开。`opacity` 用于末尾的小字脚注。
 *
 * @desc Only the first `<tspan>` carries an `x`; the rest continue the text flow. Even if the real font
 *       is slightly wider or narrower than estimated, the line shifts as a whole instead of
 *       overlapping or spreading out. `opacity` serves the smaller footnote.
 * */
function tspans(runs: readonly TextRun[], x: number, defaultColor: string, opacity?: number): string {
    const alpha = opacity === undefined ? "" : ` fill-opacity="${opacity}"`;

    return runs
        .map((run, index) => {
            const attrs = [
                `fill="${run.color ?? defaultColor}"`,
                run.bold ? `font-weight="bold"` : "",
                run.italic ? `font-style="italic"` : "",
                run.underline ? `text-decoration="underline"` : "",
                index === 0 ? `x="${round(x)}"` : "",
                alpha
            ]
                .filter(Boolean)
                .join(" ");

            return `<tspan ${attrs}>${escapeXml(run.text)}</tspan>`;
        })
        .join("");
}

/** @summary 保留两位小数（SVG 里的坐标没必要更长）/ @summary Round to two decimals (SVG coordinates need no more) */
function round(value: number): number {
    return Math.round(value * 100) / 100;
}

/**
 * @summary 按填充色明度选取字面颜色
 *
 * @summary Pick the letter color from the fill's luminance
 *
 * @desc 图标里的字面要压在填充色上：深色填充配白字、浅色填充配深字，避免"深紫底黑字"这类看不清的
 *       组合。只认 `#rgb` / `#rrggbb`，解析不出来就退回白色（深色主题更常见）。
 *
 * @desc The letter sits on the fill: a dark fill takes white and a light fill takes a dark letter, so
 *       unreadable pairs like black-on-deep-purple cannot happen. Only `#rgb` / `#rrggbb` are parsed;
 *       anything else falls back to white, which suits the more common dark theme.
 * */
function contrastText(fill: string): string {
    const match = /^#([0-9a-f]{3}|[0-9a-f]{6})$/i.exec(fill.trim());

    if (!match) return "#ffffff";

    const hex = match[1].length === 3 ? [...match[1]].map(ch => ch + ch).join("") : match[1];

    const value = Number.parseInt(hex, 16);
    const luminance = (0.299 * ((value >> 16) & 0xff) + 0.587 * ((value >> 8) & 0xff) + 0.114 * (value & 0xff)) / 255;

    return luminance > 0.6 ? "#1a1a1a" : "#ffffff";
}

/** 已排版的一行：普通文本行、字段行、分隔线或脚注 */
type CardLine =
    | { kind: "runs"; runs: TextRun[]; indent: number }
    | { kind: "fields"; label: string; values: TextRun[][]; labelX: number; valueX: number }
    | { kind: "rule" }
    | { kind: "footer"; runs: TextRun[] };

/** @summary 行 2 的片段：`(语法类型)` + 表达式 + `: 数据` + `= 计算值` / @summary Runs of line 2 */
function expressionRuns(card: HoverCard): TextRun[] {
    const runs: TextRun[] = [];

    if (card.syntax?.length) runs.push({ text: `(${runsText(card.syntax)}) ` });

    if (card.expression?.length) runs.push(...card.expression);

    if (card.data?.length) runs.push({ text: ": " }, ...card.data);

    if (card.computed?.length) runs.push({ text: " = " }, ...card.computed);

    return runs;
}

/** @summary 行 1 的片段：显示物 + 类型（图标由渲染器另行绘制）/ @summary Runs of line 1 (the icon is drawn separately) */
function headRuns(card: HoverCard): TextRun[] {
    const runs: TextRun[] = [...(card.display ?? [])];

    if (card.type?.length) {
        if (runs.length) runs.push({ text: " " });

        runs.push(...card.type);
    }

    return runs;
}

/**
 * @summary 把卡片渲染成 SVG 图片（Markdown 图片语法）
 *
 * @summary Render a card into an SVG image (Markdown image syntax)
 *
 * @desc 布局"先算后画"：所有宽度只由**等宽字体的格数**决定（见 {@link textWidth}），因此列对齐与
 *       换行都不依赖浏览器测量；有图标时所有内容行统一让出一条左侧装订线，图标就画在装订线里。
 *
 * @desc The layout is computed before drawing: every width comes from the **cell count in a monospace
 *       font** (see {@link textWidth}), so neither column alignment nor wrapping needs browser
 *       measurement. With an icon, every content line leaves a left gutter and the icon is drawn in it.
 *
 * @param card 卡片内容
 * @param card The card content
 * @param options 渲染选项
 * @param options Rendering options
 *
 * @returns Markdown 图片（`![alt](data:image/svg+xml;base64,…)`）；卡片为空时返回空字符串
 * @returns A Markdown image (`![alt](data:image/svg+xml;base64,…)`), or an empty string for an empty card
 * */
export function renderSvgCard(card: HoverCard, options: HoverCardOptions = {}): string {
    const fontSize = options.fontSize ?? 13;
    const padding = options.padding ?? 8;
    const maxWidth = options.maxWidth ?? DEFAULT_HOVER_MAX_WIDTH;
    const lineHeight = Math.round(fontSize * 1.6);
    const cell = fontSize * CELL_WIDTH_EM;
    const defaultColor = s("common.text") || "inherit";

    // 有图标时全部内容行缩进一条装订线，图标画在其中
    const gutter = card.badge ? BADGE_SIZE + BADGE_GAP : 0;
    const left = padding + gutter;
    const limit = maxWidth - padding * 2 - gutter;

    const fields = card.fields ?? [];
    const labelCells = fields.reduce((max, field) => Math.max(max, textCells(`${field.label}:`)), 0);

    // 字段内容列 = 左边距 + 最长字段（含冒号）+ 一个空格
    const valueColumn = left + (labelCells + 1) * cell;

    const lines: CardLine[] = [];
    let rows = 0;

    const pushRuns = (runs: TextRun[]) => {
        for (const line of wrapRuns(runs, limit, fontSize)) {
            lines.push({ kind: "runs", runs: line, indent: 0 });
            rows++;
        }
    };

    // 图标固定占第 1 行：头部没有文字时也要占位，否则图标会挤到表达式那一行上
    const head = headRuns(card);

    if (head.length) pushRuns(head);
    else if (card.badge) {
        lines.push({ kind: "runs", runs: [], indent: 0 });
        rows++;
    }

    const expression = expressionRuns(card);

    if (expression.length) pushRuns(expression);

    if (fields.length) {
        lines.push({ kind: "rule" });
        rows++;

        for (const field of fields) {
            const values = wrapRuns(field.value, maxWidth - valueColumn, fontSize);
            const rowsOfField = values.length ? values : [[]];

            lines.push({
                kind: "fields",
                label: `${field.label}:`,
                values: rowsOfField,
                labelX: left,
                valueX: valueColumn
            });

            rows += rowsOfField.length;
        }
    }

    if (card.footer?.length) {
        const footerText = runsText(card.footer);
        const footerLimit = maxWidth - padding * 2;

        // 路径可能很长：按格数从**左侧**截断，保留更有信息量的尾部
        const footerCells = Math.floor(footerLimit / (fontSize * FOOTER_SCALE * CELL_WIDTH_EM));
        const shortened =
            textCells(footerText) > footerCells
                ? `…${truncateLeft(footerText, Math.max(1, footerCells - 1))}`
                : footerText;

        lines.push({ kind: "footer", runs: [{ ...card.footer[0], text: shortened }] });
        rows++;
    }

    if (!rows) return "";

    // 卡片宽度：所有行里最宽的那条 + 左右内边距，且不超过 maxWidth（另设一个最小宽度）
    let widest = 0;

    for (const line of lines)
        if (line.kind === "runs") widest = Math.max(widest, left + runsWidth(line.runs, fontSize));
        else if (line.kind === "footer") widest = Math.max(widest, padding + runsWidth(line.runs, fontSize * FOOTER_SCALE));
        else if (line.kind === "fields")
            widest = Math.max(
                widest,
                line.valueX - padding + Math.max(0, ...line.values.map(value => runsWidth(value, fontSize)))
            );

    const width = Math.ceil(Math.min(Math.max(widest + padding, cell * 16), maxWidth));
    const height = rows * lineHeight + padding * 2;

    // 逐行绘制：`row` 是**实际占用的行号**，字段换行会占多行，因此不能用数组下标当行号
    // （否则换行后的内容会与后面的行重叠——指令详情换行压住文件路径就是这个原因）
    let row = 0;

    const body = lines
        .map(line => {
            const middle = padding + row * lineHeight + lineHeight / 2;

            if (line.kind === "rule") {
                row++;

                return `  <line x1="${padding}" y1="${round(middle)}" x2="${width - padding}" y2="${round(middle)}" stroke="${defaultColor}" stroke-opacity="0.3" shape-rendering="crispEdges"/>`;
            }

            if (line.kind === "fields") {
                row += line.values.length;

                return (
                    `  <text x="${round(line.labelX)}" y="${round(middle)}" dominant-baseline="central">${tspans(
                        [{ text: line.label }],
                        line.labelX,
                        defaultColor
                    )}</text>\n` +
                    line.values
                        .map(
                            (value, valueIndex) =>
                                `  <text x="${round(line.valueX)}" y="${round(middle + valueIndex * lineHeight)}" dominant-baseline="central">${tspans(
                                    value,
                                    line.valueX,
                                    defaultColor
                                )}</text>`
                        )
                        .join("\n")
                );
            }

            if (line.kind === "footer") {
                row++;

                return `  <text x="${padding}" y="${round(middle)}" dominant-baseline="central" font-size="${round(
                    fontSize * FOOTER_SCALE
                )}">${tspans(line.runs, padding, defaultColor, 0.65)}</text>`;
            }

            // 行 1 的图标：类型色外圈 + 标识符色填充 + 稍小的大写字面
            const badge =
                row === 0 && card.badge
                    ? `  <rect x="${padding}" y="${round(middle - BADGE_SIZE / 2)}" width="${BADGE_SIZE}" height="${BADGE_SIZE}" rx="4" fill="${
                          card.badge.fill
                      }" stroke="${card.badge.ring ?? card.badge.fill}" stroke-width="1.4"/>\n` +
                      `  <text x="${round(padding + BADGE_SIZE / 2)}" y="${round(middle)}" text-anchor="middle" dominant-baseline="central" font-size="${round(
                          fontSize * 0.72
                      )}" font-weight="bold" fill="${contrastText(card.badge.fill)}">${escapeXml(card.badge.abbr)}</text>\n`
                    : "";

            row++;

            return `${badge}  <text x="${round(left + line.indent)}" y="${round(middle)}" dominant-baseline="central">${tspans(
                line.runs,
                left + line.indent,
                defaultColor
            )}</text>`;
        })
        .join("\n");

    const svg =
        `<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}" ` +
        `font-family="monospace" font-size="${fontSize}" xml:space="preserve">\n${body}\n</svg>`;

    const alt = escapeAlt(
        [
            card.badge?.abbr ?? "",
            runsText(headRuns(card)),
            runsText(expressionRuns(card)),
            ...fields.map(field => `${field.label}: ${runsText(field.value)}`),
            card.footer ? runsText(card.footer) : ""
        ]
            .filter(Boolean)
            .join(" | ")
    );

    return `![${alt}](data:image/svg+xml;base64,${Buffer.from(svg, "utf8").toString("base64")})`;
}

/** @summary 从左侧截断文本（保留尾部）/ @summary Truncate a text from the left, keeping its tail */
function truncateLeft(text: string, cells: number): string {
    let result = "";
    let used = 0;

    for (let index = text.length - 1; index >= 0; index--) {
        const size = charCells(text.codePointAt(index)!);

        if (used + size > cells) break;

        result = text[index] + result;
        used += size;
    }

    return result;
}

/**
 * @summary 把卡片渲染成原生 Markdown
 *
 * @summary Render a card into native Markdown
 *
 * @desc 与 SVG 版同一份内容：图标化作文本（大写字面）、行 1、行 2、分隔线、字段行、末尾脚注。
 *       Markdown 无法按列对齐（这正是 SVG 模式存在的意义），因此字段写成 `**字段**: 内容`。
 *
 * @desc 行 1 与行 2 之间用**行尾两个空格**的硬换行：Markdown 里单个换行只是软换行，两行会被渲染成
 *       同一段落在同一行上。分隔线作为独立块，前后各留一个空行。
 *
 * @desc The same content as the SVG version: the icon becomes text (its uppercase letter), then line 1,
 *       line 2, the rule, the field lines and the footnote. Markdown cannot align columns (which is
 *       exactly why the SVG mode exists), so fields are written as `**field**: content`.
 *
 * @desc Lines 1 and 2 are separated by a **two-trailing-space hard break**: in Markdown a single
 *       newline is a soft break, so both lines would render as one paragraph on one line. The rule
 *       forms its own block with a blank line on either side.
 *
 * @param card 卡片内容
 * @param card The card content
 *
 * @returns Markdown 文本；卡片为空时返回空字符串
 * @returns Markdown text, or an empty string for an empty card
 * */
export function renderMarkdownCard(card: HoverCard): string {
    const blocks: string[] = [];
    const fields = card.fields ?? [];
    const header: string[] = [];

    if (card.badge || headRuns(card).length) {
        const parts = [
            card.badge ? `\`${card.badge.abbr}\`` : "",
            (card.display ?? []).map(run => `**${runsText([run])}**`).join(""),
            card.type?.length ? runsText(card.type) : ""
        ].filter(Boolean);

        if (parts.length) header.push(parts.join(" "));
    }

    const expression = expressionRuns(card);

    if (expression.length) header.push(`\`${runsText(expression)}\``);

    // 行 1 与行 2 之间是硬换行（行尾两个空格）
    if (header.length) blocks.push(header.join("  \n"));

    if (fields.length) {
        blocks.push("---");

        blocks.push(
            fields
                .map(field => {
                    const value = runsText(field.value).trim() || "---";

                    return `**${field.label}**: ${field.code ? `\`${value}\`` : value}`;
                })
                .join("\n")
        );
    }

    if (card.footer?.length) blocks.push(`*${runsText(card.footer)}*`);

    return blocks.join("\n\n").trim();
}
