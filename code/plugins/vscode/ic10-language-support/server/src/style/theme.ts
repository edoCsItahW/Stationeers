/**
 * @file theme.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 12:35
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { themeSchemas, type ThemeName } from "./themes";
import type { StylePaths, ValueAtPath } from "./type";
import { EventEmitter } from "common";

type ThemeSchemas = typeof themeSchemas;
type AllPaths = StylePaths<ThemeSchemas>; // 'dark.common.bg' | 'dark.button.bg' | 'light.common.bg' ...

class ThemeController<T extends Record<ThemeName, any>> extends EventEmitter {
    private currentTheme: ThemeName;
    private schemas: T;
    private fallbackTheme: ThemeName | null = null;

    constructor(schemas: T, initialTheme: ThemeName, options?: { fallback?: ThemeName }) {
        super();
        this.schemas = schemas;
        this.currentTheme = initialTheme;
        this.fallbackTheme = options?.fallback || null;
    }

    get<P extends AllPaths>(path: P): ValueAtPath<T, P> {
        const keys = path.split(".");
        let result: any = this.schemas;

        for (const key of keys) {
            if (result === null || result === undefined) break;
            result = result[key];
        }

        // 如果没找到，尝试 fallback 主题
        if (result === null || result === undefined) {
            if (this.fallbackTheme && this.fallbackTheme !== this.currentTheme) {
                let fallbackResult: any = this.schemas[this.fallbackTheme];
                // 从 fallback 主题的第二级开始找（跳过主题键）
                for (let i = 1; i < keys.length; i++) {
                    if (fallbackResult === null || fallbackResult === undefined) break;
                    fallbackResult = fallbackResult[keys[i]];
                }
                if (fallbackResult !== null && fallbackResult !== undefined) return fallbackResult;
            }
            // 终极降级：返回路径本身（方便调试）
            console.warn(`[Theme] Missing style: ${path}`);
            return path as ValueAtPath<T, P>;
        }
        return result;
    }

    // ---------- 获取当前主题下的路径（无需写 dark/light） ----------
    getCurrent<P extends StylePaths<T[ThemeName]>>(path: P): ValueAtPath<T[ThemeName], P> {
        return this.get(`${this.currentTheme}.${path}` as AllPaths) as ValueAtPath<T[ThemeName], P>;
    }

    // ---------- 切换主题 ----------
    setTheme(theme: ThemeName): void {
        if (theme === this.currentTheme) return;
        if (!this.schemas[theme]) {
            console.warn(`[Theme] Theme "${theme}" not found.`);
            return;
        }
        this.currentTheme = theme;
        this.emit("themeChange", theme);
        // 自动注入 CSS 变量（如果想自动生效，可放开注释）
        // this.injectCSSVariables();
    }

    getTheme(): ThemeName {
        return this.currentTheme;
    }

    // ---------- 动态扩展主题（模块懒加载） ----------
    extendTheme(theme: ThemeName, newStyles: Partial<T[ThemeName]>): void {
        if (!this.schemas[theme]) (this.schemas as any)[theme] = {};

        Object.assign(this.schemas[theme], newStyles);
    }
}

/**
 * @summary 主题控制器单例，管理主题切换和样式值查询
 *
 * @summary Singleton theme controller managing theme switching and style value lookups
 *
 * @desc 默认初始化为深色主题，fallback 为浅色主题。支持通过 get() 按完整路径查询、
 * getCurrent() 自动使用当前主题查询、setTheme() 切换并触发 themeChange 事件。
 *
 * @desc Defaults to dark theme with light fallback. Supports get() for full-path queries,
 * getCurrent() for current-theme-aware queries, and setTheme() which emits themeChange.
 * */
export const themeController = new ThemeController(
    themeSchemas,
    "dark", // 初始主题
    { fallback: "light" }
);

/**
 * @summary 快捷样式查询函数，自动使用当前主题
 *
 * @summary Shorthand style lookup function that auto-uses the current theme
 *
 * @desc 等同于 `themeController.getCurrent()`，绑定为独立函数便于简洁调用。
 * 传入相对于当前主题的路径（如 `'hover.labelDef.identifier'`）即可获取颜色值。
 *
 * @desc Equivalent to `themeController.getCurrent()`, bound as a standalone function.
 * Accepts a path relative to the current theme (e.g. `'hover.labelDef.identifier'`)
 * and returns the corresponding color value.
 * */
// 快捷调用（自动使用当前主题）
export const s = themeController.getCurrent.bind(themeController);

/**
 * @summary 完整路径样式查询函数
 *
 * @summary Full-path style lookup function
 *
 * @desc 等同于 `themeController.get()`，需要指定包含主题名的完整路径
 * （如 `'dark.common.bg'`）。不依赖当前主题设置。
 *
 * @desc Equivalent to `themeController.get()`, requires a full path including
 * the theme name (e.g. `'dark.common.bg'`). Does not depend on current theme.
 * */
// 也可以直接导出全路径方法
export const getStyle = themeController.get.bind(themeController);
