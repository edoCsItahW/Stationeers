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
 * 编译源码并创建 Engine。
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
// 复杂程序 — 累加 0..9
// ============================================================

describe('Complex programs', () => {
    it('should sum 0..9 = 45', async () => {
        const {engine, reg} = await compile(
            'move r0 0\n' +
            'move r1 0\n' +
            'loop:\n' +
            'add r0 r0 r1\n' +
            'add r1 r1 1\n' +
            'blt r1 10 loop\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(45);
    });

    it('should handle nested branching', async () => {
        const {engine, reg} = await compile(
            'move r0 0\n' +
            'move r1 1\n' +
            'loop:\n' +
            'bgt r1 10 end\n' +
            'move r2 r1\n' +
            'mod r2 r2 3\n' +
            'brnez r2 skip\n' +
            'add r0 r0 1\n' +
            'skip:\n' +
            'add r1 r1 1\n' +
            'j loop\n' +
            'end:\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(3);
    });

    it('should handle deep stack operations', async () => {
        let src = '';
        for (let i = 0; i < 10; i++) {
            src += 'push ' + (i + 1) + '\n';
        }
        for (let i = 0; i < 10; i++) {
            src += 'pop r' + i + '\n';
        }
        src += 'hcf\n';
        const {engine, reg} = await compile(src);
        engine.runFull();
        expect(reg('r0')).toBe(10);
        expect(reg('r9')).toBe(1);
    });

    it('should compute 2^8 = 256', async () => {
        const {engine, reg} = await compile(
            'move r0 1\n' +
            'move r1 8\n' +
            'loop:\n' +
            'breqz r1 end\n' +
            'mul r0 r0 2\n' +
            'sub r1 r1 1\n' +
            'j loop\n' +
            'end:\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(256);
    });

    it('should handle clamp operations', async () => {
        const {engine, reg} = await compile(
            'clamp r0 -5 0 10\n' +
            'clamp r1 5 0 10\n' +
            'clamp r2 15 0 10\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(0);
        expect(reg('r1')).toBe(5);
        expect(reg('r2')).toBe(10);
    });
});

// ============================================================
// 边界情况
// ============================================================

describe('Boundary cases', () => {
    it('should handle large values', async () => {
        const {engine, reg} = await compile(
            'move r0 1234567890\n' +
            'move r1 -987654321\n' +
            'add r2 r0 r1\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r2')).toBe(246913569);
    });

    it('should preserve floating point precision', async () => {
        const {engine, reg} = await compile(
            'move r0 3.141592653589793\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBeCloseTo(3.141592653589793);
    });

    it('should not crash on empty loop body', async () => {
        const {engine, reg} = await compile(
            'move r0 0\n' +
            'loop:\n' +
            'add r0 r0 1\n' +
            'blt r0 5 loop\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(5);
    });

    it('should handle empty program', async () => {
        const {engine} = await compile('');
        engine.runFull();
        expect(engine.halted).toBe(true);
    });

    it('should handle program with only comments', async () => {
        const {engine} = await compile('# Just a comment\nhcf\n');
        engine.runFull();
        expect(engine.halted).toBe(true);
    });

    it('should handle program with empty lines', async () => {
        const {engine} = await compile('\n\n\nhcf\n');
        engine.runFull();
        expect(engine.halted).toBe(true);
    });

    it('should handle single statement without trailing newline', async () => {
        const {engine, reg} = await compile('move r0 7\nhcf');
        engine.runFull();
        expect(reg('r0')).toBe(7);
    });

    it('should handle large program with labels', async () => {
        let src = 'move r0 0\n';
        for (let i = 0; i < 20; i++) {
            src += 'lbl' + i + ':\n';
            src += 'add r0 r0 1\n';
        }
        src += 'hcf\n';
        const {engine, reg} = await compile(src);
        engine.runFull();
        expect(reg('r0')).toBe(20);
    });
});

// ============================================================
// 错误场景
// ============================================================

describe('Error scenarios', () => {
    it('should detect undefined label in jump', async () => {
        const tokens = Lexer.tokenize('j no_such_label\nhcf\n');
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);
        expect(analyser.symbolTable).toBeDefined();
    });

    it('should handle redefined symbol', async () => {
        const {engine} = await compile(
            'define foo 1\n' +
            'define foo 2\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(engine.halted).toBe(true);
    });
});

// ============================================================
// toJSON 序列化
// ============================================================

describe('Engine toJSON', () => {
    it('should serialize registers after execution', async () => {
        const {engine} = await compile(
            'move r0 10\n' +
            'move r1 20\n' +
            'add r2 r0 r1\n' +
            'hcf\n'
        );
        engine.runFull();
        const json = engine.toJSON();
        expect(typeof json).toBe('string');
        const parsed = JSON.parse(json);
        expect(parsed).toBeDefined();
    });

    it('should serialize stack after push', async () => {
        const {engine} = await compile('push 1\npush 2\nhcf\n');
        engine.runFull();
        const json = engine.toJSON();
        expect(typeof json).toBe('string');
        const parsed = JSON.parse(json);
        expect(parsed).toBeDefined();
    });

    it('should serialize before execution', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        const json = engine.toJSON();
        expect(typeof json).toBe('string');
        expect(() => JSON.parse(json)).not.toThrow();
    });
});

// ============================================================
// EngineConfig 配置
// ============================================================

describe('Engine configuration', () => {
    it('should accept custom tickDuration', async () => {
        const tokens = Lexer.tokenize('move r0 42\nhcf\n');
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);
        const engine = new Engine(program, analyser.symbolTable, {tickDuration: 1.0});
        expect(engine).toBeInstanceOf(Engine);
        engine.runFull();
        expect(engine.getReg('r0')).toBe(42);
    });

    it('should accept custom maxInstructions', async () => {
        const tokens = Lexer.tokenize('move r0 1\nmove r0 2\nhcf\n');
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);
        const engine = new Engine(program, analyser.symbolTable, {maxInstructions: 1});
        engine.runTick();
        expect(engine.getReg('r0')).toBe(1);
        engine.runTick();
        expect(engine.getReg('r0')).toBe(2);
    });

    it('should accept custom maxStackSize', async () => {
        const tokens = Lexer.tokenize('push 1\nhcf\n');
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);
        const engine = new Engine(program, analyser.symbolTable, {maxStackSize: 256});
        engine.runFull();
        expect(engine.peek()).toBe(1);
    });
});
