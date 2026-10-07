/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file diagnostics.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:33
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";

import { delay, openFixture, waitForDiagnostics } from "./helpers";


/**
 * @if zh
 * @summary 端到端诊断测试
 *
 * @details 断言用**来源 + 范围 + 文案里的标识符**，不用本地化字句：诊断文案随 `ic10.language` 变化，
 *          而"范围正好覆盖出问题的那个标识符"与语言无关，也正是用户看到的那条波浪线。
 *
 * @details 等价类与边界：语义错误各一类（访问权限不符 `TargetX`、槽位不适用 `Charge`），
 *          以及没有任何错误的干净文件（负例）。诊断是**拉取式**的，因此正例等待出现、
 *          负例给一个稳定的窗口后再断言仍然为空。
 *
 * @else
 * @summary End-to-end diagnostics tests
 *
 * @details Assertions use the **source, the range and the identifier in the message**, never the
 *          localized wording: diagnostics follow `ic10.language`, whereas "the range covers exactly the
 *          offending identifier" is language independent and is the squiggle the user actually sees.
 *
 * @details Equivalence classes and boundaries: one per violated semantic rule (access mismatch on
 *          `TargetX`, slot not applicable on `Charge`) plus a file without any error (negative).
 *          Diagnostics are **pulled**, so positives wait for them while the negative gets a settle window
 *          before asserting emptiness.
 *
 * @endif
 * */
suite("诊断 / diagnostics", () => {
    test("访问权限与槽位不适用各报一条 / reports one diagnostic per violated rule", async () => {
        const document = await openFixture("diagnostics.ic");
        const diagnostics = await waitForDiagnostics(document.uri, 2);

        assert.strictEqual(
            diagnostics.length,
            2,
            `应有 2 条诊断 / expected 2 diagnostics: ${diagnostics.map(item => `${item.source} ${item.message}`).join(" | ")}`
        );
        assert.ok(
            diagnostics.every(item => item.severity === vscode.DiagnosticSeverity.Error),
            `两条都应是错误级 / both must be errors: ${JSON.stringify(diagnostics.map(item => item.severity))}`
        );

        // 每条诊断都指向出问题的那条语句：范围覆盖语句里的**设备操作数**（编译器的选择，实测如此），
        // 文案里则点名出问题的标识符。两者都与本地化文案无关。
        // Each diagnostic points at the offending statement: the range covers its **device operand**
        // (the compiler's choice, measured), while the message names the offending identifier. Neither
        // depends on localized wording.
        for (const [identifier, source, statement] of [
            ["TargetX", "IWA25", "l r0 robot TargetX"],
            ["Charge", "IWA26", "ls r0 robot 1 Charge"]
        ] as const) {
            const found = diagnostics.find(item => item.source === source);

            assert.ok(found, `缺少来源为 ${source} 的诊断 / missing diagnostic from ${source}`);
            assert.strictEqual(
                document.getText(found.range),
                "robot",
                `${source} 的范围应覆盖语句的设备操作数 / the range must cover the statement's device operand（实际覆盖 / covered: ${JSON.stringify(document.getText(found.range))}）`
            );
            assert.strictEqual(
                found.range.start.line,
                document.positionAt(document.getText().indexOf(statement)).line,
                `${source} 的范围应落在出问题的那条语句上 / the range must sit on the offending statement`
            );
            assert.ok(
                found.message.includes(identifier),
                `${source} 的文案里应提到 ${identifier} / the message should mention it: ${found.message}`
            );
        }
    });

    test("干净文件没有诊断（负例）/ a clean file has no diagnostics", async () => {
        const document = await openFixture("clean.ic");

        // 负例无法"等待出现"，给服务端一个稳定的窗口后再断言仍为空
        // A negative cannot be waited for, so it gets a settle window before asserting emptiness
        await delay(1000);

        assert.deepStrictEqual(
            vscode.languages.getDiagnostics(document.uri),
            [],
            "干净文件不该有诊断 / a clean file must have no diagnostics"
        );
    });
});
