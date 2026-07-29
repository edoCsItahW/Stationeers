/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file utils.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/27 23:57
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Optional } from "../types";

export interface PropertyOptions<This, Value> {
    setterChecker?: (value: Value) => boolean;
    initProc?: (this: This, value: Value) => Value;
}

export type PropertyDecoratorResult<This, Value> =
    | ClassAccessorDecoratorResult<This, Value>
    | ((accessor: ClassAccessorDecoratorTarget<This, Value>) => ClassAccessorDecoratorResult<This, Value>);

export function property<This, Value>(
    accessor: ClassAccessorDecoratorTarget<This, Value>
): ClassAccessorDecoratorResult<This, Value>;

export function property<This, Value>(
    options: PropertyOptions<This, Value>
): (accessor: ClassAccessorDecoratorTarget<This, Value>) => ClassAccessorDecoratorResult<This, Value>;

export function property<This, Value>(
    arg: ClassAccessorDecoratorTarget<This, Value> | PropertyOptions<This, Value>
): PropertyDecoratorResult<This, Value> {
    function decorate(
        { get, set }: ClassAccessorDecoratorTarget<This, Value>,
        { setterChecker, initProc }: PropertyOptions<This, Value>
    ): ClassAccessorDecoratorResult<This, Value> {
        return {
            get(this: This): Value {
                return get.call(this);
            },
            set(this: This, value: Value): void {
                if (setterChecker?.(value) || !setterChecker) set.call(this, value);
            },
            init(this: This, value: Value): Value {
                return initProc ? initProc.call(this, value) : value;
            }
        };
    }

    if (arg && typeof arg === "object" && "get" in arg && "set" in arg)
        return decorate(arg as ClassAccessorDecoratorTarget<This, Value>, {});

    return (accessor: ClassAccessorDecoratorTarget<This, Value>) => decorate(accessor, arg);
}

/**
 * 在已排序数组中查找目标值，返回其索引，若不存在则返回 -1
 * @param arr 已按升序排列的数组
 * @param target 要查找的值
 * @param compare 比较函数，返回负数表示 a < b，0 表示相等，正数表示 a > b
 */
export function binarySearch<T>(
    arr: T[],
    target: T,
    compare: (a: T, b: T) => number = (a, b) => (a as any) - (b as any)
): number {
    let low = 0;
    let high = arr.length - 1;

    while (low <= high) {
        const mid = Math.floor((low + high) / 2);
        const cmp = compare(arr[mid], target);

        if (cmp === 0) return mid;
        if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

/**
 * 查找第一个满足条件的元素的索引（前提：pred 在数组上呈 [false, ..., false, true, ..., true] 分布）
 */
export function lowerBound<T>(arr: T[], pred: (item: T) => boolean): number {
    let low = 0;
    let high = arr.length; // 注意这里是 length，不是 length - 1

    while (low < high) {
        const mid = (low + high) >> 1; // 等价于 Math.floor((low+high)/2)
        if (pred(arr[mid]))
            high = mid; // 满足条件，向左收缩
        else low = mid + 1; // 不满足条件，向右收缩
    }

    return low; // 第一个 true 的索引
}

export function upperBound<T>(arr: T[], pred: (item: T) => boolean): number {
    let low = 0;
    let high = arr.length;

    while (low < high) {
        const mid = (low + high) >> 1;
        if (pred(arr[mid]))
            low = mid + 1; // 满足条件，向右收缩
        else high = mid;
    }

    return low - 1; // 最后一个 true 的索引，如果都不满足则返回 -1
}

/**
 * 在无序数组中，找出满足指定条件的最大元素
 * @param arr 待搜索的数组（无序）
 * @param condition 判断元素是否可参与候选
 * @param compare 比较函数，返回负数表示 a < b，0 相等，正数表示 a > b
 * @returns 满足条件的最大元素，若无则返回 undefined
 */
export function findMaxByCondition<T>(
    arr: T[],
    condition: (item: T) => boolean,
    compare: (a: T, b: T) => number = defaultCompare
): Optional<T> {
    let best: T | undefined;

    for (const item of arr) {
        if (!condition(item)) continue;
        if (best === undefined || compare(item, best) > 0) best = item;
    }

    return best;
}

/**
 * 在无序数组中，找出满足指定条件的最小元素
 * @param arr 待搜索的数组（无序）
 * @param condition 判断元素是否可参与候选
 * @param compare 比较函数，返回负数表示 a < b，0 相等，正数表示 a > b
 * @returns 满足条件的最小元素，若无则返回 undefined
 */
export function findMinByCondition<T>(
    arr: T[],
    condition: (item: T) => boolean,
    compare: (a: T, b: T) => number = defaultCompare
): Optional<T> {
    let best: T | undefined;

    for (const item of arr) {
        if (!condition(item)) continue;
        if (best === undefined || compare(item, best) < 0) best = item;
    }

    return best;
}

function defaultCompare<T>(a: T, b: T): number {
    return (a as any) - (b as any);
}

export function getEnumName<T extends Record<string, string | number>>(
    enumObj: T,
    value: string | number
): keyof T | undefined {
    // 获取所有键（类型为 (string|number)[]，但数值枚举有数字索引，需过滤）
    const keys = Object.keys(enumObj) as (keyof T)[];

    // 在键数组中查找，使得 enumObj[key] === value
    return keys.find(key => enumObj[key] === value);
}
