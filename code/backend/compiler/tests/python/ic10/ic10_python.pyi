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

import enum
from typing import List, Dict, Any, Optional, TypedDict, Literal, Union


class TypeTable:
    """Type table for semantic analysis."""
    def __init__(self) -> None: ...
    def toJSON(self) -> str: ...


# ============================================================================
# Type Table JSON Interfaces
# ============================================================================

class DescValue(TypedDict):
    """Description value (text or link)."""
    kind: str
    value: str

class DeviceSlot(TypedDict):
    """Device slot entry."""
    index: str
    direction: str
    desc: Optional[DescValue]

class DeviceLogic(TypedDict):
    """Device logic entry."""
    name: str
    access: str

class DeviceMode(TypedDict):
    """Device mode entry."""
    index: str
    desc: Optional[DescValue]

class DeviceConnect(TypedDict):
    """Device connect entry."""
    index: str
    desc: Optional[DescValue]

class DeviceTypeJSON(TypedDict):
    """Device type (from toJSON())."""
    type: Literal["device"]
    name: str
    desc: Optional[DescValue]
    slots: List[DeviceSlot]
    logics: List[DeviceLogic]
    modes: List[DeviceMode]
    logicSlots: List[str]
    connects: List[DeviceConnect]

class EnumValueEntry(TypedDict):
    """Enum value entry."""
    name: str
    value: str
    desc: Optional[DescValue]

class EnumTypeJSON(TypedDict):
    """Enum type (from toJSON())."""
    type: Literal["enum"]
    name: str
    desc: Optional[DescValue]
    values: List[EnumValueEntry]

CustomTypeJSON = Union[DeviceTypeJSON, EnumTypeJSON]

TypeTableMap = Dict[str, CustomTypeJSON]
"""Type table JSON dictionary."""


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

class TokenType(enum.IntEnum):
    """IC10 token type enumeration.

    Auto-synced with C++ ``ic10::TokenType`` via pybind11 ``py::enum_``.
    Values are contiguous starting from 0.
    """
    # 数字
    INTEGER = 0
    FLOAT
    HEX_NUMBER
    BINARY_NUMBER
    STRING
    # 变量名
    IDENTIFIER
    # 寄存器设备
    REGISTER
    DEVICE
    # 符号
    LPAREN
    RPAREN
    COLON
    # 注释
    HEX_COMMENT
    SLASH_COMMENT
    # 注解
    DOC_COMMENT
    TYPE_HINT
    # 换行
    NEWLINE
    # 关键字 - 宏与函数
    KEYWORD_HASH
    KEYWORD_STR
    # 关键字 - 常量
    KEYWORD_NAN
    KEYWORD_PINF
    KEYWORD_NINF
    KEYWORD_PI
    KEYWORD_TAU
    KEYWORD_DEG2RAD
    KEYWORD_RAD2DEG
    KEYWORD_EPSILON
    KEYWORD_RGAS
    # 文件结束标记
    END
    # 未知标记
    UNKNOWN
    # 关键字 - 空指令
    KEYWORD_HCF
    KEYWORD_YIELD
    # 关键字 - 预处理指令
    KEYWORD_ALIAS
    KEYWORD_DEFINE
    # 关键字 - 一元指令
    KEYWORD_PEEK
    KEYWORD_POP
    KEYWORD_PUSH
    KEYWORD_CLR
    KEYWORD_J
    KEYWORD_JAL
    KEYWORD_JR
    KEYWORD_RAND
    KEYWORD_SLEEP
    KEYWORD_CLRD
    # 关键字 - 二元指令
    KEYWORD_ABS
    KEYWORD_ACOS
    KEYWORD_ADD
    KEYWORD_ASIN
    KEYWORD_ATAN
    KEYWORD_ATAN2
    KEYWORD_CEIL
    KEYWORD_COS
    KEYWORD_DIV
    KEYWORD_EXP
    KEYWORD_FLOOR
    KEYWORD_LOG
    KEYWORD_MAX
    KEYWORD_MIN
    KEYWORD_MOD
    KEYWORD_MUL
    KEYWORD_POW
    KEYWORD_ROUND
    KEYWORD_SIN
    KEYWORD_SQRT
    KEYWORD_SGN
    KEYWORD_SUB
    KEYWORD_TAN
    KEYWORD_TRUNC
    KEYWORD_NOT
    KEYWORD_MOVE
    KEYWORD_POKE
    KEYWORD_BEQZ
    KEYWORD_BEQZAL
    KEYWORD_BNEZ
    KEYWORD_BNEZAL
    KEYWORD_BGEZ
    KEYWORD_BGEZAL
    KEYWORD_BGTZ
    KEYWORD_BGTZAL
    KEYWORD_BLEZ
    KEYWORD_BLEZAL
    KEYWORD_BLTZ
    KEYWORD_BLTZAL
    KEYWORD_BNAN
    KEYWORD_BDNS
    KEYWORD_BDNSAL
    KEYWORD_BDSE
    KEYWORD_BDSEAL
    KEYWORD_BREQZ
    KEYWORD_BRNEZ
    KEYWORD_BRGEZ
    KEYWORD_BRGTZ
    KEYWORD_BRLEZ
    KEYWORD_BRLTZ
    KEYWORD_BRNAN
    KEYWORD_BRDNS
    KEYWORD_BRDSE
    KEYWORD_SEQZ
    KEYWORD_SNEZ
    KEYWORD_SGEZ
    KEYWORD_SGTZ
    KEYWORD_SLEZ
    KEYWORD_SLTZ
    KEYWORD_SNAN
    KEYWORD_SNANZ
    KEYWORD_SDNS
    KEYWORD_SDSE
    # 关键字 - 三元指令
    KEYWORD_AND
    KEYWORD_NOR
    KEYWORD_OR
    KEYWORD_SLA
    KEYWORD_SLL
    KEYWORD_SRA
    KEYWORD_SRL
    KEYWORD_XOR
    KEYWORD_GET
    KEYWORD_PUT
    KEYWORD_L
    KEYWORD_LS
    KEYWORD_LR
    KEYWORD_S
    KEYWORD_SB
    KEYWORD_ROL
    KEYWORD_ROR
    KEYWORD_RMAP
    KEYWORD_BEQ
    KEYWORD_BEQAL
    KEYWORD_BNE
    KEYWORD_BNEAL
    KEYWORD_BGE
    KEYWORD_BGEAL
    KEYWORD_BGT
    KEYWORD_BGTAL
    KEYWORD_BLE
    KEYWORD_BLEAL
    KEYWORD_BLT
    KEYWORD_BLTAL
    KEYWORD_BAPZ
    KEYWORD_BAPZAL
    KEYWORD_BNAZ
    KEYWORD_BNAZAL
    KEYWORD_BDNVL
    KEYWORD_BDNVS
    KEYWORD_BREQ
    KEYWORD_BRNE
    KEYWORD_BRGE
    KEYWORD_BRGT
    KEYWORD_BRLE
    KEYWORD_BRLT
    KEYWORD_BRAPZ
    KEYWORD_BRNAZ
    KEYWORD_SAPZ
    KEYWORD_SNAZ
    KEYWORD_SEQ
    KEYWORD_SNE
    KEYWORD_SGE
    KEYWORD_SGT
    KEYWORD_SLE
    KEYWORD_SLT
    # 关键字 - 四元指令
    KEYWORD_LERP
    KEYWORD_CLAMP
    KEYWORD_EXT
    KEYWORD_INS
    KEYWORD_SS
    KEYWORD_LB
    KEYWORD_SBN
    KEYWORD_SBS
    KEYWORD_BAP
    KEYWORD_BAPAL
    KEYWORD_BNA
    KEYWORD_BNAAL
    KEYWORD_BRAP
    KEYWORD_BRNA
    KEYWORD_SAP
    KEYWORD_SNA
    KEYWORD_SELECT
    # 关键字 - 五元指令
    KEYWORD_LBN
    KEYWORD_LBS
    # 关键字 - 六元指令
    KEYWORD_LBNS


class TokenCategory(enum.IntEnum):
    """IC10 token category enumeration.

    Auto-synced with C++ ``ic10::TokenCategory`` via pybind11 ``py::enum_``.
    """
    LITERAL = 0
    SYMBOL
    COMMENT
    ANNOTATION
    WHITESPACE
    END
    INVALID


class _TypeOfNodeEntry(TypedDict):
    """Type of node entry."""
    kind: int
    category: int


TypeOfNode: Dict[str, _TypeOfNodeEntry]
"""AST node type mapping.

Maps node type names to their BasicType and TypeCategory.

Example:
    >>> TypeOfNode['Integer']
    {'kind': BasicType.INTEGER, 'category': TypeCategory.NUMBER}
"""


class OperandType(enum.IntEnum):
    """IC10 operand type enumeration.

    Auto-synced with C++ ``ic10::OperandType`` via pybind11 ``py::enum_``.
    Used in AST JSON serialization for type1/type2/... fields as numeric values.
    """
    REG_IDENT = 0
    """Register or identifier."""
    DEV_ALIAS
    """Device alias reference."""
    REG_NUM
    """Register or number."""
    DEV_REF
    """Device reference."""
    LOGIC_SLOT
    """Logic slot type."""
    REAGENT_MODE
    """Reagent mode."""
    JUMP_TARGET
    """Jump target."""
    LOGIC_TYPE
    """Logic type."""
    SLOT_IDX
    """Slot index."""
    BATCH_MODE
    """Batch mode."""


class BasicType(enum.IntEnum):
    """IC10 basic type enumeration.

    Auto-synced with C++ ``ic10::BasicType`` via pybind11 ``py::enum_``.
    Used in Symbol JSON serialization for the ``type`` field as numeric values.
    """
    STRING = 0
    """String type."""
    INTEGER
    """Integer type."""
    FLOAT
    """Float type."""
    REGISTER
    """Register type."""
    DEVICE
    """Device type."""
    UNKNOWN
    """Unknown type."""
    ENUM
    """Enum type."""


class TypeCategory(enum.IntEnum):
    """IC10 type category enumeration.

    Auto-synced with C++ ``ic10::TypeCategory`` via pybind11 ``py::enum_``.
    Used in Symbol JSON serialization for the ``category`` field as numeric values.
    """
    LABEL = 0
    """Label category."""
    STR_CALL
    """String call category."""
    HASH_CALL
    """Hash call category."""
    CONSTANT
    """Constant category."""
    NUMBER
    """Number category."""
    BASIC
    """Basic type category."""


class Symbol(TypedDict):
    """Symbol information from SymbolTable.toJSON()."""

    name: str
    """Symbol name."""
    type: int
    """Basic type (BasicType numeric value)."""
    category: int
    """Type category (TypeCategory numeric value)."""
    typeName: Optional[str]
    """Optional type name (e.g. device type name)."""
    value: Optional[str]
    """Optional symbol value."""
    desc: Optional[str]
    """Optional description."""


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

    @property
    def diagnostics(self) -> List["Diagnostic"]:
        """Get the list of diagnostics from lexical analysis.

        Each diagnostic is a dict with level, id, start, end, message fields.
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

    @property
    def diagnostics(self) -> List["Diagnostic"]:
        """Get the list of diagnostics from parsing.

        Each diagnostic is a dict with level, id, start, end, message fields.
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
# Diagnostic
# ============================================================================

class Diagnostic(TypedDict):
    """Diagnostic information from analysis.

    @ivar level: Diagnostic level - "error", "warning", or "info"
    @ivar id: Diagnostic ID string (e.g. "IEA1_2", "IMP17")
    @ivar start: Start position in source code
    @ivar end: End position in source code
    @ivar message: Diagnostic message text
    """

    level: str
    id: str
    start: "Pos"
    end: "Pos"
    message: str


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

        After analysis, results are available via symbolTable and diagnostics.

        @param program: Program AST to visit
        """
        ...

    @property
    def symbolTable(self) -> SymbolTable:
        """Get the symbol table after analysis."""
        ...

    @property
    def typeTable(self) -> TypeTable:
        """Get the type table after analysis."""
        ...

    @property
    def diagnostics(self) -> List[Diagnostic]:
        """Get the list of diagnostics after analysis.

        Each diagnostic is a dict with level, id, start, end, message fields.
        """
        ...


# ---------------------------------------------------------------------------
# Linker - 链接器
# ---------------------------------------------------------------------------

class UnitInfo:
    """Compilation unit information from Linker.

    @ivar path: Source file path
    @ivar diagnostics: List of diagnostics for this unit
    """

    path: str
    """Source file path."""

    @property
    def diagnostics(self) -> List[Diagnostic]:
        """Get the list of diagnostics for this unit."""
        ...


class Linker:
    """IC10 linker for linking multiple compilation units.

    Used to link multiple IC10 compilation units, merge their symbol tables,
    and collect all diagnostics.

    @example
    @code
    # Create linker and link multiple units
    linker = ic10.Linker()

    # Add source strings
    linker.addUnit('alias sensor0 console0')
    linker.addUnit('move r0 r1', 'file.ic10')

    # Add parsed Program object
    tokens = ic10.Lexer.tokenize('add r2 r0 r1')
    parser = ic10.Parser(tokens)
    program = parser.parse()
    linker.addUnit(program, 'another.ic10')

    # Perform linking
    symbol_table = linker.link()

    # Check diagnostics
    if linker.diagnostics:
        print('Diagnostics:', linker.diagnostics)
    @endcode
    """

    def __init__(self) -> None: ...

    def addUnit(self, program: Program) -> None:
        """Add compilation unit (Program object).

        @param program: Program object to link
        """
        ...

    def addUnit(self, source: str) -> None:
        """Add compilation unit (source code string).

        @param source: IC10 source code string
        """
        ...

    def addUnit(self, program: Program, path: str) -> None:
        """Add compilation unit with path.

        @param program: Program object to link
        @param path: Source file path
        """
        ...

    def addUnit(self, source: str, path: str) -> None:
        """Add compilation unit with path (source string).

        @param source: IC10 source code string
        @param path: Source file path
        """
        ...

    def link(self) -> SymbolTable:
        """Perform linking.

        @return: Merged SymbolTable object
        """
        ...

    @property
    def diagnostics(self) -> List[Diagnostic]:
        """Get all diagnostics.

        List of diagnostics, each is a dict with level, id, start, end, message fields.
        """
        ...

    @property
    def units(self) -> List[UnitInfo]:
        """Get all compilation units.

        List of UnitInfo objects.
        """
        ...


# ---------------------------------------------------------------------------
#  增量编译相关
# ---------------------------------------------------------------------------

class IncLexerResult:
    tokens: List[Token]
    incremental: bool
    relexedLines: int
    changedStartLine: int
    oldChangedEndLine: int
    newChangedEndLine: int
    def __init__(self) -> None: ...

class IncLexer:
    def __init__(self) -> None: ...
    def tokenizeFull(self, source: str) -> IncLexerResult: ...
    def tokenizeInc(self, newSource: str) -> IncLexerResult: ...
    def hasCache(self) -> bool: ...
    def clear(self) -> None: ...

class IncParserResult:
    ast: Program
    incremental: bool
    reparsedStmts: int
    affectedStmtStart: int
    def __init__(self) -> None: ...

class IncParser:
    def __init__(self) -> None: ...
    def parseFull(self, tokens: List[Token]) -> IncParserResult: ...
    def parseInc(self, tokens: List[Token], changedStartLine: int) -> IncParserResult: ...
    def hasCache(self) -> bool: ...
    def clear(self) -> None: ...

class IncCompileResult:
    tokens: List[Token]
    ast: Program
    incremental: bool
    relexedLines: int
    reparsedStmts: int
    def __init__(self) -> None: ...

class IncCompiler:
    def __init__(self) -> None: ...
    def compileFull(self, source: str) -> IncCompileResult: ...
    def compileInc(self, source: str) -> IncCompileResult: ...
    def hasCache(self) -> bool: ...
    def clear(self) -> None: ...
