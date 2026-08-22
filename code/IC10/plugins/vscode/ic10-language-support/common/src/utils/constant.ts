/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file constant.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/28 13:21
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

/**
 * @summary VS Code 插件配置节的名称
 *
 * @summary The VS Code extension configuration section name
 *
 * @desc 用于 `vscode.workspace.getConfiguration()` 等 API 中标识 IC10 插件的配置分组。
 *
 * @desc Identifies the IC10 extension's configuration group in APIs such as
 * `vscode.workspace.getConfiguration()`.
 * */
export const CONFIGURATION_SECTION_NAME = "ic10" as const;

/**
 * @summary 通信事件通道名称
 *
 * @summary Communication event channel name
 *
 * @desc 用于 LSP 客户端与服务端之间的自定义事件通信通道。
 *
 * @desc Used for custom event communication between the LSP client and server.
 * */
export const COMM_EVENT_NAME = "ic10/event" as const;
