/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @summary 开发期监听：tsc 增量类型检查与 esbuild 增量打包并行
 *
 * @summary Development watcher: tsc incremental type checking alongside esbuild bundling
 *
 * @desc F5 调试加载的是 bundle（package.json 的 main 指向 packages/client/dist/extension.js），
 * 因此只跑 `tsc -b -w` 不够，必须同时让 esbuild 重建 bundle。
 *
 * @desc F5 loads the bundle (package.json main points at packages/client/dist/extension.js), so
 * `tsc -b -w` alone is not enough; esbuild must rebuild the bundles at the same time.
 *
 * 用法 / Usage: npm run watch
 * */

import { spawn } from "node:child_process";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const scripts = dirname(fileURLToPath(import.meta.url));
const root = join(scripts, "..");

/** @type {import("node:child_process").ChildProcess[]} */
const children = [
    // tsc：类型检查与增量诊断 / type checking and incremental diagnostics
    spawn("tsc", ["-b", "-w"], { cwd: root, stdio: "inherit", shell: true }),
    // esbuild：产出 F5 实际加载的 bundle / produces the bundle F5 actually loads
    spawn(process.execPath, [join(scripts, "bundle.mjs"), "--watch"], { cwd: root, stdio: "inherit" })
];

for (const child of children) child.on("exit", code => process.exit(code ?? 0));
