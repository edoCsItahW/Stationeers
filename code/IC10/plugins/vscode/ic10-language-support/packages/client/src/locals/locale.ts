// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file locale.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 18:00
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { CONFIGURATION_SECTION_NAME, Locale, type Optional } from "@ic10/common";
import { workspace } from "vscode";

import zhHans from "./languages/zh-hans";
import enUS from "./languages/en-us";


export const resources = {
    "zh-hans": zhHans,
    "en-us": enUS
} as const;

export type Language = keyof typeof resources;

/**
 * @summary 客户端共享的 Locale 实例
 *
 * @summary The locale instance shared by the client
 *
 * @desc 基于中英文双语资源初始化的 Locale 实例，默认语言为 en-us、回退语言为 zh-hans。
 * 语言由插件设置 `ic10.language` 决定（与服务端使用同一个设置），因此客户端与服务端的界面文本
 * 始终保持一致。
 *
 * @desc A Locale instance initialized with the bilingual resources; default language en-us, fallback
 * zh-hans. The language comes from the extension setting `ic10.language` (the same setting the server
 * reads), so client and server UI text always agree.
 * */
export const locale = new Locale("en-us", resources, {
    fallbackLocale: "zh-hans"
});

/**
 * @summary 翻译函数
 *
 * @summary Translation function
 *
 * @desc `locale.t` 的绑定版本，用法与服务端、调试器的 `t` 一致。
 *
 * @desc Bound version of `locale.t`, used exactly like the `t` of the server and the debugger.
 * */
export const t = locale.t.bind(locale) as typeof locale.t;

/**
 * @summary 按插件设置应用客户端语言
 *
 * @summary Apply the client language from the extension setting
 *
 * @desc 读取 `ic10.language`（缺省为 `en-us`）并切换界面语言，返回生效的语言；取值非法时不切换并
 * 返回 undefined。设置变化时再次调用即可。其它界面模块（如调试器）可用返回值跟随同一语言。
 *
 * @desc Reads `ic10.language` (defaults to `en-us`), switches the UI language and returns the applied
 * language; an invalid value changes nothing and returns undefined. Call it again when the setting
 * changes. Other UI modules (e.g. the debugger) can follow the same language via the return value.
 * */
export function applyLanguage(): Optional<Language> {
    const language = workspace.getConfiguration(CONFIGURATION_SECTION_NAME).get<string>("language");

    if (language !== "zh-hans" && language !== "en-us") return undefined;

    locale.setLocale(language);

    return language;
}
