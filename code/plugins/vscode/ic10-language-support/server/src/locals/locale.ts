/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file locale.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 00:03
 *
 * @desc IC10 语言支持插件服务端的国际化（i18n）入口模块。该模块负责加载中英文双语资源，
 * 创建全局共享的 Locale 实例，并导出绑定了该实例的翻译函数 `t`，供服务端各模块统一使用。
 *
 * @desc The i18n entry point for the IC10 Language Support server. This module loads
 * Chinese and English bilingual resources, creates a globally shared Locale instance,
 * and exports a bound translation function `t` for uniform use across server modules.
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import zhHans from "./languages/zh-hans";
import enUS from "./languages/en-us";
import { Locale } from "common";

const resources = {
    "zh-hans": zhHans,
    "en-us": enUS
} as const;

/**
 * @summary 全局共享的 Locale 实例
 *
 * @summary Globally shared Locale instance
 *
 * @desc 基于中英文双语资源初始化的 Locale 实例，默认语言为 en-us，
 * 回退语言为 zh-hans。当当前语言的翻译键缺失时，会自动回退到中文翻译。
 * 该实例继承自 EventEmitter，支持监听 `localeChange` 事件。
 *
 * @desc A Locale instance initialized with Chinese and English bilingual resources.
 * Default language is en-us with zh-hans as fallback. When a translation key is
 * missing in the current language, it automatically falls back to Chinese.
 * Inherits from EventEmitter and supports the `localeChange` event.
 * */
export const locale = new Locale("en-us", resources, {
    fallbackLocale: "zh-hans"
});

/**
 * @summary 国际化翻译函数
 *
 * @summary i18n translation function
 *
 * @desc `locale.t` 的绑定版本。接收点号分隔的翻译路径（如 `"error.not_found"`）和可选的
 * 占位符参数对象，返回对应语言的翻译字符串。支持 `{key}` 占位符插值替换和复数形式
 * （通过 `{ count }` 参数配合 `Intl.PluralRules` 自动选择 `zero`/`one`/`other` 等复数形式）。
 * 当翻译键在当前语言和回退语言中都不存在时，返回路径字符串本身作为兜底。
 *
 * @desc Bound version of `locale.t`. Accepts a dot-separated translation path
 * (e.g., `"error.not_found"`) and an optional placeholder params object, returning
 * the translated string in the current locale. Supports `{key}` placeholder interpolation
 * and pluralization (via `{ count }` parameter with `Intl.PluralRules` to automatically
 * select plural forms like `zero`/`one`/`other`). Falls back to returning the path
 * string itself when the key is missing in both current and fallback locales.
 *
 * @remarks 所有服务端模块应统一使用此 `t` 函数进行文本国际化，不要自行创建 Locale 实例。
 *
 * @remarks All server modules should use this `t` function for text i18n rather than
 * creating their own Locale instances.
 * */
export const t = locale.t.bind(locale) as typeof locale.t;
