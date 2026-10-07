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

/**
 * @summary 补全范围事件通道名称
 *
 * @summary Completion scope event channel name
 *
 * @desc 客户端在用户按下"收窄补全范围"的按键时，通过该通道把范围推给服务端；
 * 范围是**一次性**的，被下一次补全请求消费掉，因此正常补全行为不受影响。
 *
 * @desc When the user presses a "narrow the completion scope" key, the client pushes the scope to
 * the server through this channel. The scope is **one-shot**: it is consumed by the next completion
 * request, so ordinary completion stays unaffected.
 * */
export const COMPLETION_SCOPE_EVENT_NAME = "ic10/completionScope" as const;

/**
 * @summary 设备类型索引请求通道名称
 *
 * @summary Device type index request channel name
 *
 * @desc 客户端在用户唤出"搜索设备类型"时通过该通道向服务端拉取**整个**设备类型索引
 * （类型名 + 当前语言下的显示名与描述，见 `DeviceTypeInfo`）。索引一次拉取后由客户端缓存，
 * 之后的模糊过滤与排序都在客户端做，不必每次按键都往返一次。
 *
 * @desc The client pulls the **whole** device type index (type name plus the display name and
 * description in the current language, see `DeviceTypeInfo`) through this channel when the user
 * invokes "search device type". The index is fetched once and cached by the client; fuzzy filtering
 * and ranking then happen client-side, so no round trip is needed per keystroke.
 * */
export const DEVICE_TYPE_EVENT_NAME = "ic10/deviceTypes" as const;
