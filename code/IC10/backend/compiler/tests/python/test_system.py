# Copyright (c) 2026. All rights reserved.
# This source code is licensed under the CC BY-NC-SA
# (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
# This software is protected by copyright law. Reproduction, distribution, or use for commercial
# purposes is prohibited without the author's permission. If you have any questions or require
# permission, please contact the author: edocsitahw@qq.com

"""
@file test_system.py
@brief IC10 Python 绑定 - 系统测试
@details 端到端的 IC10 程序编译系统测试，覆盖真实场景程序：
          - 气闸控制程序
          - 气体过滤程序
          - 简单循环程序
          - 所有指令元数覆盖
          - 所有预处理指令覆盖
          - 大型程序编译
"""

import os
import sys
import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "publish", "python"))

from ic10c_python import (
    IC10Local,
    Lexer,
    Parser,
    Analyser,
    IncCompiler,
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


# ============================================================
# 真实 IC10 程序测试
# ============================================================

class TestRealPrograms:
    """Tests with real-world IC10 programs."""

    def test_airlock_control_program(self):
        """气闸控制程序"""
        source = "\n".join([
            "# Airlock Control Program",
            "alias ic0 d0",
            "alias ic1 d1",
            "alias status r0",
            "alias on_val r2",
            "# Initialize",
            "start:",
            "l r1 ic0 Setting",
            "move on_val 1",
            "s ic1 On on_val",
            "yield",
            "j start",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) >= 7

    def test_gas_filter_program(self):
        """气体过滤程序"""
        source = "\n".join([
            "# Gas Filter Program",
            "alias device d0",
            "alias pressure r0",
            "alias target r1",
            "alias on_val r3",
            "",
            "main:",
            "l pressure device Pressure",
            "move target 100",
            "move on_val 1",
            "slt r2 pressure target",
            "beqz r2 skip",
            "s device On on_val",
            "skip:",
            "yield",
            "j main",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) >= 9

    def test_simple_loop_program(self):
        """简单循环程序"""
        source = "\n".join([
            "# Simple counting loop",
            "alias counter r0",
            "define MAX 100",
            "",
            "init:",
            "move counter 0",
            "",
            "loop:",
            "add counter counter 1",
            "yield",
            "blt counter MAX loop",
            "",
            "done:",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) >= 6

    def test_define_with_hash_macro(self):
        """使用 HASH 宏的 define 指令"""
        source = "\n".join([
            'define PRESSURE HASH("Pressure")',
            'define DEVICE HASH("Device0")',
            "alias ic d0",
            "l r0 ic PRESSURE",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 5


# ============================================================
# 所有指令元数覆盖测试
# ============================================================

class TestAllInstructionArities:
    """Tests covering all instruction arities."""

    def test_all_arities_compile(self):
        """所有元数指令都能编译通过"""
        source = "\n".join([
            "# 零元",
            "hcf",
            "yield",
            "",
            "# 一元",
            "sleep 100",
            "j target",
            "target:",
            "",
            "# 二元",
            "move r0 0",
            "abs r0 r1",
            "sgn r0 r1",
            "",
            "# 三元",
            "add r0 r1 r2",
            "sub r0 r1 r2",
            "mul r0 r1 r2",
            "rol r0 r1 r2",
            "ror r0 r1 r2",
            "",
            "# 四元",
            "lerp r0 r1 r2 r3",
            "clamp r0 r1 r2 r3",
            "",
            "# 五元",
            "lbn r0 100 200 2 Average",
            "lbs r0 100 3 Quantity Average",
            "",
            "# 六元",
            "lbns r0 100 200 3 Quantity Average",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        # 期望语句数 >= 18（含 label）
        assert len(program.statements) >= 18


# ============================================================
# 所有预处理指令覆盖测试
# ============================================================

class TestAllPreprocessorDirectives:
    """Tests covering all preprocessor directives."""

    def test_all_directives(self):
        """所有预处理指令"""
        source = "\n".join([
            "# alias 设备别名",
            "alias ic d0",
            "alias pump d1",
            "",
            "# alias 寄存器别名",
            "alias temp r0",
            "alias counter r1",
            "",
            "# define 数值常量",
            "define MAX 100",
            "define MIN 0",
            "define HEX_VAL $FF",
            "define BIN_VAL %1010",
            "",
            "# define HASH 宏",
            'define PRESSURE HASH("Pressure")',
            'define TEMPERATURE HASH("Temperature")',
            "",
            "# define STR 宏",
            'define LABEL STR("main_loop")',
            "",
            "# 标签",
            "main:",
            "loop:",
            "",
            "yield",
            "j main",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        # alias(4) + define(7) + label(2) + yield(1) + j(1) = 15
        assert len(program.statements) >= 15


# ============================================================
# 大型程序测试
# ============================================================

class TestLargeProgram:
    """Tests with large programs."""

    def test_hundred_line_program(self):
        """100+ 行程序编译测试"""
        lines = [
            "# Large program test",
            "alias ic d0",
            "alias counter r0",
            "alias temp r1",
            "define LIMIT 50",
            "",
            "init:",
            "move counter 0",
            "",
        ]

        for i in range(40):
            lines.append(f"add counter counter {i % 5}")
            lines.append("yield")
            if i % 10 == 0:
                lines.append(f"# checkpoint {i}")

        lines.extend([
            "",
            "done:",
            "hcf",
        ])

        source = "\n".join(lines) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) >= 80

    def test_incremental_large_program(self):
        """大型程序增量编译"""
        lines = []
        for i in range(30):
            lines.append(f"move r{i % 16} {i}")
            lines.append(f"add r0 r0 r{i % 16}")

        source_v1 = "\n".join(lines) + "\n"
        source_v2 = source_v1.replace("move r0 0", "move r0 42")

        compiler = IncCompiler()
        result_v1 = compiler.compileFull(source_v1)

        assert len(result_v1.tokens) > 0
        assert result_v1.incremental is False

        result_v2 = compiler.compileInc(source_v2)

        assert len(result_v2.tokens) > 0
        assert compiler.hasCache() is True


# ============================================================
# 常量使用测试
# ============================================================

class TestConstantsUsedInInstructions:
    """Tests with constants used in instructions."""

    def test_math_constants(self):
        """数学常量在指令中使用"""
        source = "\n".join([
            "move r0 pi",
            "move r1 tau",
            "move r2 nan",
            "move r3 pinf",
            "move r4 ninf",
            "move r5 rgas",
            "hcf",
        ]) + "\n"

        # 只验证解析器不产生语法错误
        tokens = Lexer.tokenize(source)
        parser = Parser(tokens)
        parser.parse()

        assert len(parser.diagnostics) == 0


# ============================================================
# 设备交互程序测试
# ============================================================

class TestDeviceInteraction:
    """Tests for device interaction programs."""

    def test_load_store_device(self):
        """设备加载/存储程序"""
        source = "\n".join([
            "alias ic d0",
            "alias sensor d1",
            "alias valve d2",
            "",
            "main:",
            "l r0 sensor Pressure",
            "l r1 sensor Temperature",
            "s valve Setting r0",
            "s valve On r1",
            "yield",
            "j main",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) >= 8

    def test_batch_operations(self):
        """批量操作程序"""
        source = "\n".join([
            "alias mode r0",
            "",
            "main:",
            "lb r0 100 2 Average",
            "sb 100 Setting r0",
            "sbn 100 200 0 r0",
            "yield",
            "j main",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) >= 5


# ============================================================
# 文档注释与类型提示测试
# ============================================================

class TestDocCommentsAndTypeHints:
    """Doc comment and type hint tests in system context."""

    def test_program_with_device_doc_comment(self):
        """包含设备文档注释的程序"""
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @desc 炉窑设备",
            "#> @end-device",
            "alias furnace d0 #: @type Furnace",
            "main:",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 4

    def test_program_with_enum_doc_comment(self):
        """包含枚举文档注释的程序"""
        source = "\n".join([
            "#> @enum",
            "#> @name GasType",
            "#> @value Oxygen 1 氧气",
            "#> @value Nitrogen 2 氮气",
            "#> @end-enum",
            "main:",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 3

    def test_program_with_mixed_doc_comments(self):
        """混合文档注释和代码的程序"""
        source = "\n".join([
            "#> @device",
            "#> @name Pump",
            "#> @desc 液体泵",
            "#> @end-device",
            "",
            "#> @device",
            "#> @name Sensor",
            "#> @desc 压力传感器",
            "#> @end-device",
            "",
            "alias pump d0 #: @type Pump",
            "alias sensor d1 #: @type Sensor",
            "main:",
            "l r0 sensor Pressure",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 7

    def test_alias_with_type_hint(self):
        """带类型提示的别名定义"""
        source = "\n".join([
            "#> @device",
            "#> @name Furnace",
            "#> @logic Pressure r",
            "#> @end-device",
            "alias myDevice d0 #: @type Furnace",
            "alias myReg r0",
            "main:",
            "l r0 myDevice Pressure",
            "hcf",
        ]) + "\n"

        program, parser, analyser = compile_program(source)

        assert len(parser.diagnostics) == 0
        assert len(program.statements) == 6
