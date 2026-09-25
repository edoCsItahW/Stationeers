// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file locale.d.ts
 * @author edocsitahw
 * @version 1.2
 * @date 2026/09/24
 * @desc 诊断消息的本地化设置：导出 {@link IC10CompilerLocal}，用于切换编译器输出的语言。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 本地化设置类
 *
 * @desc 提供 IC10 编译器的本地化支持，允许设置错误消息和诊断信息的语言。
 *       语言是**全局**状态，一旦设置对后续所有词法/语法/语义/链接阶段生效。
 *
 * @public
 */
export class IC10CompilerLocal {
    /**
     * @summary 设置编译器的语言环境
     *
     * @param language - 语言代码：`'en-us'`（英文，默认）或 `'zh-hans'`（简体中文）。
     *                   传入其它值不会报错，但消息会退回默认语言。
     *
     * @desc 影响此后产生的所有诊断消息文本（例如词法错误由
     *       `Unclosed paired symbols ...` 变为 `未闭合的成对符号 ...`）。
     *
     * @example
     * ```typescript
     * import { IC10CompilerLocal } from 'ic10c-node';
     *
     * IC10CompilerLocal.setLanguage('zh-hans');   // 简体中文
     * IC10CompilerLocal.setLanguage('en-us');     // 英文
     * ```
     *
     * @public
     */
    static setLanguage(language: string): void;
}