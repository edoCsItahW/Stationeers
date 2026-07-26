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

// 提取主题名称类型
export type ThemeName = keyof typeof themeSchemas; // 'dark' | 'light'