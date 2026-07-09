# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: edocsitahw@qq.com

import os
import sys
import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python", "ic10"))

from ic10_python import (
    IncLexer,
    IncLexerResult,
    IncParser,
    IncParserResult,
    IncCompiler,
    IncCompileResult,
    Program,
)

SRC_BASELINE = "\n".join(
    [
        "alias ic d0",
        "alias something r0",
        "move r0 42",
        "add r0 r0 1",
        "yield",
    ]
)


class TestIncLexer:
    def test_tokenize_full(self):
        lexer = IncLexer()
        result = lexer.tokenizeFull(SRC_BASELINE)

        assert isinstance(result, IncLexerResult)
        assert len(result.tokens) > 0
        assert result.incremental is False
        assert result.relexedLines > 0
        assert result.changedStartLine == 1

    def test_tokenize_inc_unchanged(self):
        lexer = IncLexer()
        lexer.tokenizeFull(SRC_BASELINE)
        result = lexer.tokenizeInc(SRC_BASELINE)

        assert result.incremental is True
        assert result.relexedLines == 0
        assert len(result.tokens) > 0

    def test_tokenize_inc_single_line_change(self):
        lexer = IncLexer()
        lexer.tokenizeFull(SRC_BASELINE)

        modified = "\n".join(
            [
                "alias ic d0",
                "alias something r0",
                "move r0 99",
                "add r0 r0 1",
                "yield",
            ]
        )

        result = lexer.tokenizeInc(modified)

        assert result.incremental is True
        assert result.relexedLines >= 1
        assert result.changedStartLine >= 1

    def test_tokenize_inc_append_lines(self):
        lexer = IncLexer()
        lexer.tokenizeFull(SRC_BASELINE)

        appended = SRC_BASELINE + "\nmove r1 10"

        result = lexer.tokenizeInc(appended)

        assert result.incremental is True
        assert result.relexedLines >= 1

    def test_tokenize_inc_fallback_when_cache_empty(self):
        lexer = IncLexer()
        result = lexer.tokenizeInc(SRC_BASELINE)

        assert result.incremental is False
        assert result.relexedLines > 0

    def test_has_cache_and_clear(self):
        lexer = IncLexer()
        assert lexer.hasCache() is False

        lexer.tokenizeFull(SRC_BASELINE)
        assert lexer.hasCache() is True

        lexer.clear()
        assert lexer.hasCache() is False


class TestIncParser:
    @staticmethod
    def _get_tokens(src: str):
        lexer = IncLexer()
        return lexer.tokenizeFull(src)

    def test_parse_full(self):
        parser = IncParser()
        lexer_result = self._get_tokens(SRC_BASELINE)
        result = parser.parseFull(lexer_result.tokens)

        assert isinstance(result, IncParserResult)
        assert isinstance(result.ast, Program)
        assert result.incremental is False
        assert result.reparsedStmts > 0

    def test_parse_inc(self):
        parser = IncParser()
        lexer_result = self._get_tokens(SRC_BASELINE)

        parser.parseFull(lexer_result.tokens)

        modified = "\n".join(
            [
                "alias ic d0",
                "alias something r0",
                "move r0 99",
                "add r0 r0 1",
                "yield",
            ]
        )
        new_lexer_result = self._get_tokens(modified)

        result = parser.parseInc(
            new_lexer_result.tokens,
            new_lexer_result.changedStartLine,
        )

        assert isinstance(result.ast, Program)
        assert hasattr(result.ast, "statements")

    def test_has_cache_and_clear(self):
        parser = IncParser()
        assert parser.hasCache() is False

        lexer_result = self._get_tokens(SRC_BASELINE)
        parser.parseFull(lexer_result.tokens)
        assert parser.hasCache() is True

        parser.clear()
        assert parser.hasCache() is False


class TestIncCompiler:
    def test_compile_full(self):
        compiler = IncCompiler()
        result = compiler.compileFull(SRC_BASELINE)

        assert isinstance(result, IncCompileResult)
        assert len(result.tokens) > 0
        assert isinstance(result.ast, Program)
        assert result.incremental is False

    def test_compile_inc_unchanged(self):
        compiler = IncCompiler()
        compiler.compileFull(SRC_BASELINE)
        result = compiler.compileInc(SRC_BASELINE)

        assert len(result.tokens) > 0
        assert isinstance(result.ast, Program)

    def test_compile_inc_detect_change(self):
        compiler = IncCompiler()
        compiler.compileFull(SRC_BASELINE)

        modified = "\n".join(
            [
                "alias ic d0",
                "move r0 99",
                "add r0 r0 1",
                "yield",
            ]
        )

        result = compiler.compileInc(modified)

        assert len(result.tokens) > 0
        assert isinstance(result.ast, Program)

    def test_compile_inc_fallback_when_cache_empty(self):
        compiler = IncCompiler()
        result = compiler.compileInc(SRC_BASELINE)

        assert result.incremental is False
        assert len(result.tokens) > 0

    def test_has_cache_and_clear(self):
        compiler = IncCompiler()
        assert compiler.hasCache() is False

        compiler.compileFull(SRC_BASELINE)
        assert compiler.hasCache() is True

        compiler.clear()
        assert compiler.hasCache() is False
