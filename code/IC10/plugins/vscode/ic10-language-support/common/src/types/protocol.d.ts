// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/28 19:24
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Nullable, Optional } from "./utils";


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
