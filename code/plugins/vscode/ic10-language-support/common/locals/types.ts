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

export type Paths<T, P extends string = '', D extends number = 10> = D extends 0
    ? P
    : T extends string | number | boolean | null | undefined
        ? P
        : {
            [K in keyof T]: Paths<T[K], P extends '' ? K & string : `${P}.${K & string}`, D extends 0 ? 0 : D extends 1 ? 0 : D extends 2 ? 1 : D extends 3 ? 2 : D extends 4 ? 3 : D extends 5 ? 4 : D extends 6 ? 5 : D extends 7 ? 6 : D extends 8 ? 7 : D extends 9 ? 8 : D extends 10 ? 9 : 0>;
        }[keyof T];

// 2. 根据点路径获取值类型
export type ValueAtPath<T, P extends string> = P extends `${infer Head}.${infer Tail}`
    ? Head extends keyof T
        ? ValueAtPath<T[Head], Tail>
        : never
    : P extends keyof T
        ? T[P]
        : never;

// 3. 从模板字符串中提取变量名（如 "Hello {name}" -> "name"）
export type ExtractPlaceholders<T extends string> =
    T extends `${string}{${infer Param}}${infer Rest}`
        ? Param | ExtractPlaceholders<Rest>
        : never;

// 4. 检查是否为复数对象（具有 one/other 等属性）
export type IsPluralObject<T> = T extends { other: string } ? true : false;

// 5. 根据路径推导 t 函数的参数类型
export type ParamsForPath<T, P extends string> =
    ValueAtPath<T, P> extends string
        ? ExtractPlaceholders<ValueAtPath<T, P>> extends never
            ? undefined // 无需参数
            : Record<ExtractPlaceholders<ValueAtPath<T, P>>, string | number>
        : ValueAtPath<T, P> extends { other: string }
            ? { count: number } // 复数必须传 count
            : undefined;