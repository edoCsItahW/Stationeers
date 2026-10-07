/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file inlayHint.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 16:30
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";

import { delay, openFixture } from "./helpers";


/** @if zh 夹具名 @else Fixture name @endif */
const FIXTURE = "inlayHint.ic";

/**
 * @if zh
 * @summary 决定提示可见性的编辑器设置
 *
 * @details 插件默认把 ic10 的这个设置设为 `offUnlessPressed`（**按住 Ctrl+Alt**，macOS 为 Ctrl+Option，
 *          才显示）。用例显式设为 `on`，这样断言的是"服务端给了什么"，与用户的按键状态无关。
 *
 * @else
 * @summary The editor setting that decides hint visibility
 *
 * @details The plugin defaults ic10's setting to `offUnlessPressed` (visible only while **Ctrl+Alt**, or
 *          Ctrl+Option on macOS, is held). The cases set it to `on` explicitly so that they assert what the
 *          server offers, independent of any key being pressed.
 *
 * @endif
 * */
/** @if zh 该设置所在的配置节 @else The configuration section the setting lives in @endif */
const SECTION = "editor";

/**
 * @if zh
 * @summary 决定提示可见性的编辑器设置（`editor` 节下的键名）
 *
 * @details 插件默认把 ic10 的这个设置设为 `offUnlessPressed`（**按住 Ctrl+Alt**，macOS 为 Ctrl+Option，
 *          才显示）。用例显式设为 `on`，这样断言的是"服务端给了什么"，与用户的按键状态无关。
 *
 * @else
 * @summary The editor setting that decides hint visibility (its key inside the `editor` section)
 *
 * @details The plugin defaults ic10's setting to `offUnlessPressed` (visible only while **Ctrl+Alt**, or
 *          Ctrl+Option on macOS, is held). The cases set it to `on` explicitly so that they assert what the
 *          server offers, independent of any key being pressed.
 *
 * @endif
 * */
const SETTING = "inlayHints.enabled";

/**
 * @if zh
 * @summary 期望的伪代码：**源代码行**（去掉首尾空白）→ 行尾伪代码
 *
 * @details 以源代码行为键而不是行号：夹具调整空行也不会让用例失效。改动模板或夹具都必须同步这里，
 *          这正是这条用例的价值——伪代码的写法是被逐字钉住的。
 *
 * @else
 * @summary Expected pseudocode: **source line** (trimmed) → the pseudocode at its end
 *
 * @details Keyed by source line rather than line number so that inserting blank lines in the fixture cannot
 *          break the cases. Changing a template or the fixture has to update this table — which is exactly
 *          the point: the spelling of the pseudocode is pinned verbatim.
 *
 * @endif
 * */
const EXPECTED = new Map<string, string>([
    ["add a a 1", "a = a + 1"],
    ['lbn r0 HASH("StructureBatteryMedium") Setting Average', "r0 = sum Battery (Medium).Setting (Average)"],
    ["s heater On r6", "heater.On = r6"],
    ["l r3 heater Setting", "r3 = heater.Setting"],
    ["ls r4 heater 1 Charge", "r4 = heater[1].Charge"],
    ["beq a 10 5", "if a == 10 goto 5"],
    ["j 20", "goto 20"]
]);

/**
 * @if zh
 * @summary 对齐列：最长的那条指令行（提示左端都排在这一列）
 *
 * @summary The alignment column: the longest instruction line (every hint starts there)
 *
 * @endif
 * */
const ALIGN_COLUMN = Math.max(...[...EXPECTED.keys()].map(line => line.length));

/**
 * @if zh
 * @summary 端到端内联提示测试
 *
 * @details 提示默认**平时不显示**（插件的 `configurationDefaults` 让 ic10 的
 *          `editor.inlayHints.enabled` 为 `offUnlessPressed`，**按住 Ctrl+Alt**（macOS 为 Ctrl+Option）才显示）；
 *          用例把这个设置显式设为 `on` 以断言服务端产出，结束时恢复。
 *
 * @details 等价类与边界：有模板的指令（正例，逐字比对伪代码 + 行尾位置 + **左端对齐**）、
 *          `HASH("…")` 还原成设备名、分支的多行形态（tooltip）、没有模板的指令（负例，由逐字比对覆盖）、
 *          只请求可见区间（边界：区间外不返回）。
 *
 * @else
 * @summary End-to-end inlay hint tests
 *
 * @details Hints are **hidden by default** (the plugin's `configurationDefaults` sets ic10's
 *          `editor.inlayHints.enabled` to `offUnlessPressed`, so they show while **Ctrl+Alt**, or Ctrl+Option
 *          on macOS, is held); the cases set that setting to `on` to assert what the server produces and
 *          restore it afterwards.
 *
 * @details Equivalence classes and boundaries: templated instructions (positive, verbatim comparison of the
 *          pseudocode plus the end-of-line position and the **left alignment**), `HASH("…")` resolved to a
 *          device name, the multi-line form of a branch (tooltip), instructions without a template (negative,
 *          covered by the very same verbatim comparison) and a request for only part of the document (boundary:
 *          nothing outside the range comes back).
 *
 * @endif
 * */
suite("内联提示 / inlay hints", () => {
    suiteSetup(async () => {
        await vscode.workspace.getConfiguration(SECTION).update(SETTING, "on", vscode.ConfigurationTarget.Workspace);
    });

    suiteTeardown(async () => {
        await vscode.workspace
            .getConfiguration(SECTION)
            .update(SETTING, undefined, vscode.ConfigurationTarget.Workspace);
    });

    test("有模板的指令都在行尾左对齐地得到伪代码 / every templated instruction gets its pseudocode, left aligned at the line end", async () => {
        const document = await openFixture(FIXTURE);
        const hints = await waitForHints(document, EXPECTED.size);
        const actual = hints.map(hint => [document.lineAt(hint.position.line).text.trim(), labelOf(hint).trim()]);

        assert.deepStrictEqual(
            [...actual].sort(),
            [...EXPECTED.entries()].sort(),
            `伪代码与期望不一致 / pseudocode mismatch:\n实际 / actual: ${JSON.stringify(actual)}\n期望 / expected: ${JSON.stringify([...EXPECTED])}`
        );

        for (const hint of hints) {
            const line = document.lineAt(hint.position.line);

            assert.strictEqual(
                hint.position.character,
                line.text.length,
                `提示应落在行尾 / the hint must sit at the end of its line: ${JSON.stringify(line.text)}`
            );
            assert.strictEqual(
                startColumn(hint),
                ALIGN_COLUMN,
                `提示左端应对齐到第 ${ALIGN_COLUMN} 列 / the hint must start at the alignment column: ${JSON.stringify(labelOf(hint))}`
            );
        }

        assert.ok(
            hints.every(hint => hint.paddingLeft),
            "提示应与代码之间留出间隔 / hints need paddingLeft"
        );
    });

    test('HASH("…") 还原为设备名 / the HASH literal is resolved to a device name', async () => {
        const document = await openFixture(FIXTURE);
        const hints = await waitForHints(document, EXPECTED.size);
        const hash = hints.find(hint => document.lineAt(hint.position.line).text.includes("HASH("));

        assert.ok(hash, '含 HASH("…") 的行应有提示 / the line with HASH("…") must carry a hint');

        const label = labelOf(hash!);

        assert.ok(label.includes("Battery (Medium)"), `应还原成设备名 / expected a device name: ${label}`);
        assert.ok(!label.includes("HASH("), `不该保留原字面量 / the literal must not survive: ${label}`);
    });

    test("分支的多行形态放在 tooltip 里 / the multi-line form of a branch lives in the tooltip", async () => {
        const document = await openFixture(FIXTURE);
        const hints = await waitForHints(document, EXPECTED.size);
        const branch = hints.find(hint => document.lineAt(hint.position.line).text.trimStart().startsWith("beq"));

        assert.ok(branch, "条件分支应有提示 / the branch must carry a hint");
        assert.strictEqual(
            tooltipOf(branch!),
            "```\nif a == 10\n    goto 5\n```",
            "分支的 tooltip 应是两行形态 / the branch tooltip must hold the two-line form"
        );
    });

    test("只请求可见区间时，区间外不返回（边界）/ a partial request returns nothing outside the range (boundary)", async () => {
        const document = await openFixture(FIXTURE);
        const lines = document.getText().split(/\r?\n/);
        const first = lines.findIndex(line => line.trim() === "beq a 10 5");
        const last = lines.findIndex(line => line.trim() === "j 20");

        assert.ok(first >= 0 && last > first, "夹具里应能找到分支与跳转两行 / the fixture must contain both lines");

        const hints = await vscode.commands.executeCommand<vscode.InlayHint[]>(
            "vscode.executeInlayHintProvider",
            document.uri,
            new vscode.Range(first, 0, last, lines[last].length)
        );
        const labels = (hints ?? []).map(hint => labelOf(hint).trim());

        assert.deepStrictEqual(
            labels.sort(),
            ["goto 20", "if a == 10 goto 5"],
            `区间内应只有这两条提示 / only these two hints belong to the range: ${JSON.stringify(labels)}`
        );
    });
});

/**
 * @if zh
 * @brief 提示文本（`label` 可能是字符串，也可能是分片数组）
 *
 * @else
 * @brief The text of a hint (`label` is either a string or an array of parts)
 *
 * @endif
 * */
function labelOf(hint: vscode.InlayHint): string {
    return typeof hint.label === "string" ? hint.label : hint.label.map(part => part.value).join("");
}

/**
 * @if zh
 * @brief 提示的 tooltip 文本（可能是字符串或 MarkdownString）
 *
 * @else
 * @brief The tooltip text of a hint (either a string or a MarkdownString)
 *
 * @endif
 * */
function tooltipOf(hint: vscode.InlayHint): string {
    const tooltip = hint.tooltip;

    return typeof tooltip === "string" ? tooltip : (tooltip?.value ?? "");
}

/**
 * @if zh
 * @brief 提示实际开始的列：位置列 + label 里的前导空格
 *
 * @else
 * @brief The column a hint actually starts at: its position plus the leading spaces of its label
 *
 * @endif
 * */
function startColumn(hint: vscode.InlayHint): number {
    const label = labelOf(hint);

    return hint.position.character + (label.length - label.trimStart().length);
}

/**
 * @if zh
 * @brief 查询整篇文档的内联提示（走真实的内联提示提供器命令）
 *
 * @else
 * @brief Request the inlay hints of the whole document (through the real provider command)
 *
 * @endif
 * */
async function requestHints(document: vscode.TextDocument): Promise<vscode.InlayHint[]> {
    const last = document.lineCount - 1;

    return (
        (await vscode.commands.executeCommand<vscode.InlayHint[]>(
            "vscode.executeInlayHintProvider",
            document.uri,
            new vscode.Range(0, 0, last, document.lineAt(last).text.length)
        )) ?? []
    );
}

/**
 * @if zh
 * @brief 等到提示条数达到期望（配置变更与提示请求都是异步的）
 *
 * @else
 * @brief Wait until the number of hints matches (both config changes and hint requests are asynchronous)
 *
 * @endif
 * */
async function waitForHints(document: vscode.TextDocument, count: number): Promise<vscode.InlayHint[]> {
    const deadline = Date.now() + 30_000;

    let hints: vscode.InlayHint[] = [];

    while (Date.now() < deadline) {
        hints = await requestHints(document);

        if (hints.length === count) return hints;

        await delay(200);
    }

    return hints;
}
