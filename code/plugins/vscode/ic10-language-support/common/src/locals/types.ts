/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file types.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/21 23:55
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Paths as PathsOf, Get } from "type-fest";

/**
 * @summary 递归提取对象类型中所有叶子节点的点路径
 *
 * @summary Recursively extracts all leaf-level dot-separated paths from an object type
 *
 * @desc 基于 type-fest 的 Paths 工具类型，设定 `leavesOnly: true`，
 * 无深度限制地返回所有终端节点的路径字符串联合类型。
 *
 * @desc Based on type-fest's Paths with `leavesOnly: true`, returning a union of
 * all terminal node path strings with no depth limit.
 * */
// 1. 递归提取所有叶子节点的点路径（无深度限制）
export type Paths<T> = PathsOf<T, { leavesOnly: true }>;

/**
 * @summary 根据点路径获取值的类型
 *
 * @summary Gets the value type at a given dot-separated path
 *
 * @desc 基于 type-fest 的 Get 工具类型。例如 `ValueAtPath<Resources, 'greeting.hello'>` 返回
 * `greeting.hello` 路径对应的值类型。
 *
 * @desc Based on type-fest's Get utility. E.g., `ValueAtPath<Resources, 'greeting.hello'>`
 * returns the value type at the `greeting.hello` path.
 * */
// 2. 根据点路径获取值类型
export type ValueAtPath<T, P extends string> = Get<T, P>;

/**
 * @summary 从模板字符串中提取占位符变量名
 *
 * @summary Extracts placeholder variable names from a template string
 *
 * @desc 递归解析模板字符串中的 `{name}` 占位符，返回所有变量名的联合类型。
 * 例如 `"Hello {name}, you have {count} messages"` → `"name" | "count"`。
 *
 * @desc Recursively parses `{name}` placeholders in template strings, returning a union
 * of all variable names. E.g., `"Hello {name}, you have {count} messages"` → `"name" | "count"`.
 * */
// 3. 从模板字符串中提取变量名（如 "Hello {name}" -> "name"）
export type ExtractPlaceholders<T extends string> = T extends `${string}{${infer Param}}${infer Rest}`
    ? Param | ExtractPlaceholders<Rest>
    : never;

type ParamsType<P extends string> = {
    [K in P]: string | number;
};

/**
 * @summary 检查类型是否为复数对象（具有 other 属性）
 *
 * @summary Checks whether a type is a plural object (has an `other` property)
 *
 * @desc 如果 T 具有 `other: string` 属性则返回 true，否则返回 false。
 * 用于区分普通翻译字符串和复数翻译对象。
 *
 * @desc Returns true if T has an `other: string` property, false otherwise.
 * Used to distinguish plain translation strings from plural translation objects.
 * */
// 4. 检查是否为复数对象（具有 one/other 等属性）
export type IsPluralObject<T> = T extends { other: string } ? true : false;

/**
 * @summary 根据翻译路径推导 t 函数的参数类型
 *
 * @summary Derives the parameter type for the t function from a translation path
 *
 * @desc 根据路径对应的翻译值类型，自动推导所需的参数：
 * - 字符串类型 → 提取占位符，生成对应的 params 对象类型
 * - 复数对象 → 需要 `{ count: number }`
 * - 其他 → undefined
 *
 * @desc Automatically derives required parameters based on the translation value type:
 * - String type → extracts placeholders and generates the corresponding params object type
 * - Plural object → requires `{ count: number }`
 * - Other → undefined
 * */
// 5. 根据路径推导 t 函数的参数类型
export type ParamsForPath<T, P extends string> =
    ValueAtPath<T, P> extends string
        ? ExtractPlaceholders<ValueAtPath<T, P>> extends never
            ? Record<string, string | number> | undefined // 无占位符时允许任意对象或 undefined
            : ParamsType<ExtractPlaceholders<ValueAtPath<T, P>>>
        : ValueAtPath<T, P> extends { other: string }
          ? { count: number }
          : undefined;
