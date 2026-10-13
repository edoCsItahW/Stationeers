/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file cliOptions.ts
 * @author edocsitahw
 * @version 1.0
 * @date 2026/10/08 00:00
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 *
 * @if zh
 * @summary 命令行开关：让同一个语言服务器按客户端适配呈现方式
 *
 * @details 各 LSP 客户端渲染同一份协议内容的能力并不相同。VS Code 能显示 SVG 悬停卡片（由 `ic10.hoverRenderer`
 *          控制，默认 `svg`），也能把内联提示的等宽对齐用出来；JetBrains 的 LSP 客户端这两点都做不到，
 *          而它没有 VS Code 那条 workspace/configuration 通道，没法把设置推给服务器。所以这类差异不靠配置，
 *          而由**启动方**在命令行上显式声明：JetBrains 插件在自己的 `createCommandLine()` 里传参。
 *
 * @details 不传任何开关时行为与从前完全一致——VS Code 走的正是这条路，因此本文件不会改变扩展的既有表现。
 *          值非法时不猜、也不静默：写一条 stderr 警告后保持默认值，避免"参数写错了却看起来像渲染坏了"。
 *
 * @else
 * @summary Command-line switches that let one server adapt its presentation per client
 *
 * @details LSP clients differ in what they can render. VS Code can show the SVG hover card (selected by
 *          `ic10.hoverRenderer`, `svg` by default) and can honour the monospace alignment of the inlay
 *          hints; the JetBrains LSP client can do neither, and it has no workspace/configuration channel
 *          to push settings through. Such differences therefore are not configuration but an explicit
 *          declaration by whoever starts the server: the JetBrains plugin passes them from its own
 *          `createCommandLine()`.
 *
 * @details With no switch the behaviour is exactly what it was — the path VS Code takes — so this file
 *          cannot change the extension's existing output. An invalid value is neither guessed nor
 *          swallowed: it is reported on stderr and the default is kept, so a typo cannot look like a
 *          rendering bug.
 *
 * @endif
 * */

/**
 * @if zh
 * @summary 命令行开关解析结果
 *
 * @details 字段为 `undefined` 表示"未声明"，此时保持客户端的设置或默认值。
 *
 * @else
 * @summary Parsed command-line switches
 *
 * @details An `undefined` field means "not declared", so the client's setting or the default is kept.
 *
 * @endif
 */
export interface CliOptions {
    /** @if zh 悬停渲染器覆盖 / @else Hover renderer override / @endif */
    hoverRenderer?: "svg" | "markdown";

    /** @if zh 是否对客户端声明内联提示能力 / @else Whether the inlay hints capability is advertised / @endif */
    inlayHints?: boolean;

    /** @if zh 悬停 Markdown 的硬换行形式 / @else Hard-break form of the hover Markdown / @endif */
    hoverBreaks?: "spaces" | "html";
}

const HOVER_RENDERER_FLAG = "--hover-renderer=";
const HOVER_BREAKS_FLAG = "--hover-breaks=";
const INLAY_HINTS_FLAG = "--inlay-hints=";

/**
 * @if zh
 * @summary 解析命令行开关
 *
 * @details 只识别本服务器自己的两个开关，其余参数（`--stdio`、`--node-ipc`、`--socket=` 等）由
 *          `vscode-languageserver` 自行处理，这里不做拦截也不报错。
 *
 * @param argv - 参数列表，默认取当前进程
 * @param argv - Argument list, the current process by default
 * @returns 解析出的开关，未出现的项为 `undefined`
 * @returns The parsed switches, with absent ones left `undefined`
 *
 * @else
 * @summary Parses the command-line switches
 *
 * @details Only this server's own two switches are recognised; everything else (`--stdio`, `--node-ipc`,
 *          `--socket=` and friends) is left to `vscode-languageserver` — neither intercepted nor reported.
 *
 * @param argv - Argument list, the current process by default
 * @returns The parsed switches, with absent ones left `undefined`
 *
 * @endif
 */
export function parseCliOptions(argv: readonly string[] = process.argv): CliOptions {
    const options: CliOptions = {};

    for (const arg of argv) {
        if (arg.startsWith(HOVER_RENDERER_FLAG)) {
            const value = arg.slice(HOVER_RENDERER_FLAG.length);

            if (value === "svg" || value === "markdown") options.hoverRenderer = value;
            else warn(arg, "svg|markdown");
        } else if (arg.startsWith(HOVER_BREAKS_FLAG)) {
            const value = arg.slice(HOVER_BREAKS_FLAG.length);

            if (value === "spaces" || value === "html") options.hoverBreaks = value;
            else warn(arg, "spaces|html");
        } else if (arg.startsWith(INLAY_HINTS_FLAG)) {
            const value = arg.slice(INLAY_HINTS_FLAG.length);

            if (value === "on" || value === "off") options.inlayHints = value === "on";
            else warn(arg, "on|off");
        }
    }

    return options;
}

/**
 * @if zh
 * @summary 报告一个语法正确但取值非法的开关
 *
 * @details 直接写 stderr：此时连接尚未建立，不能用 `window/logMessage`，而静默忽略会让"参数写错"看起来像
 *          "客户端渲染有问题"。
 *
 * @else
 * @summary Reports a well-formed switch with an invalid value
 *
 * @details Writes straight to stderr: the connection does not exist yet, so `window/logMessage` is not
 *          available, and swallowing the value would make a typo look like a client rendering problem.
 *
 * @endif
 */
function warn(arg: string, expected: string): void {
    process.stderr.write(`[IC10 LSP] 忽略非法开关 / ignoring invalid switch "${arg}" (expected ${expected})\n`);
}
