/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file inlayHint.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 16:11
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Console, debug, lowerBound } from "@ic10/common";
import { InlayHint } from "vscode-languageserver";
import type { Connection } from "vscode-languageserver/node";

import { DocumentCache } from "../../cache";
import { t, locale } from "../../../locals";
import { AST } from "../../../utils";
import { pseudocode } from "./pseudocode";


/** 内联提示请求的参数（由 `languages.inlayHint` 特性的处理器签名推出） */
type InlayHintRequest = Parameters<Parameters<Connection["languages"]["inlayHint"]["on"]>[0]>[0];

/**
 * @if zh
 * @summary 内联提示（Inlay Hint）处理器：在指令行末尾渲染一段伪代码
 *
 * @summary Inlay hint handler rendering a pseudocode rendering at the end of instruction lines
 *
 * @desc 走 LSP 的 `textDocument/inlayHint`：编辑器只为**可见区间**请求提示，因此这里按请求区间二分定位
 *       语句，只渲染落在区间内的指令，复杂度与可见行数成正比，不会随文件增长。
 *
 * @desc 提示默认**平时不显示**：插件的 `configurationDefaults` 把 ic10 的
 *       `editor.inlayHints.enabled` 设为 `offUnlessPressed`，**按住 Ctrl+Alt**（macOS 为 Ctrl+Option）
 *       才显示。因此服务端总是给出提示，可见性完全交给编辑器那一个设置（这个修饰键组合由 VS Code
 *       固定，见该设置自身的说明）。
 *
 * @desc 同一份默认值还把 ic10 的 `editor.inlayHints.maximumLength` 设为 `0`（不截断）：左对齐是靠
 *       **在标签前补空格**实现的，而 VS Code 默认按 43 字符截断标签，空格会先吃掉预算，于是短行的
 *       伪代码会变成 `…`。
 *
 * @desc 伪代码的写法与覆盖面见 {@link pseudocode}；**没有模板的指令不出提示**。
 *
 * @desc Uses LSP's `textDocument/inlayHint`: the editor only requests hints for the **visible range**, so
 *       statements are located by binary search within that range and only instructions inside it are
 *       rendered — the cost grows with the visible lines, not with the file.
 *
 * @desc Hints are **hidden by default**: the plugin's `configurationDefaults` sets ic10's
 *       `editor.inlayHints.enabled` to `offUnlessPressed`, so they appear while **Ctrl+Alt** (Ctrl+Option on
 *       macOS) is held. The server therefore always produces them and visibility rests on that single editor
 *       setting (the modifier combination is fixed by VS Code, see that setting's own description).
 *
 * @desc The same defaults set ic10's `editor.inlayHints.maximumLength` to `0` (no truncation): the left
 *       alignment is done by **leading spaces in the label**, and VS Code truncates labels at 43 characters by
 *       default — the spaces would eat that budget first and short lines would render as `…`.
 *
 * @desc The spelling and coverage of the pseudocode are described in {@link pseudocode}; **instructions
 *       without a template produce no hint**.
 *
 * @endif
 * */
export class InlayHintHandler {
    constructor(private readonly docCache: DocumentCache) {}

    /**
     * @if zh
     * @brief 处理内联提示请求
     *
     * @details 伪代码按**全文档**最长的指令行左对齐：更短的行在提示前面补空格，于是所有提示的左端排在
     *          同一列（用全文档而不是可见区间，滚动时不会左右跳动）。用行尾（而不是语句末尾）作位置，
     *          带尾随注释的行上提示落在注释之后，对齐关系保持不变。
     *
     * @param params 请求参数（文档与可见区间）
     * @return 落在区间内的伪代码提示；文档未知时为 `[]`
     *
     * @else
     * @brief Handle an inlay hint request
     *
     * @details Pseudocode is left-aligned against the longest instruction line of the **whole document**:
     *          shorter lines get leading spaces so that every hint starts at the same column (the whole
     *          document rather than the visible range, so scrolling does not shift them). The position is the
     *          **end of the line** rather than the end of the statement, so on a line with a trailing comment
     *          the hint lands after it and the alignment still holds.
     *
     * @param params The request parameters (document and visible range)
     * @return Pseudocode hints inside the range, or `[]` when the document is unknown
     *
     * @endif
     * */
    @debug({
        message: err => t("server.handler.error", { name: "inlay hint", err: (err as Error).message }),
        logger: msg => Console.error(msg, "inlay hint"),
        rethrow: false
    })
    handle(params: InlayHintRequest): InlayHint[] {
        const cache = this.docCache.getCache(params.textDocument.uri);

        if (!cache?.ast) return [];

        const statements = cache.ast.statements;

        // 夹到行尾与对齐都要用：语句末尾可能比该行末尾多出一点（`HASH("…")` 这类宏的 end 会多算两位）
        // Used for clamping and alignment: a statement end can overshoot its line a little (macros such as
        // `HASH("…")` report an end two characters further)
        const lines = cache.source.split(/\r?\n/);

        // 对齐列：全文档最长的指令行（含尾随注释）；用全文档而非可见区间，滚动时提示不会左右跳
        // Alignment column: the longest instruction line of the whole document (trailing comment included);
        // the whole document rather than the visible range, so hints do not jump while scrolling
        let column = 0;

        for (const statement of statements) {
            if (!AST.belongInstruction(statement)) continue;

            column = Math.max(column, lines[statement.end.line - 1]?.length ?? 0);
        }

        const startLine = params.range.start.line + 1;
        const endLine = params.range.end.line + 1;

        // 从可能跨越区间起点的那一条开始（二分定位第一条不早于起点的语句，再退回一条）
        const first = Math.max(0, lowerBound(statements, item => item.position.line >= startLine) - 1);

        const hints: InlayHint[] = [];

        for (let index = first; index < statements.length; index++) {
            const statement = statements[index];

            if (statement.position.line > endLine) break;
            if (statement.end.line < startLine) continue;
            if (!AST.belongInstruction(statement)) continue;

            const rendered = pseudocode(statement, locale.getLocale());

            if (!rendered) continue;

            const line = statement.end.line - 1;
            const length = lines[line]?.length ?? 0;

            hints.push({
                position: { line, character: length },
                // 补足到对齐列：左端排在同一列，右端随伪代码长短自然错开
                // Padded up to the alignment column: left edges line up, right edges follow the pseudocode
                label: " ".repeat(Math.max(0, column - length)) + rendered.text,
                tooltip: rendered.tooltip,
                paddingLeft: true
            });
        }

        return hints;
    }
}
