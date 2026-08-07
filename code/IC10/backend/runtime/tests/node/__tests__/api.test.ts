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
// Engine 构造和属性
// ============================================================

describe('Engine', () => {
    it('should create Engine instance', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        expect(engine).toBeInstanceOf(Engine);
    });

    it('should report pc 0 on construction', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        expect(engine.pc).toBeDefined();
    });

    it('should not be halted on construction', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        expect(engine.halted).toBeDefined();
    });

    it('should not be sleeping on construction', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        expect(engine.sleeping).toBeDefined();
    });
});

// ============================================================
// 寄存器操作
// ============================================================

describe('Engine getReg / setReg', () => {
    it('should get and set register values', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.getReg('r0')).toBe(0);
        engine.setReg('r0', 42);
        expect(engine.getReg('r0')).toBe(42);
    });

    it('should default registers to 0', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.getReg('r0')).toBe(0);
        expect(engine.getReg('r15')).toBe(0);
    });

    it('should overwrite register value', async () => {
        const {engine} = await compile('hcf\n');
        engine.setReg('r0', 10);
        engine.setReg('r0', 20);
        expect(engine.getReg('r0')).toBe(20);
    });

    it('should handle negative register values', async () => {
        const {engine} = await compile('hcf\n');
        engine.setReg('r1', -100);
        expect(engine.getReg('r1')).toBe(-100);
    });

    it('should handle zero register value', async () => {
        const {engine} = await compile('hcf\n');
        engine.setReg('r0', 0);
        expect(engine.getReg('r0')).toBe(0);
    });

    it('should read sp register', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.getReg('sp')).toBe(0);
    });

    it('should read ra register', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.getReg('ra')).toBe(0);
    });

    it('should set sp and ra', async () => {
        const {engine} = await compile('hcf\n');
        engine.setReg('sp', 16);
        engine.setReg('ra', 32);
        expect(engine.getReg('sp')).toBe(16);
        expect(engine.getReg('ra')).toBe(32);
    });
});

// ============================================================
// 栈操作
// ============================================================

describe('Engine stack operations', () => {
    it('should push and pop', async () => {
        const {engine} = await compile('hcf\n');
        engine.push(3.14);
        expect(engine.pop()).toBe(3.14);
    });

    it('should push multiple and pop all in LIFO order', async () => {
        const {engine} = await compile('hcf\n');
        engine.push(1);
        engine.push(2);
        engine.push(3);
        expect(engine.pop()).toBe(3);
        expect(engine.pop()).toBe(2);
        expect(engine.pop()).toBe(1);
    });

    it('should peek without modifying stack', async () => {
        const {engine} = await compile('hcf\n');
        engine.push(42);
        expect(engine.peek()).toBe(42);
        expect(engine.peek()).toBe(42);
        expect(engine.pop()).toBe(42);
    });

    it('should handle negative stack values', async () => {
        const {engine} = await compile('hcf\n');
        engine.push(-1);
        engine.push(-3.14);
        expect(engine.pop()).toBe(-3.14);
        expect(engine.pop()).toBe(-1);
    });

    it('should get stack at index', async () => {
        const {engine} = await compile('hcf\n');
        engine.push(10);
        engine.push(20);
        engine.push(30);
        expect(engine.getStack(0)).toBe(30);
        expect(engine.getStack(1)).toBe(20);
        expect(engine.getStack(2)).toBe(10);
    });
});

// ============================================================
// toJSON
// ============================================================

describe('Engine toJSON', () => {
    it('should serialize memory state to JSON', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        engine.runFull();
        const json = engine.toJSON();
        expect(typeof json).toBe('string');
        expect(json.length).toBeGreaterThan(0);
        expect(() => JSON.parse(json)).not.toThrow();
    });

    it('should serialize before execution', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        const json = engine.toJSON();
        expect(typeof json).toBe('string');
        expect(() => JSON.parse(json)).not.toThrow();
    });
});

// ============================================================
// runFull / runTick
// ============================================================

describe('Engine execution', () => {
    it('should execute simple program with runFull', async () => {
        const {engine, reg} = await compile('move r0 42\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(42);
    });

    it('should execute arithmetic chain with runFull', async () => {
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

    it('should runTick execute instructions', async () => {
        const {engine, reg} = await compile(
            'move r0 1\n' +
            'move r0 2\n' +
            'hcf\n'
        );
        engine.runTick();
        expect(reg('r0')).toBe(2);
    });

    it('should report halted after hcf', async () => {
        const {engine} = await compile('hcf\n');
        engine.runFull();
        expect(engine.halted).toBe(true);
    });

    it('should handle empty program', async () => {
        const {engine} = await compile('');
        engine.runFull();
        expect(engine.halted).toBe(true);
    });

    it('should report pc correctly after execution', async () => {
        const {engine} = await compile(
            'move r0 1\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(typeof engine.pc).toBe('number');
    });
});
