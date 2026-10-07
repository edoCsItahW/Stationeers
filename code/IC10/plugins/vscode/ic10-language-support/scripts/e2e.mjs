/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file e2e.mjs
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 00:23
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 *
 * @if zh
 * @summary 端到端测试入口（跨平台）
 *
 * @details 先确认扩展产物与测试产物都在，再拉起 VS Code 运行套件。之所以在这里检查产物而不是
 *          "顺手编译一下"：编译要走 `tsc -b` 与 esbuild，跑测试的人应当明确知道自己测的是哪一版
 *          产物；缺产物时直接失败并给出该执行的命令，避免静默地测一个旧包。
 *
 * @details 路径全部由脚本自身位置推出（不依赖 `pwd`），因此在 Windows、Linux 与 macOS 上一致；
 *          环境变量 `CODE_TESTS_PATH` / `CODE_TESTS_WORKSPACE` 仍会被尊重（见 runTest.ts 与 suite/index.ts）。
 *
 * @else
 * @summary End-to-end test entry (cross-platform)
 *
 * @details It first makes sure both the extension artifacts and the compiled tests exist, then launches
 *          VS Code to run the suite. The check is explicit rather than "just compile something": whoever
 *          runs the tests should know which build is under test, so a missing artifact fails loudly with
 *          the command to run instead of silently testing a stale bundle.
 *
 * @details Every path is derived from this script's own location (never `pwd`), so it behaves the same on
 *          Windows, Linux and macOS; `CODE_TESTS_PATH` / `CODE_TESTS_WORKSPACE` are still honored
 *          (see runTest.ts and suite/index.ts).
 *
 * @endif
 * */
import { spawnSync } from "node:child_process";
import { existsSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const root = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const runner = join(root, "packages", "client", "out", "test", "runTest.js");

const required = [
    runner,
    join(root, "packages", "client", "dist", "extension.js"),
    join(root, "packages", "server", "dist", "server.js")
];
const missing = required.filter(file => !existsSync(file));

if (missing.length) {
    console.error("[e2e] 缺少产物，请先执行 `npm run compile && npm run bundle` / missing artifacts, run `npm run compile && npm run bundle` first:");

    for (const file of missing) console.error(`  - ${file}`);

    process.exit(1);
}

const result = spawnSync(process.execPath, [runner], {
    stdio: "inherit",
    env: {
        ...process.env,
        CODE_TESTS_PATH: join(root, "packages", "client", "out", "test"),
        CODE_TESTS_WORKSPACE: join(root, "packages", "client", "testFixture")
    }
});

process.exit(result.status ?? 1);
