# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: edocsitahw@qq.com

"""
@file test_parser.py
@brief IC10 Python 绑定 - 语法分析器测试
@details 测试 Parser 类的各项功能，包括：
          - 静态 parsing 方法
          - 实例 parse 方法
          - 各类指令解析（零元到六元）
          - 标签定义解析
          - 预处理指令解析（alias、define）
          - 宏调用解析（HASH、STR）
          - AST 序列化
          - 诊断报告
"""

import os
import sys
import json
import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python", "ic10"))

from ic10_python import (
    IC10Local,
    TokenType,
    Lexer,
    Parser,
    Program,
)


def setup_module(module):
    """Set up test module: configure locale."""
    IC10Local.setLanguage("zh-hans")


# ============================================================
# 辅助函数
# ============================================================

def parse(source):
    """快捷编译：源码 → AST"""
    tokens = Lexer.tokenize(source)
    parser = Parser(tokens)
    return parser.parse()


def parse_with_diags(source):
    """快捷编译并返回 (program, parser)"""
    tokens = Lexer.tokenize(source)
    parser = Parser(tokens)
    program = parser.parse()
    return program, parser


# ============================================================
# Parser 基础测试
# ============================================================

class TestParserBasics:
    """Basic parser tests."""

    def test_empty_source(self):
        program = parse("")

        assert isinstance(program, Program)
        assert len(program.statements) == 0

    def test_source_with_only_comments(self):
        program = parse("# comment\n// comment\n")

        assert len(program.statements) == 0

    def test_source_with_only_newlines(self):
        program = parse("\n\n\n")

        assert len(program.statements) == 0

    def test_valid_program_instance(self):
        program = parse("hcf\n")

        assert isinstance(program, Program)
        assert program.statements is not None
        assert len(program.statements) > 0

    def test_to_string(self):
        program = parse("hcf\n")

        s = program.toString()
        assert s
        assert isinstance(s, str)

    def test_to_json(self):
        program = parse("alias ic d0\nhcf\n")

        json_str = program.toJSON()
        assert json_str

        data = json.loads(json_str)
        assert data is not None
        assert "statements" in data


# ============================================================
# 预处理指令解析测试
# ============================================================

class TestPreprocessorDirectives:
    """Preprocessor directive parsing tests."""

    def test_alias_directive(self):
        program, parser = parse_with_diags("alias ic d0\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

    def test_define_directive_with_integer(self):
        program, parser = parse_with_diags("define MAX 42\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

    def test_define_directive_with_hex(self):
        program, parser = parse_with_diags("define VAL $FF\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

    def test_define_directive_with_hash_macro(self):
        source = 'define PUMP HASH("StructureLiquidVolumePump")\n'
        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

    def test_define_directive_with_str_macro(self):
        source = 'define LABEL STR("SomeHash")\n'
        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

    def test_multiple_alias_and_define(self):
        source = "\n".join([
            "alias devA d0",
            "alias devB d1",
            "define COUNT 10",
            "define HEX_VAL $FF",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 5


# ============================================================
# 文档注释与类型提示解析测试
# ============================================================

class TestDocCommentsAndTypeHints:
    """Doc comment and type hint parsing tests."""

    def test_device_doc_comment(self):
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @desc 炉窑",
            "#> @end-device",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DeviceDocComment"
        assert data["statements"][0]["name"] == "Furnace"
        assert data["statements"][0]["desc"] == "炉窑"

    def test_device_doc_comment_with_slots(self):
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @slot 0 inout 输入槽",
            "#> @slot 1 output 输出槽",
            "#> @end-device",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DeviceDocComment"
        assert len(data["statements"][0]["slots"]) == 2
        assert data["statements"][0]["slots"][0]["number"] == "0"
        assert data["statements"][0]["slots"][0]["direction"] == "inout"
        assert data["statements"][0]["slots"][0]["desc"] == "输入槽"
        assert data["statements"][0]["slots"][1]["number"] == "1"
        assert data["statements"][0]["slots"][1]["direction"] == "output"
        assert data["statements"][0]["slots"][1]["desc"] == "输出槽"

    def test_device_doc_comment_with_logics(self):
        source = "\n".join([
            "#> @device",
            "#> @name Sensor",
            "#> @logic Pressure rw",
            "#> @logic Temperature r",
            "#> @end-device",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DeviceDocComment"
        assert len(data["statements"][0]["logics"]) == 2
        assert data["statements"][0]["logics"][0]["name"] == "Pressure"
        assert data["statements"][0]["logics"][0]["access"] == "rw"
        assert data["statements"][0]["logics"][1]["name"] == "Temperature"
        assert data["statements"][0]["logics"][1]["access"] == "r"

    def test_device_doc_comment_with_modes(self):
        source = "\n".join([
            "#> @device",
            "#> @name Pump",
            "#> @mode 0 待机模式",
            "#> @mode 1 运行模式",
            "#> @end-device",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DeviceDocComment"
        assert len(data["statements"][0]["modes"]) == 2
        assert data["statements"][0]["modes"][0]["number"] == "0"
        assert data["statements"][0]["modes"][0]["desc"] == "待机模式"
        assert data["statements"][0]["modes"][1]["number"] == "1"
        assert data["statements"][0]["modes"][1]["desc"] == "运行模式"

    def test_device_doc_comment_with_logic_slots(self):
        source = "\n".join([
            "#> @device",
            "#> @name IC10",
            "#> @logicSlot db",
            "#> @logicSlot r0",
            "#> @end-device",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DeviceDocComment"
        assert len(data["statements"][0]["logicSlots"]) == 2
        assert data["statements"][0]["logicSlots"][0]["name"] == "db"
        assert data["statements"][0]["logicSlots"][1]["name"] == "r0"

    def test_device_doc_comment_with_connects(self):
        source = "\n".join([
            "#> @device",
            "#> @name Pipe",
            "#> @connect 0 入口",
            "#> @connect 1 出口",
            "#> @end-device",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DeviceDocComment"
        assert len(data["statements"][0]["connects"]) == 2
        assert data["statements"][0]["connects"][0]["number"] == "0"
        assert data["statements"][0]["connects"][0]["desc"] == "入口"
        assert data["statements"][0]["connects"][1]["number"] == "1"
        assert data["statements"][0]["connects"][1]["desc"] == "出口"

    def test_enum_doc_comment(self):
        source = "\n".join([
            "#> @enum",
            "#> @name GasType",
            "#> @value Oxygen 1 氧气",
            "#> @value Nitrogen 2 氮气",
            "#> @end-enum",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "EnumDocComment"
        assert data["statements"][0]["name"] == "GasType"
        assert len(data["statements"][0]["values"]) == 2

    def test_alias_with_type_hint(self):
        source = "alias myFurnace d0 #: @type Furnace\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "AliasDirective"
        assert data["statements"][0]["aliasType"] == "Furnace"

    def test_alias_without_type_hint(self):
        source = "alias myFurnace d0\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "AliasDirective"
        assert "aliasType" not in data["statements"][0]
        assert "aliasDesc" not in data["statements"][0]

    def test_alias_with_desc_type_hint(self):
        source = "alias myFurnace d0 #: @desc 炉窑设备\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "AliasDirective"
        assert "aliasType" not in data["statements"][0]
        assert data["statements"][0]["aliasDesc"] == "炉窑设备"

    def test_alias_with_type_and_desc(self):
        source = "alias myFurnace d0 #: @type Furnace @desc 炉窑\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "AliasDirective"
        assert data["statements"][0]["aliasType"] == "Furnace"
        assert data["statements"][0]["aliasDesc"] == "炉窑"

    def test_define_with_desc_type_hint(self):
        source = "define MAX 100 #: @desc 最大值\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DefineDirective"
        assert "defineType" not in data["statements"][0]
        assert data["statements"][0]["defineDesc"] == "最大值"

    def test_define_with_type_and_desc(self):
        source = "define PRESSURE 101325 #: @type Pressure @desc 标准大气压\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "DefineDirective"
        assert data["statements"][0]["defineType"] == "Pressure"
        assert data["statements"][0]["defineDesc"] == "标准大气压"

    def test_standalone_type_hint_error(self):
        source = "#: @type Furnace\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) > 0
        assert len(program.statements) == 1

        data = json.loads(program.toJSON())
        assert data["statements"][0]["type"] == "Error"

    def test_mixed_doc_comment_and_code(self):
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @end-device",
            "alias f d0",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2


# ============================================================
# 标签定义解析测试
# ============================================================

class TestLabelDefinitions:
    """Label definition parsing tests."""

    def test_simple_label(self):
        program, parser = parse_with_diags("main:\nhcf\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2

    def test_multiple_labels(self):
        source = "\n".join([
            "label1:",
            "hcf",
            "label2:",
            "yield",
            "label3:",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 6

    def test_unicode_label_names(self):
        program, parser = parse_with_diags("主循环:\nhcf\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2


# ============================================================
# 指令元数解析测试
# ============================================================

class TestInstructionArities:
    """Instruction arity parsing tests."""

    def test_nullary_instructions(self):
        source = "hcf\nyield\n"
        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2

    def test_unary_instructions(self):
        source = "\n".join([
            "sleep 100",
            "j main",
            "main:",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 4

    def test_binary_instructions(self):
        source = "\n".join([
            "move r0 0",
            "abs r0 r1",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 3

    def test_ternary_instructions(self):
        source = "\n".join([
            "add r0 r1 r2",
            "mul r0 r1 r2",
            "div r0 r1 r2",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 4

    def test_quaternary_instructions(self):
        source = "\n".join([
            "lerp r0 r1 r2 r3",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2

    def test_quinary_instructions(self):
        source = "\n".join([
            "lbn r0 100 200 2 Average",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2

    def test_senary_instructions(self):
        source = "\n".join([
            "lbns r0 100 200 3 Quantity Average",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2


# ============================================================
# 指令类型覆盖测试
# ============================================================

class TestInstructionTypes:
    """Instruction type coverage tests."""

    def test_load_store_instructions(self):
        source = "\n".join([
            "alias dev d0",
            "l r0 dev Pressure",
            "s dev Setting r0",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 4

    def test_branch_instructions(self):
        source = "\n".join([
            "start:",
            "beq r0 r1 end",
            "bge r0 r1 start",
            "blt r0 r1 start",
            "end:",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 6

    def test_jump_instructions(self):
        source = "\n".join([
            "j target",
            "jal target",
            "target:",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 4

    def test_arithmetic_instructions(self):
        source = "\n".join([
            "add r0 r1 r2",
            "sub r0 r1 r2",
            "mul r0 r1 r2",
            "div r0 r1 r2",
            "mod r0 r1 r2",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 6

    def test_math_function_instructions(self):
        source = "\n".join([
            "abs r0 r1",
            "ceil r0 r1",
            "floor r0 r1",
            "round r0 r1",
            "sqrt r0 r1",
            "sin r0 r1",
            "cos r0 r1",
            "tan r0 r1",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 9


# ============================================================
# 数字类型解析测试
# ============================================================

class TestNumberTypes:
    """Number type parsing tests."""

    def test_integer_operands(self):
        program, parser = parse_with_diags("move r0 42\nhcf\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2

    def test_float_operands(self):
        program, parser = parse_with_diags("move r0 3.14\nhcf\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2

    def test_hex_number_operands(self):
        program, parser = parse_with_diags("move r0 $FF\nhcf\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2

    def test_binary_number_operands(self):
        program, parser = parse_with_diags("move r0 %1010\nhcf\n")

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 2


# ============================================================
# 注释处理测试
# ============================================================

class TestComments:
    """Comment handling tests."""

    def test_hash_comments(self):
        source = "# header comment\nhcf  # inline comment\n"
        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

    def test_slash_comments(self):
        source = "// header comment\nhcf  // inline comment\n"
        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 1

    def test_mixed_comments(self):
        source = "\n".join([
            "# hash comment",
            "// slash comment",
            "alias foo r0  # inline hash",
            "define MAX 10  // inline slash",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 3


# ============================================================
# 常量引用测试
# ============================================================

class TestConstants:
    """Constant reference parsing tests."""

    def test_pi_and_tau_constants(self):
        source = "move r0 pi\nmove r1 tau\nhcf\n"
        _, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0

    def test_nan_pinf_ninf_constants(self):
        source = "move r0 nan\nmove r1 pinf\nmove r2 ninf\nhcf\n"
        _, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0

    def test_rgas_constant(self):
        source = "move r0 rgas\nhcf\n"
        _, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0


# ============================================================
# 设备引用测试
# ============================================================

class TestDeviceReferences:
    """Device reference parsing tests."""

    def test_d0_to_d5_in_instructions(self):
        source = "\n".join([
            "l r0 d0 Pressure",
            "l r1 d1 Temperature",
            "l r2 d2 Quantity",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 4

    def test_db_and_dn_device_references(self):
        source = "\n".join([
            "l r0 db Setting",
            "l r1 dn Setting",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 3


# ============================================================
# 寄存器边界测试
# ============================================================

class TestRegisterBoundaries:
    """Register boundary tests."""

    def test_r0_and_r15(self):
        source = "\n".join([
            "move r0 0",
            "move r15 0",
            "hcf",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 3


# ============================================================
# 错误恢复测试
# ============================================================

class TestErrorRecovery:
    """Error recovery tests."""

    def test_missing_newline_between_statements(self):
        source = "hcf yield\n"
        _, parser = parse_with_diags(source)

        assert len(parser.diagnostics) > 0

    def test_invalid_instruction(self):
        source = "invalid_instruction r0\n"
        _, parser = parse_with_diags(source)

        assert len(parser.diagnostics) > 0

    def test_continue_parsing_after_error(self):
        source = "invalid_instruction r0\nmove r0 1\nhcf\n"
        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) > 0
        assert len(program.statements) > 0


# ============================================================
# 多语句程序测试
# ============================================================

class TestMultiStatementPrograms:
    """Multi-statement program tests."""

    def test_counting_loop_program(self):
        source = "\n".join([
            "alias counter r0",
            "define LIMIT 10",
            "start:",
            "move r0 0",
            "loop:",
            "add r0 r0 1",
            "blt r0 LIMIT loop",
            "yield",
            "j start",
        ]) + "\n"

        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 9

    def test_large_program(self):
        lines = ["alias counter r0", "define LIMIT 100", "start:", "move r0 0"]
        for _ in range(50):
            lines.append("add r0 r0 1")
            lines.append("yield")
        lines.append("blt r0 LIMIT start")
        lines.append("hcf")

        source = "\n".join(lines) + "\n"
        program, parser = parse_with_diags(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) >= 100


# ============================================================
# 静态方法测试
# ============================================================

class TestParserParsingStatic:
    """Parser.parsing static method tests."""

    def test_same_result_as_instance_parse(self):
        source = "alias ic d0\nmove r0 1\nhcf\n"
        tokens = Lexer.tokenize(source)

        instance_parser = Parser(tokens)
        instance_program = instance_parser.parse()

        static_program = Parser.parsing(tokens)

        assert isinstance(static_program, Program)
        assert len(static_program.statements) == len(instance_program.statements)


# ============================================================
# AST 序列化测试
# ============================================================

class TestAstSerialization:
    """AST serialization tests."""

    def test_program_to_json_with_statements(self):
        source = "alias foo r0\ndefine MAX 10\nstart:\nhcf\n"
        program = parse(source)

        data = json.loads(program.toJSON())

        assert "statements" in data
        assert isinstance(data["statements"], list)
        assert len(data["statements"]) == 4

    def test_empty_program_to_json(self):
        program = parse("")

        data = json.loads(program.toJSON())

        assert "statements" in data
        assert len(data["statements"]) == 0

    def test_to_string_contains_info(self):
        program = parse("hcf\n")

        s = program.toString()
        assert s
        assert len(s) > 0
