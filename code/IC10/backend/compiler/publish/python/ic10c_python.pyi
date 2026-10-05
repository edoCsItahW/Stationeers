# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: 2207150234@st.sziit.edu.cn

"""
IC10 compiler Python bindings - type stubs

@file ic10c_python.pyi
@brief Type stubs for the ic10c_python extension module (pybind11)
@details Provides type hints for all exported classes, enums, and functions
         from the IC10 compiler C++ core.
@note JSON keys typed ``Optional[...]`` are OMITTED from the emitted JSON when they
      are unset - the C++ ``toJson`` helper drops empty ``std::optional`` values, so
      such a key is absent rather than present with a ``null`` value.
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

class PosJSON(TypedDict):
    """Source position, as emitted by ``Pos::toJSON()``."""
    line: int
    column: int
    offset: int


class StringNodeJSON(TypedDict):
    """JSON of the ``String`` AST leaf node."""
    nodeName: Literal["String"]
    position: PosJSON
    end: PosJSON
    value: str


class LinkNodeJSON(TypedDict):
    """JSON of the ``Link`` AST node (``.path.to.field``)."""
    nodeName: Literal["Link"]
    position: PosJSON
    end: PosJSON
    paths: List[str]
    fields: List[str]


class TokenJSON(TypedDict):
    """JSON of a ``Token`` (as embedded by ``ErrorNode``)."""
    type: int
    """Token type (TokenType numeric value)."""
    pos: PosJSON
    lexeme: str
    category: int
    """Token category (TokenCategory numeric value)."""


class ErrorNodeJSON(TypedDict):
    """JSON of the ``Error`` AST node."""
    nodeName: Literal["Error"]
    position: PosJSON
    end: PosJSON
    token: TokenJSON
    message: str


DescriptionJSON = Union[StringNodeJSON, LinkNodeJSON, ErrorNodeJSON]
"""A ``desc`` value: a NESTED node object, never a plain string.

Use ``nodeName`` to discriminate the variant. There is no ``kind`` key.
"""


class DeviceAnnotationLogic(TypedDict):
    """JSON of one ``@logic`` line.

    @note Optional keys are absent from the JSON when the line does not declare
          them; ``slotIndices`` is ``[]`` only when declared as ``()``.
    """
    nodeName: Literal["DeviceAnnotationLogic"]
    position: PosJSON
    end: PosJSON
    name: str
    value: str
    access: Optional[Literal["r", "w", "rw"]]
    """Read/write access; absent when not declared."""
    slotIndices: Optional[List[int]]
    """Applicable slot indices; absent when not declared."""
    defaultValue: Optional[str]
    """Default value; absent when not declared."""
    desc: Optional[DescriptionJSON]
    """Description; absent when not declared."""


class DeviceAnnotationLogicSlot(TypedDict):
    """JSON of one ``@logic-slot`` line.

    @note Optional keys are absent from the JSON when the line does not declare
          them; ``slotIndices`` is ``[]`` only when declared as ``()``.
    """
    nodeName: Literal["DeviceAnnotationLogicSlot"]
    position: PosJSON
    end: PosJSON
    name: str
    value: str
    access: Optional[Literal["r", "w", "rw"]]
    """Read/write access; absent when not declared."""
    slotIndices: Optional[List[int]]
    """Applicable slot indices; absent when not declared."""
    defaultValue: Optional[str]
    """Default value; absent when not declared."""
    desc: Optional[DescriptionJSON]
    """Description; absent when not declared."""


class DeviceAnnotationSlot(TypedDict):
    """JSON of one ``@slot`` line.

    @note Optional keys are absent from the JSON when the line does not declare
          them; ``slotIndices`` is ``[]`` only when declared as ``()``.
    """
    nodeName: Literal["DeviceAnnotationSlot"]
    position: PosJSON
    end: PosJSON
    name: str
    value: str
    access: Optional[Literal["r", "w", "rw"]]
    """Read/write access; absent when not declared."""
    slotIndices: Optional[List[int]]
    """Applicable slot indices; absent when not declared."""
    defaultValue: Optional[str]
    """Default value; absent when not declared."""
    desc: Optional[DescriptionJSON]
    """Description; absent when not declared."""


class DeviceAnnotationDeviceHash(TypedDict):
    """JSON of an ``@device-hash`` entry (value-only)."""
    nodeName: Literal["DeviceAnnotationDeviceHash"]
    position: PosJSON
    end: PosJSON
    value: str


class DeviceAnnotationNameHash(TypedDict):
    """JSON of a ``@name-hash`` entry (value-only)."""
    nodeName: Literal["DeviceAnnotationNameHash"]
    position: PosJSON
    end: PosJSON
    value: str


class DeviceAnnotationReagentHash(TypedDict):
    """JSON of a ``@reagent-hash`` entry (value-only)."""
    nodeName: Literal["DeviceAnnotationReagentHash"]
    position: PosJSON
    end: PosJSON
    value: str


class DeviceAnnotationJSON(TypedDict):
    """JSON of a ``@device`` type annotation.

    @note Optional keys are absent from the JSON when not declared; the four
          array keys are always present and are ``[]`` when empty.
    """
    nodeName: Literal["DeviceAnnotation"]
    position: PosJSON
    end: PosJSON
    name: str
    desc: Optional[DescriptionJSON]
    """Description; absent when not declared."""
    deviceHash: Optional[DeviceAnnotationDeviceHash]
    """Device hash; absent when not declared."""
    nameHash: Optional[DeviceAnnotationNameHash]
    """Name hash; absent when not declared."""
    logics: List[DeviceAnnotationLogic]
    logicSlots: List[DeviceAnnotationLogicSlot]
    reagentHashes: List[DeviceAnnotationReagentHash]
    slots: List[DeviceAnnotationSlot]


class EnumAnnotationValueJSON(TypedDict):
    """JSON of one ``@value`` line of an ``@enum`` annotation."""
    nodeName: Literal["EnumAnnotationValue"]
    position: PosJSON
    end: PosJSON
    name: str
    value: str
    desc: Optional[DescriptionJSON]
    """Description; absent when not declared."""
    tag: Literal["value"]


class EnumAnnotationJSON(TypedDict):
    """JSON of an ``@enum`` type annotation.

    @note ``desc`` is absent from the JSON when not declared.
    """
    nodeName: Literal["EnumAnnotation"]
    position: PosJSON
    end: PosJSON
    name: str
    desc: Optional[DescriptionJSON]
    values: List[EnumAnnotationValueJSON]


TypeTableMap = Dict[str, Union[DeviceAnnotationJSON, EnumAnnotationJSON]]
"""Type table JSON dictionary.

``TypeTable.toJSON()`` is this flat map itself: annotation name -> annotation node.
"""


class SymbolTableJSON(TypedDict):
    """Two-key envelope emitted by ``SymbolTable.toJSON()``."""
    symbols: Dict[str, "Symbol"]
    builtinSymbols: Dict[str, "Symbol"]


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

    @note Instruction keywords are no longer members of this enumeration; they are
          provided separately by :class:`InstructionKeyword`.
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
    DOT
    SUB
    DIV
    # 注释
    HEX_COMMENT
    SLASH_COMMENT
    # 换行
    NEWLINE
    # 关键字
    KEYWORD
    KEYWORD_HASH
    KEYWORD_STR
    KEYWORD_ALIAS
    KEYWORD_DEFINE
    # 文件结束标记
    END
    # 未知标记
    UNKNOWN
    # 类型注解前缀与标签
    TYPE_HINT_PREFIX
    TYPE_ANNOTATION_PREFIX
    TAG


class InstructionKeyword(enum.IntEnum):
    """IC10 instruction keyword enumeration.

    Auto-synced with C++ ``ic10::InstructionKeyword`` via pybind11 ``py::enum_``.
    Values are contiguous 0..146 in declaration order.

    @note The C++ declaration guards some members with
          ``#ifndef STATIONEERS_SIMPLE_DEBUG_MODE``; that macro is defined nowhere in
          the tree, so every member below is exported.
    """
    # 空指令
    HCF = 0
    YIELD
    # 一元指令
    PEEK
    POP
    PUSH
    CLR
    J
    JAL
    JR
    RAND
    SLEEP
    CLRD
    # 二元指令
    ABS
    ACOS
    ASIN
    ATAN
    ATAN2
    CEIL
    COS
    DIV
    EXP
    FLOOR
    LOG
    MAX
    MIN
    MOD
    MUL
    POW
    ROUND
    SIN
    SQRT
    SGN
    SUB
    TAN
    TRUNC
    NOT
    MOVE
    POKE
    BEQZ
    BEQZAL
    BNEZ
    BNEZAL
    BGEZ
    BGEZAL
    BGTZ
    BGTZAL
    BLEZ
    BLEZAL
    BLTZ
    BLTZAL
    BNAN
    BDNS
    BDNSAL
    BDSE
    BDSEAL
    BREQZ
    BRGEZ
    BRGTZ
    BRLEZ
    BRLTZ
    BRNAN
    BRNEZ
    BRDNS
    BRDSE
    SEQZ
    SNEZ
    SGEZ
    SGTZ
    SLEZ
    SLTZ
    SNAN
    SNANZ
    SDNS
    SDSE
    # 三元指令
    ADD
    AND
    NOR
    OR
    SLA
    SLL
    SRA
    SRL
    XOR
    GET
    PUT
    L
    LS
    LR
    S
    SB
    ROL
    ROR
    RMAP
    BEQ
    BEQAL
    BNE
    BNEAL
    BGE
    BGEAL
    BGT
    BGTAL
    BLE
    BLEAL
    BLT
    BLTAL
    BAPZ
    BAPZAL
    BNAZ
    BNAZAL
    BDNVL
    BDNVS
    BREQ
    BRNE
    BRGE
    BRGT
    BRLE
    BRLT
    BRAPZ
    BRNAZ
    SAPZ
    SNAZ
    SEQ
    SNE
    SGE
    SGT
    SLE
    SLT
    # 四元指令
    CLAMP
    LERP
    EXT
    INS
    SS
    LB
    SBN
    SBS
    BAP
    BAPAL
    BNA
    BNAAL
    BRAP
    BRNA
    SAP
    SNA
    SELECT
    # 五元指令
    LBN
    LBS
    # 六元指令
    LBNS


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
    REG_TARGET = 0
    """Register or identifier (the target of an operation)."""
    REG_OR_DEV
    """Register or device (``alias`` directive only)."""
    NUM_VALUE
    """Number, register, identifier, or enum."""
    JUMP_LINE
    """Jump target: number, register, or label."""
    ADDRESS
    """Memory address (same candidate set as ``NUM_VALUE``)."""
    SLOT_IDX
    """Slot index (same candidate set as ``NUM_VALUE``)."""
    HARDWARE_ID
    """Hardware/device hash (same candidate set as ``NUM_VALUE``)."""
    REAGENT_HASH
    """Reagent hash (same candidate set as ``NUM_VALUE``)."""
    DEVICE_REF
    """Device or alias reference."""
    DEVICE_REF_STRICT
    """Device reference without alias."""
    LOGIC_PROP
    """Logic property name or hash."""
    LOGIC_SLOT_PROP
    """Slot logic property name or hash."""
    AGG_MODE
    """Aggregate mode."""
    REAGENT_MODE
    """Reagent mode."""
    DEVICE_HASH
    """Device hash (number, register, identifier, or HASH macro)."""
    NAME_HASH
    """Device name hash (number, register, identifier, or HASH macro)."""
    CONST_NUM
    """Constant value of a ``define`` directive."""


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
    """Symbol information from SymbolTable.toJSON().

    @note ``typeName``, ``value`` and ``desc`` are absent when unset;
          ``builtin`` is always present.
    """

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
    desc: Optional[DescriptionJSON]
    """Optional description, as a nested AST node object."""
    builtin: bool
    """Whether the symbol is a built-in symbol."""


# ============================================================================
# IC10Local - Localization
# ============================================================================

class IC10CompilerLocal:
    """IC10 compiler localization settings.

    @note Not constructible from Python (the binding registers no ``py::init``).
    """

    @staticmethod
    def setLanguage(code: str) -> None:
        """Set the compiler language locale.

        @param code: Language code, either 'en-us' or 'zh-hans'
        """
        ...


# ============================================================================
# Pos - Position
# ============================================================================

class Pos:
    """Position in source code (line, column, offset).

    @note ``line``, ``column`` and ``offset`` are read-only in the binding.
    """

    @property
    def line(self) -> int:
        """Line number (read-only, 1-based)."""
        ...

    @property
    def column(self) -> int:
        """Column number (read-only, 1-based)."""
        ...

    @property
    def offset(self) -> int:
        """Byte offset (read-only, 0-based)."""
        ...

    def __init__(self) -> None: ...

    def newline(self) -> None:
        """Advance to the next line (line++, column=1, offset++)."""
        ...

    def next(self, byte: int = ...) -> None:
        """Advance one byte; the column grows only for UTF-8 leading bytes.

        @param byte: Current byte value (default 0x00, i.e. ASCII)
        """
        ...

    def move(self, charOffset: int, byteOffset: int) -> None:
        """Move by the given character and byte distances.

        @param charOffset: Character distance added to the column
        @param byteOffset: Byte distance added to the offset
        """
        ...

    def __repr__(self) -> str: ...


# ============================================================================
# Token
# ============================================================================

class Token:
    """Lexical token from IC10 source code."""

    type: TokenType
    pos: Pos
    lexeme: str
    category: TokenCategory

    def __init__(self) -> None:
        """Create a default token (type ``UNKNOWN``, category ``INVALID``)."""
        ...

    def __init__(
        self,
        type: TokenType,
        pos: Pos,
        lexeme: Optional[str] = ...,
        category: Optional[TokenCategory] = ...,
    ) -> None:
        """Create a token from the given parts.

        @param type: Token type
        @param pos: Token start position
        @param lexeme: Raw token text (default: empty string)
        @param category: Token category (default: ``INVALID``)
        """
        ...

    @property
    def keyword(self) -> Optional[int]:
        """Instruction keyword as a plain ``int``, or None when not a keyword.

        @note The binding casts the raw ``InstructionKeyword`` value to ``int``
              rather than returning an :class:`InstructionKeyword` member.
        """
        ...

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

    def __init__(self) -> None:
        """Create a lexer without a source string."""
        ...

    def __init__(self, src: str, debug: bool = ...) -> None:
        """Create a lexer for the given source code.

        @param src: IC10 source code string
        @param debug: If True, preserve comment tokens
        """
        ...

    @staticmethod
    def tokenize(src: str, debug: bool = ...) -> List[Token]:
        """Tokenize source code (static convenience method).

        @param src: IC10 source code string
        @param debug: If True, preserve comment tokens
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

    nodeName: str
    """Static node name; always 'Program' (read-only, static)."""

    def __init__(self) -> None: ...

    @property
    def statements(self) -> List[Dict[str, Any]]:
        """List of statement nodes as dicts (read-only, parsed from JSON)."""
        ...

    @property
    def end(self) -> Pos:
        """Get the program end position (read-only)."""
        ...

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

    def __init__(self) -> None:
        """Create a parser without tokens."""
        ...

    def __init__(self, tokens: List[Token], debug: bool = ...) -> None:
        """Create a parser from a list of tokens.

        @param tokens: List of tokens from the lexer
        @param debug: Enable debug output
        """
        ...

    @staticmethod
    def parsing(tokens: List[Token], debug: bool = ...) -> Program:
        """Parse tokens into a Program (static convenience method).

        @param tokens: List of tokens
        @param debug: Enable debug output
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
        """Serialize symbol table to JSON string.

        @return: JSON of the SymbolTableJSON envelope
        """
        ...


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

    @note Not constructible from Python (the binding registers no ``py::init``);
          instances come from ``Linker.units``.

    @ivar path: Source file path (read-only)
    @ivar diagnostics: List of diagnostics for this unit (read-only)
    """

    @property
    def path(self) -> str:
        """Source file path (read-only)."""
        ...

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

    @property
    def type_table(self) -> TypeTable:
        """Get the global type table merged during linking."""
        ...


# ---------------------------------------------------------------------------
#  增量编译相关
# ---------------------------------------------------------------------------

class IncLexerResult:
    """Result of an incremental lexing pass.

    @note Every field is read-only in the binding; the count fields are C++
          ``std::size_t``.
    """

    @property
    def tokens(self) -> List[Token]: ...

    @property
    def incremental(self) -> bool: ...

    @property
    def relexedLines(self) -> int: ...

    @property
    def changedStartLine(self) -> int: ...

    @property
    def oldChangedEndLine(self) -> int: ...

    @property
    def newChangedEndLine(self) -> int: ...

    def __init__(self) -> None: ...

class IncLexer:
    def __init__(self) -> None: ...
    def tokenizeFull(self, source: str) -> IncLexerResult: ...
    def tokenizeInc(self, newSource: str) -> IncLexerResult: ...
    def hasCache(self) -> bool: ...
    def clear(self) -> None: ...

class IncParserResult:
    """Result of an incremental parsing pass.

    @note Every field is read-only in the binding; the count fields are C++
          ``std::size_t``.
    """

    @property
    def ast(self) -> Program: ...

    @property
    def incremental(self) -> bool: ...

    @property
    def reparsedStmts(self) -> int: ...

    @property
    def affectedStmtStart(self) -> int: ...

    def __init__(self) -> None: ...

class IncParser:
    def __init__(self) -> None: ...
    def parseFull(self, tokens: List[Token]) -> IncParserResult: ...
    def parseInc(self, tokens: List[Token], changedStartLine: int) -> IncParserResult: ...
    def hasCache(self) -> bool: ...
    def clear(self) -> None: ...

class IncCompileResult:
    """Result of an incremental compilation pass.

    @note Every field is read-only in the binding; the count fields are C++
          ``std::size_t``.
    """

    @property
    def tokens(self) -> List[Token]: ...

    @property
    def ast(self) -> Program: ...

    @property
    def incremental(self) -> bool: ...

    @property
    def relexedLines(self) -> int: ...

    @property
    def reparsedStmts(self) -> int: ...

    def __init__(self) -> None: ...

class IncCompiler:
    def __init__(self) -> None: ...
    def compileFull(self, source: str) -> IncCompileResult: ...
    def compileInc(self, source: str) -> IncCompileResult: ...
    def hasCache(self) -> bool: ...
    def clear(self) -> None: ...
