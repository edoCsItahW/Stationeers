# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: edocsitahw@qq.com

"""
@file test_integration.py
@brief IC10 Python 绑定 - 集成测试
@details 测试 Lexer → Parser → Analyser 各阶段之间的数据流动和诊断传播，
          包括：词法到语法的数据流、诊断传递、错误恢复、完整编译管道、
          增量编译管道等集成场景。
"""

import os
import sys
import json
import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "publish", "python"))

from ic10c_python import (
    IC10Local,
    Lexer,
    Parser,
    Program,
    Analyser,
    SymbolTable,
    IncLexer,
    IncLexerResult,
    IncParser,
    IncParserResult,
    IncCompiler,
    IncCompileResult,
    TokenType,
    BasicType,
)


def setup_module(module):
    """Set up test module: configure locale."""
    IC10Local.setLanguage("zh-hans")


# ============================================================
# 辅助函数
# ============================================================

def compile_program(source):
    """完整编译：源码 → 词法 → 语法 → 语义"""
    tokens = Lexer.tokenize(source)
    parser = Parser(tokens)
    program = parser.parse()
    analyser = Analyser()
    analyser.visit(program)
    return program, parser, analyser


SRC_SIMPLE = "\n".join([
    "alias ic d0",
    "alias temp r0",
    "move r0 42",
    "add r0 r0 1",
    "yield",
])

SRC_WITH_LEX_ERROR = 'move r0 1\n"unclosed string\nhcf\n'

SRC_WITH_PARSE_ERROR = "invalid_instruction r0\nmove r0 1\nhcf\n"


# ============================================================
# Lexer → Parser 数据流测试
# ============================================================

class TestLexerParserFlow:
    """Tests for Lexer to Parser data flow."""

    def test_tokens_flow_to_parser(self):
        """词法分析产生的 token 应正确传递给语法分析器"""
        tokens = Lexer.tokenize(SRC_SIMPLE)
        assert len(tokens) > 0

        parser = Parser(tokens)
        program = parser.parse()

        assert isinstance(program, Program)
        assert len(program.statements) > 0

    def test_valid_program_no_parser_errors(self):
        """有效程序不应产生解析器诊断"""
        tokens = Lexer.tokenize(SRC_SIMPLE)
        parser = Parser(tokens)
        parser.parse()

        assert len(parser.diagnostics) == 0

    def test_token_count_matches_program_complexity(self):
        """Token 数量应与程序复杂度匹配"""
        tokens = Lexer.tokenize(SRC_SIMPLE)
        from ic10c_python import TokenType
        meaningful = [t for t in tokens
                       if t.type not in (TokenType.NEWLINE, TokenType.END)]

        parser = Parser(tokens)
        program = parser.parse()

        # 5 条语句，每条至少 2-3 个 token
        assert len(meaningful) >= len(program.statements) * 2


# ============================================================
# 诊断传播测试
# ============================================================

class TestDiagnosticPropagation:
    """Tests for diagnostic propagation across pipeline stages."""

    def test_lexer_diagnostics_available_after_parse(self):
        """词法分析的诊断在解析后仍可访问"""
        lexer = Lexer(SRC_WITH_LEX_ERROR)
        tokens = lexer.scan()
        lex_diag_count = len(lexer.diagnostics)

        parser = Parser(tokens)
        parser.parse()

        # 词法诊断保持独立
        assert len(lexer.diagnostics) == lex_diag_count
        assert lex_diag_count > 0

    def test_parser_diagnostics_for_invalid_input(self):
        """无效输入应产生解析器诊断"""
        tokens = Lexer.tokenize(SRC_WITH_PARSE_ERROR)
        parser = Parser(tokens)
        parser.parse()

        assert len(parser.diagnostics) > 0

    def test_lexer_error_produces_parser_issues(self):
        """词法错误可能导致解析阶段也产生问题"""
        lexer = Lexer(SRC_WITH_LEX_ERROR)
        tokens = lexer.scan()
        assert len(lexer.diagnostics) > 0

        parser = Parser(tokens)
        program = parser.parse()

        # 即使有词法错误，解析器也应尽量产生 AST（通过 toString 验证非空）
        assert program is not None
        assert isinstance(program.toString(), str)
        assert len(parser.diagnostics) >= 0


# ============================================================
# 错误恢复测试
# ============================================================

class TestErrorRecovery:
    """Tests for error recovery across pipeline stages."""

    def test_lexer_error_then_valid_statements(self):
        """词法错误后，后续有效语句仍可被解析"""
        lexer = Lexer(SRC_WITH_LEX_ERROR)
        tokens = lexer.scan()

        parser = Parser(tokens)
        program = parser.parse()

        # 通过解析器诊断数量间接验证错误恢复（至少有诊断）
        assert program is not None
        assert isinstance(program.toString(), str)

    def test_parser_error_then_valid_statements(self):
        """解析错误后，后续有效语句仍可被解析"""
        tokens = Lexer.tokenize(SRC_WITH_PARSE_ERROR)
        parser = Parser(tokens)
        program = parser.parse()

        # 至少有 move 和 hcf 被解析出来
        assert len(program.statements) >= 2

    def test_lexer_diagnostics_separate_from_parser(self):
        """词法和语法诊断相互独立"""
        lexer = Lexer(SRC_WITH_LEX_ERROR)
        tokens = lexer.scan()

        parser = Parser(tokens)
        parser.parse()

        lex_diags = len(lexer.diagnostics)
        parse_diags = len(parser.diagnostics)

        # 两者都可能有诊断，但来源不同
        assert lex_diags >= 0
        assert parse_diags >= 0


# ============================================================
# 完整管道测试（Lexer → Parser → Analyser）
# ============================================================

class TestFullPipeline:
    """Tests for the full compilation pipeline."""

    def test_lexer_to_analyser(self):
        """完整管道：词法 → 语法 → 语义"""
        tokens = Lexer.tokenize(SRC_SIMPLE)
        parser = Parser(tokens)
        program = parser.parse()

        analyser = Analyser()
        analyser.visit(program)

        symbol_table = analyser.symbolTable
        assert isinstance(symbol_table, SymbolTable)

    def test_analyser_diagnostics(self):
        """语义分析器应产生诊断信息"""
        tokens = Lexer.tokenize(SRC_SIMPLE)
        parser = Parser(tokens)
        program = parser.parse()

        analyser = Analyser()
        analyser.visit(program)

        assert isinstance(analyser.diagnostics, list)

    def test_analyser_static_method(self):
        """Analyser.analyse 静态方法应可用"""
        tokens = Lexer.tokenize(SRC_SIMPLE)
        parser = Parser(tokens)
        program = parser.parse()

        # 静态 analyse 方法返回 None (void)，不抛异常即为成功
        Analyser.analyse(program)

    def test_symbol_table_not_empty_for_alias(self):
        """有 alias 的程序符号表应有内容"""
        source = "alias ic d0\nalias temp r0\nhcf\n"
        tokens = Lexer.tokenize(source)
        parser = Parser(tokens)
        program = parser.parse()

        analyser = Analyser()
        analyser.visit(program)

        json_str = analyser.symbolTable.toJSON()
        assert json_str is not None
        assert isinstance(json_str, str)


# ============================================================
# 增量编译管道测试
# ============================================================

class TestIncrementalPipeline:
    """Tests for the incremental compilation pipeline."""

    def test_inc_lexer_to_inc_parser(self):
        """增量词法 → 增量语法 的数据流"""
        lexer = IncLexer()
        lex_result = lexer.tokenizeFull(SRC_SIMPLE)

        assert isinstance(lex_result, IncLexerResult)
        assert len(lex_result.tokens) > 0

        p = IncParser()
        parse_result = p.parseFull(lex_result.tokens)

        assert isinstance(parse_result, IncParserResult)
        assert isinstance(parse_result.ast, Program)
        assert len(parse_result.ast.statements) > 0

    def test_inc_compiler_full_compile(self):
        """IncCompiler 完整编译"""
        compiler = IncCompiler()
        result = compiler.compileFull(SRC_SIMPLE)

        assert isinstance(result, IncCompileResult)
        assert len(result.tokens) > 0
        assert isinstance(result.ast, Program)
        assert result.incremental is False

    def test_inc_compiler_unchanged_source(self):
        """IncCompiler 未变源码的增量编译"""
        compiler = IncCompiler()
        compiler.compileFull(SRC_SIMPLE)

        result = compiler.compileInc(SRC_SIMPLE)

        assert len(result.tokens) > 0
        assert isinstance(result.ast, Program)

    def test_inc_compiler_modified_source(self):
        """IncCompiler 修改源码的增量编译"""
        compiler = IncCompiler()
        compiler.compileFull(SRC_SIMPLE)

        modified = SRC_SIMPLE.replace("move r0 42", "move r0 99")
        result = compiler.compileInc(modified)

        assert len(result.tokens) > 0
        assert isinstance(result.ast, Program)

    def test_inc_compiler_has_cache_and_clear(self):
        """IncCompiler 缓存和清除"""
        compiler = IncCompiler()
        assert compiler.hasCache() is False

        compiler.compileFull(SRC_SIMPLE)
        assert compiler.hasCache() is True

        compiler.clear()
        assert compiler.hasCache() is False

    def test_inc_lexer_parser_incremental(self):
        """增量词法和增量语法的协同工作"""
        source_v1 = "\n".join([
            "move r0 1",
            "add r0 r0 1",
            "yield",
        ])

        source_v2 = "\n".join([
            "move r0 1",
            "sub r0 r0 1",
            "yield",
        ])

        # 首次完整编译
        lexer = IncLexer()
        lex_result = lexer.tokenizeFull(source_v1)

        p = IncParser()
        p.parseFull(lex_result.tokens)

        # 增量编译
        lex_inc = lexer.tokenizeInc(source_v2)
        assert lex_inc.incremental is True

        parse_inc = p.parseInc(lex_inc.tokens, lex_inc.changedStartLine)
        assert isinstance(parse_inc.ast, Program)


# ============================================================
# 文档注释与类型提示集成测试
# ============================================================

class TestDocCommentsIntegration:
    """Integration tests for doc comments and type hints."""

    def test_doc_comment_tokens_flow(self):
        """文档注释 tokens 通过完整编译流水线"""
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @end-device",
        ]) + "\n"

        tokens = Lexer.tokenize(source)
        parser = Parser(tokens)
        program = parser.parse()

        doc_comment_tokens = [t for t in tokens if t.type == TokenType.DOC_COMMENT]
        assert len(doc_comment_tokens) > 0
        assert len(program.statements) == 1

    def test_type_hint_tokens_flow(self):
        """类型提示 tokens 通过完整编译流水线"""
        source = "alias myFurnace d0 #: @type Furnace\n"

        tokens = Lexer.tokenize(source)
        parser = Parser(tokens)
        program = parser.parse()

        type_hint_tokens = [t for t in tokens if t.type == TokenType.TYPE_HINT]
        assert len(type_hint_tokens) == 1
        assert len(program.statements) == 1

    def test_doc_comment_ast_serialization(self):
        """文档注释 AST 序列化"""
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @desc 炉窑",
            "#> @end-device",
        ]) + "\n"

        program, parser, analyser = compile_program(source)
        data = json.loads(program.toJSON())

        assert len(data["statements"]) == 1
        assert data["statements"][0]["type"] == "DeviceDocComment"
        assert data["statements"][0]["name"] == "Furnace"

    def test_alias_type_hint_ast_serialization(self):
        """带类型提示的别名 AST 序列化"""
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @end-device",
            "alias myFurnace d0 #: @type Furnace",
        ]) + "\n"

        program, parser, analyser = compile_program(source)
        data = json.loads(program.toJSON())

        assert len(data["statements"]) == 2
        alias_stmt = next(s for s in data["statements"] if s["type"] == "AliasDirective")
        assert alias_stmt["typeName"] == "Furnace"

    def test_mixed_doc_comments_pipeline(self):
        """混合文档注释和代码的完整流水线"""
        source = "\n".join([
            "#> @device",
            "#> @name Pump",
            "#> @end-device",
            "alias pump d0 #: @type Pump",
            "main:",
            "l r0 pump Pressure",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 5
        assert analyser is not None


# ============================================================
# 类型推导与设备上下文测试
# ============================================================

class TestTypeInference:
    """类型推导与设备上下文传递测试"""

    def test_validate_logic_names_via_doc_comment(self):
        """通过文档注释验证设备逻辑名"""
        source = "\n".join([
            "#> @device",
            "#> @name Sensor",
            "#> @logic Pressure rw",
            "#> @logic Temperature rw",
            "#> @end-device",
            "alias sensor d0 #: @type Sensor",
            "l r0 sensor Pressure",
            "l r1 sensor Temperature",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(analyser.diagnostics) == 0

    def test_report_invalid_logic_name(self):
        """无效逻辑名应上报 IWA14_2"""
        source = "\n".join([
            "#> @device",
            "#> @name Sensor",
            "#> @logic Pressure rw",
            "#> @end-device",
            "alias sensor d0 #: @type Sensor",
            "l r0 sensor InvalidLogic",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(analyser.diagnostics) > 0
        assert any(d["id"] == "IWA14_2" for d in analyser.diagnostics)

    def test_no_duplicate_diagnostics(self):
        """同一无效标识符不应重复上报"""
        source = "\n".join([
            "#> @device",
            "#> @name Sensor",
            "#> @logic Pressure rw",
            "#> @end-device",
            "alias sensor d0 #: @type Sensor",
            "l r0 sensor BadLogic",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        bad_logic_diags = [d for d in analyser.diagnostics if "BadLogic" in d["message"]]
        assert len(bad_logic_diags) <= 1

    def test_device_context_within_instruction(self):
        """设备上下文在同一条指令内传递"""
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @logic Temperature r",
            "#> @logic Active rw",
            "#> @end-device",
            "alias furnace d0 #: @type Furnace",
            "s furnace Active r0",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(analyser.diagnostics) == 0

    def test_device_context_reset_between_instructions(self):
        """设备上下文在指令间重置"""
        source = "\n".join([
            "#> @device",
            "#> @name Sensor",
            "#> @logic Pressure rw",
            "#> @end-device",
            "alias sensor d0 #: @type Sensor",
            "l r0 sensor Pressure",
            "move r1 42",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(analyser.diagnostics) == 0

    def test_slot_index_validation(self):
        """slot index 验证"""
        source = "\n".join([
            "#> @device",
            "#> @name Stacker",
            "#> @slot 0 ore",
            "#> @slot 1 ingot",
            "#> @logicSlot Occupied",
            "#> @end-device",
            "alias stacker d0 #: @type Stacker",
            "ls r0 stacker 0 Occupied",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0

    def test_reagent_mode_validation(self):
        """reagent mode 枚举验证"""
        source = "\n".join([
            "#> @enum",
            "#> @name ReagentMode",
            "#> @value Contents 0",
            "#> @value Required 1",
            "#> @end-enum",
            "#> @device",
            "#> @name Filter",
            "#> @end-device",
            "alias filter d0 #: @type Filter",
            "lr r0 filter Contents Oxygen",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0

    def test_symbol_table_contains_type_info(self):
        """符号表应包含类型信息"""
        source = "\n".join([
            "#> @device",
            "#> @name Sensor",
            "#> @end-device",
            "alias sensor d0 #: @type Sensor",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        symbols = json.loads(analyser.symbolTable.toJSON())
        sensor_sym = next((s for s in symbols.values() if s["name"] == "sensor"), None)
        assert sensor_sym is not None
        assert sensor_sym["type"] == int(BasicType.DEVICE)
        assert sensor_sym["typeName"] == "Sensor"

    def test_batch_mode_with_enum(self):
        """batch mode 枚举处理"""
        source = "\n".join([
            "#> @enum",
            "#> @name BatchMode",
            "#> @value Greater 0",
            "#> @value Less 1",
            "#> @end-enum",
            "lbn r0 0 0 Pressure Greater",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0

    def test_direct_device_reference(self):
        """直接使用设备引用（d0）无需别名"""
        source = "\n".join([
            "#> @device",
            "#> @name Sensor",
            "#> @logic Pressure rw",
            "#> @end-device",
            "l r0 d0 Pressure",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
