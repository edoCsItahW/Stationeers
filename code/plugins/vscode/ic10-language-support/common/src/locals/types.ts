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

// 1. 递归提取所有叶子节点的点路径（无深度限制）
export type Paths<T> = PathsOf<T, { leavesOnly: true }>;

// 2. 根据点路径获取值类型
export type ValueAtPath<T, P extends string> = Get<T, P>;

// 3. 从模板字符串中提取变量名（如 "Hello {name}" -> "name"）
export type ExtractPlaceholders<T extends string> = T extends `${string}{${infer Param}}${infer Rest}`
    ? Param | ExtractPlaceholders<Rest>
    : never;

type ParamsType<P extends string> = {
    [K in P]: string | number;
};

// 4. 检查是否为复数对象（具有 one/other 等属性）
export type IsPluralObject<T> = T extends { other: string } ? true : false;

// 5. 根据路径推导 t 函数的参数类型
export type ParamsForPath<T, P extends string> =
    ValueAtPath<T, P> extends string
        ? ExtractPlaceholders<ValueAtPath<T, P>> extends never
            ? Record<string, string | number> | undefined // 无占位符时允许任意对象或 undefined
            : ParamsType<ExtractPlaceholders<ValueAtPath<T, P>>>
        : ValueAtPath<T, P> extends { other: string }
          ? { count: number }
          : undefined;
