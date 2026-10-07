/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file runTest.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 00:20
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { runTests } from "@vscode/test-electron";
import * as path from "path";


/**
 * @if zh
 * @summary 端到端测试入口：拉起一个真实的 VS Code，装入本扩展并运行 mocha 套件
 *
 * @details 端到端测试要验证的正是"扩展宿主 + 语言客户端 + 语言服务端"整条链路，因此必须跑在真实的
 *          VS Code 里：由 `@vscode/test-electron` 下载并启动一个 VS Code 实例，把插件根目录作为
 *          `extensionDevelopmentPath`（根 `package.json` 在这里）装入，再运行编译后的套件。
 *
 * @details 编译产物必须在运行前就绪（`npm run compile && npm run bundle`，由 `scripts/e2e.mjs` 负责
 *          检查并给出提示），套件本身不做编译。
 *
 * @else
 * @summary End-to-end test entry: launch a real VS Code, load this extension and run the mocha suite
 *
 * @details An end-to-end test verifies the whole "extension host + language client + language server"
 *          chain, so it has to run inside a real VS Code: `@vscode/test-electron` downloads and launches
 *          one, loads the plugin root as the `extensionDevelopmentPath` (the root `package.json` lives
 *          there) and runs the compiled suite.
 *
 * @details The compiled artifacts must exist beforehand (`npm run compile && npm run bundle`, checked
 *          with a hint by `scripts/e2e.mjs`); the suite itself does not compile anything.
 *
 * @endif
 * */
async function main() {
    // 被测扩展的开发目录：插件根（根 package.json 在 `packages` 的上一级）
    const extensionDevelopmentPath = path.resolve(__dirname, "../../../..");

    // 套件入口：与 runTest 一起编译到 out/test 下的 suite/index.js
    const extensionTestsPath = path.resolve(__dirname, "./suite/index");

    // 打开的工作区：testFixture（可用 CODE_TESTS_WORKSPACE 覆盖，例如临时指向别处的 .ic 工程）
    const workspace = process.env.CODE_TESTS_WORKSPACE ?? path.resolve(__dirname, "../../testFixture");

    try {
        await runTests({
            extensionDevelopmentPath,
            extensionTestsPath,
            // 关掉其它已安装扩展，避免无关扩展干扰断言（被测扩展由 extensionDevelopmentPath 装入）
            launchArgs: [workspace, "--disable-extensions"]
        });
    } catch (error) {
        console.error("端到端测试失败 / End-to-end tests failed:", error);
        process.exit(1);
    }
}

void main();
