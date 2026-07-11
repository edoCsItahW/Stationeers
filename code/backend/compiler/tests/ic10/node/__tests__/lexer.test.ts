/*
 * Copyright (c) 2026. All rights reserved.
 * This source code is licensed under the CC BY-NC-SA
 * (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
 * This software is protected by copyright law. Reproduction, distribution, or use for commercial
 * purposes is prohibited without the author's permission. If you have any questions or require
 * permission, please contact the author: 2207150234@st.sziit.edu.cn
 */
/**
 * @file lexer.test.ts
 * @brief IC10 Node.js 绑定 - 词法分析器测试
 * @details 测试 Lexer 类的各项功能，包括：
 *          - 静态 tokenize 方法
 *          - 实例 scan 方法
 *          - 各类 Token 识别（关键字、标识符、数字、设备、寄存器等）
 *          - 位置信息正确性
 *          - 注释处理
 *          - 诊断报告
 *          - 错误恢复
 */
import IC10NodeAPI = require('ic10-node-api');
import {setupUTF8Console} from '../utils';

const {IC10Local, TokenType, TokenCategory, Lexer} = IC10NodeAPI;

beforeAll(() => {
    setupUTF8Console();
    if (typeof IC10Local.setLanguage === 'function')
        IC10Local.setLanguage('zh-hans');
});

// ============================================================
// 辅助函数
// ============================================================

/** Token 数组类型别名 */
type TokenArray = ReturnType<typeof Lexer.tokenize>;

/** 过滤掉 NEWLINE 和 END Token，只保留有意义的 Token */
function meaningfulTokens(tokens: TokenArray): TokenArray {
    return tokens.filter(t => t.type !== TokenType.NEWLINE && t.type !== TokenType.END);
}

// ============================================================
// Token 基础测试
// ============================================================

describe('Token', () => {
    it('should have type and lexeme accessible', () => {
        const tokens = Lexer.tokenize('move r0 42');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[0].type).toBe(TokenType.KEYWORD_MOVE);
        expect(meaningful[0].lexeme).toBe('move');
    });

    it('should produce toString output', () => {
        const tokens = Lexer.tokenize('move');
        const meaningful = meaningfulTokens(tokens);

        const str = meaningful[0].toString();
        expect(str).toBeTruthy();
        expect(typeof str).toBe('string');
    });

    it('should produce toJSON output', () => {
        const tokens = Lexer.tokenize('42');
        const meaningful = meaningfulTokens(tokens);

        const json = meaningful[0].toJSON();
        expect(json).toBeTruthy();

        const parsed = JSON.parse(json);
        expect(parsed).toBeDefined();
        expect(parsed.lexeme).toBe('42');
    });

    it('should have pos property', () => {
        const tokens = Lexer.tokenize('move');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[0].pos).toBeDefined();
        expect(typeof meaningful[0].pos.line).toBe('number');
    });
});

// ============================================================
// Lexer.tokenize 静态方法测试
// ============================================================

describe('Lexer.tokenize', () => {
    it('should tokenize simple alias directive', () => {
        const tokens = Lexer.tokenize('alias ic d0');

        const meaningful = meaningfulTokens(tokens);
        expect(meaningful).toHaveLength(3);
        expect(meaningful[0].type).toBe(TokenType.KEYWORD_ALIAS);
        expect(meaningful[1].type).toBe(TokenType.IDENTIFIER);
        expect(meaningful[2].type).toBe(TokenType.DEVICE);
    });

    it('should tokenize define directive with number', () => {
        const tokens = Lexer.tokenize('define MAX 42');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[0].type).toBe(TokenType.KEYWORD_DEFINE);
        expect(meaningful[1].type).toBe(TokenType.IDENTIFIER);
        expect(meaningful[2].type).toBe(TokenType.INTEGER);
        expect(meaningful[2].lexeme).toBe('42');
    });

    it('should tokenize label definition', () => {
        const tokens = Lexer.tokenize('main:');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[0].type).toBe(TokenType.IDENTIFIER);
        expect(meaningful[1].type).toBe(TokenType.COLON);
    });

    it('should tokenize all register r0-r15', () => {
        for (let i = 0; i < 16; i++) {
            const tokens = Lexer.tokenize(`move r${i} 0`);
            const meaningful = meaningfulTokens(tokens);

            // r0-r15 应被识别为 REGISTER
            expect(meaningful[1].type).toBe(TokenType.REGISTER);
            expect(meaningful[1].lexeme).toBe(`r${i}`);
        }
    });

    it('should tokenize all devices d0-d5', () => {
        for (let i = 0; i < 6; i++) {
            const tokens = Lexer.tokenize(`alias dev d${i}`);
            const meaningful = meaningfulTokens(tokens);

            expect(meaningful[2].type).toBe(TokenType.DEVICE);
            expect(meaningful[2].lexeme).toBe(`d${i}`);
        }
    });

    it('should tokenize device references db and dn', () => {
        // NOTE: db 和 dn 在词法层面被识别为 IDENTIFIER
        // 语义分析阶段才将其解析为设备引用
        const tokens = Lexer.tokenize('l r0 db Setting');
        const meaningful = meaningfulTokens(tokens);

        // 找到 db token
        const dbToken = meaningful.find(t => t.lexeme === 'db');
        expect(dbToken).toBeDefined();
    });

    it('should tokenize r16+ as identifier (out of register range)', () => {
        const tokens = Lexer.tokenize('move r16 0');
        const meaningful = meaningfulTokens(tokens);

        // r16 超出寄存器范围，应被识别为 IDENTIFIER
        expect(meaningful[1].type).toBe(TokenType.IDENTIFIER);
        expect(meaningful[1].lexeme).toBe('r16');
    });

    it('should tokenize d6+ as identifier (out of device range)', () => {
        const tokens = Lexer.tokenize('alias dev d6');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[2].type).toBe(TokenType.IDENTIFIER);
    });

    it('should tokenize integer literals', () => {
        const tokens = Lexer.tokenize('move r0 42');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[2].type).toBe(TokenType.INTEGER);
        expect(meaningful[2].lexeme).toBe('42');
    });

    it('should tokenize float literals', () => {
        const tokens = Lexer.tokenize('move r0 3.14');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[2].type).toBe(TokenType.FLOAT);
        expect(meaningful[2].lexeme).toBe('3.14');
    });

    it('should tokenize hex numbers with $ prefix', () => {
        const tokens = Lexer.tokenize('move r0 $FF');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[2].type).toBe(TokenType.HEX_NUMBER);
        expect(meaningful[2].lexeme).toBe('$FF');
    });

    it('should tokenize binary numbers with % prefix', () => {
        const tokens = Lexer.tokenize('move r0 %1010');
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[2].type).toBe(TokenType.BINARY_NUMBER);
        expect(meaningful[2].lexeme).toBe('%1010');
    });

    it('should tokenize string literals', () => {
        const tokens = Lexer.tokenize('define NAME HASH("test")');
        const meaningful = meaningfulTokens(tokens);

        // 找到 STRING token
        const strToken = meaningful.find(t => t.type === TokenType.STRING);
        expect(strToken).toBeDefined();
        expect(strToken!.lexeme).toBe('"test"');
    });

    it('should tokenize constants (pi, tau, nan, etc.)', () => {
        // NOTE: 常量在词法层面被识别为 IDENTIFIER
        // 语义分析阶段才将其解析为常量值
        const constants = ['pi', 'tau', 'nan', 'pinf', 'ninf', 'rgas'];
        for (const c of constants) {
            const tokens = Lexer.tokenize(`move r0 ${c}`);
            const meaningful = meaningfulTokens(tokens);

            // 常量 token 应存在
            const constToken = meaningful.find(t => t.lexeme === c);
            expect(constToken).toBeDefined();
        }
    });

    it('should tokenize hash and slash comments', () => {
        const tokens = Lexer.tokenize('# hash comment\n// slash comment\nhcf');
        const meaningful = meaningfulTokens(tokens);

        // 应有 hcf 关键字
        const hcf = meaningful.find(t => t.type === TokenType.KEYWORD_HCF);
        expect(hcf).toBeDefined();
    });

    it('should tokenize multi-line program', () => {
        const source = [
            'alias ic d0',
            'main:',
            'move r0 0',
            'add r0 r0 1',
            'yield',
            'j main',
        ].join('\n');

        const tokens = Lexer.tokenize(source);
        const meaningful = meaningfulTokens(tokens);

        // 每行至少有一个有意义的 token
        expect(meaningful.length).toBeGreaterThanOrEqual(10);
    });

    it('should produce END token at end of input', () => {
        const tokens = Lexer.tokenize('hcf');
        const endToken = tokens.find(t => t.type === TokenType.END);

        expect(endToken).toBeDefined();
    });
});

// ============================================================
// Lexer.scan 实例方法测试
// ============================================================

describe('Lexer.scan', () => {
    it('should produce same result as tokenize', () => {
        const source = 'alias ic d0\nmove r0 1\n';

        const staticTokens = Lexer.tokenize(source);
        const lexer = new Lexer(source);
        const instanceTokens = lexer.scan();

        expect(instanceTokens.length).toBe(staticTokens.length);
    });

    it('should preserve comments as tokens', () => {
        // NOTE: C++ 词法分析器的 debug 标志当前未影响 token 生成，
        // 注释在两种模式下均被保留为 HEX_COMMENT / SLASH_COMMENT token。
        const source = '# comment\nhcf\n';

        const lexer = new Lexer(source);
        const tokens = lexer.scan();

        // 应包含 HEX_COMMENT token
        const commentToken = tokens.find(t => t.type === TokenType.HEX_COMMENT);
        expect(commentToken).toBeDefined();

        // 也应包含 hcf 关键字
        const hcfToken = tokens.find(t => t.type === TokenType.KEYWORD_HCF);
        expect(hcfToken).toBeDefined();
    });

    it('should report diagnostics for unclosed string', () => {
        const source = 'move r0 1\n"unclosed\nhcf\n';
        const lexer = new Lexer(source);
        lexer.scan();

        expect(lexer.diagnostics.length).toBeGreaterThan(0);
    });

    it('should handle empty source', () => {
        const lexer = new Lexer('');
        const tokens = lexer.scan();

        // 空输入应至少有 END token
        expect(tokens.length).toBeGreaterThanOrEqual(1);
        expect(lexer.diagnostics).toHaveLength(0);
    });

    it('should handle source with only newlines', () => {
        const lexer = new Lexer('\n\n\n');
        const tokens = lexer.scan();

        expect(tokens.filter(t => t.type === TokenType.NEWLINE).length).toBe(3);
        expect(lexer.diagnostics).toHaveLength(0);
    });

    it('should handle source with only comments', () => {
        const lexer = new Lexer('# comment\n// comment\n');
        const tokens = lexer.scan();

        expect(lexer.diagnostics).toHaveLength(0);
    });
});

// ============================================================
// 位置信息测试
// ============================================================

describe('Token positions', () => {
    it('should track line numbers correctly', () => {
        const source = 'move r0 1\nmove r1 2\nmove r2 3\n';
        const tokens = Lexer.tokenize(source);
        const meaningful = meaningfulTokens(tokens);

        // 第一行的 move
        expect(meaningful[0].pos.line).toBe(1);
        // 第二行的 move
        const secondMove = meaningful.find(t => t.type === TokenType.KEYWORD_MOVE && t.pos.line === 2);
        expect(secondMove).toBeDefined();
        // 第三行的 move
        const thirdMove = meaningful.find(t => t.type === TokenType.KEYWORD_MOVE && t.pos.line === 3);
        expect(thirdMove).toBeDefined();
    });

    it('should track column numbers correctly', () => {
        const source = 'move r0 1';
        const tokens = Lexer.tokenize(source);
        const meaningful = meaningfulTokens(tokens);

        // move 在第 1 列
        expect(meaningful[0].pos.column).toBe(1);
        // r0 在第 6 列
        expect(meaningful[1].pos.column).toBe(6);
        // 1 在第 9 列
        expect(meaningful[2].pos.column).toBe(9);
    });

    it('should track offsets correctly', () => {
        const source = 'move r0 1';
        const tokens = Lexer.tokenize(source);
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[0].pos.offset).toBe(0);
        expect(meaningful[1].pos.offset).toBe(5);
        expect(meaningful[2].pos.offset).toBe(8);
    });
});

// ============================================================
// Unicode 标识符测试
// ============================================================

describe('Unicode identifiers', () => {
    it('should tokenize Chinese identifiers', () => {
        const source = 'alias 中文变量 d0\n';
        const tokens = Lexer.tokenize(source);
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[1].type).toBe(TokenType.IDENTIFIER);
        expect(meaningful[1].lexeme).toBe('中文变量');
    });

    it('should tokenize Chinese labels', () => {
        const source = '主循环:\nhcf\n';
        const tokens = Lexer.tokenize(source);
        const meaningful = meaningfulTokens(tokens);

        expect(meaningful[0].type).toBe(TokenType.IDENTIFIER);
        expect(meaningful[0].lexeme).toBe('主循环');
    });
});

// ============================================================
// 关键字完整性测试
// ============================================================

describe('Keyword recognition', () => {
    it('should recognize nullary instruction keywords', () => {
        const keywords = [
            {lexeme: 'hcf', type: TokenType.KEYWORD_HCF},
            {lexeme: 'yield', type: TokenType.KEYWORD_YIELD},
        ];

        for (const {lexeme, type} of keywords) {
            const tokens = Lexer.tokenize(lexeme);
            const meaningful = meaningfulTokens(tokens);
            expect(meaningful[0].type).toBe(type);
        }
    });

    it('should recognize preprocessor keywords', () => {
        expect(meaningfulTokens(Lexer.tokenize('alias'))[0].type).toBe(TokenType.KEYWORD_ALIAS);
        expect(meaningfulTokens(Lexer.tokenize('define'))[0].type).toBe(TokenType.KEYWORD_DEFINE);
    });

    it('should recognize unary instruction keywords', () => {
        const keywords = ['sleep', 'j', 'jal', 'jr', 'rand', 'peek', 'pop', 'push', 'clr'];
        for (const kw of keywords) {
            const tokens = Lexer.tokenize(kw);
            const meaningful = meaningfulTokens(tokens);
            expect(meaningful[0].type).not.toBe(TokenType.IDENTIFIER);
            expect(meaningful[0].lexeme).toBe(kw);
        }
    });

    it('should recognize binary instruction keywords', () => {
        const keywords = ['move', 'add', 'sub', 'mul', 'div', 'abs', 'ceil', 'floor', 'round', 'sqrt'];
        for (const kw of keywords) {
            const tokens = Lexer.tokenize(kw);
            const meaningful = meaningfulTokens(tokens);
            expect(meaningful[0].type).not.toBe(TokenType.IDENTIFIER);
        }
    });

    it('should recognize macro keywords', () => {
        expect(meaningfulTokens(Lexer.tokenize('HASH'))[0].type).toBe(TokenType.KEYWORD_HASH);
        expect(meaningfulTokens(Lexer.tokenize('STR'))[0].type).toBe(TokenType.KEYWORD_STR);
    });
});
