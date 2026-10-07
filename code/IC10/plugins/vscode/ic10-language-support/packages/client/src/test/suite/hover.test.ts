/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file hover.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:31
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";

import { hoverText, openFixture, positionAfter, positionAtLineStart } from "./helpers";


/**
 * @if zh
 * @summary 端到端悬停测试
 *
 * @details 断言以**范围**为主：悬停文案是本地化的（随 `ic10.language` 变化），不适合写死字句；
 *          而"悬停范围正好覆盖某个标识符"既与语言无关，又能证明这是我们作用在该节点上的悬停。
 *
 * @details 等价类与边界：命中的指令名（正例，范围覆盖三个字符）、文件末尾语句范围之外（负例，不该有任何
 *          悬停**内容**：服务端找不到语句时回一个空悬停对象，见用例内的说明）。
 *
 * @else
 * @summary End-to-end hover tests
 *
 * @details Assertions are **range**-based: the hover text is localized (it follows `ic10.language`) and
 *          unsuitable for literal assertions, whereas "the hover range covers exactly this identifier" is
 *          language independent and proves it is our hover on that node.
 *
 * @details Equivalence classes and boundaries: a hit on an instruction name (positive, the range covers
 *          the three characters) and a position outside every statement at the end of the file (negative,
 *          no hover **content** at all: the server answers with an empty hover object when it finds no
 *          statement, see the case's comment).
 *
 * @endif
 * */
suite("悬停 / hover", () => {
    test("指令名上的悬停覆盖该指令名 / the hover on an instruction name covers it", async () => {
        const document = await openFixture("completion.ic");
        const start = positionAtLineStart(document, "lbn");
        const hovers = await vscode.commands.executeCommand<vscode.Hover[]>(
            "vscode.executeHoverProvider",
            document.uri,
            new vscode.Position(start.line, start.character + 1)
        );

        assert.ok((hovers ?? []).length > 0, "悬停没有任何应答 / hover returned nothing");

        const hover = hovers[0];

        assert.ok(hover.range, "悬停应带上覆盖指令名的范围 / hover should carry the keyword range");
        assert.strictEqual(hover.range.start.line, start.line, "悬停范围起始行不对 / wrong hover range line");
        assert.strictEqual(
            hover.range.start.character,
            start.character,
            "悬停范围应始于指令名 / the hover range should start at the instruction name"
        );
        assert.strictEqual(
            hover.range.end.character,
            start.character + "lbn".length,
            "悬停范围应覆盖整个指令名 / the hover range should cover the whole instruction name"
        );
        assert.ok(hoverText(hover).length > 0, `悬停内容为空 / empty hover content: ${JSON.stringify(hover.contents)}`);
    });

    test("语句范围之外没有悬停内容（边界）/ no hover content outside every statement (boundary)", async () => {
        const document = await openFixture("completion.ic");

        // 负例取**文件末尾**的空行：它落在所有语句的范围之外，服务端找不到语句时回 `{ contents: [] }`。
        // 注意：语句**之间**的空行不算——服务端按行查找语句，那种空行会落到下一条语句上（实测行为，
        // 见提交说明），所以不能拿它当负例。
        // The negative case is the blank line at the **end of the file**: it lies outside every statement, so
        // the server finds none and answers `{ contents: [] }`. Note that a blank line *between* statements
        // does not qualify — the server looks statements up by line and such a line resolves to the next
        // statement (measured behavior, see the commit message).
        const last = document.lineCount - 1;

        assert.strictEqual(
            document.lineAt(last).text.trim(),
            "",
            "夹具末行应是空行 / the fixture must end with a blank line"
        );

        const hovers = await vscode.commands.executeCommand<vscode.Hover[]>(
            "vscode.executeHoverProvider",
            document.uri,
            new vscode.Position(last, 0)
        );

        // 空悬停（`contents: []`）用户看不见，有内容的悬停才是缺陷
        // An empty hover is invisible to the user; a hover with content here would be the defect
        const withContent = (hovers ?? []).filter(hover => hoverText(hover).length > 0);

        assert.deepStrictEqual(
            withContent.map(hoverText),
            [],
            `语句范围之外不该有悬停内容 / no hover content expected outside every statement: ${JSON.stringify(withContent)}`
        );
    });
});
