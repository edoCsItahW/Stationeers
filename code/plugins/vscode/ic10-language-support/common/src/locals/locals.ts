/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file locals.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/21 23:37
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Paths, ParamsForPath } from "./types";
import { Console } from "../exception/debug";
import { EventEmitter } from "../utils";

type Resources = Record<string, any>; // 语言键 -> 翻译表

/**
 * @summary 国际化（i18n）翻译引擎，支持参数插值和复数规则
 *
 * @summary Internationalization (i18n) translation engine with parameter interpolation and pluralization
 *
 * @desc 管理多语言资源表，通过点路径查找翻译文本。
 * 支持 `{name}` 占位符插值、基于 Intl.PluralRules 的复数形式选择、
 * 以及 fallback 语言降级。继承 EventEmitter，在切换语言时触发 localeChange 事件。
 *
 * @desc Manages multilingual resource tables and looks up translations via dot-separated paths.
 * Supports `{name}` placeholder interpolation, Intl.PluralRules-based plural form selection,
 * and fallback locale degradation. Extends EventEmitter, firing localeChange on language switch.
 *
 * @remarks 复数模板要求 `count` 参数。未找到翻译时降级到 fallbackLocale，
 * 最终降级为返回路径字符串本身。
 *
 * @remarks Plural templates require a `count` parameter. Missing translations
 * fall back to fallbackLocale, and ultimately return the path string itself.
 * */
export class Locale<T extends Resources> extends EventEmitter {
    private currentLocale: keyof T;
    private resources: T;
    private fallbackLocale: keyof T | null = null;

    constructor(locale: keyof T, resources: T, options?: { fallbackLocale?: keyof T }) {
        super();
        this.currentLocale = locale;
        this.resources = resources;
        this.fallbackLocale = options?.fallbackLocale || null;
    }

    t<P extends Paths<T[keyof T]>>(path: P, params?: ParamsForPath<T[keyof T], P>): string {
        const value = this.lookupValue(path as string);
        return this.resolveValue(value, path as string, params as any);
    }

    setLocale(locale: keyof T): void {
        if (locale === this.currentLocale) return;
        if (!this.resources[locale]) {
            Console.warning(`Locale "${String(locale)}" not found.`, "Locale");
            return;
        }

        this.currentLocale = locale;
        this.emit("localeChange", locale);
    }

    getLocale(): keyof T {
        return this.currentLocale;
    }

    extendResources(locale: keyof T, newResources: Partial<T[keyof T]>): void {
        if (!this.resources[locale]) this.resources[locale] = {} as T[keyof T];

        // 深度合并（简易版，生产可调 lodash.merge）
        Object.assign(this.resources[locale], newResources);
    }

    private lookupValue(path: string): any {
        const keys = path.split(".");
        let result: any = this.resources[this.currentLocale];

        for (const key of keys) {
            if (result === null || result === undefined) break;
            result = result[key];
        }

        // 如果没找到，尝试 fallback
        if (result === null || result === undefined) {
            if (this.fallbackLocale && this.fallbackLocale !== this.currentLocale) {
                let fallbackResult: any = this.resources[this.fallbackLocale];
                for (const key of keys) {
                    if (fallbackResult === null || fallbackResult === undefined) break;
                    fallbackResult = fallbackResult[key];
                }

                if (fallbackResult !== null && fallbackResult !== undefined) return fallbackResult;
            }

            // 终极降级：返回路径本身
            return path;
        }
        return result;
    }

    private resolveValue(
        value: any,
        path: string,
        params?: Record<string, string | number> | { count: number }
    ): string {
        // ----- 情况1：字符串（直接插值） -----
        if (typeof value === "string") return this.interpolate(value, params || {});

        // ----- 情况2：复数对象（{ zero, one, other }） -----
        if (typeof value === "object" && value !== null && "other" in value) {
            const count = (params as any)?.count;
            if (typeof count !== "number") {
                Console.warning(`Plural key "${path}" requires param "count", got ${count}.`, "Locale");
                return value.other || path;
            }

            return this.pluralize(value, count);
        }

        // ----- 情况3：未知类型 -----
        Console.warning(`Invalid translation value at "${path}"`, "Locale");
        return String(value) || path;
    }

    private interpolate(text: string, params: Record<string, string | number>): string {
        return text.replace(/\{([^}]+)}/g, (_, key: string) => {
            const val = params[key];
            if (val === undefined || val === null) return `{${key}}`; // 保留占位符，便于发现漏传

            return String(val);
        });
    }

    private pluralize(forms: Record<string, string>, count: number): string {
        const locale = String(this.currentLocale);
        const rules = new Intl.PluralRules(locale);
        const key = rules.select(count);
        const result = forms[key] || forms.other || forms.one || JSON.stringify(forms);
        // 如果复数模板里也有插值变量（如 {count}），继续处理
        if (typeof result === "string") return this.interpolate(result, { count });

        return String(result);
    }
}
