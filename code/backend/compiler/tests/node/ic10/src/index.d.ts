/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */

declare module "ic10-node-api" {

    export class IC10Local {
        static setLanguage(language: string): void;
    }

    /**
     * 位置信息（行、列）
     * 具体实现未公开，但通常包含 line 和 column 属性
     */
    export class Pos {
        /** 行号（从 0 或 1 开始，取决于内部实现） */
        line: number;
        /** 列号（从 0 或 1 开始） */
        column: number;

        offset: number;

        newline(): void;

        next(): void;

        move(index: number): void;
    }

    /**
     * 词法单元（Token）
     */
    export class Token {
        /**
         * @param type      Token 类型（数字枚举，对应 ic::TokenType）
         * @param pos       位置对象
         * @param lexeme    可选，词素字符串
         * @param category  可选，Token 类别（数字枚举，对应 ic::TokenCategory）
         */
        constructor(type: number, pos: Pos, lexeme?: string, category?: number);

        /** Token 类型（数字） */
        type: number;
        /** 位置信息 */
        pos: Pos;
        /** 词素字符串 */
        lexeme: string;
        /** Token 类别（数字） */
        category: number;

        /** 返回可读字符串表示 */
        toString(): string;
        /** 返回 JSON 字符串表示 */
        toJSON(): string;
    }

    /**
     * 词法分析器
     */
    export class Lexer {
        /**
         * @param source       待分析的源代码
         * @param debug 是否保留注释（可选，默认为 false）
         */
        constructor(source: string, debug?: boolean);

        /**
         * 静态方法：直接对源代码进行词法分析，返回 Token 数组
         * @param source 源代码字符串
         * @returns Token 数组
         */
        static tokenize(source: string): Token[];

        /**
         * 实例方法：对构造时传入的源代码进行词法分析，返回 Token 数组
         * @returns Token 数组
         */
        scan(): Token[];
    }

    export class Program {
        nodeName: string;

        statements: object[]

        end: Pos;

        toString(): string;

        toJSON(): string;
    }

    export class Parser {
        constructor(tokens: Token[], debug: boolean = false);

        parse(): Program;

        parsing(tokens: Token[], debug: boolean = false): Program;
    }

    export class SymbolTable {

        toJSON(): string;

    }


    export class Analyser {

        get symbolTable(): SymbolTable;

        get errors(): Error[];

        static analyse(program: Program): void;

        visit(program: Program): void;

    }

}


