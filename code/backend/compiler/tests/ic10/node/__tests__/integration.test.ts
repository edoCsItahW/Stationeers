/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */
/**
 * @file integration.test.ts
 * @brief IC10 Node.js 绑定 - 集成测试
 * @details 测试 Lexer → Parser → Analyser 三阶段间的数据流和交互：
 *          - Token 序列正确传递到 Parser
 *          - AST 正确传递到 Analyser
 *          - 诊断在各阶段正确生成和传播
 *          - 符号表在分析后正确构建
 *          - 增量编译与完整编译结果一致性
 */
import IC10NodeAPI = require('ic10-node-api');
import {setupUTF8Console} from '../utils';

const {IC10Local, TokenType, Lexer, Program, Parser, Analyser, SymbolTable, IncCompiler} = IC10NodeAPI;

beforeAll(() => {
    setupUTF8Console();
    if (typeof IC10Local.setLanguage === 'function')
        IC10Local.setLanguage('zh-hans');
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

/** 统计所有阶段的诊断总数 */
function totalDiags(result: CompileResult): number {
    return result.parser.diagnostics.length + result.analyser.diagnostics.length;
}

// ============================================================
// Lexer → Parser 数据流测试
// ============================================================

describe('Lexer → Parser data flow', () => {
    it('should pass token sequence from Lexer to Parser correctly', async () => {
        const source = 'alias ic d0\nmove r0 42\nhcf\n';
        const result = await compile(source);

        // Token 数组应非空
        expect(result.tokens.length).toBeGreaterThan(0);

        // AST 语句数应与源码行数对应（3 条语句）
        expect(result.program.statements).toHaveLength(3);
    });

    it('should preserve token count consistency', async () => {
        const source = 'hcf\n';
        const result = await compile(source);

        // 至少有 hcf token, NEWLINE, END
        expect(result.tokens.length).toBeGreaterThanOrEqual(3);
    });

    it('should handle empty token stream', async () => {
        const source = '';
        const result = await compile(source);

        expect(result.program.statements).toHaveLength(0);
    });

    it('should handle comment-only token stream', async () => {
        const source = '# comment\n// comment\n';
        const result = await compile(source);

        expect(result.program.statements).toHaveLength(0);
    });

    it('should pass debug-mode tokens to Parser correctly', async () => {
        const source = '# comment\nhcf\n';
        const debugLexer = new Lexer(source, true);
        const debugTokens = debugLexer.scan();

        const parser = new Parser(debugTokens);
        const program = parser.parse();

        // 即使有注释 Token，Parser 也应正确解析出 1 条语句
        expect(program.statements).toHaveLength(1);
    });
});

// ============================================================
// Parser → Analyser 数据流测试
// ============================================================

describe('Parser → Analyser data flow', () => {
    it('should pass AST to Analyser and produce symbol table', async () => {
        const source = 'alias ic d0\ndefine MAX 10\nmain:\nhcf\n';
        const result = await compile(source);

        // Analyser 应生成符号表
        expect(result.analyser.symbolTable).toBeInstanceOf(SymbolTable);

        // 符号表应可序列化
        const json = result.analyser.symbolTable.toJSON();
        expect(json).toBeTruthy();

        const parsed = JSON.parse(json);
        expect(parsed).toBeDefined();
    });

    it('should report analyser diagnostics for undefined identifiers', async () => {
        const source = 'move r0 undefined_var\nhcf\n';
        const result = await compile(source);

        // 分析器应报告未定义标识符错误
        expect(result.analyser.diagnostics.length).toBeGreaterThan(0);
    });

    it('should report analyser diagnostics for redefined symbols', async () => {
        const source = [
            'alias foo r0',
            'alias foo r1',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        // 分析器应报告重定义错误
        expect(result.analyser.diagnostics.length).toBeGreaterThan(0);
    });

    it('should resolve forward references in analyser', async () => {
        const source = [
            'j target',
            'move r0 1',
            'target:',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        // 前向引用（j target 在 target: 之前）应能正确解析
        // 不应有未定义标识符错误
        const undefinedErrors = result.analyser.diagnostics.filter(
            (d: any) => d.id.includes('IEA3')
        );
        expect(undefinedErrors).toHaveLength(0);
    });
});

// ============================================================
// 完整编译流水线测试
// ============================================================

describe('Full pipeline', () => {
    it('should compile simple program without diagnostics', async () => {
        const source = 'alias ic d0\nmain:\nhcf\n';
        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
        expect(result.analyser.diagnostics).toHaveLength(0);
    });

    it('should compile program with preprocessor directives', async () => {
        const source = [
            'alias devA d0',
            'alias regA r0',
            'define COUNT 42',
            'main:',
            'move r0 COUNT',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });

    it('should compile program with labels and jumps', async () => {
        const source = [
            'start:',
            'move r0 0',
            'add r0 r0 1',
            'blt r0 10 start',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });

    it('should compile program with load/store instructions', async () => {
        const source = [
            'alias dev d0',
            'l r0 dev Pressure',
            's dev Setting r0',
            'hcf',
        ].join('\n') + '\n';

        const result = await compile(source);

        expect(result.parser.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// 诊断传播测试
// ============================================================

describe('Diagnostic propagation', () => {
    it('should propagate lexer diagnostics through pipeline', async () => {
        const source = 'move r0 1\n"unclosed\nhcf\n';
        const result = await compile(source);

        // Lexer 的诊断（未闭合字符串）不应阻止后续阶段
        // Parser 和 Analyser 仍应执行
        expect(result.program).toBeInstanceOf(Program);
    });

    it('should propagate parser diagnostics through pipeline', async () => {
        const source = 'hcf yield\n';  // 缺少换行分隔
        const result = await compile(source);

        // Parser 诊断不应阻止 Analyser 执行
        expect(result.parser.diagnostics.length).toBeGreaterThan(0);
        expect(result.analyser).toBeDefined();
    });

    it('should collect diagnostics from all stages independently', async () => {
        const source = 'hcf yield\nmove r0 undefined_var\n';
        const result = await compile(source);

        // Parser 应报告缺少换行的错误
        expect(result.parser.diagnostics.length).toBeGreaterThan(0);

        // Analyser 应报告未定义标识符的错误
        expect(result.analyser.diagnostics.length).toBeGreaterThan(0);
    });
});

// ============================================================
// AST 序列化一致性测试
// ============================================================

describe('AST serialization consistency', () => {
    it('should produce valid JSON from full pipeline', async () => {
        const source = 'alias ic d0\nmove r0 1\nhcf\n';
        const result = await compile(source);

        const json = JSON.parse(result.program.toJSON());

        expect(json.statements).toBeDefined();
        expect(json.statements).toHaveLength(3);
    });

    it('should produce consistent toString output', async () => {
        const source = 'alias ic d0\nhcf\n';
        const result1 = await compile(source);
        const result2 = await compile(source);

        expect(result1.program.toString()).toBe(result2.program.toString());
    });

    it('should serialize symbol table to valid JSON', async () => {
        const source = 'alias ic d0\ndefine MAX 10\nmain:\nhcf\n';
        const result = await compile(source);

        const symJson = JSON.parse(result.analyser.symbolTable.toJSON());

        expect(symJson).toBeDefined();
        expect(typeof symJson).toBe('object');
    });
});

// ============================================================
// 增量编译与完整编译一致性测试
// ============================================================

describe('IncCompiler vs full pipeline consistency', () => {
    it('should produce same AST statement count as full pipeline', async () => {
        const source = [
            'alias ic d0',
            'move r0 0',
            'add r0 r0 1',
            'yield',
            'hcf',
        ].join('\n') + '\n';

        const fullResult = await compile(source);

        const incCompiler = new IncCompiler();
        const incResult = incCompiler.compileFull(source);

        expect(incResult.ast.statements).toHaveLength(fullResult.program.statements.length);
    });

    it('should handle incremental recompile after change', async () => {
        const baseline = [
            'alias ic d0',
            'move r0 0',
            'hcf',
        ].join('\n') + '\n';

        const compiler = new IncCompiler();
        const fullResult = compiler.compileFull(baseline);

        expect(fullResult.incremental).toBe(false);

        const modified = [
            'alias ic d0',
            'move r0 99',
            'hcf',
        ].join('\n') + '\n';

        const incResult = compiler.compileInc(modified);

        expect(incResult.incremental).toBe(true);
        expect(incResult.ast.statements).toHaveLength(3);
    });

    it('should handle incremental recompile after append', async () => {
        const baseline = 'alias ic d0\nmove r0 0\n';
        const compiler = new IncCompiler();
        compiler.compileFull(baseline);

        const appended = baseline + 'add r0 r0 1\nhcf\n';
        const incResult = compiler.compileInc(appended);

        expect(incResult.incremental).toBe(true);
        expect(incResult.ast.statements).toHaveLength(4);
    });

    it('should fallback to full compile when cache is empty', async () => {
        const source = 'alias ic d0\nmove r0 0\nhcf\n';
        const compiler = new IncCompiler();

        const result = compiler.compileInc(source);

        expect(result.incremental).toBe(false);
        expect(result.ast.statements).toHaveLength(3);
    });
});

// ============================================================
// 符号表构建测试
// ============================================================

describe('Symbol table construction', () => {
    it('should contain alias definitions in symbol table', async () => {
        const source = 'alias foo r0\nalias bar d0\nhcf\n';
        const result = await compile(source);

        const symJson = JSON.parse(result.analyser.symbolTable.toJSON());
        const symStr = JSON.stringify(symJson);

        // 符号表应包含定义的别名
        expect(symStr).toContain('foo');
        expect(symStr).toContain('bar');
    });

    it('should contain define constants in symbol table', async () => {
        const source = 'define MAX 42\ndefine MIN 1\nhcf\n';
        const result = await compile(source);

        const symJson = JSON.parse(result.analyser.symbolTable.toJSON());
        const symStr = JSON.stringify(symJson);

        expect(symStr).toContain('MAX');
        expect(symStr).toContain('MIN');
    });

    it('should contain label definitions in symbol table', async () => {
        const source = 'start:\nmove r0 0\nend:\nhcf\n';
        const result = await compile(source);

        const symJson = JSON.parse(result.analyser.symbolTable.toJSON());
        const symStr = JSON.stringify(symJson);

        expect(symStr).toContain('start');
        expect(symStr).toContain('end');
    });
});
