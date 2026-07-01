# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: 2207150234@st.sziit.edu.cn

"""
IC10 Python API tests

@file test_api.py
@brief Test suite for the IC10 Python bindings (pybind11)
@details Covers Token, Lexer, Parser, Program, Analyser, SymbolTable, and Pos
"""

import sys
import os
import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python", "ic10"))

from ic10_python import (
    IC10Local,
    Pos,
    Token,
    Lexer,
    Parser,
    Program,
    Analyser,
    SymbolTable,
    TokenType,
    TokenCategory,
)


def setup_module(module):
    """Set up test module: configure locale."""
    IC10Local.setLanguage("zh-hans")


class TestPos:
    """Tests for the Pos class."""

    def test_default_constructor(self):
        pos = Pos()
        assert pos.line == 1
        assert pos.column == 1
        assert pos.offset == 0

    def test_newline(self):
        pos = Pos()
        pos.newline()
        assert pos.line == 2
        assert pos.column == 1

    def test_next(self):
        pos = Pos()
        pos.next()
        assert pos.column == 2
        assert pos.offset == 1

    def test_move(self):
        pos = Pos()
        pos.move(42)
        assert pos.offset == 42


class TestToken:
    """Tests for the Token class."""

    def test_constructor(self):
        pos = Pos()
        token = Token(TokenType.KEYWORD_ALIAS, pos, "alias", TokenCategory.SYMBOL)
        assert token.type == TokenType.KEYWORD_ALIAS
        assert token.lexeme == "alias"
        assert token.category == TokenCategory.SYMBOL

    def test_to_string(self):
        pos = Pos()
        token = Token(TokenType.KEYWORD_ALIAS, pos, "alias", TokenCategory.SYMBOL)
        assert "alias" in str(token)

    def test_to_json(self):
        pos = Pos()
        token = Token(TokenType.KEYWORD_ALIAS, pos, "alias", TokenCategory.SYMBOL)
        import json
        data = json.loads(token.toJSON())
        assert data["lexeme"] == "alias"


class TestLexer:
    """Tests for the Lexer class."""

    def test_tokenize_static(self):
        tokens = Lexer.tokenize("alias ic d0")
        assert len(tokens) == 4

    def test_scan_instance(self):
        lexer = Lexer("alias ic d0")
        tokens = lexer.scan()
        assert len(tokens) == 4

    def test_tokenize_with_line_ending(self):
        tokens = Lexer.tokenize("move r0 r1\n")
        assert len(tokens) > 0


class TestParser:
    """Tests for the Parser class."""

    def test_parse_tokens(self):
        tokens = Lexer.tokenize("alias ic d0")
        parser = Parser(tokens)
        program = parser.parse()
        assert isinstance(program, Program)

    def test_program_statements(self):
        tokens = Lexer.tokenize("alias ic d0")
        parser = Parser(tokens)
        program = parser.parse()
        assert hasattr(program, "statements")
        assert len(program.statements) > 0


class TestAnalyser:
    """Tests for the Analyser class."""

    def test_visit_program(self):
        tokens = Lexer.tokenize("alias ic d0")
        parser = Parser(tokens)
        program = parser.parse()
        analyser = Analyser()
        analyser.visit(program)
        symbol_table = analyser.symbolTable
        assert isinstance(symbol_table, SymbolTable)
        assert symbol_table.toJSON() is not None

    def test_analyse_static(self):
        tokens = Lexer.tokenize("alias ic d0")
        parser = Parser(tokens)
        program = parser.parse()
        Analyser.analyse(program)

    def test_analyser_errors(self):
        tokens = Lexer.tokenize("alias ic d0")
        parser = Parser(tokens)
        program = parser.parse()
        analyser = Analyser()
        analyser.visit(program)
        assert isinstance(analyser.errors, list)


class TestSymbolTable:
    """Tests for the SymbolTable class."""

    def test_to_json(self):
        st = SymbolTable()
        json_str = st.toJSON()
        assert isinstance(json_str, str)
