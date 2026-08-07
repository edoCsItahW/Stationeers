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
 * 编译源码并以 maxInstructions=1 创建 Engine，实现单步执行。
 */
async function compileStepByStep(source: string): Promise<{
    engine: InstanceType<typeof Engine>;
    reg: (name: string) => number;
    step: () => void;
}> {
    const tokens = Lexer.tokenize(source);
    const parser = new Parser(tokens);
    const program = parser.parse();
    const analyser = new Analyser();
    await analyser.visit(program);
    const engine = new Engine(program, analyser.symbolTable, { maxInstructions: 1 });
    return {
        engine,
        reg: (name: string) => engine.getReg(name),
        step: () => engine.runTick()
    };
}

/**
 * 编译源码并创建默认 Engine（用于 runFull）。
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
// 零元指令
// ============================================================

describe('Nullary instructions', () => {
    it('should halt on hcf', async () => {
        const {engine, step} = await compileStepByStep('hcf\n');
        step();
        expect(engine.halted).toBe(true);
    });

    it('should advance PC on yield', async () => {
        const {engine, step} = await compileStepByStep('yield\nyield\nhcf\n');
        step();  // yield
        expect(engine.pc).toBe(1);
        step();  // yield
        expect(engine.pc).toBe(2);
    });
});

// ============================================================
// 一元指令 — move / not / sgn
// ============================================================

describe('Unary move / not / sgn', () => {
    it('should move immediate', async () => {
        const {engine, reg} = await compile('move r0 42\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(42);
    });

    it('should move register to register', async () => {
        const {engine, reg} = await compile('move r0 10\nmove r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(10);
    });

    it('should move negative', async () => {
        const {engine, reg} = await compile('move r0 -5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(-5);
    });

    it('should bitwise not', async () => {
        const {engine, step, reg} = await compileStepByStep('move r0 0\nnot r1 r0\nhcf\n');
        step();  // move r0 0
        step();  // not r1 r0
        expect(reg('r1')).toBe(~0);
    });

    it('should sgn positive → +1', async () => {
        const {engine, reg} = await compile('move r0 3.14\nsgn r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(1);
    });

    it('should sgn negative → -1', async () => {
        const {engine, reg} = await compile('move r0 -2.5\nsgn r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(-1);
    });

    it('should sgn zero → 0', async () => {
        const {engine, reg} = await compile('move r0 0\nsgn r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(0);
    });
});

// ============================================================
// 一元 set-if 指令
// ============================================================

describe('Unary set-if', () => {
    it('should seqz when zero', async () => {
        const {engine, reg} = await compile('move r0 0\nseqz r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(1);
    });

    it('should seqz when non-zero', async () => {
        const {engine, reg} = await compile('move r0 5\nseqz r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(0);
    });

    it('should snez when non-zero', async () => {
        const {engine, reg} = await compile('move r0 5\nsnez r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(1);
    });

    it('should snez when zero', async () => {
        const {engine, reg} = await compile('move r0 0\nsnez r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(0);
    });
});

// ============================================================
// 一元数学
// ============================================================

describe('Unary math', () => {
    it('abs positive', async () => {
        const {engine, reg} = await compile('move r0 5\nabs r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(5);
    });

    it('abs negative', async () => {
        const {engine, reg} = await compile('move r0 -5\nabs r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(5);
    });

    it('abs zero', async () => {
        const {engine, reg} = await compile('move r0 0\nabs r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(0);
    });

    it('ceil', async () => {
        const {engine, reg} = await compile('move r0 3.14\nceil r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(4);
    });

    it('floor', async () => {
        const {engine, reg} = await compile('move r0 3.14\nfloor r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(3);
    });

    it('trunc', async () => {
        const {engine, reg} = await compile('move r0 -3.14\ntrunc r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(-3);
    });

    it('round', async () => {
        const {engine, reg} = await compile('move r0 3.6\nround r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(4);
    });

    it('sqrt', async () => {
        const {engine, reg} = await compile('move r0 16\nsqrt r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBe(4);
    });

    it('sin 0', async () => {
        const {engine, reg} = await compile('move r0 0\nsin r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBeCloseTo(0);
    });

    it('cos 0', async () => {
        const {engine, reg} = await compile('move r0 0\ncos r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBeCloseTo(1);
    });

    it('exp 0', async () => {
        const {engine, reg} = await compile('move r0 0\nexp r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBeCloseTo(1);
    });

    it('log 1', async () => {
        const {engine, reg} = await compile('move r0 1\nlog r1 r0\nhcf\n');
        engine.runFull();
        expect(reg('r1')).toBeCloseTo(0);
    });
});

// ============================================================
// 二元数学运算
// ============================================================

describe('Binary math', () => {
    it('add', async () => {
        const {engine, reg} = await compile('add r0 2 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(5);
    });

    it('sub', async () => {
        const {engine, reg} = await compile('sub r0 10 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(7);
    });

    it('mul', async () => {
        const {engine, reg} = await compile('mul r0 4 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(20);
    });

    it('div', async () => {
        const {engine, reg} = await compile('div r0 15 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(5);
    });

    it('mod', async () => {
        const {engine, reg} = await compile('mod r0 17 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(2);
    });

    it('max', async () => {
        const {engine, reg} = await compile('max r0 3 7\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(7);
    });

    it('min', async () => {
        const {engine, reg} = await compile('min r0 3 7\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(3);
    });

    it('pow', async () => {
        const {engine, reg} = await compile('pow r0 2 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(8);
    });

    it('atan2', async () => {
        const {engine, reg} = await compile('atan2 r0 0 -1\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBeCloseTo(Math.PI, 10);
    });

    it('add with registers', async () => {
        const {engine, reg} = await compile('move r1 10\nmove r2 20\nadd r0 r1 r2\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(30);
    });
});

// ============================================================
// 位运算
// ============================================================

describe('Bitwise operations', () => {
    it('and', async () => {
        const {engine, step, reg} = await compileStepByStep('move r0 $FF\nand r1 r0 $F0\nhcf\n');
        step();
        step();
        expect(reg('r1')).toBe(0xF0);
    });

    it('or', async () => {
        const {engine, step, reg} = await compileStepByStep('move r0 $0F\nor r1 r0 $F0\nhcf\n');
        step();
        step();
        expect(reg('r1')).toBe(0xFF);
    });

    it('xor', async () => {
        const {engine, step, reg} = await compileStepByStep('move r0 $FF\nxor r1 r0 $0F\nhcf\n');
        step();
        step();
        expect(reg('r1')).toBe(0xF0);
    });

    it('nor', async () => {
        const {engine, step, reg} = await compileStepByStep('move r0 0\nnor r1 r0 0\nhcf\n');
        step();
        step();
        expect(reg('r1')).toBe(~0);
    });

    it('sll', async () => {
        const {engine, reg} = await compile('sll r0 1 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(8);
    });

    it('srl', async () => {
        const {engine, reg} = await compile('srl r0 16 2\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(4);
    });

    it('sla arithmetic positive', async () => {
        const {engine, reg} = await compile('sla r0 1 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(8);
    });

    it('sra arithmetic', async () => {
        const {engine, reg} = await compile('sra r0 16 2\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(4);
    });

    it('rol', async () => {
        const {engine, step, reg} = await compileStepByStep('move r0 1\nrol r1 r0 1\nhcf\n');
        step();
        step();
        expect(reg('r1')).toBe(2);
    });

    it('ror', async () => {
        const {engine, step, reg} = await compileStepByStep('move r0 2\nror r1 r0 1\nhcf\n');
        step();
        step();
        expect(reg('r1')).toBe(1);
    });
});

// ============================================================
// 比较 set-if
// ============================================================

describe('Comparison set-if', () => {
    it('seq equal', async () => {
        const {engine, reg} = await compile('seq r0 5 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('seq not equal', async () => {
        const {engine, reg} = await compile('seq r0 5 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });

    it('sne equal', async () => {
        const {engine, reg} = await compile('sne r0 5 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });

    it('sne not equal', async () => {
        const {engine, reg} = await compile('sne r0 5 3\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('sge gt', async () => {
        const {engine, reg} = await compile('sge r0 7 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('sge equal', async () => {
        const {engine, reg} = await compile('sge r0 5 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('sge lt', async () => {
        const {engine, reg} = await compile('sge r0 3 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });

    it('sgt', async () => {
        const {engine, reg} = await compile('sgt r0 7 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('sgt equal', async () => {
        const {engine, reg} = await compile('sgt r0 5 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });

    it('sle lt', async () => {
        const {engine, reg} = await compile('sle r0 3 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('sle equal', async () => {
        const {engine, reg} = await compile('sle r0 5 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('slt', async () => {
        const {engine, reg} = await compile('slt r0 3 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('sapz positive', async () => {
        const {engine, reg} = await compile('sapz r0 5 5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('snaz negative', async () => {
        const {engine, reg} = await compile('snaz r0 -1 -2\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });
});

// ============================================================
// 近似比较
// ============================================================

describe('Approximate comparison', () => {
    it('sap approx equal', async () => {
        const {engine, reg} = await compile('sap r0 1.0 1.00001 0.001\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });

    it('sap approx not equal', async () => {
        const {engine, reg} = await compile('sap r0 1.0 2.0 0.001\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });

    it('sna approx not equal', async () => {
        const {engine, reg} = await compile('sna r0 1.0 2.0 0.001\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(1);
    });
});

// ============================================================
// 分支指令 — 无条件跳转
// ============================================================

describe('Branch — unconditional jump', () => {
    it('j absolute via label', async () => {
        const {engine, reg} = await compile(
            'j end\n' +
            'move r0 1\n' +
            'end:\n' +
            'move r0 2\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(2);
    });
});

// ============================================================
// 分支指令 — breqz / brnez
// ============================================================

describe('Branch — breqz / brnez', () => {
    it('breqz when zero jumps', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'breqz 0 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('breqz when non-zero no jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'breqz 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();  // no jump
        step();  // move r0 1
        expect(reg('r0')).toBe(1);
    });

    it('brnez when non-zero jumps', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'brnez 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });
});

// ============================================================
// 分支指令 — beq / bne / ble / blt / bge / bgt
// ============================================================

describe('Branch — binary conditions', () => {
    it('beq when equal jumps', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'beq 5 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('beq when not equal no jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'beq 5 3 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();  // no jump
        step();  // move r0 1
        expect(reg('r0')).toBe(1);
    });

    it('bne when not equal jumps', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bne 5 3 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('ble true', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'ble 3 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('blt true', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'blt 3 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('bge true', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bge 7 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('bgt true', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bgt 7 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });
});

// ============================================================
// 分支指令 — 带链接
// ============================================================

describe('Branch — with link', () => {
    it('beqal sets ra', async () => {
        const {engine, step} = await compileStepByStep(
            'beqal 5 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(engine.getReg('ra')).toBe(1);
    });
});

// ============================================================
// 四元指令 — clamp / lerp / select
// ============================================================

describe('Quaternary — clamp / lerp / select', () => {
    it('clamp within range', async () => {
        const {engine, reg} = await compile('clamp r0 5 0 10\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(5);
    });

    it('clamp below min', async () => {
        const {engine, reg} = await compile('clamp r0 -5 0 10\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(0);
    });

    it('clamp above max', async () => {
        const {engine, reg} = await compile('clamp r0 15 0 10\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(10);
    });

    it('lerp', async () => {
        const {engine, reg} = await compile('lerp r0 0 10 0.5\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(5);
    });

    it('select true condition', async () => {
        const {engine, reg} = await compile('select r0 1 10 20\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(10);
    });

    it('select false condition', async () => {
        const {engine, reg} = await compile('select r0 0 10 20\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(20);
    });
});

// ============================================================
// 四元指令 — ext / ins
// ============================================================

describe('Quaternary — ext / ins', () => {
    it('ext extracts bits', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'move r0 $FF\n' +
            'ext r1 r0 4 4\n' +
            'hcf\n'
        );
        step();
        step();
        expect(reg('r1')).toBe(15);
    });

    it('ins inserts bits', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'move r0 15\n' +
            'ins r1 r0 4 4\n' +
            'hcf\n'
        );
        step();
        step();
        expect(reg('r1')).toBe(240);
    });
});

// ============================================================
// 四元 — 精确分支 bap / bna
// ============================================================

describe('Quaternary — bap / bna', () => {
    it('bap approx equal → jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bap 1.0 1.00001 0.001 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('bna approx not equal → jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bna 1.0 2.0 0.001 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });
});

// ============================================================
// 数制比较 — bgez / bltz
// ============================================================

describe('Sign-based branches', () => {
    it('bgez positive → jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bgez 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('bgez zero → jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bgez 0 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('bgez negative → no jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bgez -1 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();  // no jump
        step();  // move r0 1
        expect(reg('r0')).toBe(1);
    });

    it('bltz negative → jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bltz -5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(reg('r0')).toBe(0);
    });

    it('bltz positive → no jump', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'bltz 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();  // no jump
        step();
        expect(reg('r0')).toBe(1);
    });

    it('bgezal sets ra', async () => {
        const {engine, step} = await compileStepByStep(
            'bgezal 5 2\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(engine.getReg('ra')).toBe(1);
    });
});

// ============================================================
// Stack 指令 — push / pop / peek / poke
// ============================================================

describe('Stack instructions', () => {
    it('push and pop', async () => {
        const {engine, reg} = await compile('push 42\npop r0\nhcf\n');
        engine.runFull();
        expect(reg('r0')).toBe(42);
    });

    it('push multiple and pop all', async () => {
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

    it('peek stack', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'push 99\n' +
            'peek r0\n' +
            'hcf\n'
        );
        step();  // push 99
        step();  // peek r0
        expect(reg('r0')).toBe(99);
    });

    it('poke stack', async () => {
        const {engine, step, reg} = await compileStepByStep(
            'push 1\n' +
            'push 2\n' +
            'poke 0 99\n' +
            'pop r0\n' +
            'hcf\n'
        );
        step();  // push 1
        step();  // push 2
        step();  // poke 0 99
        step();  // pop r0
        expect(reg('r0')).toBe(99);
    });
});

// ============================================================
// Sleep / jal
// ============================================================

describe('Sleep and jal', () => {
    it('sleep pauses execution', async () => {
        const {engine, step} = await compileStepByStep(
            'sleep 1\n' +
            'move r0 42\n' +
            'hcf\n'
        );
        step();
        expect(engine.sleeping).toBe(true);
    });

    it('jal sets ra and jumps', async () => {
        const {engine, step} = await compileStepByStep(
            'jal 4\n' +
            'move r0 1\n' +
            'hcf\n'
        );
        step();
        expect(engine.getReg('ra')).toBe(1);
    });
});

// ============================================================
// 多指令序列 / 标签跳转 / 循环
// ============================================================

describe('Multi-instruction sequences', () => {
    it('should execute instruction chain', async () => {
        const {engine, reg} = await compile(
            'move r0 10\n' +
            'move r1 20\n' +
            'add r2 r0 r1\n' +
            'mul r3 r2 2\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(10);
        expect(reg('r1')).toBe(20);
        expect(reg('r2')).toBe(30);
        expect(reg('r3')).toBe(60);
    });

    it('should jump to label', async () => {
        const {engine, reg} = await compile(
            'j end\n' +
            'move r0 1\n' +
            'end:\n' +
            'move r0 2\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(2);
    });

    it('should loop with blt', async () => {
        const {engine, reg} = await compile(
            'move r0 0\n' +
            'loop:\n' +
            'add r0 r0 1\n' +
            'blt r0 3 loop\n' +
            'hcf\n'
        );
        engine.runFull();
        expect(reg('r0')).toBe(3);
    });
});
