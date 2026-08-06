// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file index.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/29 15:20
 *
 * @desc IC10 语言支持公共包（`common`）的统一导出入口（barrel 模块）。
 * 汇总并重新导出调试工具、国际化模块、类型定义和通用工具，供客户端和服务端共享引用。
 *
 * @desc The unified barrel export entry for the IC10 Language Support common package.
 * Aggregates and re-exports debug utilities, i18n module, type definitions, and
 * general utilities for shared use by both client and server.
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export * from "./exception/debug";
export * from "./locals";
export type * from "./types";
export * from "./utils";
