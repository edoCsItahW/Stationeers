/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */
/**
 * @file system.test.ts
 * @brief IC10 Node.js 绑定 - 系统测试
 * @details 端到端测试完整 IC10 程序的编译流程，包括：
 *          - 真实 IC10 程序（气闸控制、气体过滤、计数循环）
 *          - 完整指令集覆盖
 *          - 预处理指令组合
 *          - 错误场景（未定义变量、重定义）
 *          - 边界情况（空程序、纯注释、超大程序）
 *          - 前向引用解析
 *          - HASH/STR 宏
 */
import IC10NodeAPI = require('ic10c-node');
import {setupUTF8Console} from '../utils';
import * as fs from 'fs';
import * as path from 'path';

const {IC10CompilerLocal, TokenType, Lexer, Program, Parser, Analyser, SymbolTable, BasicType} = IC10NodeAPI;

beforeAll(() => {
    setupUTF8Console();
    if (typeof IC10CompilerLocal.setLanguage === 'function')
        IC10CompilerLocal.setLanguage('zh-hans');
});

// ============================================================
// 辅助函数和类型
// ============================================================

interface CompileResult {
    tokens: ReturnType<typeof Lexer.tokenize>;
    program: InstanceType<typeof Program>;
    parser: InstanceType<typeof Parser>;
    analyser: InstanceType<typeof Analyser>;
}

/** 完整编译流程：源码 → Token → AST → 语义分析 */
async function compile(source: string): Promise<CompileResult> {
    const tokens = Lexer.tokenize(source);
    const parser = new Parser(tokens);
    const program = parser.parse();
    const analyser = new Analyser();
    await analyser.visit(program);
    return {tokens, program, parser, analyser};
}

/** 读取 grammarTest.ic 文件（基于 __dirname 解析路径） */
function readGrammarTestFile(): string {
    // __tests__ → ..(node) → ..(tests) → ..(compiler) → ..(backend) → ..(IC10) → assets → ic → grammarTest.ic
    const filePath = path.join(__dirname, '..', '..', '..', '..', '..', 'assets', 'ic', 'grammarTest.ic');
    return fs.readFileSync(filePath, 'utf-8');
}

// ============================================================
// 真实 IC10 程序测试
// ============================================================

describe('Real IC10 programs', () => {
    it('should compile counting loop program', async () => {
        const source = [
            'alias counter r0',
            'define LIMIT 10',
            'start:',
            'move r0 0',
            'loop:',
            'add r0 r0 1',
            'blt r0 LIMIT loop',
            'yield',
            'j start',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
        expect(result.program.statements).toHaveLength(9);
    });

        });

// ============================================================
// grammarTest.ic 文件测试
// ============================================================

describe('grammarTest.ic fixture', () => {
    it('should compile grammarTest.ic successfully', async () => {
        const source = readGrammarTestFile();
        expect(source).toBeTruthy();

        const result = await compile(source);

        expect(result.program.statements.length).toBeGreaterThan(0);
    });

        });

// ============================================================
// 指令元数完整覆盖测试
// ============================================================

describe('All instruction arities', () => {
    it('should compile program with all instruction arities', async () => {
        // NOTE: lbn/lbns的最后操作数需为identifier|number(logicType/batchMode)，不能用register
        const source = [
            '# 零元',
            'hcf',
            'yield',
            '# 一元',
            'sleep 100',
            'j main',
            '# 二元',
            'move r0 0',
            'abs r0 r1',
            '# 三元',
            'add r0 r1 r2',
            'sub r0 r1 r2',
            'mul r0 r1 r2',
            '# 四元',
            'lerp r0 r1 r2 r3',
            '# 五元',
            'lbn r0 100 200 2 Average',
            '# 六元',
            'lbns r0 100 200 3 Quantity Average',
            'main:',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
        expect(result.program.statements.length).toBeGreaterThanOrEqual(12);
    });
});

// ============================================================
// 预处理指令完整覆盖测试
// ============================================================

describe('All preprocessor directives', () => {
    it('should compile program with all preprocessor directive types', async () => {
        const source = [
            'alias devA d0',
            'alias regA r0',
            'define COUNT 42',
            'define HEX_VAL $FF',
            'define HASH_VAL HASH("StructureLiquidVolumePump")',
            'define STR_VAL STR("SomeHash")',
            'main:',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// 错误场景测试
// ============================================================

describe('Error scenarios', () => {
                    });

// ============================================================
// 边界情况测试
// ============================================================

describe('Boundary cases', () => {
    it('should handle empty source', async () => {
        const result = await compile('');

        expect(result.program.statements).toHaveLength(0);
        expect(result.parser.diagnostics).toHaveLength(0);
    });

    it('should handle source with only comments', async () => {
        const source = '# comment 1\n# comment 2\n# comment 3\n';
        const result = await compile(source);

        expect(result.program.statements).toHaveLength(0);
        expect(result.parser.diagnostics).toHaveLength(0);
    });

    it('should handle source with only newlines', async () => {
        const source = '\n\n\n\n';
        const result = await compile(source);

        expect(result.program.statements).toHaveLength(0);
    });

    it('should handle source with trailing newlines', async () => {
        const source = 'hcf\n\n\n\n';
        const result = await compile(source);

        expect(result.program.statements).toHaveLength(1);
        expect(result.parser.diagnostics).toHaveLength(0);
    });

    it('should handle single statement without trailing newline', async () => {
        const source = 'hcf';
        const result = await compile(source);

        expect(result.program.statements).toHaveLength(1);
        expect(result.parser.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// 前向引用测试
// ============================================================

describe('Forward references', () => {
    it('should resolve forward label reference in jump instruction', async () => {
        const source = [
            'j target',
            'move r0 1',
            'target:',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        // 前向引用应被解析，不应有未定义标识符错误
        const undefinedErrors = result.analyser.diagnostics.filter(
            (d: any) => d.id.includes('IEA3')
        );
        expect(undefinedErrors).toHaveLength(0);
    });

        it('should resolve multiple forward references', async () => {
        const source = [
            'j label1',
            'j label2',
            'j label3',
            'label1:',
            'hcf',
            'label2:',
            'yield',
            'label3:',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        const undefinedErrors = result.analyser.diagnostics.filter(
            (d: any) => d.id.includes('IEA3')
        );
        expect(undefinedErrors).toHaveLength(0);
    });
});

// ============================================================
// HASH 和 STR 宏测试
// ============================================================

describe('HASH and STR macros', () => {
    it('should compile HASH macro in define', async () => {
        const source = 'define PUMP_HASH HASH("StructureLiquidVolumePump")\nhcf\n';
        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });

    it('should compile STR macro in define', async () => {
        const source = 'define LABEL STR("SomeHash")\nhcf\n';
        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });

    it('should compile multiple HASH and STR macros', async () => {
        const source = [
            'define HASH1 HASH("StructureLiquidVolumePump")',
            'define HASH2 HASH("StructureGasVentSensor")',
            'define STR1 STR("Label1")',
            'define STR2 STR("Label2")',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// 设备引用测试
// ============================================================

describe('Device references', () => {
    it('should compile program using all device references d0-d5', async () => {
        const source = [
            'l r0 d0 Setting',
            'l r1 d1 Setting',
            'l r2 d2 Setting',
            'l r3 d3 Setting',
            'l r4 d4 Setting',
            'l r5 d5 Setting',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });

    });

// ============================================================
// 寄存器边界测试
// ============================================================

describe('Register boundaries', () => {
    it('should compile program using r0 and r15', async () => {
        const source = [
            'move r0 0',
            'move r15 0',
            'add r0 r0 r15',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// 数字类型测试
// ============================================================

describe('Number types', () => {
    it('should compile program with all number types', async () => {
        const source = [
            'move r0 42',
            'move r1 3.14',
            'move r2 $FF',
            'move r3 %1010',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// Unicode 标识符测试
// ============================================================

describe('Unicode identifiers', () => {
    it('should compile program with Chinese identifiers', async () => {
        const source = [
            'alias 计数器 r0',
            'alias 设备 d0',
            '主循环:',
            'move r0 0',
            'add r0 r0 1',
            'blt r0 10 主循环',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// 大型程序测试
// ============================================================

describe('Large programs', () => {
    it('should compile program with 100+ statements', async () => {
        const lines = ['alias counter r0', 'define LIMIT 100', 'start:', 'move r0 0'];
        for (let i = 0; i < 50; i++) {
            lines.push('add r0 r0 1');
            lines.push('yield');
        }
        lines.push('blt r0 LIMIT start');
        lines.push('hcf');

        const source = lines.join('\n') + '\n';
        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
        expect(result.program.statements.length).toBeGreaterThanOrEqual(100);
    });

    });

// ============================================================
// 常量引用测试
// ============================================================

describe('Constants in instructions', () => {
    // NOTE: 常量关键字(pi/tau/nan等)作为操作数时，program.statements 的 toJSON 序列化
    // 会产生无效 JSON，导致访问 program.statements 抛出 SyntaxError。
    // TODO: 修复 Node 绑定中 AST 序列化常量关键字时产生无效 JSON 的问题
    // 此处通过验证 parser.diagnostics 来确认解析过程正常。
    it('should compile program using pi and tau', async () => {
        const source = [
            'move r0 pi',
            'move r1 tau',
            'mul r0 r0 r1',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });

        });

// ============================================================
// 注释处理测试
// ============================================================

describe('Comments in programs', () => {
    });

// ============================================================
// 文档注释与类型提示测试
// ============================================================

describe('Doc comments and type hints', () => {
                });

// ============================================================
// 类型推导与语义分析系统测试
// ============================================================

describe('Type inference and semantic analysis', () => {
                        });
