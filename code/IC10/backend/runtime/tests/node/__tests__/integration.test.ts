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
 * 编译源码并执行 runFull，返回指定寄存器值。
 */
async function runAndGetReg(source: string, regName: string): Promise<number> {
    const tokens = Lexer.tokenize(source);
    const parser = new Parser(tokens);
    const program = parser.parse();
    const analyser = new Analyser();
    await analyser.visit(program);
    const engine = new Engine(program, analyser.symbolTable, undefined, analyser.typeTable);
    engine.runFull();
    return engine.context.memory.getReg(regName);
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
    const engine = new Engine(program, analyser.symbolTable, undefined, analyser.typeTable);
    return {
        engine,
        reg: (name: string) => engine.context.memory.getReg(name)
    };
}

/**
 * 注册一个外部设备并返回其 JS 包装对象（读取写入结果用）。
 */
function registerDevice(engine: InstanceType<typeof Engine>, name: string) {
    const manager = engine.context.manager;
    manager.setExternalDevice(name, undefined as never);
    return manager.getDevice(name);
}

/**
 * 最小标准库片段：逻辑属性名需命中 LogicType 枚举才算合法。
 */
const LOGIC_TYPES =
    '#> @enum\n' +
    '#> @name LogicType\n' +
    '#> @value Setting 12\n' +
    '#> @value On 28\n' +
    '#> @value Color 3\n' +
    '#> @value Pressure 5\n' +
    '#> @end-enum\n' +
    '#> @enum\n' +
    '#> @name BatchMode\n' +
    '#> @value Average 0\n' +
    '#> @value Sum 1\n' +
    '#> @end-enum\n';

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
        expect(parser.diagnostics).toBeDefined();
        expect(Array.isArray(parser.diagnostics)).toBe(true);
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
        const engine = new Engine(program, analyser.symbolTable, undefined, analyser.typeTable);
        engine.runFull();
        expect(engine.context.memory.getReg('r0')).toBe(42);
    });
});

// ============================================================
// v3 放宽语法：别名写入目标、设备引用、写入值形态
// ============================================================

describe('v3 relaxed syntax', () => {
    it('should write through an alias target', async () => {
        const {engine, reg} = await compile(
            'alias tmp r3\n' +
            'move tmp 7\n' +
            'move r4 tmp\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r3')).toBe(7);
        expect(reg('r4')).toBe(7);
    });

    it('should accept constant, literal and register write values', async () => {
        const {engine, reg} = await compile(
            LOGIC_TYPES +
            'alias led d0\n' +
            'define Level 3\n' +
            'move r1 5\n' +
            's led Setting Level\n' +
            's led On r1\n' +
            's led Color 7\n' +
            'l r0 led Setting\n' +
            'hcf\n'
        );
        const device = registerDevice(engine, 'd0');

        engine.runFull();

        expect(device.readLogic('Setting')).toBe(3);
        expect(device.readLogic('On')).toBe(5);
        expect(device.readLogic('Color')).toBe(7);
        expect(reg('r0')).toBe(3);
    });

    it('should resolve a dynamic device port from its register', async () => {
        const {engine} = await compile(
            LOGIC_TYPES +
            'move r0 1\n' +
            's dr0 Setting 9\n' +
            'hcf\n'
        );
        const device = registerDevice(engine, 'd1');

        engine.runFull();

        expect(device.readLogic('Setting')).toBe(9);
    });

    it('should report an error for an out-of-range dynamic port', async () => {
        const {engine} = await compile(
            LOGIC_TYPES +
            'move r0 9\n' +
            's dr0 Setting 1\n' +
            'hcf\n'
        );
        engine.runFull();

        expect(engine.diagnostics.length).toBeGreaterThan(0);
    });

    it('should evaluate an enum constant as a write value', async () => {
        const {engine} = await compile(
            LOGIC_TYPES +
            '#> @enum\n' +
            '#> @name Color\n' +
            '#> @value Green 2\n' +
            '#> @end-enum\n' +
            'alias led d0\n' +
            's led Color Color.Green\n' +
            'hcf\n'
        );
        const device = registerDevice(engine, 'd0');

        engine.runFull();

        expect(device.readLogic('Color')).toBe(2);
    });

    it('should take a device hash from a register', async () => {
        const {engine} = await compile(
            LOGIC_TYPES +
            'alias Hash r2\n' +
            'move Hash 0\n' +
            'sb Hash Setting 4\n' +
            'hcf\n'
        );
        const device = registerDevice(engine, 'd0');

        engine.runFull();

        expect(device.readLogic('Setting')).toBe(4);
    });

    it('should pass a legacy numeric logic prop as its decimal text', async () => {
        const {engine} = await compile(
            LOGIC_TYPES +
            'alias led d0\n' +
            's led 3 8\n' +
            'hcf\n'
        );
        const device = registerDevice(engine, 'd0');

        engine.runFull();

        expect(device.readLogic('3')).toBe(8);
    });

    it('should resolve a bare aggregate mode member name', async () => {
        const {engine, reg} = await compile(
            LOGIC_TYPES +
            'define DevType 0\n' +
            'lb r0 DevType Pressure Average\n' +
            'hcf\n'
        );
        const first = registerDevice(engine, 'd0');
        const second = registerDevice(engine, 'd1');
        first.writeLogic('Pressure', 6);
        second.writeLogic('Pressure', 12);
        // 芯片自身也是类型哈希为 0 的设备，lb 的设备集合包含它
        engine.context.manager.getDevice('db').writeLogic('Pressure', 3);

        engine.runFull();

        // Average = (6 + 12 + 3) / 3
        expect(reg('r0')).toBe(7);
    });
});
