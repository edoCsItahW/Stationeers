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

/**
 * @summary 通用函数类型别名
 *
 * @summary Generic function type alias
 *
 * @desc 表示任意参数列表和返回类型的函数签名。
 *
 * @desc Represents a function signature with arbitrary argument list and return type.
 * */
export type FunctionType<Args extends unknown[] = unknown[], Return = unknown> = (...args: Args) => Return;

/**
 * @summary 类方法类型，包含 this 上下文绑定
 *
 * @summary Class method type with this-context binding
 *
 * @desc 与 FunctionType 类似但显式声明 this 参数，用于类方法装饰器的类型标注。
 *
 * @desc Similar to FunctionType but explicitly declares the `this` parameter,
 * used for type annotations in class method decorators.
 * */
export type ClassMethodType<This = unknown, Args extends unknown[] = unknown[], Return = unknown> = (
    this: This,
    ...args: Args
) => Return;

/**
 * @summary 类方法装饰器的参数签名类型
 *
 * @summary Parameter signature type for class method decorators
 *
 * @desc ECMAScript 装饰器规范中类方法装饰器接收的两个参数：
 * target（原型对象）和 context（装饰器上下文）。
 *
 * @desc The two parameters received by class method decorators per the
 * ECMAScript decorator spec: target (prototype) and context (decorator context).
 * */
export type ClassMethodDecoratorSignature<This = unknown, Args extends unknown[] = unknown[], Return = unknown> = [
    target: This,
    context: ClassMethodDecoratorContext<This, ClassMethodType<This, Args, Return>>
];

/**
 * @summary 类方法装饰器函数类型
 *
 * @summary Class method decorator function type
 *
 * @desc 接收 ClassMethodDecoratorSignature 参数、无返回值的装饰器函数类型。
 *
 * @desc A decorator function type that accepts ClassMethodDecoratorSignature arguments
 * and returns void.
 * */
export type ClassMethodDecorator<This = unknown, Args extends unknown[] = unknown[], Return = unknown> = (
    ...args: ClassMethodDecoratorSignature<This, Args, Return>
) => void;

/**
 * @summary 方法装饰器工厂，为类方法添加错误捕获和日志记录
 *
 * @summary Method decorator factory that adds error catching and logging to class methods
 *
 * @desc 返回一个 ECMAScript 类方法装饰器，自动包装原方法并捕获异常。
 * 支持自定义错误消息、日志函数和是否重新抛出。默认用 console.error 输出并重新抛出。
 *
 * @desc Returns an ECMAScript class method decorator that wraps the original method
 * with try-catch. Supports custom error messages, logger, and rethrow control.
 * Defaults to console.error logging with rethrow enabled.
 *
 * @remarks `rethrow: false` 时会在异常时返回 undefined，可能导致类型不匹配。
 * 建议保持默认的 `rethrow: true`。
 *
 * @remarks Setting `rethrow: false` returns undefined on error, which may cause
 * type mismatches. Keeping the default `rethrow: true` is recommended.
 * */
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
            typeof target === "function" ? target : target?.[methodName];

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

let FIRST: boolean = true;

export function traceback(exit: boolean = false, rethrow: boolean = false) {
    return function (orgMth: Function, context: ClassMethodDecoratorContext) {
        return function (this: any, ...args: any[]) {
            try {
                return orgMth.apply(this, args);
            } catch (e: any) {
                if (rethrow) throw e;

                const { msg, stack } = regexStack(e.stack);

                if (FIRST) {
                    // 确保仅在第一次调用时打印
                    console.error(`Traceback (most recent call last):`);
                    FIRST = false;
                }

                console.error(
                    `    File "${stack[0].file}", line ${stack[0].line}, in <${orgMth.name}>\n\t${e.message}`
                );

                if (exit) process.exit(1);
            }
        };
    };
}

export interface IError {
    func: string;
    file: string;
    line: string;
}

export function regexStack(stack: string): {
    msg: string;
    stack: IError[];
} {
    const stackRegex: RegExp = /(?<=at)\s+([^(]+?)\s+[(]([^)]+):(\d+):(\d+)[)]/g; // 如: at Function.main (server.ts:12:13)

    const matches: IError[] = [];
    let match: RegExpExecArray | null;

    while ((match = stackRegex.exec(stack)) !== null) {
        matches.push({
            func: match[1] || "main",
            file: match[2] || "unknown",
            line: match[3]
        });
    }

    return { msg: stack.match(/^(.*?)(?=\n)/)?.[1] || "", stack: matches };
}


enum Level {
    Info = 0,
    Warning,
    Error
}

/**
 * @summary 带统一前缀的静态日志工具类
 *
 * @summary Static logging utility with a unified prefix
 *
 * @desc 所有日志输出均带有 `[IC10 LSP]` 前缀和可选的模块标签。
 * 提供 info、warning、error 三个级别的便捷静态方法。
 *
 * @desc All log output carries the `[IC10 LSP]` prefix and an optional module tag.
 * Provides convenience static methods for info, warning, and error levels.
 * */
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
