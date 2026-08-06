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

/**
 * @summary 属性装饰器的可选配置
 *
 * @summary Optional configuration for the property decorator
 *
 * @desc 包含 setterChecker（值校验函数）和 initProc（初始化处理函数）。
 *
 * @desc Contains setterChecker (value validation function) and initProc
 * (initialization processing function).
 * */
export interface PropertyOptions<This, Value> {
    setterChecker?: (value: Value) => boolean;
    initProc?: (this: This, value: Value) => Value;
}

/**
 * @summary 属性装饰器的返回类型
 *
 * @summary Return type of the property decorator
 *
 * @desc 可能是 ClassAccessorDecoratorResult 或返回它的工厂函数，
 * 取决于装饰器是否带参数调用。
 *
 * @desc Either a ClassAccessorDecoratorResult or a factory returning one,
 * depending on whether the decorator is called with arguments.
 * */
export type PropertyDecoratorResult<This, Value> =
    | ClassAccessorDecoratorResult<This, Value>
    | ((accessor: ClassAccessorDecoratorTarget<This, Value>) => ClassAccessorDecoratorResult<This, Value>);

/**
 * @summary 类访问器属性装饰器，支持值校验和初始化处理
 *
 * @summary Class accessor property decorator with value validation and init processing
 *
 * @desc ECMAScript 装饰器，可重载为直接使用或带配置使用。
 * 支持 setterChecker 拦截非法值、initProc 在初始化时转换值。
 *
 * @desc ECMAScript decorator with overloads for direct use or use with options.
 * Supports setterChecker for intercepting invalid values and initProc for
 * transforming values during initialization.
 * */
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
 * @summary 二分查找，在已排序数组中定位目标值
 *
 * @summary Binary search for locating a target value in a sorted array
 *
 * @desc 在升序排列的数组中查找目标值，返回其索引；若不存在则返回 -1。
 * 支持自定义比较函数。时间复杂度 O(log n)。
 *
 * @desc Finds a target value in an ascending array, returning its index or -1 if absent.
 * Supports custom comparison function. Time complexity O(log n).
 *
 * @param arr 已按升序排列的数组
 * @param target 要查找的值
 * @param compare 比较函数，返回负数表示 a < b，0 表示相等，正数表示 a > b
 * */
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
 * @summary 二分查找第一个满足条件的元素索引（lower_bound）
 *
 * @summary Binary search for the first element satisfying a predicate (lower_bound)
 *
 * @desc 前提：pred 在数组上呈 [false, ..., false, true, ..., true] 分布。
 * 返回第一个使 pred(item) 为 true 的索引。若无满足项则返回 arr.length。
 *
 * @desc Requires that pred is monotonic: [false, ..., false, true, ..., true] across the array.
 * Returns the index of the first element where pred(item) is true, or arr.length if none match.
 * */
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

/**
 * @summary 二分查找最后一个满足条件的元素索引（upper_bound）
 *
 * @summary Binary search for the last element satisfying a predicate (upper_bound)
 *
 * @desc 前提：pred 在数组上呈 [false, ..., false, true, ..., true] 分布。
 * 返回最后一个使 pred(item) 为 true 的索引。若无满足项则返回 -1。
 *
 * @desc Requires that pred is monotonic: [false, ..., false, true, ..., true] across the array.
 * Returns the index of the last element where pred(item) is true, or -1 if none match.
 * */
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
 * @summary 在无序数组中找出满足条件的最大元素
 *
 * @summary Finds the maximum element satisfying a condition in an unsorted array
 *
 * @desc 遍历数组，筛选出满足 condition 的元素，再按 compare 比较取最大值。
 * 时间复杂度 O(n)。
 *
 * @desc Scans the array, filters elements by condition, then picks the maximum
 * via compare. Time complexity O(n).
 *
 * @param arr 待搜索的数组（无序）
 * @param condition 判断元素是否可参与候选
 * @param compare 比较函数，返回负数表示 a < b，0 相等，正数表示 a > b
 * @returns 满足条件的最大元素，若无则返回 undefined
 * */
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
 * @summary 在无序数组中找出满足条件的最小元素
 *
 * @summary Finds the minimum element satisfying a condition in an unsorted array
 *
 * @desc 遍历数组，筛选出满足 condition 的元素，再按 compare 比较取最小值。
 * 时间复杂度 O(n)。
 *
 * @desc Scans the array, filters elements by condition, then picks the minimum
 * via compare. Time complexity O(n).
 *
 * @param arr 待搜索的数组（无序）
 * @param condition 判断元素是否可参与候选
 * @param compare 比较函数，返回负数表示 a < b，0 相等，正数表示 a > b
 * @returns 满足条件的最小元素，若无则返回 undefined
 * */
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

/**
 * @summary 根据枚举值反查枚举键名
 *
 * @summary Reverse-lookup an enum key by its value
 *
 * @desc 在枚举对象中查找给定值对应的键名。适用于数值枚举和字符串枚举。
 * 若值不存在则返回 undefined。
 *
 * @desc Searches an enum object for the key matching a given value.
 * Works with both numeric and string enums. Returns undefined if not found.
 * */
export function getEnumName<T extends Record<string, string | number>>(
    enumObj: T,
    value: string | number
): keyof T | undefined {
    // 获取所有键（类型为 (string|number)[]，但数值枚举有数字索引，需过滤）
    const keys = Object.keys(enumObj) as (keyof T)[];

    // 在键数组中查找，使得 enumObj[key] === value
    return keys.find(key => enumObj[key] === value);
}
