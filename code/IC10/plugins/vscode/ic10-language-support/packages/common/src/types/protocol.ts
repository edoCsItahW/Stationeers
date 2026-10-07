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
 * @if zh
 * @summary 一条设备类型索引项
 *
 * @desc 由服务端从标准库（`stdLib.ic` 的 `#> @device` 块）取出类型名与型号哈希，再按设备型号名
 * （型号名压成标识符后即类型名）配上 `@ic10/metadata` 的本地化文案，因此 `title` / `desc` 用的
 * 是**当前语言**，缺失时为 `undefined`（该设备在元数据里没有本地化条目，或该语言没有译文）。
 *
 * @else
 * @summary One device type index entry
 *
 * @desc Built by the server from the standard library (`#> @device` blocks in `stdLib.ic`) for the
 * type name and model hash, joined with the localized text of `@ic10/metadata` by model name (a model
 * name turns into the type name once collapsed into an identifier). `title` / `desc` are therefore in
 * the **current language** and are `undefined` when the device has no localized entry, or that
 * language has no translation.
 *
 * @endif
 * */
export interface DeviceTypeInfo {
    /** @if zh @brief 类型名（写进 `#: @type` 的标识符） @else @brief Type name (the identifier written after `#: @type`) @endif */
    typeName: string;

    /** @if zh @brief 设备型号哈希（`#> @device-hash`） @else @brief Device model hash (`#> @device-hash`) @endif */
    hash: Optional<number>;

    /** @if zh @brief 当前语言下的显示名 @else @brief Display name in the current language @endif */
    title: Optional<string>;

    /**
     * @if zh
     * @brief 英文显示名（跨语言的稳定别名）
     *
     * @desc 非英语界面下与 {@link title} 一起展示（`本地化名 + 英文名`）：只记得英文设备名的用户
     *       不必去猜本地化名对应哪个型号，将来增加语言也不会让候选越来越杂——第二显示名始终是英语。
     *       当前语言就是英语时它与 `title` 相同，前端只展示一次。
     *
     * @else
     * @brief English display name (the language-independent alias)
     *
     * @desc Shown next to {@link title} outside English locales (`localized name + English name`), so a
     *       user who only remembers the English device name need not guess which model a localized name
     *       maps to, and adding more languages later cannot make the list messier — the second name stays
     *       English. It equals `title` under an English locale and is then shown only once.
     *
     * @endif
     * */
    englishTitle: Optional<string>;

    /** @if zh @brief 当前语言下的描述 @else @brief Description in the current language @endif */
    desc: Optional<string>;
}

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
