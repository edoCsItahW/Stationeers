// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file protocol.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/28 19:24
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Optional } from "./utils";


export interface RequestEventDataBase {
    type: string;
    data: any;
}

export interface ResponseEventDataBase {
    type: string;
    data: any;
}

export interface AstRequestEventData extends RequestEventDataBase {
    type: "ast";
    data: {
        uri: string;
    };
}

export interface AstResponseEventData extends ResponseEventDataBase {
    type: "ast";
    data: Optional<{
        // 跨进程 JSON-RPC 无法传递 C++ 包装对象(Program/SymbolTable) -> 传递源码
        source: string;
    }>;
}

export type RequestEventData = AstRequestEventData;

export type ResponseEventData = AstResponseEventData;


export interface Transfer {
    handle(data: RequestEventData): Promise<ResponseEventData>;
}

/**
 * @summary 补全范围：把补全候选收窄到某一类操作数
 *
 * @summary Completion scope: narrow the completion candidates to one kind of operand
 *
 * @desc 取值对应补全提供器的书写形式族（见服务端的 `SemanticMap`）。`keyword` 用于语句头部
 * （补全指令关键字），`all` 表示不限制（清空范围）。
 *
 * @desc The values mirror the writing-form families of the completion providers (see the server's
 * `SemanticMap`). `keyword` targets the statement head (instruction keywords) and `all` means no
 * restriction (clears the scope).
 * */
export type CompletionScope = "register" | "device" | "identifier" | "number" | "enum" | "keyword" | "all";

/**
 * @summary 补全范围事件的数据
 *
 * @summary Payload of the completion scope event
 * */
export interface CompletionScopeEventData {
    /** @if zh @brief 文档 URI @else @brief Document URI @endif */
    uri: string;

    /** @if zh @brief 期望的操作数类别 @else @brief The expected operand kind @endif */
    scope: CompletionScope;

    /**
     * @if zh
     * @brief 按键时光标所在行（1-based）
     *
     * @desc 范围在这一行内保持，光标换到别的行即自动清除：同一行里依次填操作数时不必反复按键，
     * 换行后也不会影响下一次补全。
     *
     * @else
     * @brief The cursor line when the key was pressed (1-based)
     *
     * @desc The scope lasts for that line and is dropped once the cursor moves elsewhere, so filling
     * operands on one line needs no repeated key presses and the next line stays unaffected.
     *
     * @endif
     * */
    line: number;
}
