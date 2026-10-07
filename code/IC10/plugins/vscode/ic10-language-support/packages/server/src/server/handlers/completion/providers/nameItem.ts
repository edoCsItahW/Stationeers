/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file nameItem.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 23:10
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItem } from "vscode-languageserver";
import type { Optional } from "@ic10/common";


/**
 * @if zh
 * @brief 拼出候选的匹配文本
 *
 * @details 本地化名、英文名、名字本身三段都参与匹配且去重：用户记得哪一段都能搜到，
 *          又不会因为同一个名字重复出现而反复命中。
 *
 * @else
 * @brief Build the text a candidate matches against
 *
 * @details The localized name, the English name and the name itself all take part, deduplicated:
 *          whichever one the user remembers finds the candidate without the same name matching twice.
 *
 * @endif
 * */
function matchText(...texts: (string | undefined)[]): string {
    return [...new Set(texts.filter((text): text is string => !!text))].join(" ");
}

/**
 * @if zh
 * @brief 造一个"名字"候选：本地化名 + 英文名 + 名字本身
 *
 * @details 需要按名字补全的地方共用同一套展示（`#: @type` 的类型名、`HASH("…")` 的型号名）：
 *          主标签是当前语言的显示名，英文名放 `labelDetails.description`（界面语言是英语、
 *          两者相同时不重复显示），`detail` 放**名字本身**——也就是实际插入的内容；
 *          三段都参与匹配，于是输入中文名、英文名或名字本身都能命中。
 *          调用方再补上 `kind` 等各自特有的字段。
 *
 * @param name 实际插入的名字（类型名或型号名）
 * @param title 当前语言的显示名
 * @param englishTitle 英文显示名
 * @return 候选（不含 `kind` 与 `data`）
 *
 * @else
 * @brief Build a "name" candidate: localized name + English name + the name itself
 *
 * @details Everywhere a name is completed shares one presentation (the type name of `#: @type`, the
 *          model name of `HASH("…")`): the label is the display name of the current language, the
 *          English name goes into `labelDetails.description` (skipped under an English locale, where
 *          both are equal) and `detail` holds the **name itself** — the text actually inserted. All
 *          three take part in matching, so the localized name, the English name and the name itself
 *          all find it. Callers add their own `kind` and the like.
 *
 * @param name The name actually inserted (a type name or a model name)
 * @param title Display name in the current language
 * @param englishTitle English display name
 * @return The candidate (without `kind` or `data`)
 *
 * @endif
 * */
export function nameItem(
    name: string,
    title: Optional<string>,
    englishTitle: Optional<string>
): CompletionItem {
    const label = title ?? englishTitle ?? name;

    return {
        label,
        // LSP 的 label 只能是字符串：英文名放 labelDetails 的 description 位（右侧灰色小字）
        // The LSP label is a plain string: the English name goes into labelDetails.description
        labelDetails: englishTitle && englishTitle !== label ? { description: englishTitle } : undefined,
        insertText: name,
        filterText: matchText(title, englishTitle, name),
        detail: name
    };
}
