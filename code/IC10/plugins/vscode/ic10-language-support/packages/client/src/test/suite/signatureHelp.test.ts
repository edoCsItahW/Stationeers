/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file signatureHelp.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:32
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";

import { openFixture, positionAfter, positionAtLineStart } from "./helpers";


/**
 * @if zh
 * @summary 端到端签名帮助测试
 *
 * @details 输入串取自元数据的指令签名（`lbn r? deviceHash nameHash logicType batchMode`），
 *          因此断言"签名标签里含指令名"既稳定又与语言无关。
 *
 * @details 等价类与边界：语句头部之后（有签名，正例）、空白行（没有任何语句，负例）。
 *
 * @else
 * @summary End-to-end signature help tests
 *
 * @details The label comes from the instruction metadata
 *          (`lbn r? deviceHash nameHash logicType batchMode`), so asserting that it contains the
 *          instruction name is both stable and language independent.
 *
 * @details Equivalence classes and boundaries: past a statement head (a signature, positive) and a blank
 *          line (no statement, negative).
 *
 * @endif
 * */
suite("签名帮助 / signature help", () => {
    test("语句头部的参数位置给出该指令的签名 / offers the signature after a statement head", async () => {
        const document = await openFixture("completion.ic");
        const help = await vscode.commands.executeCommand<vscode.SignatureHelp>(
            "vscode.executeSignatureHelpProvider",
            document.uri,
            positionAfter(document, "lbn "),
            " "
        );

        assert.ok((help?.signatures ?? []).length > 0, "没有任何签名 / no signature offered");

        const labels = help!.signatures.map(signature => signature.label);

        assert.ok(labels.some(label => label.includes("lbn")), `签名里没有指令名 lbn：${JSON.stringify(labels)}`);
    });

    test("空白行上没有签名（边界）/ no signature on a blank line (boundary)", async () => {
        const document = await openFixture("completion.ic");

        // 夹具里 `lbn` 行的上一行是空行：没有语句，自然没有签名
        // The line above `lbn` in the fixture is blank: no statement, hence no signature
        const lbn = positionAtLineStart(document, "lbn");
        const help = await vscode.commands.executeCommand<vscode.SignatureHelp>(
            "vscode.executeSignatureHelpProvider",
            document.uri,
            new vscode.Position(lbn.line - 1, 0),
            " "
        );

        assert.ok(
            !help || (help.signatures ?? []).length === 0,
            `空白行不该有签名 / no signature expected: ${JSON.stringify(help)}`
        );
    });
});
