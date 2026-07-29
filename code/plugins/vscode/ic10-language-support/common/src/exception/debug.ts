// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file debug.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/24 11:40
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Optional } from "../types";

interface DebugOptions {
    message?: string | ((error: unknown, ...args: any[]) => string);
    logger?: (message: string) => void;
    rethrow?: boolean;
}

export type FunctionType<Args extends unknown[] = unknown[], Return = unknown> = (...args: Args) => Return;

export type ClassMethodType<This = unknown, Args extends unknown[] = unknown[], Return = unknown> = (
    this: This,
    ...args: Args
) => Return;

export type ClassMethodDecoratorSignature<This = unknown, Args extends unknown[] = unknown[], Return = unknown> = [
    target: This,
    context: ClassMethodDecoratorContext<This, ClassMethodType<This, Args, Return>>
];

export type ClassMethodDecorator<This = unknown, Args extends unknown[] = unknown[], Return = unknown> = (
    ...args: ClassMethodDecoratorSignature<This, Args, Return>
) => void;


export function debug(options: DebugOptions = {}) {
    const {
        message: msgOrFn = (err: unknown) => `Error in method: ${err instanceof Error ? err.message : String(err)}`,
        logger = console.error,
        rethrow = true
    } = options;

    return function <This, Args extends any[], Return>(
        target: any, // 转译模式下为原函数本身，原生模式下为 undefined（实例方法）或构造函数（静态方法）
        context: ClassMethodDecoratorContext<This, (this: This, ...args: Args) => Return>
    ): ((this: This, ...args: Args) => Return) | void {
        if (context.kind !== "method") return;

        const methodName = String(context.name);
        const original: (this: This, ...args: Args) => Return =
            typeof target === "function"
                ? target
                : target?.[methodName];

        if (typeof original !== "function") return;

        // 返回包装函数，__esDecorate 会用它替换原型上的方法
        return function (this: This, ...args: Args): Return {
            try {
                return original.apply(this, args);
            } catch (err) {
                const errorMessage =
                    typeof msgOrFn === "function"
                        ? msgOrFn(err, ...args)
                        : `${msgOrFn}: ${err instanceof Error ? err.message : String(err)}`;
                logger(errorMessage);

                if (rethrow) throw err;
                // 若 rethrow = false，则返回 undefined，这可能导致类型不匹配
                // 但编译器会警告，因此建议保持 rethrow 为 true（默认）
                return undefined as any as Return;
            }
        };
    };
}

function applyDebug<This extends object, Args extends unknown[], Return>(
    target: This,
    context: ClassMethodDecoratorContext<This, ClassMethodType<This, Args, Return>>,
    options: DebugOptions
) {
    if (context.kind !== "method") return;

    const mthName = String(context.name);
    const original = target[mthName as keyof This] as ClassMethodType<This, Args, Return>;
    if (typeof original !== "function") return;

    const { message = `Error in method ${mthName}`, logger = console.error, rethrow = true } = options;

    (target as any)[mthName] = function (this: This, ...args: Args): Optional<Return> {
        try {
            return original.apply(this, args);
        } catch (err) {
            let msg =
                typeof message === "function"
                    ? message(err, ...args)
                    : `${message}: ${err instanceof Error ? err.message : String(err)}`;

            logger(msg);

            if (rethrow) throw err;
        }
    };
}

enum Level {
    Info = 0,
    Warning,
    Error
}

export class Console {
    static debug(level: Level, message?: string, part: string = "") {
        switch (level) {
            case Level.Info:
                console.log(`[IC10 LSP] <${part}> INFO: ${message}`);
                break;
            case Level.Warning:
                console.warn(`[IC10 LSP] <${part}> WARNING: ${message}`);
                break;
            case Level.Error:
                console.error(`[IC10 LSP] <${part}> ERROR: ${message}`);
                break;
        }
    }

    static info(message?: string, part: string = "") {
        Console.debug(Level.Info, message, part);
    }

    static warning(message?: string, part: string = "") {
        Console.debug(Level.Warning, message, part);
    }

    static error(message?: string, part: string = "") {
        Console.debug(Level.Error, message, part);
    }
}
