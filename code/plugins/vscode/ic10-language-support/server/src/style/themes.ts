/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file themes.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 12:37
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
/**
 * @summary 主题配色方案定义（深色 / 浅色）
 *
 * @summary Theme color schema definitions (dark / light)
 *
 * @desc 包含深色和浅色两套主题的配色映射，涵盖通用色（背景、文字）和
 * hover 提示中各语法元素（标签、别名、定义、常量）的标识符及类型颜色。
 *
 * @desc Defines dark and light theme color palettes including common colors
 * (background, text) and hover tooltip colors for syntax elements
 * (labels, aliases, defines, constants) with identifier and type styling.
 *
 * @remarks 使用 `as const` 确保类型推导为字面量类型，便于 StylePaths 工具类型提取路径。
 *
 * @remarks Uses `as const` to preserve literal types for path extraction by StylePaths utility type.
 * */
export const themeSchemas = {
    dark: {
        common: {
            bg: '#1a1a1a',
            text: '#ffffff',
        },
        hover: {
            labelDef: {
                identifier: '#E8BF6A',
                type: '#ffffff'
            },
            aliasDirective: {
                identifier: '#b0009a',
                type: '#ffffff'
            },
            defineDirective: {
                identifier: '#2AACB8',
                type: '#ffffff'
            },
            contant: {
                identifier: '#ffffff',
                type: '#C77DBB'
            }
        },
    },
    light: {
        common: {
            bg: '#ffffff',
            text: '#1a1a1a',
        },
        hover: {
            labelDef: {
                identifier: '#1a1a1a'
            },
            aliasDirective: {
                identifier: '#1a1a1a',
                type: '#b0009a'
            },
            defineDirective: {
                identifier: '#1a1a1a',
                type: '#b0009a'
            },
            contant: {
                identifier: '#1a1a1a',
                type: '#C77DBB'
            }
        }
    },
} as const;

/**
 * @summary 可用主题名称的联合类型
 *
 * @summary Union type of available theme names
 *
 * @desc 从 themeSchemas 的键自动推导，目前为 `'dark' | 'light'`。
 *
 * @desc Auto-derived from themeSchemas keys, currently `'dark' | 'light'`.
 * */
// 提取主题名称类型
export type ThemeName = keyof typeof themeSchemas; // 'dark' | 'light'