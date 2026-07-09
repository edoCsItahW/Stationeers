import IC10NodeAPI = require('ic10-node-api');
import {setupUTF8Console} from "../utils";

const {IncLexer, IncParser, IncCompiler, Program} = IC10NodeAPI;

beforeAll(() => {
    setupUTF8Console();
});

const SRC_BASELINE = [
    'alias ic d0',
    'alias something r0',
    'move r0 42',
    'add r0 r0 1',
    'yield',
].join('\n');

describe('IncLexer', () => {
    it('should tokenizeFull', () => {
        const lexer = new IncLexer();
        const result = lexer.tokenizeFull(SRC_BASELINE);

        expect(result.tokens).toBeDefined();
        expect(result.tokens.length).toBeGreaterThan(0);
        expect(result.incremental).toBe(false);
        expect(result.relexedLines).toBeGreaterThan(0);
        expect(result.changedStartLine).toBe(1);
    });

    it('should tokenizeInc unchanged source', () => {
        const lexer = new IncLexer();
        lexer.tokenizeFull(SRC_BASELINE);
        const result = lexer.tokenizeInc(SRC_BASELINE);

        expect(result.incremental).toBe(true);
        expect(result.relexedLines).toBe(0);
        expect(result.tokens.length).toBeGreaterThan(0);
    });

    it('should tokenizeInc detect single-line change', () => {
        const lexer = new IncLexer();
        lexer.tokenizeFull(SRC_BASELINE);

        const modified = [
            'alias ic d0',
            'alias something r0',
            'move r0 99',
            'add r0 r0 1',
            'yield',
        ].join('\n');

        const result = lexer.tokenizeInc(modified);

        expect(result.incremental).toBe(true);
        expect(result.relexedLines).toBeGreaterThanOrEqual(1);
        expect(result.changedStartLine).toBeGreaterThanOrEqual(1);
    });

    it('should tokenizeInc detect append lines', () => {
        const lexer = new IncLexer();
        lexer.tokenizeFull(SRC_BASELINE);

        const appended = SRC_BASELINE + '\nmove r1 10';

        const result = lexer.tokenizeInc(appended);

        expect(result.incremental).toBe(true);
        expect(result.relexedLines).toBeGreaterThanOrEqual(1);
    });

    it('should tokenizeInc fallback to full when cache is empty', () => {
        const lexer = new IncLexer();
        const result = lexer.tokenizeInc(SRC_BASELINE);

        expect(result.incremental).toBe(false);
        expect(result.relexedLines).toBeGreaterThan(0);
    });

    it('should hasCache and clear', () => {
        const lexer = new IncLexer();
        expect(lexer.hasCache()).toBe(false);

        lexer.tokenizeFull(SRC_BASELINE);
        expect(lexer.hasCache()).toBe(true);

        lexer.clear();
        expect(lexer.hasCache()).toBe(false);
    });
});

describe('IncParser', () => {
    function getTokens(src: string) {
        const lexer = new IncLexer();
        return lexer.tokenizeFull(src);
    }

    it('should parseFull', () => {
        const parser = new IncParser();
        const lexerResult = getTokens(SRC_BASELINE);
        const result = parser.parseFull(lexerResult.tokens);

        expect(result.ast).toBeInstanceOf(Program);
        expect(result.incremental).toBe(false);
        expect(result.reparsedStmts).toBeGreaterThan(0);
    });

    it('should parseInc', () => {
        const parser = new IncParser();
        const lexerResult = getTokens(SRC_BASELINE);

        parser.parseFull(lexerResult.tokens);

        const modified = [
            'alias ic d0',
            'alias something r0',
            'move r0 99',
            'add r0 r0 1',
            'yield',
        ].join('\n');
        const newLexerResult = getTokens(modified);

        const result = parser.parseInc(
            newLexerResult.tokens,
            newLexerResult.changedStartLine
        );

        expect(result.ast).toBeInstanceOf(Program);
        expect(result.ast.statements).toBeDefined();
    });

    it('should hasCache and clear', () => {
        const parser = new IncParser();
        expect(parser.hasCache()).toBe(false);

        const lexerResult = getTokens(SRC_BASELINE);
        parser.parseFull(lexerResult.tokens);
        expect(parser.hasCache()).toBe(true);

        parser.clear();
        expect(parser.hasCache()).toBe(false);
    });
});

describe('IncCompiler', () => {
    it('should compileFull', () => {
        const compiler = new IncCompiler();
        const result = compiler.compileFull(SRC_BASELINE);

        expect(result.tokens).toBeDefined();
        expect(result.tokens.length).toBeGreaterThan(0);
        expect(result.ast).toBeInstanceOf(Program);
        expect(result.incremental).toBe(false);
    });

    it('should compileInc unchanged source', () => {
        const compiler = new IncCompiler();
        compiler.compileFull(SRC_BASELINE);
        const result = compiler.compileInc(SRC_BASELINE);

        expect(result.tokens.length).toBeGreaterThan(0);
        expect(result.ast).toBeInstanceOf(Program);
    });

    it('should compileInc detect change', () => {
        const compiler = new IncCompiler();
        compiler.compileFull(SRC_BASELINE);

        const modified = [
            'alias ic d0',
            'move r0 99',
            'add r0 r0 1',
            'yield',
        ].join('\n');

        const result = compiler.compileInc(modified);

        expect(result.tokens.length).toBeGreaterThan(0);
        expect(result.ast).toBeInstanceOf(Program);
    });

    it('should compileInc fallback when cache empty', () => {
        const compiler = new IncCompiler();
        const result = compiler.compileInc(SRC_BASELINE);

        expect(result.incremental).toBe(false);
        expect(result.tokens.length).toBeGreaterThan(0);
    });

    it('should hasCache and clear', () => {
        const compiler = new IncCompiler();
        expect(compiler.hasCache()).toBe(false);

        compiler.compileFull(SRC_BASELINE);
        expect(compiler.hasCache()).toBe(true);

        compiler.clear();
        expect(compiler.hasCache()).toBe(false);
    });
});
