// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file locale.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 17:21
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 本地化设置类
 *
 * @desc 提供 IC10 编译器的本地化支持，允许设置错误消息和诊断信息的语言。
 *
 * @public
 */
export class IC10Local {
    /**
     * @summary 设置编译器的语言环境
     *
     * @param language - 语言代码（如 'en'、'zh' 等）
     *
     * @example
     * ```typescript
     * ic10.IC10Local.setLanguage('zh');
     * ```
     */
    static setLanguage(language: string): void;
}