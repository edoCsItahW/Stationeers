/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file helpers.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/06 11:30
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import * as assert from "assert";
import * as vscode from "vscode";


/** @if zh 夹具文档必须以此语言打开，否则语言功能不会生效 @else Fixture documents must open with this language, otherwise no language feature applies @endif */
export const LANGUAGE = "ic10";

/** @if zh 被测扩展的标识（publisher.name）@else Identifier of the extension under test (publisher.name) @endif */
export const EXTENSION_ID = "edocsitahw.ic10";

/**
 * @if zh
 * @brief 先激活扩展（含语言客户端就绪），再打开文档
 *
 * @details 这一步是端到端测试能成立的前提：文档必须在语言客户端**启动之后**打开，客户端才会把
 *          `didOpen` 发给服务端；否则服务端没有这份文档的解析结果，补全/诊断/悬停只会退化成空结果，
 *          症状与"提供器写错了"极难区分。`activate()` 会等待语言客户端就绪，所以 await 它即可。
 *
 * @else
 * @brief Activate the extension (which awaits the language client) before opening any document
 *
 * @details This is what makes the end-to-end tests meaningful: documents must be opened **after** the
 *          language client started, otherwise the client never sends `didOpen` and the server has no
 *          parse result — completion, diagnostics and hover then degrade to empty results, a symptom
 *          barely distinguishable from a broken provider. `activate()` awaits the client, so awaiting it
 *          is enough.
 *
 * @endif
 * */
export async function activateExtension(): Promise<void> {
    const extension = vscode.extensions.getExtension(EXTENSION_ID);

    assert.ok(extension, `找不到被测扩展 / extension under test not found: ${EXTENSION_ID}`);

    await extension.activate();
}

/**
 * @if zh
 * @brief 打开夹具文档并校验语言
 *
 * @details 语言关联按理由 `.ic` 扩展名给出，但环境差异会让文档以别的语言打开；那时提供器根本不会被
 *          调用，候选恒为空。这里显式钉住并断言，把这类环境问题一次性排除。
 *
 * @param name 夹具文件名（`testFixture` 下）
 * @return 夹具文档
 *
 * @else
 * @brief Open a fixture document and assert its language
 *
 * @details The `.ic` extension should imply the language, but environment differences can open it with
 *          another one, in which case providers are never called and results are always empty. The
 *          language is therefore pinned and asserted here.
 *
 * @param name Fixture file name (under `testFixture`)
 * @return The fixture document
 *
 * @endif
 * */
export async function openFixture(name: string): Promise<vscode.TextDocument> {
    await activateExtension();

    const workspace = vscode.workspace.workspaceFolders?.[0];

    assert.ok(workspace, "端到端测试需要 testFixture 作为工作区 / testFixture must be the workspace");

    // 工作区**就是** testFixture（见 runTest.ts 的 launchArgs），所以夹具名直接拼在工作区根上
    // The workspace **is** testFixture (see launchArgs in runTest.ts), so the fixture name is joined to
    // the workspace root directly
    const uri = vscode.Uri.joinPath(workspace.uri, name);

    let document = await vscode.workspace.openTextDocument(uri);

    if (document.languageId !== LANGUAGE) document = await vscode.languages.setTextDocumentLanguage(document, LANGUAGE);

    assert.strictEqual(
        document.languageId,
        LANGUAGE,
        "夹具文档必须以 ic10 语言打开，否则语言功能不会生效 / the fixture must open as ic10"
    );

    await vscode.window.showTextDocument(document);

    return document;
}

/**
 * @if zh
 * @brief 锚点文本之后第 `offset` 个字符的位置
 *
 * @else
 * @brief The position `offset` characters after the anchor text
 *
 * @endif
 * */
export function positionAfter(document: vscode.TextDocument, anchor: string, offset = anchor.length): vscode.Position {
    const index = document.getText().indexOf(anchor);

    assert.ok(index >= 0, `夹具里找不到锚点 / anchor not found: ${anchor}`);

    return document.positionAt(index + offset);
}

/**
 * @if zh
 * @brief 锚点所在行的行首位置（"语句头部"场景）
 *
 * @else
 * @brief The start of the line the anchor sits on (the "statement head" case)
 *
 * @endif
 * */
export function positionAtLineStart(document: vscode.TextDocument, anchor: string): vscode.Position {
    const index = document.getText().indexOf(anchor);

    assert.ok(index >= 0, `夹具里找不到锚点 / anchor not found: ${anchor}`);

    return document.positionAt(index);
}

/**
 * @if zh
 * @brief 请求某个位置的补全候选（走真实的补全提供器命令）
 *
 * @else
 * @brief Request completions at a position (through the real completion provider command)
 *
 * @endif
 * */
export async function completeAt(
    document: vscode.TextDocument,
    position: vscode.Position
): Promise<vscode.CompletionItem[]> {
    const list = await vscode.commands.executeCommand<vscode.CompletionList>(
        "vscode.executeCompletionItemProvider",
        document.uri,
        position
    );

    return list.items;
}

/**
 * @if zh
 * @brief 等到语言服务端真的给出候选为止（上限 60s）
 *
 * @details 只重试"有没有答复"，候选**内容**仍由用例断言，因此不会掩盖真正的错误。
 *
 * @else
 * @brief Wait until the language server answers with candidates (up to 60s)
 *
 * @details Only "did it answer at all" is retried; the **content** is still asserted by the cases, so
 *          real mistakes are not masked.
 *
 * @endif
 * */
export async function completeAtReady(
    document: vscode.TextDocument,
    position: vscode.Position
): Promise<vscode.CompletionItem[]> {
    const deadline = Date.now() + 60_000;

    let items: vscode.CompletionItem[] = [];

    while (Date.now() < deadline) {
        items = await completeAt(document, position);

        if (items.length) return items;

        await delay(250);
    }

    return items;
}

/**
 * @if zh
 * @brief 候选摘要（放进断言失败信息，一眼分清"没候选"与"候选不对"）
 *
 * @else
 * @brief Candidate summary for assertion messages
 *
 * @endif
 * */
export function summarize(items: vscode.CompletionItem[]): string {
    const sample = items
        .slice(0, 5)
        .map(item => (typeof item.insertText === "string" ? item.insertText : item.label.toString()));

    return `候选 ${items.length} 条 / ${items.length} item(s)，样本 ${JSON.stringify(sample)}`;
}

/**
 * @if zh
 * @brief 取出候选实际插入的文本（片段与字符串都归一到字符串）
 *
 * @else
 * @brief Extract the text each candidate inserts (snippets normalized to strings)
 *
 * @endif
 * */
export function insertedTexts(items: vscode.CompletionItem[]): string[] {
    return items
        .map(item => (typeof item.insertText === "string" ? item.insertText : item.insertText?.value))
        .filter((text): text is string => !!text);
}

/**
 * @if zh
 * @brief 取出悬停的文本内容
 *
 * @details `MarkdownString` 的正文在 `value` 上，直接 `JSON.stringify` 只会得到 `{}`。
 *
 * @else
 * @brief Extract the text of a hover (`MarkdownString` keeps its text in `value`, not in JSON)
 *
 * @endif
 * */
export function hoverText(hover: vscode.Hover): string {
    const contents = Array.isArray(hover.contents) ? hover.contents : [hover.contents];

    return contents
        .map(content => (typeof content === "string" ? content : "value" in content ? content.value : ""))
        .join("\n");
}

/**
 * @if zh
 * @brief 把 LSP 风格的文本编辑应用到文本上（从后往前应用，避免偏移失效）
 *
 * @param document 编辑所针对的文档（用于把位置换算成偏移）
 * @param text 原始文本
 * @param edits 编辑列表
 * @return 应用后的文本
 *
 * @else
 * @brief Apply LSP-style text edits (back to front so offsets stay valid)
 *
 * @param document The document the edits target (used to convert positions to offsets)
 * @param text Original text
 * @param edits The edits
 * @return The resulting text
 *
 * @endif
 * */
export function applyEdits(document: vscode.TextDocument, text: string, edits: readonly vscode.TextEdit[]): string {
    const ordered = [...edits].sort((a, b) => document.offsetAt(b.range.start) - document.offsetAt(a.range.start));

    let result = text;

    for (const edit of ordered) {
        const start = document.offsetAt(edit.range.start);
        const end = document.offsetAt(edit.range.end);

        result = result.slice(0, start) + edit.newText + result.slice(end);
    }

    return result;
}

/**
 * @if zh
 * @brief 等到某文档出现至少 `count` 条诊断（诊断是拉取式的，需要给服务端时间）
 *
 * @param uri 文档 URI
 * @param count 期望的最少条数
 * @param timeout 上限（毫秒）
 * @return 等到时的诊断列表；超时返回最后一次结果
 *
 * @else
 * @brief Wait until a document has at least `count` diagnostics (pull diagnostics need time)
 *
 * @param uri Document URI
 * @param count Minimum number of diagnostics
 * @param timeout Upper bound in milliseconds
 * @return The diagnostics once available, or the last result on timeout
 *
 * @endif
 * */
export async function waitForDiagnostics(
    uri: vscode.Uri,
    count: number,
    timeout = 30_000
): Promise<vscode.Diagnostic[]> {
    const deadline = Date.now() + timeout;

    let diagnostics: vscode.Diagnostic[] = [];

    while (Date.now() < deadline) {
        diagnostics = vscode.languages.getDiagnostics(uri);

        if (diagnostics.length >= count) return diagnostics;

        await delay(200);
    }

    return diagnostics;
}

/**
 * @if zh
 * @brief 等待若干毫秒
 *
 * @else
 * @brief Wait for the given number of milliseconds
 *
 * @endif
 * */
export function delay(milliseconds: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, milliseconds));
}
