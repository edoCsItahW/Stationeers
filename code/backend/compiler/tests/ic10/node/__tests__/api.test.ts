/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */
import IC10NodeAPI = require('ic10-node-api');
import {setupUTF8Console} from "../utils";

const {IC10Local, Token, Pos, Lexer, Program, Parser, Analyser, SymbolTable} = IC10NodeAPI;


beforeAll(() => {
    setupUTF8Console();

    if (typeof IC10Local.setLanguage === 'function')
        IC10Local.setLanguage('zh-hans');
});


describe('Token', () => {
    it('should create token and toString correctly', () => {
        const token = new Token(1, new Pos(), 'a', 3);

        expect(token.toString()).toBe("Token<1,1>[FLOAT]('a')");
    });
});


describe('Lexer', () => {
    it('should tokenize static method', () => {
        const result = Lexer.tokenize('alias ic d0');

        expect(result).toHaveLength(4); // 假设三个 token

        expect(result[0].toString()).toMatch(/alias/);
    });

    it('should scan via instance', () => {
        const lexer = new Lexer('alias ic d0');
        const result = lexer.scan();
        expect(result).toHaveLength(4);
    });
});

describe('Parser', () => {
    it('should parse tokens into Program', () => {
        const tokens = Lexer.tokenize('alias ic d0');
        const parser = new Parser(tokens);
        const program = parser.parse();
        expect(program).toBeInstanceOf(Program);
        expect(program.statements).toBeDefined();
        expect(program.toString()).toBeDefined();
    });
});

describe('Analyser', () => {
    it('should analyse program and generate symbol table', async () => {
        const tokens = Lexer.tokenize('alias ic d0');
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);
        const symbolTable = analyser.symbolTable;
        expect(symbolTable).toBeInstanceOf(SymbolTable);
        expect(symbolTable.toJSON()).toBeDefined();
    });
});

