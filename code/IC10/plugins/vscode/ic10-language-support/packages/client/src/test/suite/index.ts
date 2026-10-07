/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file index.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 00:21
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import Mocha from "mocha";
import * as fs from "fs";
import * as path from "path";


/**
 * @if zh
 * @summary 套件入口：收集并运行 out/test 下所有 `*.test.js`
 *
 * @details 用例文件由 `tsc -b` 从 `packages/client/src/test/suite` 编译而来，因此这里只需递归找文件。
 *          用 Node 自带的递归遍历（`fs.readdirSync(..., { recursive: true })`）而不是 `glob`，
 *          少一个依赖、也避开 ESM/CJS 双入口的类型摩擦。
 *
 * @details 超时给得比较宽（120s）：第一个补全请求会让语言服务端把标准库链接进类型表，
 *          冷启动比后续请求慢得多。
 *
 * @return 全部用例通过时 resolve，否则 reject（携带失败数量）
 *
 * @else
 * @summary Suite entry: collect and run every `*.test.js` under out/test
 *
 * @details Test files are compiled by `tsc -b` from `packages/client/src/test/suite`, so a recursive
 *          search is all that is needed here. It uses Node's own recursive readdir
 *          (`fs.readdirSync(..., { recursive: true })`) instead of `glob`, which drops a dependency and
 *          avoids the ESM/CJS dual-entry type friction.
 *
 * @details The timeout is generous (120s): the first completion request makes the language server link
 *          the standard library into the type table, which is far slower than later requests.
 *
 * @return Resolves when every test passes, otherwise rejects with the failure count
 *
 * @endif
 * */
export function run(): Promise<void> {
    const mocha = new Mocha({ ui: "tdd", color: true, timeout: 120_000 });

    const testsRoot = process.env.CODE_TESTS_PATH ?? path.resolve(__dirname, "..");

    const files = fs
        .readdirSync(testsRoot, { recursive: true })
        .map(file => file.toString())
        .filter(file => file.endsWith(".test.js"));

    for (const file of files) mocha.addFile(path.resolve(testsRoot, file));

    return new Promise((resolve, reject) => {
        mocha.run((failures: number) =>
            failures
                ? reject(new Error(`${failures} 个用例失败 / ${failures} test(s) failed`))
                : resolve()
        );
    });
}
