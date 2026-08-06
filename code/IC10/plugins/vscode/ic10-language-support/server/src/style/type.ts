/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file type.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 12:34
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Paths, Get } from "type-fest";


/**
 * @summary 从样式 Schema 中提取所有叶子节点的点路径
 *
 * @summary Extracts all leaf-level dot-separated paths from a style schema
 *
 * @desc 基于 type-fest 的 Paths 工具类型，设定 `leavesOnly: true`，
 * 只返回终端节点的路径（如 `'dark.common.bg'`），不包含中间对象路径。
 *
 * @desc Based on type-fest's Paths utility with `leavesOnly: true`,
 * returning only terminal node paths (e.g. `'dark.common.bg'`), excluding intermediate object paths.
 * */
export type StylePaths<T> = Paths<T, { leavesOnly: true }>

/**
 * @summary 根据点路径获取样式值的类型
 *
 * @summary Gets the value type at a given dot-separated style path
 *
 * @desc 基于 type-fest 的 Get 工具类型，接收 schema 和点路径字符串，
 * 返回该路径对应的值类型。例如 `ValueAtPath<typeof schemas, 'dark.common.bg'>` 返回 `string`。
 *
 * @desc Based on type-fest's Get utility. Takes a schema and a dot-path string,
 * returning the value type at that path. E.g., `ValueAtPath<typeof schemas, 'dark.common.bg'>` → `string`.
 * */
export type ValueAtPath<T, P extends string> = Get<T, P>;
