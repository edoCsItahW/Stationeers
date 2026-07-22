/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: edocsitahw@qq.com
 */

/**
 * @file parserPipline.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/29 17:03
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Lexer, Parser,  Linker, type SymbolMap} from "ic10-node-api";
import stdLib from "ic10-node-api/static/stdLib.ic.json"
import { createHash } from "node:crypto";

import { DocCacheValue } from "../cache";


export interface ParseResult extends DocCacheValue {
    changed: boolean;
}


export class ParserPipline {

    public async parse(code: string, cache?: DocCacheValue): Promise<ParseResult> {
        const noop: ParseResult = {
            changed: false,
            tokens: cache?.tokens ?? [],
            ast: cache?.ast!,
            diagnostics: cache?.diagnostics ?? [],
            symbols: cache?.symbols ?? null,
            hash: cache?.hash ?? ""
        };

        if (!code.trim().length) return noop;

        const hash = createHash("md5").update(code).digest("hex");
        if (cache?.hash === hash) return noop;

        const lexer = new Lexer(code);
        const tokens = lexer.scan();

        const parser = new Parser(tokens);
        const ast = parser.parse();

        const linker = new Linker();

        linker.addUnit(stdLib.content);
        linker.addUnit(ast);

        const symbolJson = linker.link().toJSON();


        return { changed: true, tokens, ast, diagnostics: linker.diagnostics, symbols: JSON.parse(symbolJson) as SymbolMap, hash };
    }

}
