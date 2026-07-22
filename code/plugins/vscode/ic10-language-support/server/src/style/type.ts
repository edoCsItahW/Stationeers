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

export type IsLeaf<T> = T extends string | number ? true : false;

export type StylePaths<T, P extends string = '', D extends number = 10> = [D] extends [0]
    ? P
    : T extends string | number | boolean | null | undefined
        ? P
        : T extends object
            ? {
                [K in keyof T]: StylePaths<T[K], P extends '' ? K & string : `${P}.${K & string}`, D extends 0 ? 0 : D extends 1 ? 0 : D extends 2 ? 1 : D extends 3 ? 2 : D extends 4 ? 3 : D extends 5 ? 4 : D extends 6 ? 5 : D extends 7 ? 6 : D extends 8 ? 7 : D extends 9 ? 8 : D extends 10 ? 9 : 0>
            }[keyof T]
            : never;

export type ValueAtPath<T, P extends string> = P extends `${infer Head}.${infer Tail}`
    ? Head extends keyof T
        ? ValueAtPath<T[Head], Tail>
        : never
    : P extends keyof T
        ? T[P]
        : never;