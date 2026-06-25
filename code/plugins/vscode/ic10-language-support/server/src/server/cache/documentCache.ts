/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file documentCache.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/27 21:40
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
//import { Token, Program } from "ic10-node-api";
import {Optional} from "common/types/utils";


export interface DocCacheValue {
//    tokens: Token[];
//    ast: Optional<Program>;
    errors: Error[];
    hash: string;
}

export class DocumentCache {
    private caches: Map<string, DocCacheValue> = new Map();

    initDocument(uri: string): void {
        this.caches.set(uri, {
//            tokens: [],
//            ast: undefined,
            errors: [],
            hash: ""
        });
    }

    getCache(uri: string): Optional<DocCacheValue> {
        return this.caches.get(uri);
    }

    invalidateHash(uri: string): void {
        const cache = this.caches.get(uri);
        if (cache) cache.hash = "";
    }

}