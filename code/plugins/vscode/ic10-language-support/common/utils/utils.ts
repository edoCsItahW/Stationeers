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

export interface PropertyOptions<This, Value> {
    setterChecker?: (value: Value) => boolean;
    initProc?: (this: This, value: Value) => Value;
}


export type PropertyDecoratorResult<This, Value> =
    ClassAccessorDecoratorResult<This, Value>
    | ((accessor: ClassAccessorDecoratorTarget<This, Value>) => ClassAccessorDecoratorResult<This, Value>);

export function property<This, Value>(accessor: ClassAccessorDecoratorTarget<This, Value>): ClassAccessorDecoratorResult<This, Value>;

export function property<This, Value>(options: PropertyOptions<This, Value>): (accessor: ClassAccessorDecoratorTarget<This, Value>) => ClassAccessorDecoratorResult<This, Value>;

export function property<This, Value>(
    arg: ClassAccessorDecoratorTarget<This, Value> | PropertyOptions<This, Value>
): PropertyDecoratorResult<This, Value> {
    function decorate({
                          get,
                          set
                      }: ClassAccessorDecoratorTarget<This, Value>, {
                          setterChecker,
                          initProc
                      }: PropertyOptions<This, Value>): ClassAccessorDecoratorResult<This, Value> {
        return {
            get(this: This): Value {
                return get.call(this);
            },
            set(this: This, value: Value): void {
                if (setterChecker?.(value) || !setterChecker)
                    set.call(this, value);
            },
            init(this: This, value: Value): Value {
                return initProc ? initProc.call(this, value) : value;
            }
        };
    }

    if (arg && typeof arg === 'object' && 'get' in arg && 'set' in arg)
        return decorate(arg as ClassAccessorDecoratorTarget<This, Value>, {});

    return (accessor: ClassAccessorDecoratorTarget<This, Value>) => decorate(accessor, arg)
}