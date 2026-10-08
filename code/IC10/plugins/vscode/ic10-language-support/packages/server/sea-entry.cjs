/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @summary SEA 入口：解包原生模块后加载语言服务端
 *
 * @summary SEA entry: unpack the native addons, then load the language server
 *
 * @desc 打包（`pnpm run sea:bundle`）后由 `node --build-sea` 注入可执行文件。因为开启了 `useVfs`，
 *       注入的主脚本从虚拟文件系统根目录执行，`__dirname` 指向 VFS 内，所以下面按包内相对路径读取
 *       assets；而 `@ic10/metadata` 这类纯 JS 依赖交给 VFS 的模块加载器解析，不需要任何 patch。
 *
 * @desc Bundled by `pnpm run sea:bundle` and injected by `node --build-sea`. With `useVfs` enabled the
 *       injected main script runs from the root of the virtual file system, so `__dirname` points inside the
 *       VFS and the assets are read by their in-package relative paths. Pure-JS dependencies such as
 *       `@ic10/metadata` are resolved by the VFS module loader instead, so they need no patching.
 *
 * @desc 唯一绕不开的限制：`.node` 不能从 VFS 加载（`process.dlopen()` 需要真实文件系统上的文件，
 *       这是 Node.js 文档明确写下的），因此先把它们写到临时目录再 dlopen，并把这两个包名重定向过去。
 *
 * @desc The one limitation that cannot be avoided: a `.node` cannot be loaded from the VFS
 *       (`process.dlopen()` needs a file on the real file system, as Node.js documents), so both addons are
 *       written to a temporary directory first, dlopen'ed, and their package names are redirected to the
 *       result.
 * */
const fs = require("node:fs");
const os = require("node:os");
const path = require("node:path");
const Module = require("node:module");

/**
 * @summary 载入随 VFS 发布的原生模块
 *
 * @param relative 相对 VFS 根目录（即包内）的路径
 * @return 该原生模块的 exports
 * @throws 当 assets 里缺少该文件时抛出（提示重新生成 sea-config.json）
 * */
function loadAddon(relative) {
    const source = path.join(__dirname, relative);

    if (!fs.existsSync(source)) {
        throw new Error(
            `找不到 ${relative}：SEA 的 assets 里缺少这个文件。` +
                `请在扩展根目录重新运行 pnpm run sea:config，然后重新打包。`
        );
    }

    const dir = fs.mkdtempSync(path.join(os.tmpdir(), "ic10-lsp-"));
    const target = path.join(dir, path.basename(relative));

    fs.writeFileSync(target, fs.readFileSync(source));

    const addon = { exports: {} };

    process.dlopen(addon, target);

    // Windows 上已加载的 .node 会被锁定，删不掉也不影响（退出后临时目录由系统回收）
    process.on("exit", () => {
        try {
            fs.rmSync(dir, { recursive: true, force: true });
        } catch {
            /* 忽略：临时目录会由系统清理 */
        }
    });

    return addon.exports;
}

const compiler = loadAddon("node_modules/@ic10/compiler/src/ic10c-node.node");
const runtime = loadAddon("node_modules/@ic10/runtime/src/ic10r-node.node");

// 原生模块无法从 VFS 加载，所以把这两个包名重定向到刚载入的 API（CJS 里没有公开的等价钩子，
// 只能接管 Module._load）；其余依赖（@ic10/metadata、vscode-languageserver 等）走 VFS 正常解析。
const load = Module._load;

Module._load = function (request, parent, isMain) {
    if (request === "@ic10/compiler") return compiler;
    if (request === "@ic10/runtime") return runtime;

    return load.apply(this, arguments);
};

require("./dist/server.js");
