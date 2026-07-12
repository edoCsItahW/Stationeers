# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: edocsitahw@qq.com

"""
@file test_lexer.py
@brief IC10 Python 绑定 - 词法分析器测试
@details 测试 Lexer 类的各项功能，包括：
          - 静态 tokenize 方法
          - 实例 scan 方法
          - 各类 Token 识别（关键字、标识符、数字、设备、寄存器等）
          - 位置信息正确性
          - 注释处理
          - 诊断报告
          - 错误恢复
"""

import os
import sys
import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python", "ic10"))

from ic10_python import (
    IC10Local,
    Pos,
    Token,
    Lexer,
    TokenType,
    TokenCategory,
)


def setup_module(module):
    """Set up test module: configure locale."""
    IC10Local.setLanguage("zh-hans")


# ============================================================
# 辅助函数
# ============================================================

def meaningful_tokens(tokens):
    """过滤掉 NEWLINE 和 END Token，只保留有意义的 Token"""
    return [t for t in tokens if t.type != TokenType.NEWLINE and t.type != TokenType.END]


# ============================================================
# Token 基础测试
# ============================================================

class TestToken:
    """Tests for the Token class."""

    def test_type_and_lexeme_accessible(self):
        tokens = Lexer.tokenize("move r0 42")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].type == TokenType.KEYWORD_MOVE
        assert meaningful[0].lexeme == "move"

    def test_to_string(self):
        tokens = Lexer.tokenize("move")
        meaningful = meaningful_tokens(tokens)

        s = str(meaningful[0])
        assert s
        assert isinstance(s, str)

    def test_to_json(self):
        import json
        tokens = Lexer.tokenize("42")
        meaningful = meaningful_tokens(tokens)

        json_str = meaningful[0].toJSON()
        assert json_str

        parsed = json.loads(json_str)
        assert parsed is not None
        assert parsed["lexeme"] == "42"

    def test_pos_property(self):
        tokens = Lexer.tokenize("move")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].pos is not None
        assert isinstance(meaningful[0].pos.line, int)


# ============================================================
# Lexer.tokenize 静态方法测试
# ============================================================

class TestLexerTokenize:
    """Tests for Lexer.tokenize static method."""

    def test_alias_directive(self):
        tokens = Lexer.tokenize("alias ic d0")
        meaningful = meaningful_tokens(tokens)

        assert len(meaningful) == 3
        assert meaningful[0].type == TokenType.KEYWORD_ALIAS
        assert meaningful[1].type == TokenType.IDENTIFIER
        assert meaningful[2].type == TokenType.DEVICE

    def test_define_directive_with_number(self):
        tokens = Lexer.tokenize("define MAX 42")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].type == TokenType.KEYWORD_DEFINE
        assert meaningful[1].type == TokenType.IDENTIFIER
        assert meaningful[2].type == TokenType.INTEGER
        assert meaningful[2].lexeme == "42"

    def test_label_definition(self):
        tokens = Lexer.tokenize("main:")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].type == TokenType.IDENTIFIER
        assert meaningful[1].type == TokenType.COLON

    def test_all_registers_r0_to_r15(self):
        for i in range(16):
            tokens = Lexer.tokenize(f"move r{i} 0")
            meaningful = meaningful_tokens(tokens)

            assert meaningful[1].type == TokenType.REGISTER
            assert meaningful[1].lexeme == f"r{i}"

    def test_all_devices_d0_to_d5(self):
        for i in range(6):
            tokens = Lexer.tokenize(f"alias dev d{i}")
            meaningful = meaningful_tokens(tokens)

            assert meaningful[2].type == TokenType.DEVICE
            assert meaningful[2].lexeme == f"d{i}"

    def test_device_references_db_and_dn(self):
        tokens = Lexer.tokenize("l r0 db Setting")
        meaningful = meaningful_tokens(tokens)

        db_token = next((t for t in meaningful if t.lexeme == "db"), None)
        assert db_token is not None

    def test_r16_plus_as_identifier(self):
        tokens = Lexer.tokenize("move r16 0")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[1].type == TokenType.IDENTIFIER
        assert meaningful[1].lexeme == "r16"

    def test_d6_plus_as_identifier(self):
        tokens = Lexer.tokenize("alias dev d6")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[2].type == TokenType.IDENTIFIER

    def test_integer_literals(self):
        tokens = Lexer.tokenize("move r0 42")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[2].type == TokenType.INTEGER
        assert meaningful[2].lexeme == "42"

    def test_float_literals(self):
        tokens = Lexer.tokenize("move r0 3.14")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[2].type == TokenType.FLOAT
        assert meaningful[2].lexeme == "3.14"

    def test_hex_numbers_with_dollar_prefix(self):
        tokens = Lexer.tokenize("move r0 $FF")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[2].type == TokenType.HEX_NUMBER
        assert meaningful[2].lexeme == "$FF"

    def test_binary_numbers_with_percent_prefix(self):
        tokens = Lexer.tokenize("move r0 %1010")
        meaningful = meaningful_tokens(tokens)

        assert meaningful[2].type == TokenType.BINARY_NUMBER
        assert meaningful[2].lexeme == "%1010"

    def test_string_literals(self):
        tokens = Lexer.tokenize('define NAME HASH("test")')
        meaningful = meaningful_tokens(tokens)

        str_token = next((t for t in meaningful if t.type == TokenType.STRING), None)
        assert str_token is not None
        assert str_token.lexeme == '"test"'

    def test_constants(self):
        constants = ["pi", "tau", "nan", "pinf", "ninf", "rgas"]
        for c in constants:
            tokens = Lexer.tokenize(f"move r0 {c}")
            meaningful = meaningful_tokens(tokens)

            const_token = next((t for t in meaningful if t.lexeme == c), None)
            assert const_token is not None

    def test_hash_and_slash_comments(self):
        tokens = Lexer.tokenize("# hash comment\n// slash comment\nhcf")
        meaningful = meaningful_tokens(tokens)

        hcf = next((t for t in meaningful if t.type == TokenType.KEYWORD_HCF), None)
        assert hcf is not None

    def test_multi_line_program(self):
        source = "\n".join([
            "alias ic d0",
            "main:",
            "move r0 0",
            "add r0 r0 1",
            "yield",
            "j main",
        ])

        tokens = Lexer.tokenize(source)
        meaningful = meaningful_tokens(tokens)

        assert len(meaningful) >= 10

    def test_end_token_at_end_of_input(self):
        tokens = Lexer.tokenize("hcf")
        end_token = next((t for t in tokens if t.type == TokenType.END), None)

        assert end_token is not None


# ============================================================
# Lexer.scan 实例方法测试
# ============================================================

class TestLexerScan:
    """Tests for Lexer.scan instance method."""

    def test_same_result_as_tokenize(self):
        source = "alias ic d0\nmove r0 1\n"

        static_tokens = Lexer.tokenize(source)
        lexer = Lexer(source)
        instance_tokens = lexer.scan()

        assert len(instance_tokens) == len(static_tokens)

    def test_preserve_comments_as_tokens(self):
        source = "# comment\nhcf\n"

        lexer = Lexer(source)
        tokens = lexer.scan()

        comment_token = next((t for t in tokens if t.type == TokenType.HEX_COMMENT), None)
        assert comment_token is not None

        hcf_token = next((t for t in tokens if t.type == TokenType.KEYWORD_HCF), None)
        assert hcf_token is not None

    def test_report_diagnostics_for_unclosed_string(self):
        source = 'move r0 1\n"unclosed\nhcf\n'
        lexer = Lexer(source)
        lexer.scan()

        assert len(lexer.diagnostics) > 0

    def test_handle_empty_source(self):
        lexer = Lexer("")
        tokens = lexer.scan()

        assert len(tokens) >= 1
        assert len(lexer.diagnostics) == 0

    def test_handle_source_with_only_newlines(self):
        lexer = Lexer("\n\n\n")
        tokens = lexer.scan()

        assert sum(1 for t in tokens if t.type == TokenType.NEWLINE) == 3
        assert len(lexer.diagnostics) == 0

    def test_handle_source_with_only_comments(self):
        lexer = Lexer("# comment\n// comment\n")
        tokens = lexer.scan()

        assert len(lexer.diagnostics) == 0


# ============================================================
# 位置信息测试
# ============================================================

class TestTokenPositions:
    """Tests for token position tracking."""

    def test_track_line_numbers(self):
        source = "move r0 1\nmove r1 2\nmove r2 3\n"
        tokens = Lexer.tokenize(source)
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].pos.line == 1

        second_move = next(
            (t for t in meaningful if t.type == TokenType.KEYWORD_MOVE and t.pos.line == 2),
            None
        )
        assert second_move is not None

        third_move = next(
            (t for t in meaningful if t.type == TokenType.KEYWORD_MOVE and t.pos.line == 3),
            None
        )
        assert third_move is not None

    def test_track_column_numbers(self):
        source = "move r0 1"
        tokens = Lexer.tokenize(source)
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].pos.column == 1
        assert meaningful[1].pos.column == 6
        assert meaningful[2].pos.column == 9

    def test_track_offsets(self):
        source = "move r0 1"
        tokens = Lexer.tokenize(source)
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].pos.offset == 0
        assert meaningful[1].pos.offset == 5
        assert meaningful[2].pos.offset == 8


# ============================================================
# Unicode 标识符测试
# ============================================================

class TestUnicodeIdentifiers:
    """Tests for Unicode identifier support."""

    def test_chinese_identifiers(self):
        source = "alias 中文变量 d0\n"
        tokens = Lexer.tokenize(source)
        meaningful = meaningful_tokens(tokens)

        assert meaningful[1].type == TokenType.IDENTIFIER
        assert meaningful[1].lexeme == "中文变量"

    def test_chinese_labels(self):
        source = "主循环:\nhcf\n"
        tokens = Lexer.tokenize(source)
        meaningful = meaningful_tokens(tokens)

        assert meaningful[0].type == TokenType.IDENTIFIER
        assert meaningful[0].lexeme == "主循环"


# ============================================================
# 关键字完整性测试
# ============================================================

class TestKeywordRecognition:
    """Tests for keyword recognition."""

    def test_nullary_instruction_keywords(self):
        keywords = [
            ("hcf", TokenType.KEYWORD_HCF),
            ("yield", TokenType.KEYWORD_YIELD),
        ]

        for lexeme, token_type in keywords:
            tokens = Lexer.tokenize(lexeme)
            meaningful = meaningful_tokens(tokens)
            assert meaningful[0].type == token_type

    def test_preprocessor_keywords(self):
        alias_tokens = meaningful_tokens(Lexer.tokenize("alias"))
        assert alias_tokens[0].type == TokenType.KEYWORD_ALIAS

        define_tokens = meaningful_tokens(Lexer.tokenize("define"))
        assert define_tokens[0].type == TokenType.KEYWORD_DEFINE

    def test_unary_instruction_keywords(self):
        keywords = ["sleep", "j", "jal", "jr", "rand", "peek", "pop", "push", "clr", "clrd"]
        for kw in keywords:
            tokens = Lexer.tokenize(kw)
            meaningful = meaningful_tokens(tokens)
            assert meaningful[0].type != TokenType.IDENTIFIER
            assert meaningful[0].lexeme == kw

    def test_binary_instruction_keywords(self):
        keywords = ["move", "add", "sub", "mul", "div", "abs", "ceil", "floor", "round", "sqrt", "sgn"]
        for kw in keywords:
            tokens = Lexer.tokenize(kw)
            meaningful = meaningful_tokens(tokens)
            assert meaningful[0].type != TokenType.IDENTIFIER

    def test_macro_keywords(self):
        hash_tokens = meaningful_tokens(Lexer.tokenize("HASH"))
        assert hash_tokens[0].type == TokenType.KEYWORD_HASH

        str_tokens = meaningful_tokens(Lexer.tokenize("STR"))
        assert str_tokens[0].type == TokenType.KEYWORD_STR
