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
import { createHash } from "node:crypto";
import { DocCacheValue } from "../cache";
import {Analyser, Lexer, Parser, Program, Token} from "ic10-node-api";


export interface ParseResult {
    changed: boolean;
    tokens: Token[];
    ast: Program;
    errors: Error[];
    hash: string;
}


export class ParserPipline {

    public async parse(code: string, cache?: DocCacheValue) {
        const noop: ParseResult = {
            changed: false,
            tokens: cache?.tokens ?? [],
            ast: cache?.ast!,
            errors: cache?.errors ?? [],
            hash: cache?.hash ?? ""
        };

        if (!code.trim().length) return noop;

        const hash = createHash("md5").update(code).digest("hex");
        if (cache?.hash === hash) return noop;

        const lexer = new Lexer(code);
        const tokens = lexer.scan();

        const parser = new Parser(tokens);
        const ast = parser.parse();

        const analyser = new Analyser();
        let errors: Error[] = [];
        try {
            await analyser.visit(ast);
            errors = analyser.errors as Error[];
        } catch (e) {
            console.error("[IC10 LSP] Analyser error:", e);
        }

        return { changed: true, tokens, ast, errors, hash };
    }

}
