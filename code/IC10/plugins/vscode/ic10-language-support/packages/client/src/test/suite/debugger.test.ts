/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file debugger.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:37
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { RUNTIME_CONFIG_KEYS } from "@ic10/debugger";
import * as assert from "assert";
import * as vscode from "vscode";

import { openFixture } from "./helpers";


/**
 * @if zh
 * @summary 端到端调试测试
 *
 * @details 真起一个调试会话（`stopOnEntry` 停在入口，随即停掉），因为调试配置提供器**没有**查询 API：
 *          只有真正启动会话，才能拿到 `activeDebugSession.configuration` 这一份"已被提供器处理过"的配置。
 *
 * @details 一条用例同时覆盖两条规则（一个等价类 + 一个边界）：
 *          未写明的运行时项取自 `ic10.runtime.*` 设置，而已写明的项**不被设置覆盖**。
 *          启动带超时，会话起不来时明确失败，不会把用例挂死。
 *
 * @else
 * @summary End-to-end debugging test
 *
 * @details A real debug session is started (with `stopOnEntry` so it halts at the entry, then stopped),
 *          because the debug configuration provider has **no** query API: only by actually starting a
 *          session can `activeDebugSession.configuration` — the configuration the provider has processed —
 *          be observed.
 *
 * @details One case covers both rules (an equivalence class plus a boundary): unset runtime keys come from
 *          the `ic10.runtime.*` settings while an explicitly written key is **not** overwritten. Starting
 *          is bounded by a timeout so a session that never comes up fails loudly instead of hanging.
 *
 * @endif
 * */
suite("调试 / debugging", () => {
    test("运行时项按 launch > 设置 的优先级补齐 / runtime keys follow launch.json > settings", async () => {
        const document = await openFixture("clean.ic");
        const folder = vscode.workspace.workspaceFolders?.[0];

        assert.ok(folder, "端到端测试需要工作区 / a workspace is required");

        const [explicitKey, ...inheritedKeys] = RUNTIME_CONFIG_KEYS;
        const explicit = 12345;
        const session = await startDebugSession(folder, {
            program: document.uri.fsPath,
            [explicitKey]: explicit
        });

        try {
            const settings = vscode.workspace.getConfiguration("ic10.runtime");

            assert.strictEqual(
                session.configuration[explicitKey],
                explicit,
                `${explicitKey} 已写明，不该被设置覆盖 / an explicit key must not be overwritten`
            );

            for (const key of inheritedKeys)
                assert.strictEqual(
                    session.configuration[key],
                    settings.get(key),
                    `${key} 未写明，应取自工作区设置 / an unset key must come from the workspace settings`
                );
        } finally {
            await vscode.debug.stopDebugging(session);
        }
    });
});

/**
 * @if zh
 * @brief 启动一个 ic10 调试会话（超时则明确失败）
 *
 * @param folder 工作区文件夹
 * @param extra 额外的 launch 配置项
 * @return 已启动的调试会话
 *
 * @else
 * @brief Start an ic10 debug session (failing loudly on timeout)
 *
 * @param folder Workspace folder
 * @param extra Additional launch configuration entries
 * @return The started debug session
 *
 * @endif
 * */
function startDebugSession(
    folder: vscode.WorkspaceFolder,
    extra: Record<string, unknown>
): Promise<vscode.DebugSession> {
    return new Promise((resolve, reject) => {
        let subscription: vscode.Disposable | undefined;

        const timer = setTimeout(() => {
            subscription?.dispose();
            reject(new Error("调试会话未在 30s 内启动 / the debug session did not start within 30s"));
        }, 30_000);

        const settle = (action: () => void) => {
            clearTimeout(timer);
            subscription?.dispose();
            action();
        };

        subscription = vscode.debug.onDidStartDebugSession(session => {
            if (session.type !== "ic10") return;

            settle(() => resolve(session));
        });

        void vscode.debug
            .startDebugging(folder, {
                // 用例自带的项在前，必需项在后：既保留用例的 `program`/显式运行时项，也防止覆盖必需项
                // Case-provided entries first, required ones last: keeps `program` and the explicit runtime
                // key while preventing the required fields from being overridden
                ...extra,
                type: "ic10",
                request: "launch",
                name: "端到端测试 / e2e",
                stopOnEntry: true
            })
            .then(
                started => {
                    if (!started) settle(() => reject(new Error("startDebugging 返回 false / startDebugging returned false")));
                },
                error => settle(() => reject(error))
            );
    });
}
