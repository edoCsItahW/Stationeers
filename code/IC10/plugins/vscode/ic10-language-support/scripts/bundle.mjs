/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @summary 把 client / server 各打包成单个 CJS 文件
 *
 * @summary Bundle client / server each into a single CJS file
 *
 * @desc pnpm 的 node_modules 是符号链接农场，而符号链接无法在 VSIX 的压缩与安装过程中存活
 * （压缩工具与 VS Code 解压器都会把它退化成一个空目录），因此扩展不能依赖各 package 下的
 * node_modules，必须在打包阶段把依赖内联进 bundle。
 *
 * @desc pnpm's node_modules is a symlink farm, and symlinks do not survive the VSIX zip/install
 * round-trip (both archive tools and the VS Code extractor degrade them into empty directories).
 * The extension therefore must not depend on the per-package node_modules; dependencies are
 * inlined into the bundles at package time instead.
 *
 * 保持 external 的依赖 / Dependencies kept external:
 * - `vscode`：由扩展宿主注入 / injected by the extension host
 * - `@ic10/compiler`：包主入口是原生模块 `src/ic10c-node.node`，且 `src/stdLib.ic` 在运行时
 *   通过 `require.resolve` 读取，必须保持真实文件 / native main entry plus a runtime
 *   `require.resolve` of `src/stdLib.ic`
 * - `ic10r-node`：原生模块 / native addon
 *
 * 用法 / Usage:
 *   node scripts/bundle.mjs            一次性构建 / one-shot build
 *   node scripts/bundle.mjs --watch    监听重建 / rebuild on change
 * */

import { build, context } from "esbuild";
import { existsSync, readFileSync, rmSync, statSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const root = join(dirname(fileURLToPath(import.meta.url)), "..");

/**
 * 必须保持 external、随 VSIX 一起发布的运行时依赖 @type {string[]}
 *
 * - `@ic10/compiler`：包主入口就是原生模块 `src/ic10c-node.node`，且 `src/stdLib.ic`
 *   在运行时经 `require.resolve` 读取
 * - `ic10r-node`：原生模块
 * - `@ic10/metadata`：`astHelper.ts` 的 `DescriptionSolver.link()` 用拼接出来的 specifier
 *   （如 `@ic10/metadata/locals/enums`）做动态 `require`，esbuild 无法静态解析，只能原样保留。
 *   因此它必须同时是**根** package.json 的依赖（vsce 只收集根依赖，这样它才会随 VSIX 发布），
 *   且根上的版本规格需与 packages/server 一致，见 assertMetadataVersionMatches()。
 * */
const externalRuntime = ["@ic10/compiler", "@ic10/compiler/*", "ic10r-node", "@ic10/metadata", "@ic10/metadata/*"];

/**
 * 必须随 VSIX 一起发布的运行时外部依赖。
 *
 * 它们是 external，esbuild 不会内联、也不会校验其存在性；而服务端的 `try/catch` 会把
 * `MODULE_NOT_FOUND` 吞掉，最终表现为"扩展装上了却什么都没运行"。因此这里显式校验，
 * 让缺失依赖在打包阶段就失败，而不是产出一个静默损坏的包。
 *
 * 注：pnpm 把这些包装成指向 `node_modules/.pnpm` 的符号链接，vsce 以 `npm ls --parseable`
 * 报告的路径（即上述符号链接路径）为根收集文件，落进 VSIX 的是 `node_modules/@ic10/compiler/...`
 * 这类真实路径，符号链接本身不参与打包，故无需在此实体化。
 *
 * @type {{name: string, file: string}[]}
 * */
const requiredRuntimeFiles = [
    { name: "@ic10/compiler", file: join(root, "node_modules", "@ic10", "compiler", "src", "ic10c-node.node") },
    { name: "@ic10/metadata", file: join(root, "node_modules", "@ic10", "metadata", "dist", "locals", "enums.js") },
    { name: "ic10r-node", file: join(root, "node_modules", "ic10r-node", "src", "ic10r-node.node") }
];

/** 打包目标 @type {{name: string, entry: string, outfile: string, external: string[]}[]} */
const targets = [
    {
        name: "client",
        entry: join(root, "packages", "client", "src", "extension.ts"),
        outfile: join(root, "packages", "client", "dist", "extension.js"),
        external: ["vscode"]
    },
    {
        name: "server",
        entry: join(root, "packages", "server", "src", "server", "server.ts"),
        outfile: join(root, "packages", "server", "dist", "server.js"),
        external: externalRuntime
    }
];

/** 生成单个目标的 esbuild 配置 @param {{entry: string, outfile: string, external: string[]}} target */
function options({ entry, outfile, external }) {
    return {
        entryPoints: [entry],
        outfile,
        bundle: true,
        platform: "node",
        format: "cjs",
        target: "node20",
        sourcemap: true,
        external,
        logLevel: "info"
    };
}

/**
 * 校验 bundle 的 external 依赖确实存在
 *
 * @throws 当原生依赖缺失（通常是没跑过 pnpm install）时抛出
 * */
function assertRuntimeDependencies() {
    for (const { name, file } of requiredRuntimeFiles) {
        if (!existsSync(file)) {
            throw new Error(`缺少运行时依赖 ${name}（找不到 ${file}）。请先在扩展目录执行 pnpm install。`);
        }

        console.log(`[bundle] ${name} -> ${statSync(file).size} bytes`);
    }
}

/**
 * 校验 `@ic10/metadata` 的根副本与 server 解析到的版本一致
 *
 * @throws 当两者解析到不同版本时抛出
 *
 * @remarks 根 package.json 的依赖规格是给 vsce 用的（它只收集根依赖，好让这个包随 VSIX 发布），
 * 而 server 解析到的那份才决定了开发期看到的数据。两者一旦解析出不同版本，dev 与打包产物就会
 * 用上两份不同的元数据，且症状是静默的数据错乱而非报错，所以这里直接失败。
 *
 * @remarks The root package.json specifier exists so vsce (which only collects root dependencies)
 * ships the package with the VSIX, while the copy resolved from `packages/server` is what
 * development actually sees. If they resolve to different versions, dev and the packaged artifact
 * silently use two different metadata sets, so fail instead.
 * */
function assertMetadataVersionMatches() {
    const read = file => JSON.parse(readFileSync(file, "utf8")).version;
    const at = dir => join(dir, "node_modules", "@ic10", "metadata", "package.json");

    const packaged = read(at(root));
    const resolved = read(at(join(root, "packages", "server")));

    if (packaged !== resolved) {
        throw new Error(
            `@ic10/metadata 版本不一致：根 node_modules 是 ${packaged}，packages/server 解析到 ${resolved}。` +
                `请把根 package.json 的 "@ic10/metadata" 规格改成与 packages/server 一致，然后重新 pnpm install。`
        );
    }

    console.log(`[bundle] @ic10/metadata -> ${packaged}`);
}

if (process.argv.includes("--watch")) {
    for (const target of targets) {
        const ctx = await context(options(target));
        rmSync(dirname(target.outfile), { recursive: true, force: true });

        await ctx.watch();
        console.log(`[bundle] watching ${target.name} -> ${target.outfile}`);
    }
} else {
    for (const target of targets) {
        rmSync(dirname(target.outfile), { recursive: true, force: true });

        await build(options(target));
        console.log(`[bundle] ${target.name} -> ${target.outfile}`);
    }

    assertRuntimeDependencies();
    assertMetadataVersionMatches();
}
