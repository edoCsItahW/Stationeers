/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

import {DocumentCache} from "../cache";
import {Connection} from "vscode-languageserver/node";
import {Program, ASTNode, Token} from "ic10-node-api";
import {Optional} from "../../../../common/types/utils";


type OnHoverHandlerType = Parameters<Connection["onHover"]>[0];


/**
 * @file hover.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/27 21:19
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */


export class HoverHandler {
    constructor(private docCache: DocumentCache) {
    }

    public handle(...[{position, textDocument}]: Parameters<OnHoverHandlerType>): ReturnType<OnHoverHandlerType> {
        const line = position.line + 1;
        const character = position.character + 1;
        const cache = this.docCache.getCache(textDocument.uri);

        if (!cache?.ast) return {contents: []};

        let left = 0, right = cache.tokens.length - 1;
        while (left <= right) {
            const mid = Math.floor((left + right) / 2);

            const token = cache.tokens[mid];

            const tLine = token.pos.line;
            const tCol = token.pos.column;

            if (tLine > line || (tLine === line && tCol > character)) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        const idx = right;
        if (idx < 0 || idx >= cache.tokens.length) return {contents: []};

        const token = cache.tokens[idx];
        if (token.pos.line !== line) return {contents: []};

        const endCol = token.pos.column + token.lexeme.length;
        if (character < token.pos.column || character > endCol) return {contents: []};

        return {contents: [{language: "markdown", value: `(${token.type}, ${token.lexeme})`}]};
    }
}
