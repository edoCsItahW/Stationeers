/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file completion.test.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:38
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";

import { completeAtReady, insertedTexts, openFixture, positionAfter, positionAtLineStart, summarize } from "./helpers";


/**
 * @if zh
 * @summary 端到端补全测试
 *
 * @details 覆盖"用户敲下按键 → 语言客户端 → 语言服务端 → 候选回到编辑器"的整条链路：不直接调用
 *          服务端模块（那是集成测试的粒度），而是走 `vscode.executeCompletionItemProvider`，
 *          因此客户端注册、LSP 往返、服务端分派与提供器全都在测。
 *
 * @details 用例按**等价类**划分，并各取一个**边界**：
 *          - 类型提示的取值：设备类型名（正例）/ 枚举类型名（不该出现）；
 *          - 类型提示的取值：显示名（本地化名与英文名相同，只给一套）与插入文本；
 *          - `HASH("…")` 的字符串：空串（边界）/ 闭引号之后（负例）/ 非 `HASH` 的 `STR`（负例）；
 *          - 语句头部（正例）与类型提示里的标签名（正例），后者同时作为"语言服务端确实在应答"的哨兵。
 *
 * @details 夹具见 `packages/client/testFixture/completion.ic`，锚点用文本查找而不是行号，以免调整
 *          夹具行序后失效。注意类型提示的写法：`#:` 必须与语句**同行**且在其后
 *          （`alias hinted r1 #: @type `）。
 *
 * @else
 * @summary End-to-end completion tests
 *
 * @details They cover the whole "keystroke → language client → language server → candidates back in the
 *          editor" chain: instead of calling server modules directly (that is integration-test
 *          granularity) they go through `vscode.executeCompletionItemProvider`, so client registration,
 *          the LSP round trip, server dispatch and the providers are all exercised.
 *
 * @details Cases are split by **equivalence class**, each with a **boundary**:
 *          - the value of a type hint: device type names (positive) / enum type names (must not appear);
 *          - the value of a type hint: the display name (localized and English coincide, so only one is
 *            shown) and the inserted text;
 *          - the string of `HASH("…")`: empty (boundary) / past the closing quote (negative) / the
 *            non-`HASH` `STR` (negative);
 *          - a statement head (positive) and hint tag names (positive), the latter doubling as a sentinel
 *            that the language server really answers.
 *
 * @details Fixture: `packages/client/testFixture/completion.ic`; anchors are located by text rather than
 *          line numbers so reordering the fixture cannot silently break them. Note the type hint syntax:
 *          `#:` must trail its statement on the same line (`alias hinted r1 #: @type `).
 *
 * @endif
 * */
const FIXTURE = "completion.ic";

/**
 * @if zh
 * @summary 夹具锚点：定位到具体用例所在的位置
 *
 * @else
 * @summary Fixture anchors pointing at each case
 *
 * @endif
 * */
const ANCHOR = {
    /** `#: @type ` 的值槽位（在标签之后的空格处） */
    typeValue: "#: @type ",
    /** 空字符串的 `HASH("`，默认偏移落在开引号与闭引号之间（边界：还没有输入任何字符） */
    hashEmpty: 'HASH("',
    /** 完整字符串的 `HASH("StructureLiquidVolumePump")`，默认偏移落在闭引号之后，用于验证那里不该再给候选 */
    hashNamed: 'HASH("StructureLiquidVolumePump")',
    /** 非 `HASH` 的字符串（`STR("`），负例 */
    strEmpty: 'STR("',
    /** 语句头部（指令关键字） */
    statementHead: "lbn",
    /** 类型提示里的标签名（光标落在 `@t` 之后） */
    hintTag: "@type"
} as const;

/** @if zh 带括号的原始型号名：用它证明给的是型号名而不是压平后的类型名 @else Raw parenthesized model name: proves model names are offered, not collapsed type names @endif */
const PARENTHESIZED_MODEL = "StructureTransformerMedium(Reversed)";

/** @if zh 用作显示名断言的设备 @else The device used for display-name assertions @endif */
const DEVICE_TYPE = "ApplianceMicrowave";

suite("#: @type 取值补全 / @type value completion", () => {
    test("给出设备类型名，且不给出枚举类型名 / offers device type names, not enum type names", async () => {
        const document = await openFixture(FIXTURE);
        const items = await completeAtReady(document, positionAfter(document, ANCHOR.typeValue));
        const inserted = insertedTexts(items);

        assert.ok(
            inserted.includes(DEVICE_TYPE),
            `缺少设备类型名 / missing device type name: ${DEVICE_TYPE}（${summarize(items)}）`
        );
        assert.ok(
            !inserted.includes("LogicType"),
            `枚举类型名不应出现在 \`@type\` 的取值里 / enum type names must not appear（${summarize(items)}）`
        );
    });

    test("显示名只给一套、不重复，插入的仍是类型名 / shows one display name and still inserts the type name", async () => {
        const document = await openFixture(FIXTURE);
        const items = await completeAtReady(document, positionAfter(document, ANCHOR.typeValue));
        const device = items.find(item => item.insertText === DEVICE_TYPE);

        assert.ok(device, `缺少候选 / missing candidate: ${DEVICE_TYPE}（${summarize(items)}）`);

        // 服务端的 `labelDetails.description` 到了扩展 API 是 `CompletionItemLabel.description`
        // The server's `labelDetails.description` surfaces here as `CompletionItemLabel.description`
        const label: vscode.CompletionItemLabel =
            typeof device.label === "string" ? { label: device.label } : device.label;

        assert.strictEqual(
            label.label,
            "Microwave",
            "显示名应是该设备在 en-us 下的名字（夹具不设 ic10.language，语言服务端默认 en-us）"
        );
        assert.strictEqual(
            label.description,
            undefined,
            "本地化名与英文名相同时不该重复显示（英文环境下自然只有一套）"
        );
        assert.strictEqual(device.detail, DEVICE_TYPE, "详情位应是实际插入的类型名");
        assert.ok(device.filterText?.includes(DEVICE_TYPE), "类型名必须参与匹配 / the type name must take part in matching");
    });
});

suite('HASH("…") 型号名补全 / model name completion inside HASH', () => {
    test("空字符串里给出原始型号名 / offers raw model names inside an empty string", async () => {
        const document = await openFixture(FIXTURE);
        const items = await completeAtReady(document, positionAfter(document, ANCHOR.hashEmpty));
        const inserted = insertedTexts(items);

        assert.ok(
            inserted.includes(PARENTHESIZED_MODEL),
            `缺少带括号的原始型号名 / missing raw parenthesized model name: ${PARENTHESIZED_MODEL}（${summarize(items)}）`
        );
        assert.ok(
            inserted.includes(DEVICE_TYPE),
            `缺少设备型号名 / missing device model name: ${DEVICE_TYPE}（${summarize(items)}）`
        );
    });

    test("闭引号之后不再给候选 / offers nothing past the closing quote", async () => {
        const document = await openFixture(FIXTURE);
        const items = await completeAtReady(document, positionAfter(document, ANCHOR.hashNamed));

        assert.ok(
            !insertedTexts(items).includes(PARENTHESIZED_MODEL),
            "字符串之外不该再给型号名 / model names must not be offered outside the string"
        );
    });

    test("非 HASH 的字符串不给候选 / offers nothing inside a non-HASH string", async () => {
        const document = await openFixture(FIXTURE);
        const items = await completeAtReady(document, positionAfter(document, ANCHOR.strEmpty));

        assert.ok(
            !insertedTexts(items).includes(DEVICE_TYPE),
            '`STR("…")` 不该给型号名 / `STR("…")` must not offer model names'
        );
    });
});

suite("既有补全路径 / pre-existing completion paths", () => {
    // 指令关键字与提示标签都走既有的服务端分支：它们既是对既有功能的覆盖，也是"语言服务端确实
    // 在应答"的哨兵——若它们失败，问题在服务端整体而不是新增的补全分支。
    // Instruction keywords and hint tags go through pre-existing server branches: they cover existing
    // behavior and act as a sentinel that the server really answers — if they fail, the problem is the
    // server as a whole rather than the new completion branches.
    test("语句头部给出指令关键字 / offers instruction keywords at a statement head", async () => {
        const document = await openFixture(FIXTURE);
        const items = await completeAtReady(document, positionAtLineStart(document, ANCHOR.statementHead));

        assert.ok(insertedTexts(items).includes("lbn"), `缺少指令关键字 lbn（${summarize(items)}）`);
    });

    test("类型提示里给出标签名 / offers hint tag names inside a type hint", async () => {
        const document = await openFixture(FIXTURE);
        const items = await completeAtReady(document, positionAfter(document, ANCHOR.hintTag, 2));

        assert.ok(insertedTexts(items).includes("@type"), `缺少标签 @type（${summarize(items)}）`);
        assert.ok(insertedTexts(items).includes("@default"), `缺少标签 @default（${summarize(items)}）`);
    });
});
