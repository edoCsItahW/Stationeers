/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file deviceSearch.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 21:50
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { DeviceTypeInfo } from "@ic10/common";
import { fuzzyScoreAny } from "@ic10/common";
import { commands, ExtensionContext, QuickPickItem, window } from "vscode";

import { t } from "./locals";


/**
 * @summary "搜索设备类型"命令的 ID
 *
 * @summary Command ID of "search device type"
 *
 * @desc 与根 `package.json` 的 `contributes.commands` 保持一致。
 *
 * @desc Kept in sync with `contributes.commands` in the root `package.json`.
 * */
export const DEVICE_SEARCH_COMMAND = "ic10.searchDeviceType";

/**
 * @summary 查询非空时最多列出的条数
 *
 * @summary Maximum number of entries listed for a non-empty query
 *
 * @desc 索引有近 600 条，模糊命中又常常一大片；只把得分最高的这些条交给列表，
 *       面板仍然流畅，也更容易一眼看到最贴近的那个。
 *
 * @desc The index holds nearly 600 entries and a fuzzy query often hits plenty of them; only the
 *       best-scoring ones are handed to the list, which keeps the picker responsive and makes the
 *       closest match easy to spot.
 * */
const MAX_ITEMS = 200;

/**
 * @if zh
 * @brief 列表项：额外带上类型名
 *
 * @else
 * @brief A list entry, carrying the type name
 *
 * @endif
 * */
interface DeviceQuickPickItem extends QuickPickItem {
    typeName: string;
}

/**
 * @if zh
 * @brief 把服务端索引转成列表项
 *
 * @details 展示遵循"本地化名 + 英文名"：主标签是当前语言的显示名，英文名放描述位（右侧小字），
 *          界面语言是英语时两者相同、只显示一次，因此英文用户看到的就是一套英文名。
 *          详情位先是**类型名**（实际要写进代码的标识符），后面跟描述首行。
 *          三段文本都参与模糊匹配，用户记得显示名、英文名或类型名任意一段都能找到。
 *
 * @param entries 服务端返回的索引
 * @return 列表项
 *
 * @else
 * @brief Turn the server index into list entries
 *
 * @details Presentation follows "localized name + English name": the label is the display name of the
 *          current language and the English name sits in the description slot (the dimmed text on the
 *          right); under an English locale both are equal and shown once, so English users just see one
 *          consistent set. The detail slot starts with the **type name** (the identifier actually
 *          written into code) followed by the first line of the description.
 *          All of it takes part in fuzzy matching, so remembering a display name, the English name or
 *          the type name finds the device.
 *
 * @param entries Index returned by the server
 * @return The list entries
 *
 * @endif
 * */
function toItems(entries: DeviceTypeInfo[]): DeviceQuickPickItem[] {
    return entries.map(({ typeName, title, englishTitle, desc }) => {
        const label = title ?? englishTitle ?? typeName;
        const summary = desc?.split(/\r?\n/)[0]?.trim();

        return {
            label,
            description: englishTitle && englishTitle !== label ? englishTitle : undefined,
            detail: summary ? `${typeName} · ${summary}` : typeName,
            typeName
        };
    });
}

/**
 * @if zh
 * @brief 按查询串给列表项打分排序
 *
 * @details 空查询保持服务端给的原顺序（已按当前语言的显示名排好），非空查询按模糊得分从高到低，
 *          命不中的直接剔除。
 *
 * @param items 全部列表项
 * @param query 用户输入
 * @return 排序后的列表项（可能为空）
 *
 * @else
 * @brief Score and sort the list entries against the query
 *
 * @details An empty query keeps the server order (already sorted by the display name of the current
 *          language); otherwise entries are ordered by descending fuzzy score and misses are dropped.
 *
 * @param items All list entries
 * @param query User input
 * @return The ordered entries (possibly empty)
 *
 * @endif
 * */
function rank(items: DeviceQuickPickItem[], query: string): DeviceQuickPickItem[] {
    if (!query.trim()) return items;

    return items
        .map(item => ({
            item,
            score: fuzzyScoreAny(query, item.label, item.description, item.detail)
        }))
        .filter(({ score }) => score !== undefined)
        .sort((a, b) => (b.score ?? 0) - (a.score ?? 0))
        .map(({ item }) => item);
}

/**
 * @if zh
 * @brief 把类型名插到光标处（有选中内容则替换）
 *
 * @param typeName 类型名
 *
 * @else
 * @brief Insert the type name at the cursor (replacing a selection if any)
 *
 * @param typeName The type name
 *
 * @endif
 * */
async function insertTypeName(typeName: string) {
    const editor = window.activeTextEditor;

    if (!editor) return;

    const selection = editor.selection;

    await editor.edit(builder => {
        if (selection.isEmpty) builder.insert(selection.active, typeName);
        else builder.replace(selection, typeName);
    });

    window.setStatusBarMessage(t("deviceSearch.inserted", { name: typeName }), 2000);
}

/**
 * @if zh
 * @brief 注册"搜索设备类型"命令
 *
 * @details 命令向服务端拉一份完整的设备类型索引（已按当前语言本地化），弹出快速选择面板：
 *          输入任意中英文片段模糊过滤（显示名 / 类型名 / 描述三段都参与），回车把选中的**类型名**
 *          插到光标处，便于直接补进 `#: @type` 或 `HASH()` 之类的写法里。
 *          列表项一旦设定，面板自身的过滤仍会按输入值再筛一遍，因此这里同时开启
 *          `matchOnDescription` / `matchOnDetail`，避免"只命中类型名或描述"的候选被面板丢掉。
 *
 * @param context 扩展上下文
 * @param fetchDeviceTypes 拉取索引（由扩展入口转发到语言服务端）
 *
 * @else
 * @brief Register the "search device type" command
 *
 * @details The command pulls the complete device type index from the server (already localized for the
 *          current language) and opens a quick pick: any English or Chinese fragment filters it fuzzily
 *          (the display name, type name and description all take part), and Enter inserts the picked
 *          **type name** at the cursor, ready for `#: @type` or `HASH()`-style spellings.
 *          Once items are set, the picker still filters them again by the typed value, so
 *          `matchOnDescription` / `matchOnDetail` are enabled here to keep candidates that only match
 *          through the type name or the description from being dropped.
 *
 * @param context Extension context
 * @param fetchDeviceTypes Fetches the index (the extension entry forwards it to the language server)
 *
 * @endif
 * */
export function registerDeviceTypeSearch(
    context: ExtensionContext,
    fetchDeviceTypes: () => Promise<DeviceTypeInfo[]>
) {
    context.subscriptions.push(
        commands.registerCommand(DEVICE_SEARCH_COMMAND, async () => {
            const editor = window.activeTextEditor;

            // 命令的产物要插到 IC10 文档里，没有 IC10 编辑器时不打扰用户
            if (!editor || editor.document.languageId !== "ic10") return;

            const all = toItems(await fetchDeviceTypes());
            const quickPick = window.createQuickPick<DeviceQuickPickItem>();

            quickPick.title = t("deviceSearch.title");
            quickPick.placeholder = t("deviceSearch.placeholder");
            quickPick.matchOnDescription = true;
            quickPick.matchOnDetail = true;
            quickPick.items = all;

            quickPick.onDidChangeValue(value => {
                const ranked = rank(all, value);

                quickPick.items = value.trim() ? ranked.slice(0, MAX_ITEMS) : ranked;
                quickPick.placeholder = ranked.length
                    ? t("deviceSearch.placeholder")
                    : t("deviceSearch.empty");
            });

            quickPick.onDidAccept(() => {
                const [picked] = quickPick.selectedItems;

                quickPick.hide();

                if (picked) void insertTypeName(picked.typeName);
            });

            quickPick.onDidHide(() => quickPick.dispose());
            quickPick.show();
        })
    );
}
