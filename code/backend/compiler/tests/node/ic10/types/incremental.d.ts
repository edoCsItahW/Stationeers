// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 17:11
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import { Token } from "./lexer";
import { Program } from "./parser";


export interface IncLexerResult {
    tokens: Token[];
    incremental: boolean;
    relexedLines: number;
    changedStartLine: number;
    oldChangedEndLine: number;
    newChangedEndLine: number;
}


export class IncLexer {
    constructor();

    tokenizeFull(source: string): IncLexerResult;

    tokenizeInc(newSource: string): IncLexerResult;

    hasCache(): boolean;

    clear(): void;
}


export interface IncParserResult {
    ast: Program;
    incremental: boolean;
    reparsedStmts: number;
    affectedStmtStart: number;
}


export class IncParser {
    constructor();

    parseFull(tokens: Token[]): IncParserResult;

    parseInc(tokens: Token[], changedStartLine: number): IncParserResult;

    hasCache(): boolean;

    clear(): void;
}


export interface IncCompileResult {
    tokens: Token[];
    ast: Program;
    incremental: boolean;
    relexedLines: number;
    reparsedStmts: number;
}


export class IncCompiler {
    constructor();

    compileFull(source: string): IncCompileResult;

    compileInc(newSource: string): IncCompileResult;

    hasCache(): boolean;

    clear(): void;
}