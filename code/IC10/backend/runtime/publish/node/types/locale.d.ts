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
 * @date 2026/09/28
 * @desc 诊断消息的本地化设置：导出 {@link IC10RuntimeLocal}，用于切换运行时输出的语言。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary 本地化设置类
 *
 * @desc 提供 IC10 运行时的本地化支持，用于设置执行期诊断（无法求值的操作数、栈溢出、索引越界、
 *       无效配置等）的消息语言。
 *
 * @note 语言是**全局**状态，一旦设置对后续所有执行与诊断生效；与编译器的
 *       `IC10CompilerLocal` 相互独立，两者需要分别设置。
 *
 * @example
 * ```typescript
 * import { IC10RuntimeLocal } from '@ic10/runtime';
 *
 * IC10RuntimeLocal.setLanguage('zh-hans');   // 简体中文
 * ```
 *
 * @public
 */
export class IC10RuntimeLocal {
    /**
     * @summary 设置运行时的语言环境
     *
     * @param language - 语言代码：`'en-us'`（英文，默认）或 `'zh-hans'`（简体中文）。
     *
     * @desc 影响此后产生的所有诊断消息文本（例如 `Unable to evaluate operand '...'` 变为
     *       `无法求值操作数 '...'`）。传入未注册的语言时按默认语言 `en-us` 输出。
     *
     * @example
     * ```typescript
     * import { IC10RuntimeLocal } from '@ic10/runtime';
     *
     * IC10RuntimeLocal.setLanguage('zh-hans');   // 简体中文
     * IC10RuntimeLocal.setLanguage('en-us');     // 英文
     * ```
     *
     * @public
     */
    static setLanguage(language: string): void;
}
