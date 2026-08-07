/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file globalCache.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/27 23:42
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { property } from "common";

/**
 * @summary 全局缓存管理器
 *
 * @summary Global cache manager
 *
 * @desc 存储 LSP 服务器级别的全局状态信息。包含 workspaceCfg 标志（标记工作区配置是否已加载）
 * 和当前工作区的 URI。使用 @property 装饰器实现响应式属性访问。
 * 该类作为单例使用，贯穿整个 LSP 服务器生命周期。
 *
 * @desc Stores global state information at the LSP server level. Contains a workspaceCfg
 * flag (indicating whether workspace configuration has been loaded) and the current
 * workspace URI. Uses the @property decorator for reactive property access.
 * Used as a singleton throughout the LSP server lifecycle.
 * */
export class GlobalCache {
    @property
    accessor flag = {
        workspaceCfg: false
    };

    @property
    accessor uri: string = "";
}
