// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file number.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/12 13:25
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItem, CompletionItemKind, InsertTextFormat } from "vscode-languageserver";
import { instructions } from "@ic10/metadata/std";

import { getDeviceModelNames } from "../../../services";
import { DescriptionSolver } from "../../../../utils";
import { nameItem } from "./nameItem";
import type { CompletionProviderContext, OperandProvider } from "./types";


type SpecialNumberKey = "%" | "$" | "HASH" | "STR";

const NUMBER_META = {
    "%": {
        value: instructions["binary_number"],
        name: "binary_number"
    },
    $: {
        value: instructions["hash_number"],
        name: "hash_number"
    },
    HASH: {
        value: instructions["hash"],
        name: "hash"
    },
    STR: {
        value: instructions["str"],
        name: "str"
    }
} satisfies Record<SpecialNumberKey, any>;

interface Shortcut extends CompletionItem {
    label: SpecialNumberKey;
}

const NUMBER_SHORTCUTS: Shortcut[] = [
    { label: "%", kind: CompletionItemKind.Operator, insertText: "%" },
    { label: "$", kind: CompletionItemKind.Operator, insertText: "$" },
    {
        label: "HASH",
        kind: CompletionItemKind.Function,
        insertText: 'HASH("$0")',
        insertTextFormat: InsertTextFormat.Snippet
    },
    {
        label: "STR",
        kind: CompletionItemKind.Function,
        insertText: 'STR("$0")',
        insertTextFormat: InsertTextFormat.Snippet
    }
];


export const provideNumber: OperandProvider = (ctx, opType, prefix) => {
    return NUMBER_SHORTCUTS.filter(i => i.label.startsWith(prefix))
        .map(i => {
            const meta = NUMBER_META[i.label];

            return {
                ...i,
                detail: meta.value.signature,
                data: {
                    description: DescriptionSolver.parse(meta.value.desc)
                }
            };
        });
};

/**
 * @if zh
 * @brief 补全 `HASH("…")` 的字符串实参：设备型号名
 *
 * @details 候选是元数据里的**原始型号名**（`HASH("StructureTransformerMedium(Reversed)")` 里要写的
 *          就是带括号的那个），不是 `#: @type` 用的压平类型名——两者对多数设备相同、对个别设备不同，
 *          详见 `getDeviceModelNames`。展示形状与 `#: @type` 共用 `nameItem`：本地化名 + 英文名 +
 *          型号名本身，三段都参与匹配。
 *
 * @details 这里不按前缀过滤（与 `#: @type`、枚举类型名两个提供器一致）：候选交给编辑器按
 *          `filterText` 过滤，因此中文名、英文名或型号名都能搜到，插入的始终是型号名。
 *
 * @param ctx 补全上下文
 * @return 型号名候选，按当前语言的显示名排序
 *
 * @else
 * @brief Complete the string argument of `HASH("…")`: device model names
 *
 * @details Candidates are the **raw model names** of the metadata (the parenthesized one is what
 *          `HASH("StructureTransformerMedium(Reversed)")` writes), not the collapsed type names used by
 *          `#: @type`: the two coincide for most devices and differ for a few, see
 *          `getDeviceModelNames`. The presentation is shared with `#: @type` through `nameItem`:
 *          localized name + English name + the model name itself, all three taking part in matching.
 *
 * @details No prefix filtering here (consistent with the `#: @type` and enum-type-name providers): the
 *          editor filters by `filterText`, so the localized name, the English name and the model name
 *          all find a candidate while the inserted text is always the model name.
 *
 * @param ctx Completion context
 * @return Model name candidates sorted by the display name of the current language
 *
 * @endif
 * */
export function provideHashName(ctx: CompletionProviderContext): CompletionItem[] {
    const language = ctx.getLocale();

    return getDeviceModelNames(language).map(({ modelName, title, englishTitle }) => ({
        ...nameItem(modelName, title, englishTitle),
        kind: CompletionItemKind.Class
    }));
}

