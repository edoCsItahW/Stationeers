# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: 2207150234@st.sziit.edu.cn

"""
IC10 compiler Python bindings - type stubs

@file ic10_python.pyi
@brief Type stubs for the ic10_python extension module (pybind11)
@details Provides type hints for all exported classes, enums, and functions
         from the IC10 compiler C++ core.
"""

from typing import List, Dict, Any, Optional


# ============================================================================
# Exceptions
# ============================================================================

class RedefinitionError(RuntimeError):
    """Error raised when a symbol is redefined."""
    name: str
    start: Pos
    end: Pos


class UndefinedSymbolError(RuntimeError):
    """Error raised when an undefined symbol is referenced."""
    name: str
    start: Pos
    end: Pos


class IC10RuntimeError(RuntimeError):
    """Generic IC10 runtime error."""
    name: str
    start: Pos
    end: Pos


# ============================================================================
# Enums
# ============================================================================

class TokenType:
    """IC10 token type enumeration."""
    INTEGER: TokenType
    FLOAT: TokenType
    HEX_NUMBER: TokenType
    BINARY_NUMBER: TokenType
    STRING: TokenType
    IDENTIFIER: TokenType
    REGISTER: TokenType
    DEVICE: TokenType
    LPAREN: TokenType
    RPAREN: TokenType
    COLON: TokenType
    HEX_COMMENT: TokenType
    SLASH_COMMENT: TokenType
    NEWLINE: TokenType
    KEYWORD_HASH: TokenType
    KEYWORD_STR: TokenType
    KEYWORD_NAN: TokenType
    KEYWORD_PINF: TokenType
    KEYWORD_NINF: TokenType
    KEYWORD_PI: TokenType
    KEYWORD_TAU: TokenType
    KEYWORD_DEG2RAD: TokenType
    KEYWORD_RAD2DEG: TokenType
    KEYWORD_EPSILON: TokenType
    KEYWORD_GAS_CONSTANT: TokenType
    END: TokenType
    UNKNOWN: TokenType
    KEYWORD_HCF: TokenType
    KEYWORD_YIELD: TokenType
    KEYWORD_ALIAS: TokenType
    KEYWORD_DEFINE: TokenType
    KEYWORD_PEEK: TokenType
    KEYWORD_POP: TokenType
    KEYWORD_PUSH: TokenType
    KEYWORD_CLR: TokenType
    KEYWORD_J: TokenType
    KEYWORD_JAL: TokenType
    KEYWORD_JR: TokenType
    KEYWORD_RAND: TokenType
    KEYWORD_SLEEP: TokenType
    KEYWORD_ABS: TokenType
    KEYWORD_ACOS: TokenType
    KEYWORD_ADD: TokenType
    KEYWORD_ASIN: TokenType
    KEYWORD_ATAN: TokenType
    KEYWORD_ATAN2: TokenType
    KEYWORD_CEIL: TokenType
    KEYWORD_COS: TokenType
    KEYWORD_DIV: TokenType
    KEYWORD_EXP: TokenType
    KEYWORD_FLOOR: TokenType
    KEYWORD_LOG: TokenType
    KEYWORD_MAX: TokenType
    KEYWORD_MIN: TokenType
    KEYWORD_MOD: TokenType
    KEYWORD_MUL: TokenType
    KEYWORD_SGN: TokenType
    KEYWORD_SIN: TokenType
    KEYWORD_SQRT: TokenType
    KEYWORD_TAN: TokenType
    KEYWORD_MOVE: TokenType
    KEYWORD_SDB: TokenType
    KEYWORD_SBP: TokenType
    KEYWORD_SLB: TokenType
    KEYWORD_SLT: TokenType
    KEYWORD_LB: TokenType
    KEYWORD_LBN: TokenType
    KEYWORD_LBNS: TokenType
    KEYWORD_LBS: TokenType
    KEYWORD_SB: TokenType
    KEYWORD_SBN: TokenType
    KEYWORD_SBNS: TokenType
    KEYWORD_SBS: TokenType
    KEYWORD_SAP: TokenType
    KEYWORD_SELECT: TokenType
    KEYWORD_SD: TokenType
    KEYWORD_SL: TokenType
    KEYWORD_DBNZ: TokenType
    KEYWORD_DBZ: TokenType
    KEYWORD_DGE: TokenType
    KEYWORD_DGT: TokenType
    KEYWORD_DLE: TokenType
    KEYWORD_DLT: TokenType
    KEYWORD_DNE: TokenType
    KEYWORD_DEQ: TokenType
    KEYWORD_BEZ: TokenType
    KEYWORD_BNZ: TokenType
    KEYWORD_BGE: TokenType
    KEYWORD_BGT: TokenType
    KEYWORD_BLE: TokenType
    KEYWORD_BLT: TokenType
    KEYWORD_BNE: TokenType
    KEYWORD_BEQ: TokenType
    KEYWORD_SEQZ: TokenType
    KEYWORD_SNEZ: TokenType
    KEYWORD_SGEZ: TokenType
    KEYWORD_SGTZ: TokenType
    KEYWORD_SLEZ: TokenType
    KEYWORD_SLTZ: TokenType
    KEYWORD_SNAN: TokenType
    KEYWORD_SNANZ: TokenType
    KEYWORD_LERP: TokenType
    KEYWORD_MIX: TokenType
    KEYWORD_CLAMP: TokenType
    KEYWORD_MINIMUM: TokenType
    KEYWORD_MAXIMUM: TokenType
    KEYWORD_FENCE: TokenType
    KEYWORD_RENDEZVOUS: TokenType
    KEYWORD_PASS: TokenType
    KEYWORD_PE: TokenType
    KEYWORD_PD: TokenType
    KEYWORD_ALE: TokenType
    KEYWORD_ALE: TokenType
    KEYWORD_ALG: TokenType
    KEYWORD_ALGE: TokenType
    KEYWORD_ALE_MIN: TokenType
    KEYWORD_ALE_MAX: TokenType
    KEYWORD_ALG_MIN: TokenType
    KEYWORD_ALG_MAX: TokenType
    KEYWORD_LINE: TokenType
    KEYWORD_LR: TokenType
    KEYWORD_HALT: TokenType
    KEYWORD_START: TokenType
    KEYWORD_STOP: TokenType
    KEYWORD_SLEEPING: TokenType
    KEYWORD_TRIG: TokenType
    KEYWORD_REAGENT_MODE: TokenType
    KEYWORD_REAGENT: TokenType
    KEYWORD_SLOT: TokenType
    KEYWORD_EXPORT: TokenType
    KEYWORD_IMPORT: TokenType
    KEYWORD_DEVICE: TokenType
    KEYWORD_DUP: TokenType
    KEYWORD_SWAP: TokenType


class TokenCategory:
    """IC10 token category enumeration."""
    LITERAL: TokenCategory
    SYMBOL: TokenCategory
    COMMENT: TokenCategory
    WHITESPACE: TokenCategory
    END: TokenCategory
    INVALID: TokenCategory


# ============================================================================
# IC10Local - Localization
# ============================================================================

class IC10Local:
    """IC10 compiler localization settings."""

    @staticmethod
    def setLanguage(language: str) -> None:
        """Set the compiler language locale.

        @param language: Language code (e.g. 'en', 'zh-hans')
        """
        ...


# ============================================================================
# Pos - Position
# ============================================================================

class Pos:
    """Position in source code (line, column, offset)."""

    line: int
    column: int
    offset: int

    def __init__(self) -> None: ...

    def newline(self) -> None:
        """Advance to the next line (line++, column=0)."""
        ...

    def next(self) -> None:
        """Advance to the next character (column++, offset++)."""
        ...

    def move(self, index: int) -> None:
        """Move to a specific byte offset.

        @param index: Target byte offset
        """
        ...


# ============================================================================
# Token
# ============================================================================

class Token:
    """Lexical token from IC10 source code."""

    type: TokenType
    pos: Pos
    lexeme: str
    category: TokenCategory

    def __init__(
        self,
        type: TokenType,
        pos: Pos,
        lexeme: str = ...,
        category: TokenCategory = ...,
    ) -> None: ...

    def toString(self) -> str:
        """Return human-readable string representation."""
        ...

    def toJSON(self) -> str:
        """Return JSON string representation."""
        ...

    def __repr__(self) -> str: ...


# ============================================================================
# Lexer
# ============================================================================

class Lexer:
    """IC10 lexical analyzer."""

    def __init__(self, source: str, debug: bool = ...) -> None:
        """Create a lexer for the given source code.

        @param source: IC10 source code string
        @param debug: If True, preserve comment tokens
        """
        ...

    @staticmethod
    def tokenize(source: str) -> List[Token]:
        """Tokenize source code (static convenience method).

        @param source: IC10 source code string
        @return: List of tokens
        """
        ...

    def scan(self) -> List[Token]:
        """Run lexical analysis on the source.

        @return: List of tokens
        """
        ...


# ============================================================================
# Program / AST
# ============================================================================

class Program:
    """IC10 abstract syntax tree root node."""

    statements: List[Dict[str, Any]]
    """List of statement nodes as dicts (parsed from JSON)."""

    def __init__(self) -> None: ...

    def toJSON(self) -> str:
        """Serialize program to JSON string."""
        ...

    def toString(self) -> str:
        """Return string representation."""
        ...

    def __repr__(self) -> str: ...


# ============================================================================
# Parser
# ============================================================================

class Parser:
    """IC10 recursive descent parser."""

    def __init__(self, tokens: List[Token], debug: bool = ...) -> None:
        """Create a parser from a list of tokens.

        @param tokens: List of tokens from the lexer
        @param debug: Enable debug output
        """
        ...

    @staticmethod
    def parsing(tokens: List[Token]) -> Program:
        """Parse tokens into a Program (static convenience method).

        @param tokens: List of tokens
        @return: Program AST
        """
        ...

    def parse(self) -> Program:
        """Run the parser and return the Program AST.

        @return: Program AST
        """
        ...


# ============================================================================
# SymbolTable
# ============================================================================

class SymbolTable:
    """Symbol table for semantic analysis."""

    def __init__(self) -> None: ...

    def toJSON(self) -> str:
        """Serialize symbol table to JSON string."""
        ...

    def __repr__(self) -> str: ...


# ============================================================================
# Analyser
# ============================================================================

class Analyser:
    """IC10 semantic analyser.

    @note The static analyse() method performs analysis without
          populating instance state. Use visit() for instance-level analysis.
    """

    def __init__(self) -> None: ...

    @staticmethod
    def analyse(program: Program) -> None:
        """Statically analyse a program (blocking).

        @param program: Program AST to analyse
        @note This method does not populate instance state.
        """
        ...

    def visit(self, program: Program) -> None:
        """Visit and analyse a program (blocking).

        After analysis, results are available via symbolTable and errors.

        @param program: Program AST to visit
        """
        ...

    @property
    def symbolTable(self) -> SymbolTable:
        """Get the symbol table after analysis."""
        ...

    @property
    def errors(self) -> List[BaseException]:
        """Get the list of semantic errors after analysis.

        Each error is a Python exception object with start, end, and name attributes.
        """
        ...
