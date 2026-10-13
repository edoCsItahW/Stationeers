/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @summary 生成 SEA 打包所需的 sea-config.json
 *
 * @summary Generates the sea-config.json used by the SEA build
 *
 * @desc 开启 `"useVfs": true` 后，`assets` 会被暴露成只读虚拟文件系统，VFS 内的 `require()`
 *       支持 `node_modules` 查找 —— 于是 `@ic10/metadata` 那处拼接出来的动态 `require`、以及
 *       `require.resolve("@ic10/compiler/src/stdLib.ic")` 的读取都能正常工作，不必再手工 patch bundle。
 *       因此这里把三个运行时外部包**原样**列进 `assets`（键与值都是相对扩展根目录的 POSIX 路径），
 *       并交给脚本自动扫描，避免手写几十条。
 *
 * @desc With `"useVfs": true` the `assets` are exposed as a read-only virtual file system whose `require()`
 *       supports `node_modules` lookups — so both the computed `require` inside `@ic10/metadata` and the
 *       `require.resolve("@ic10/compiler/src/stdLib.ic")` read work without patching the bundle. This script
 *       therefore lists the three runtime packages verbatim (keys and values are POSIX paths relative to the
 *       extension root) and scans them instead of hard-coding dozens of entries.
 *
 * @remarks 键必须与 VFS 内的路径一致（`node_modules/...`），否则 `require()` 找不到；
 *          相对路径按**当前工作目录**解析，所以要在扩展根目录执行（`pnpm run sea:config`）。
 *
 * @remarks Keys must match the paths inside the VFS (`node_modules/...`) or `require()` cannot find them.
 *          Relative paths resolve against the current working directory, so run this from the extension root
 *          (`pnpm run sea:config`).
 * */
import { readdirSync, statSync, writeFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const root = join(dirname(fileURLToPath(import.meta.url)), "..");
const assetManifest = "packages/server/sea-config.json";

/** @type {string[]} 需要随可执行文件发布的入口（目录会被递归展开，路径一律用 POSIX 分隔符） */
const entries = [
    // 编译器：package.json 供解析、stdLib.ic 运行时被读取、.node 是原生模块
    "node_modules/@ic10/compiler/package.json",
    "node_modules/@ic10/compiler/src/stdLib.ic",
    "node_modules/@ic10/compiler/src/ic10c-node.node",

    // 运行时：原生模块（hover 的 HASH / STR）
    "node_modules/@ic10/runtime/package.json",
    "node_modules/@ic10/runtime/src/ic10r-node.node",

    // 元数据：整棵 dist 都要（动态 require 会指向其中的子路径）
    "node_modules/@ic10/metadata/package.json",
    "node_modules/@ic10/metadata/dist"
];

/**
 * @summary 展开一个入口为文件列表
 *
 * @param entry 相对扩展根目录的 POSIX 路径
 * @return 该入口下的所有文件（目录会递归展开）
 * */
function collect(entry) {
    const absolute = join(root, entry);

    if (!statSync(absolute).isDirectory()) return [entry];

    return readdirSync(absolute, { withFileTypes: true }).flatMap(child => collect(`${entry}/${child.name}`));
}

const assets = {};

for (const entry of entries) {
    let files;

    try {
        files = collect(entry);
    } catch (error) {
        throw new Error(`缺少打包所需的资源 ${entry}：请先在扩展根目录执行 pnpm install。（${error.message}）`);
    }

    for (const file of files) assets[file] = file;
}

const config = {
    main: "./packages/server/bundle.js",
    output: "./packages/server/ic10-lsp.exe",
    disableExperimentalSEAWarning: true,
    useVfs: true,
    assets
};

writeFileSync(join(root, assetManifest), `${JSON.stringify(config, null, 4)}\n`);

const natives = Object.keys(assets).filter(file => file.endsWith(".node"));

console.log(`[sea] ${assetManifest} 已生成：${Object.keys(assets).length} 个 assets`);
console.log(`[sea] 启动时会解包并 dlopen 的原生模块：${natives.join("、")}`);
