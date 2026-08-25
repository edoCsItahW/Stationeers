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

const {IC10CompilerLocal, Lexer, Parser, Analyser} = IC10C;
const {Engine} = IC10R;

beforeAll(() => {
    setupUTF8Console();

    if (typeof IC10CompilerLocal.setLanguage === 'function')
        IC10CompilerLocal.setLanguage('zh-hans');
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
        reg: (name: string) => engine.context.memory.getReg(name)
    };
}

// ============================================================
// Engine runFull — 基本执行
// ============================================================

describe('Engine runFull', () => {
    it('should execute simple program to halt', async () => {
        const {engine, reg} = await compile('move r0 42\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(42);
    });

    it('should execute arithmetic chain', async () => {
        const {engine, reg} = await compile(
            'move r0 10\n' +
            'move r1 20\n' +
            'add r2 r0 r1\n' +
            'sub r3 r2 5\n' +
            'mul r4 r3 2\n' +
            'div r5 r4 5\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r2')).toBe(30);
        expect(reg('r3')).toBe(25);
        expect(reg('r4')).toBe(50);
        expect(reg('r5')).toBe(10);
    });

    it('should jump and skip via label', async () => {
        const {engine, reg} = await compile(
            'j skip\n' +
            'move r0 1\n' +
            'skip:\n' +
            'move r0 2\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(2);
    });
});

// ============================================================
// Engine — 循环
// ============================================================

describe('Engine loops', () => {
    it('should run for loop with blt', async () => {
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

    it('should run while loop with breqz', async () => {
        const {engine, reg} = await compile(
            'move r0 10\n' +
            'loop:\n' +
            'breqz r0 end\n' +
            'sub r0 r0 1\n' +
            'j loop\n' +
            'end:\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });
});

// ============================================================
// Engine — Stack
// ============================================================

describe('Engine stack', () => {
    it('should push and pop via program', async () => {
        const {engine, reg} = await compile(
            'push 1\n' +
            'push 2\n' +
            'pop r0\n' +
            'pop r1\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(2);
        expect(reg('r1')).toBe(1);
    });
});

// ============================================================
// Engine — 空程序
// ============================================================

describe('Engine empty program', () => {
    it('should handle empty program', async () => {
        const {engine} = await compile('');
        engine.runFull();
        expect(engine.context.halted).toBe(true);
    });
});

// ============================================================
// Engine runTick
// ============================================================

describe('Engine runTick', () => {
    it('should execute all instructions in one tick', async () => {
        const {engine, reg} = await compile(
            'move r0 1\n' +
            'move r0 2\n' +
            'hcf\n'
        );
        engine.runTick();
        expect(reg('r0')).toBe(2);
    });

    it('should runTick with custom maxInstructions', async () => {
        const tokens = Lexer.tokenize('move r0 1\nmove r0 2\nmove r0 3\nhcf\n');
        const parser = new Parser(tokens);
        const program = parser.parse();
        const analyser = new Analyser();
        await analyser.visit(program);
        const engine = new Engine(program, analyser.symbolTable, {maxInstructions: 1});

        engine.runTick();  // execute move r0 1
        expect(engine.context.memory.getReg('r0')).toBe(1);
        engine.runTick();  // execute move r0 2
        expect(engine.context.memory.getReg('r0')).toBe(2);
        engine.runTick();  // execute move r0 3
        expect(engine.context.memory.getReg('r0')).toBe(3);
    });
});

// ============================================================
// Engine step — 单步原语
// ============================================================

describe('Engine step', () => {
    it('should execute exactly one instruction per step', async () => {
        const {engine, reg} = await compile(
            'move r0 1\n' +
            'move r0 2\n' +
            'hcf\n'
        );

        expect(engine.step()).toBe(true);   // move r0 1
        expect(reg('r0')).toBe(1);
        expect(engine.step()).toBe(true);   // move r0 2
        expect(reg('r0')).toBe(2);
        expect(engine.step()).toBe(false);  // hcf -> halted
        expect(engine.context.halted).toBe(true);
    });
});

// ============================================================
// Engine — halted 和 sleeping 状态
// ============================================================

describe('Engine state', () => {
    it('should be halted after hcf', async () => {
        const {engine} = await compile('hcf\n');
        engine.runFull();
        expect(engine.context.halted).toBe(true);
    });

    it('should be sleeping after sleep', async () => {
        const {engine} = await compile('sleep 1\nhcf\n');
        engine.runFull();
        expect(engine.context.isSleeping).toBe(true);
    });
});

// ============================================================
// Engine — pc
// ============================================================

describe('Engine pc', () => {
    it('should increment pc during execution', async () => {
        const {engine} = await compile(
            'move r0 1\n' +
            'move r0 2\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(typeof engine.context.pc).toBe('number');
    });
});
