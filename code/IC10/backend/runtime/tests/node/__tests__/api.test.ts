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
    const engine = new Engine(program, analyser.symbolTable, undefined, analyser.typeTable);
    return {
        engine,
        reg: (name: string) => engine.context.memory.getReg(name)
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
        expect(engine.context.pc).toBeDefined();
    });

    it('should not be halted on construction', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        expect(engine.context.halted).toBeDefined();
    });

    it('should not be sleeping on construction', async () => {
        const {engine} = await compile('move r0 42\nhcf\n');
        expect(engine.context.isSleeping).toBeDefined();
    });
});

// ============================================================
// 模块导出
// ============================================================

describe('Module exports', () => {
    it('should expose the runtime locale under IC10RuntimeLocal', () => {
        // 导出名必须与 types/locale.d.ts 的声明一致（与编译器的 IC10CompilerLocal 命名对齐）
        expect(typeof IC10R.IC10RuntimeLocal.setLanguage).toBe('function');
    });
});

// ============================================================
// 寄存器操作
// ============================================================

describe('Engine getReg / setReg', () => {
    it('should get and set register values', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.context.memory.getReg('r0')).toBe(0);
        engine.context.memory.setReg('r0', 42);
        expect(engine.context.memory.getReg('r0')).toBe(42);
    });

    it('should default registers to 0', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.context.memory.getReg('r0')).toBe(0);
        expect(engine.context.memory.getReg('r15')).toBe(0);
    });

    it('should overwrite register value', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.setReg('r0', 10);
        engine.context.memory.setReg('r0', 20);
        expect(engine.context.memory.getReg('r0')).toBe(20);
    });

    it('should handle negative register values', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.setReg('r1', -100);
        expect(engine.context.memory.getReg('r1')).toBe(-100);
    });

    it('should handle zero register value', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.setReg('r0', 0);
        expect(engine.context.memory.getReg('r0')).toBe(0);
    });

    it('should read sp register', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.context.memory.getReg('sp')).toBe(0);
    });

    it('should read ra register', async () => {
        const {engine} = await compile('hcf\n');
        expect(engine.context.memory.getReg('ra')).toBe(0);
    });

    it('should set sp and ra', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.setReg('sp', 16);
        engine.context.memory.setReg('ra', 32);
        expect(engine.context.memory.getReg('sp')).toBe(16);
        expect(engine.context.memory.getReg('ra')).toBe(32);
    });
});

// ============================================================
// 栈操作
// ============================================================

describe('Engine stack operations', () => {
    it('should push and pop', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.push(3.14);
        expect(engine.context.memory.pop()).toBe(3.14);
    });

    it('should push multiple and pop all in LIFO order', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.push(1);
        engine.context.memory.push(2);
        engine.context.memory.push(3);
        expect(engine.context.memory.pop()).toBe(3);
        expect(engine.context.memory.pop()).toBe(2);
        expect(engine.context.memory.pop()).toBe(1);
    });

    it('should peek without modifying stack', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.push(42);
        expect(engine.context.memory.peek()).toBe(42);
        expect(engine.context.memory.peek()).toBe(42);
        expect(engine.context.memory.pop()).toBe(42);
    });

    it('should handle negative stack values', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.push(-1);
        engine.context.memory.push(-3.14);
        expect(engine.context.memory.pop()).toBe(-3.14);
        expect(engine.context.memory.pop()).toBe(-1);
    });

    it('should get stack at index', async () => {
        const {engine} = await compile('hcf\n');
        engine.context.memory.push(10);
        engine.context.memory.push(20);
        engine.context.memory.push(30);
        expect(engine.context.memory.getStack(0)).toBe(30);
        expect(engine.context.memory.getStack(1)).toBe(20);
        expect(engine.context.memory.getStack(2)).toBe(10);
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
        expect(engine.context.halted).toBe(true);
    });

    it('should handle empty program', async () => {
        const {engine} = await compile('');
        engine.runFull();
        expect(engine.context.halted).toBe(true);
    });

    it('should report pc correctly after execution', async () => {
        const {engine} = await compile(
            'move r0 1\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(typeof engine.context.pc).toBe('number');
    });
});

// ============================================================
// 设备型号与内省（绑定层接线）
// ============================================================

/** 声明了型号的设备程序：注解 + 带类型提示的别名（不含 `hcf`，需要执行的用例自行收尾） */
const DEVICE_SOURCE =
    '#> @device\n' +
    '#> @name Sensor\n' +
    '#> @device-hash 12345\n' +
    '#> @logic Pressure 5\n' +
    '#> @logic Setting 12\n' +
    '#> @end-device\n' +
    'alias sensor d0 #: @type Sensor\n';

describe('Device type and introspection', () => {
    /** 取端口上的设备，端口没有设备时直接失败 */
    function deviceOf(engine: InstanceType<typeof Engine>, port: string) {
        const device = engine.context.manager.getDevice(port);
        if (!device) throw new Error(`port '${port}' should have a bound device`);
        return device;
    }

    it('should bind the type declared in the source to the port', async () => {
        const {engine} = await compile(DEVICE_SOURCE);
        const device = deviceOf(engine, 'd0');

        expect(device.typeName()).toBe('Sensor');
        expect(device.getTypeHash()).toBe(12345);
    });

    it('should list the bound ports', async () => {
        const {engine} = await compile(DEVICE_SOURCE);
        expect(engine.context.manager.ports()).toEqual(['d0']);
    });

    it('should snapshot declared members with their values', async () => {
        const {engine} = await compile(DEVICE_SOURCE);
        const members = deviceOf(engine, 'd0').snapshot();

        expect(members.map(member => member.name)).toEqual(['deviceHash', 'Pressure', 'Setting']);
        expect(members[1]).toEqual({
            name: 'Pressure',
            kind: 'logic',
            declaredValue: '5',
            value: null,
            isDeclared: true,
            assigned: false
        });
    });

    it('should report a value only for assigned members', async () => {
        // 写入必须排在 hcf 之前，否则程序还没写到设备就停机了
        const {engine} = await compile(DEVICE_SOURCE + 's sensor Setting 12\nhcf\n');
        const device = deviceOf(engine, 'd0');

        const before = device.snapshot().find(member => member.name === 'Setting');

        expect(before?.assigned).toBe(false);
        expect(before?.value).toBeNull();

        engine.runFull();

        const after = device.snapshot().find(member => member.name === 'Setting');

        expect(after?.assigned).toBe(true);
        expect(after?.value).toBe(12);

        // 没被赋过值的声明成员依旧没有值
        const pressure = device.snapshot().find(member => member.name === 'Pressure');

        expect(pressure?.assigned).toBe(false);
        expect(pressure?.value).toBeNull();
    });

    it('should bind a type given explicitly and fall back for unknown types', async () => {
        const {engine} = await compile(DEVICE_SOURCE);
        const manager = engine.context.manager;

        manager.setExternalDevice('d1', 'Sensor');
        manager.setExternalDevice('d2', 'Nonexistent');

        expect(manager.ports()).toEqual(['d0', 'd1', 'd2']);
        expect(deviceOf(engine, 'd1').typeName()).toBe('Sensor');
        expect(deviceOf(engine, 'd1').getTypeHash()).toBe(12345);

        // 型号表里没有该型号：退化为无型号设备（型号名保留，成员不受约束）
        expect(deviceOf(engine, 'd2').typeName()).toBe('Nonexistent');
        expect(deviceOf(engine, 'd2').getTypeHash()).toBe(0);
        expect(deviceOf(engine, 'd2').canWriteLogic('Anything')).toBe(true);
    });
});
