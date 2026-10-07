/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file typeHint.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 22:20
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CompletionItem, CompletionItemKind } from "vscode-languageserver";

import { getDeviceTitles } from "../../../services";
import { AST } from "../../../../utils";
import { nameItem } from "./nameItem";
import type { CompletionProviderContext } from "./types";


/**
 * @if zh
 * @brief 补全 `#: @type` 的取值：设备类型名
 *
 * @details 候选取自类型表里的**设备**注解（标准库的设备 + 当前文件里声明的），枚举不在此列：
 *          `@type` 声明的是"这个别名/定义是哪台设备"，枚举类型与它无关（枚举成员补全走
 *          `Foo.Bar` 的枚举操作数那条路）。
 *
 * @details 展示遵循"本地化名 + 英文名"：`label` 是当前语言的显示名，`labelDetails.description` 放英文名
 *          （界面语言是英语、两者相同时不再重复显示），`detail` 放**类型名**——也就是实际插入的内容。
 *          于是中文界面看到 `微波炉 Microwave`、英文界面就是 `Microwave`；无论哪种语言，输入中文名、
 *          英文名或类型名都能匹配到，插入的始终是类型名（`@type` 只认标识符）。
 *          元数据里没有条目的设备退化成只显示类型名。
 *
 * @details 文档由 `CompletionItem.data.description`（注解里的 `@desc` 链接）在 resolve 阶段给出，
 *          因此这里不必自己查元数据。
 *
 * @param ctx 补全上下文
 * @return 设备类型名候选，保持类型表顺序（匹配质量交给编辑器排序）
 *
 * @else
 * @brief Complete the value of `#: @type`: device type names
 *
 * @details Candidates come from the **device** annotations of the type table (standard-library devices
 *          plus whatever the current file declares); enums are deliberately left out, because `@type`
 *          states which device an alias/define refers to — enum types are unrelated (enum members are
 *          completed through the `Foo.Bar` enum operand path).
 *
 * @details Presentation follows "localized name + English name": the label is the display name of the
 *          current language, `labelDetails.description` holds the English name (omitted under an English
 *          locale, where both are equal) and `detail` holds the **type name** — the text actually
 *          inserted. A Chinese UI therefore shows `微波炉 Microwave` while an English one shows just
 *          `Microwave`; in either language the Chinese name, the English name and the type name all
 *          match, and the inserted text is always the type name (`@type` takes an identifier only). A
 *          device without a metadata entry degrades to showing its type name.
 *
 * @details Documentation comes from `CompletionItem.data.description` (the annotation's `@desc` link)
 *          during resolve, so this provider need not read the metadata itself.
 *
 * @param ctx Completion context
 * @return Device type name candidates in type-table order (match quality is ranked by the editor)
 *
 * @endif
 * */
export function provideHintType(ctx: CompletionProviderContext): CompletionItem[] {
    if (!ctx.types) return [];

    const language = ctx.getLocale();

    return Object.values(ctx.types)
        .filter(AST.isDeviceAnnotation)
        .map(device => {
            const { title, englishTitle } = getDeviceTitles(device.name, language);

            return {
                ...nameItem(device.name, title, englishTitle),
                kind: CompletionItemKind.Class,
                data: { description: device.desc }
            };
        });
}
