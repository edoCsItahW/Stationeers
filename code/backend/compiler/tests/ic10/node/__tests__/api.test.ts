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

const {IC10Local, Token, TokenType, TokenCategory, Pos, Lexer, Program, Parser, Analyser, SymbolTable, Linker, TypeTable} = IC10NodeAPI;


beforeAll(() => {
    setupUTF8Console();

    if (typeof IC10Local.setLanguage === 'function')
        IC10Local.setLanguage('zh-hans');
});


describe('Token', () => {
    it('should create token and toString correctly', () => {
        const token = new Token(TokenType.FLOAT, new Pos(), 'a', TokenCategory.LITERAL);

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

describe('Linker', () => {
    it('should create Linker instance', () => {
        const linker = new Linker();
        expect(linker).toBeInstanceOf(Linker);
    });

    it('should link single unit and return SymbolTable', () => {
        const linker = new Linker();
        linker.addUnit('alias dev d0\nhcf\n');
        const symtab = linker.link();
        expect(symtab).toBeInstanceOf(SymbolTable);
    });

    it('should expose diagnostics after linking', () => {
        const linker = new Linker();
        linker.addUnit('alias dev d0\nhcf\n');
        linker.link();
        expect(Array.isArray(linker.diagnostics)).toBe(true);
    });

    it('should expose units after linking', () => {
        const linker = new Linker();
        linker.addUnit('alias dev d0\nhcf\n');
        linker.link();
        expect(Array.isArray(linker.units)).toBe(true);
    });

    it('should expose typeTable after linking', () => {
        const linker = new Linker();
        linker.addUnit('alias dev d0\nhcf\n');
        linker.link();
        const typeTable = linker.typeTable;
        expect(typeTable).toBeInstanceOf(TypeTable);
        expect(typeTable.toJSON()).toBeDefined();
    });

    it('should return non-empty typeTable with device types', () => {
        const linker = new Linker();
        linker.addUnit(
            '#> @device\n' +
            '#> @name Sensor\n' +
            '#> @logic Pressure rw\n' +
            '#> @end-device\n'
        );
        linker.addUnit('alias s d0 #: @type Sensor\nhcf\n');
        linker.link();
        const json = linker.typeTable.toJSON();
        expect(json).toContain('Sensor');
    });

    it('should return non-empty typeTable with enum types', () => {
        const linker = new Linker();
        linker.addUnit(
            '#> @enum\n' +
            '#> @name ReagentMode\n' +
            '#> @value Contents 0\n' +
            '#> @value Required 1\n' +
            '#> @end-enum\n'
        );
        linker.addUnit('alias f d0\nhcf\n');
        linker.link();
        const json = linker.typeTable.toJSON();
        expect(json).toContain('ReagentMode');
    });
});

