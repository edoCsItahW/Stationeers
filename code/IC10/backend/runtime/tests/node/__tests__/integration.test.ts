/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */
import IC10C = require('ic10c-node');
import IC10R = require('ic10r-node');
import {setupUTF8Console} from "../utils";

const {IC10Local, Lexer, Parser, Analyser} = IC10C;
const {Engine} = IC10R;

beforeAll(() => {
    setupUTF8Console();

    if (typeof IC10Local.setLanguage === 'function')
        IC10Local.setLanguage('zh-hans');
});

/**
 * 编译源码并执行 runFull，返回指定寄存器值。
 */
async function runAndGetReg(source: string, regName: string): Promise<number> {
    const tokens = Lexer.tokenize(source);
    const parser = new Parser(tokens);
    const program = parser.parse();
    const analyser = new Analyser();
    await analyser.visit(program);
    const engine = new Engine(program, analyser.symbolTable);
    engine.runFull();
    return engine.getReg(regName);
}

/**
 * 编译源码并返回 Engine 实例。
 */
async function compile(source: string): Promise<{
    engine: InstanceType<typeof Engine>;
    reg: (name: string) => number;
}> {
    const tokens = Lexer.tokenize(source);
    const parser = new Parser(tokens);
    const program = parser.parse();
    const analyser = new Analyser();
    await analyser.visit(program);
    const engine = new Engine(program, analyser.symbolTable);
    return {
        engine,
        reg: (name: string) => engine.getReg(name)
    };
}

// ============================================================
// 算术和算法
// ============================================================

describe('Arithmetic and algorithms', () => {
    it('should compute factorial 5! = 120', async () => {
        const src =
            'move r0 5\n' +
            'move r1 1\n' +
            'loop:\n' +
            'breqz r0 end\n' +
            'mul r1 r1 r0\n' +
            'sub r0 r0 1\n' +
            'j loop\n' +
            'end:\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r1')).toBe(120);
    });

    it('should compute sum 1..5 = 15', async () => {
        const src =
            'move r0 0\n' +
            'move r1 1\n' +
            'loop:\n' +
            'add r0 r0 r1\n' +
            'add r1 r1 1\n' +
            'ble r1 5 loop\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(15);
    });

    it('should compute Fibonacci F10 = 55', async () => {
        const src =
            'move r0 0\n' +
            'move r1 1\n' +
            'move r2 10\n' +
            'loop:\n' +
            'breqz r2 end\n' +
            'add r3 r0 r1\n' +
            'move r0 r1\n' +
            'move r1 r3\n' +
            'sub r2 r2 1\n' +
            'j loop\n' +
            'end:\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(55);
    });
});

// ============================================================
// 条件逻辑
// ============================================================

describe('Conditional logic', () => {
    it('should find max of three', async () => {
        const src =
            'move r1 10\n' +
            'move r2 25\n' +
            'move r3 15\n' +
            'bge r1 r2 skip1\n' +
            'move r1 r2\n' +
            'skip1:\n' +
            'bge r1 r3 skip2\n' +
            'move r1 r3\n' +
            'skip2:\n' +
            'move r0 r1\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(25);
    });

    it('should count even numbers in [1,10]', async () => {
        const src =
            'move r0 0\n' +
            'move r1 1\n' +
            'loop:\n' +
            'bgt r1 10 end\n' +
            'move r2 r1\n' +
            'mod r2 r2 2\n' +
            'brnez r2 next\n' +
            'add r0 r0 1\n' +
            'next:\n' +
            'add r1 r1 1\n' +
            'j loop\n' +
            'end:\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(5);
    });
});

// ============================================================
// Stack 操作
// ============================================================

describe('Stack operations', () => {
    it('should reverse via stack', async () => {
        const {engine, reg} = await compile(
            'push 1\n' +
            'push 2\n' +
            'push 3\n' +
            'pop r0\n' +
            'pop r1\n' +
            'pop r2\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(3);
        expect(reg('r1')).toBe(2);
        expect(reg('r2')).toBe(1);
    });
});

// ============================================================
// 相对跳转
// ============================================================

describe('Relative jump', () => {
    it('jr should skip instructions', async () => {
        const {engine, reg} = await compile(
            'jr 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });
});

// ============================================================
// define / alias
// ============================================================

describe('Define and alias', () => {
    it('should resolve define to immediate', async () => {
        const src =
            'define foo 42\n' +
            'move r0 foo\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(42);
    });

    it('should handle multiple defines', async () => {
        const src =
            'define A 10\n' +
            'define B 20\n' +
            'define C 30\n' +
            'move r0 A\n' +
            'add r0 r0 B\n' +
            'add r0 r0 C\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(60);
    });
});

// ============================================================
// 注释和空行
// ============================================================

describe('Comments and whitespace', () => {
    it('should ignore comments and empty lines', async () => {
        const src =
            '# This is a comment\n' +
            'move r0 7\n' +
            '# Another comment\n' +
            '\n' +
            'add r0 r0 3\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(10);
    });
});

// ============================================================
// 前向引用
// ============================================================

describe('Forward references', () => {
    it('should resolve forward label references', async () => {
        const src =
            'beq r0 r0 end\n' +
            'move r0 1\n' +
            'end:\n' +
            'move r0 99\n' +
            'hcf\n';
        expect(await runAndGetReg(src, 'r0')).toBe(99);
    });
});

// ============================================================
// 编译流水线集成验证
// ============================================================

describe('Compilation pipeline integration', () => {
    it('should preserve diagnostics across pipeline', async () => {
        const source = 'move r0 42\nhcf\n';
        const tokens = Lexer.tokenize(source);
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);

        expect(program).toBeDefined();
        expect(program.diagnostics).toBeDefined();
        expect(Array.isArray(program.diagnostics)).toBe(true);
        expect(analyser.symbolTable).toBeDefined();
    });

    it('should produce valid symbol table JSON', async () => {
        const source = 'alias dev d0\nhcf\n';
        const tokens = Lexer.tokenize(source);
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);

        const json = analyser.symbolTable.toJSON();
        expect(typeof json).toBe('string');
        expect(json.length).toBeGreaterThan(0);
        expect(() => JSON.parse(json)).not.toThrow();
    });

    it('should execute program with aliases from symbol table', async () => {
        const tokens = Lexer.tokenize('alias dev d0\nmove r0 42\nhcf\n');
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);
        const engine = new Engine(program, analyser.symbolTable);
        engine.runFull();
        expect(engine.getReg('r0')).toBe(42);
    });
});
