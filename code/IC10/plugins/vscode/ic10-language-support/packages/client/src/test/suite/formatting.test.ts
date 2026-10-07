/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file formatting.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:34
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";

import { applyEdits, openFixture } from "./helpers";


/**
 * @if zh
 * @summary 端到端格式化测试
 *
 * @details 期望文本不写死在用例里，而是放在夹具 `formatting.expected.ic`：一个夹具是"没格式化过的"，
 *          另一个是"已经格式化好的"，用例断言前者格式化**之后等于后者**，再断言后者再格式化**不再产生编辑**。
 *          这样"预期"与"幂等"两个性质都由夹具本身表达，改坏任何一侧都会失败。
 *
 * @details 格式化选项由用例传入（4 空格），与工作区设置无关，因此结果确定。
 *
 * @else
 * @summary End-to-end formatting tests
 *
 * @details The expected text is not hardcoded but lives in the `formatting.expected.ic` fixture: one
 *          fixture is "not formatted yet", the other "already formatted". The cases assert that
 *          formatting the former **equals the latter**, and that formatting the latter yields **no edits**.
 *          Both the expectation and idempotence are therefore expressed by the fixtures themselves.
 *
 * @details The formatting options are passed by the test (4 spaces) and independent of workspace
 *          settings, so the result is deterministic.
 *
 * @endif
 * */
suite("格式化 / formatting", () => {
    test("未格式化的文件格式化后与预期夹具一致 / reformats a messy file into the expected fixture", async () => {
        const document = await openFixture("formatting.ic");
        const expected = await openFixture("formatting.expected.ic");
        const edits = await vscode.commands.executeCommand<vscode.TextEdit[]>(
            "vscode.executeFormatDocumentProvider",
            document.uri,
            { tabSize: 4, insertSpaces: true }
        );

        assert.ok(edits && edits.length > 0, "未格式化的夹具应产生编辑 / a messy fixture must produce edits");
        assert.deepStrictEqual(
            applyEdits(document, document.getText(), edits!).split(/\r?\n/),
            expected.getText().split(/\r?\n/),
            "格式化结果应与预期夹具逐行一致 / the formatted text must match the expected fixture line by line"
        );
    });

    test("已格式化的文件不再产生编辑（边界：幂等）/ formatting an already formatted file is a no-op", async () => {
        const document = await openFixture("formatting.expected.ic");
        const edits = await vscode.commands.executeCommand<vscode.TextEdit[] | undefined>(
            "vscode.executeFormatDocumentProvider",
            document.uri,
            { tabSize: 4, insertSpaces: true }
        );

        assert.ok(
            !edits || edits.length === 0,
            `已格式化的文件不该再产生编辑 / expected no edits: ${JSON.stringify(edits)}`
        );
    });
});
