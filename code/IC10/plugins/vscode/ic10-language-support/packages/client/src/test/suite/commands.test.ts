/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file commands.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:35
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";

import { activateExtension, openFixture } from "./helpers";


/**
 * @if zh
 * @summary "收窄补全范围"的命令 ID（与 `package.json` 的 contributes 一致）
 *
 * @summary The "narrow completion scope" command IDs (in sync with `contributes` in `package.json`)
 *
 * @endif
 * */
const SCOPE_COMMANDS = ["device", "register", "number", "enum", "identifier", "keyword", "all"].map(
    scope => `ic10.completionScope.${scope}`
);

/**
 * @if zh
 * @summary 端到端命令测试
 *
 * @details 命令是用户直接触发的入口，因此既断言**注册**（`getCommands` 里存在，环境相关的失败会
 *          第一时间在这里暴露），也断言**可执行**（真跑一遍，包含客户端到服务端的通知）。
 *          设备搜索命令在"没有 ic10 编辑器"时会静默返回，所以先断言活动编辑器是夹具，
 *          "没抛错"才真的证明了它走到服务端拉取索引那一步。
 *
 * @else
 * @summary End-to-end command tests
 *
 * @details Commands are what the user triggers directly, so this asserts both **registration** (present in
 *          `getCommands`, which surfaces environment problems immediately) and **executability** (actually
 *          running them, including the client-to-server notification).
 *          The device search command returns silently without an ic10 editor, so the active editor is
 *          asserted first — only then does "it did not throw" prove it reached the server round trip.
 *
 * @endif
 * */
suite("命令 / commands", () => {
    test("七个收窄补全范围的命令都已注册 / all seven completion scope commands are registered", async () => {
        // `getCommands` 只列出**已注册**的命令，而扩展是懒激活的：必须先激活，否则这些命令还没注册
        // `getCommands` only lists **registered** commands and the extension activates lazily, so it has to
        // be activated first — otherwise the commands do not exist yet
        await activateExtension();

        const commands = await vscode.commands.getCommands(true);
        const missing = SCOPE_COMMANDS.filter(id => !commands.includes(id));

        assert.deepStrictEqual(missing, [], `以下命令未注册 / not registered: ${JSON.stringify(missing)}`);
    });

    test("收窄补全范围的命令可执行 / the completion scope commands are runnable", async () => {
        await openFixture("completion.ic");

        for (const id of SCOPE_COMMANDS)
            await assert.doesNotReject(
                async () => {
                    await vscode.commands.executeCommand(id);
                },
                `命令执行失败 / command failed: ${id}`
            );

        // 命令会强制弹出补全列表，收尾关掉，免得影响后续用例（关不掉不算产品缺陷）
        // The commands force the suggest widget open; close it so later cases are unaffected
        await vscode.commands.executeCommand("hideSuggestWidget").then(undefined, () => undefined);
    });

    test("设备搜索命令可执行（含服务端往返）/ the device search command runs, including the server round trip", async () => {
        const document = await openFixture("completion.ic");

        assert.strictEqual(
            vscode.window.activeTextEditor?.document.uri.toString(),
            document.uri.toString(),
            "活动编辑器应是夹具文档 / the fixture must be the active editor"
        );

        await assert.doesNotReject(
            async () => {
                // `executeCommand` 返回 Thenable 而不是 Promise，包一层以便用断言
                await vscode.commands.executeCommand("ic10.searchDeviceType");
            },
            "设备搜索命令未注册或执行失败 / the device search command failed"
        );

        await vscode.commands.executeCommand("workbench.action.closeQuickOpen").then(undefined, () => undefined);
    });
});
