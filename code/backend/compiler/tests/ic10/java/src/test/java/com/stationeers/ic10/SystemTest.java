// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package com.stationeers.ic10;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;

import static org.junit.jupiter.api.Assertions.*;

/**
 * System tests for the IC10 Java binding.
 * <p>
 * Mirrors the C++ {@code test_system.cpp} and Node.js {@code system.test.ts}
 * test suites at matching granularity. Validates end-to-end compilation of
 * real IC10 programs covering realistic scenarios: counting loops, airlock
 * control, gas filtering, complete instruction arities, all preprocessor
 * directives, error scenarios, boundary cases, forward references, HASH/STR
 * macros, device/register boundaries, number types, Unicode identifiers,
 * large programs, constants, comments, doc comments, type hints, and type
 * inference / semantic analysis.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("System tests")
class SystemTest {

    @BeforeAll
    static void setupLanguage() {
        IC10Local.setLanguage("zh-hans");
    }

    // ---- Helpers ----

    /** 编译结果载体：携带各阶段产物 / Compile result carrying pipeline outputs. */
    private static final class CompileResult {
        final Token[] tokens;
        final Program program;
        final Parser parser;
        final Analyser analyser;

        CompileResult(Token[] tokens, Program program, Parser parser, Analyser analyser) {
            this.tokens = tokens;
            this.program = program;
            this.parser = parser;
            this.analyser = analyser;
        }
    }

    /** 完整编译流水线：source → tokens → AST → 语义分析 / Full pipeline. */
    private static CompileResult compile(String source) {
        Token[] tokens = Lexer.tokenize(source, false);
        Parser parser = new Parser();
        parser.init(tokens, false);
        Program program = parser.parse();
        Analyser analyser = new Analyser();
        analyser.visit(program);
        return new CompileResult(tokens, program, parser, analyser);
    }

    /** 读取 grammarTest.ic fixture 文件 / Read grammarTest.ic fixture. */
    private static String readGrammarTestFile() throws IOException {
        String path = System.getProperty("grammarTest.path");
        assertNotNull(path, "grammarTest.path system property must be set");
        return new String(Files.readAllBytes(Paths.get(path)), StandardCharsets.UTF_8);
    }

    /** 判断诊断列表中是否包含指定 ID / Check if diagnostics contain the given ID. */
    private static boolean hasDiagnostic(Diagnostic[] diags, String id) {
        for (Diagnostic d : diags) {
            if (id.equals(d.id)) {
                return true;
            }
        }
        return false;
    }

    // ============================================================
    // 真实 IC10 程序测试
    // ============================================================

    @Nested
    @DisplayName("Real IC10 programs")
    class RealPrograms {

        @Test
        @DisplayName("compile counting loop program")
        void compileCountingLoopProgram() {
            String src = String.join("\n",
                    "alias counter r0",
                    "define LIMIT 10",
                    "start:",
                    "move r0 0",
                    "loop:",
                    "add r0 r0 1",
                    "blt r0 LIMIT loop",
                    "yield",
                    "j start"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics");
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("compile airlock control program")
        void compileAirlockControlProgram() {
            String src = String.join("\n",
                    "alias door d0",
                    "alias sensor d1",
                    "alias vent d2",
                    "define PRESSURE_MIN 10",
                    "define PRESSURE_MAX 50",
                    "define STATE_OPEN 1",
                    "define STATE_CLOSED 0",
                    "main:",
                    "l r0 sensor Pressure",
                    "blt r0 PRESSURE_MIN open_door",
                    "bgt r0 PRESSURE_MAX close_door",
                    "j main",
                    "open_door:",
                    "s door Open STATE_OPEN",
                    "s vent On STATE_CLOSED",
                    "j main",
                    "close_door:",
                    "s door Open STATE_CLOSED",
                    "s vent On STATE_OPEN",
                    "j main"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics");
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("compile gas filter program")
        void compileGasFilterProgram() {
            String src = String.join("\n",
                    "alias filter d0",
                    "alias analyzer d1",
                    "define MIN_MOLES 10",
                    "define STATE_OFF 0",
                    "define STATE_ON 1",
                    "loop:",
                    "l r0 analyzer Pressure",
                    "round r0 r0",
                    "bge r0 MIN_MOLES activate",
                    "s filter On STATE_OFF",
                    "j loop",
                    "activate:",
                    "s filter On STATE_ON",
                    "j loop"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics");
            assertNotNull(result.program.toJSON());
        }
    }

    // ============================================================
    // grammarTest.ic fixture 测试
    // ============================================================

    @Nested
    @DisplayName("grammarTest.ic fixture")
    class GrammarTestFixture {

        @Test
        @DisplayName("compile grammarTest.ic successfully")
        void compileGrammarTestSuccessfully() throws IOException {
            String src = readGrammarTestFile();
            assertFalse(src.isEmpty(), "grammarTest.ic should not be empty");
            CompileResult result = compile(src);
            assertNotNull(result.program);
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("no parser errors in grammarTest.ic")
        void noParserErrorsInGrammarTest() throws IOException {
            String src = readGrammarTestFile();
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "grammarTest.ic should not produce parser diagnostics");
        }

        @Test
        @DisplayName("produce valid symbol table from grammarTest.ic")
        void produceValidSymbolTableFromGrammarTest() throws IOException {
            String src = readGrammarTestFile();
            CompileResult result = compile(src);
            String symJson = result.analyser.getSymbolTable().toJSON();
            assertNotNull(symJson);
            assertFalse(symJson.isEmpty());
            // grammarTest.ic 定义了这些 alias
            assertTrue(symJson.contains("filter"), "symbol table should contain 'filter'");
            assertTrue(symJson.contains("analyzer"), "symbol table should contain 'analyzer'");
            assertTrue(symJson.contains("led"), "symbol table should contain 'led'");
            assertTrue(symJson.contains("display"), "symbol table should contain 'display'");
        }
    }

    // ============================================================
    // 指令元数完整覆盖测试
    // ============================================================

    @Nested
    @DisplayName("All instruction arities")
    class AllInstructionArities {

        @Test
        @DisplayName("compile program with all instruction arities")
        void compileAllArities() {
            // NOTE: lbn/lbns 最后操作数需为 identifier|number，不能用 register
            String src = String.join("\n",
                    "# 零元",
                    "hcf",
                    "yield",
                    "# 一元",
                    "sleep 100",
                    "j main",
                    "# 二元",
                    "move r0 0",
                    "abs r0 r1",
                    "# 三元",
                    "add r0 r1 r2",
                    "sub r0 r1 r2",
                    "mul r0 r1 r2",
                    "# 四元",
                    "lerp r0 r1 r2 r3",
                    "# 五元",
                    "lbn r0 100 200 2 Average",
                    "# 六元",
                    "lbns r0 100 200 3 Quantity Average",
                    "main:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for all arities");
        }
    }

    // ============================================================
    // 预处理指令完整覆盖测试
    // ============================================================

    @Nested
    @DisplayName("All preprocessor directives")
    class AllPreprocessorDirectives {

        @Test
        @DisplayName("compile program with all preprocessor directive types")
        void compileAllDirectives() {
            String src = String.join("\n",
                    "alias devA d0",
                    "alias regA r0",
                    "define COUNT 42",
                    "define HEX_VAL $FF",
                    "define HASH_VAL HASH(\"StructureLiquidVolumePump\")",
                    "define STR_VAL STR(\"SomeHash\")",
                    "main:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for all directives");
        }
    }

    // ============================================================
    // 错误场景测试
    // ============================================================

    @Nested
    @DisplayName("Error scenarios")
    class ErrorScenarios {

        @Test
        @DisplayName("report diagnostics for undefined variable")
        void reportUndefinedVariable() {
            CompileResult result = compile("move r0 undefined_var\nhcf\n");
            assertTrue(result.analyser.getDiagnostics().length > 0,
                    "analyser should report undefined variable");
        }

        @Test
        @DisplayName("report diagnostics for redefined alias")
        void reportRedefinedAlias() {
            String src = String.join("\n",
                    "alias foo r0",
                    "alias foo r1",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertTrue(result.analyser.getDiagnostics().length > 0,
                    "analyser should report redefined alias");
        }

        @Test
        @DisplayName("report diagnostics for redefined define")
        void reportRedefinedDefine() {
            String src = String.join("\n",
                    "define MAX 10",
                    "define MAX 20",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertTrue(result.analyser.getDiagnostics().length > 0,
                    "analyser should report redefined define");
        }

        @Test
        @DisplayName("report diagnostics for redefined label")
        void reportRedefinedLabel() {
            String src = String.join("\n",
                    "start:",
                    "move r0 0",
                    "start:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertTrue(result.analyser.getDiagnostics().length > 0,
                    "analyser should report redefined label");
        }

        @Test
        @DisplayName("report parser diagnostics for missing newline")
        void reportMissingNewline() {
            CompileResult result = compile("hcf yield\n");
            assertTrue(result.parser.getDiagnostics().length > 0,
                    "parser should report missing newline between statements");
        }
    }

    // ============================================================
    // 边界情况测试
    // ============================================================

    @Nested
    @DisplayName("Boundary cases")
    class BoundaryCases {

        @Test
        @DisplayName("handle empty source")
        void handleEmptySource() {
            CompileResult result = compile("");
            assertNotNull(result.program);
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("handle source with only comments")
        void handleOnlyComments() {
            String src = "# comment 1\n// comment 2\n# comment 3\n";
            CompileResult result = compile(src);
            assertNotNull(result.program);
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("handle source with only newlines")
        void handleOnlyNewlines() {
            CompileResult result = compile("\n\n\n\n");
            assertNotNull(result.program);
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("handle source with trailing newlines")
        void handleTrailingNewlines() {
            CompileResult result = compile("hcf\n\n\n\n");
            assertNotNull(result.program);
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("handle single statement without trailing newline")
        void handleSingleStatementNoTrailingNewline() {
            CompileResult result = compile("hcf");
            assertNotNull(result.program);
            assertNotNull(result.program.toJSON());
        }
    }

    // ============================================================
    // 前向引用测试
    // ============================================================

    @Nested
    @DisplayName("Forward references")
    class ForwardReferences {

        @Test
        @DisplayName("resolve forward label reference in jump instruction")
        void resolveForwardLabelInJump() {
            String src = String.join("\n",
                    "j target",
                    "move r0 1",
                    "target:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            // 前向引用应被解析，不应有 IEA3 系列未定义错误
            for (Diagnostic d : result.analyser.getDiagnostics()) {
                assertFalse(d.id != null && d.id.contains("IEA3"),
                        "forward reference should be resolved, but got: " + d.id);
            }
        }

        @Test
        @DisplayName("resolve forward label reference in branch instruction")
        void resolveForwardLabelInBranch() {
            String src = String.join("\n",
                    "beq r0 r1 target",
                    "move r0 1",
                    "target:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            for (Diagnostic d : result.analyser.getDiagnostics()) {
                assertFalse(d.id != null && d.id.contains("IEA3"),
                        "forward reference should be resolved, but got: " + d.id);
            }
        }

        @Test
        @DisplayName("resolve multiple forward references")
        void resolveMultipleForwardReferences() {
            String src = String.join("\n",
                    "j label1",
                    "j label2",
                    "j label3",
                    "label1:",
                    "hcf",
                    "label2:",
                    "yield",
                    "label3:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            for (Diagnostic d : result.analyser.getDiagnostics()) {
                assertFalse(d.id != null && d.id.contains("IEA3"),
                        "forward references should be resolved, but got: " + d.id);
            }
        }
    }

    // ============================================================
    // HASH 和 STR 宏测试
    // ============================================================

    @Nested
    @DisplayName("HASH and STR macros")
    class HashAndStrMacros {

        @Test
        @DisplayName("compile HASH macro in define")
        void compileHashMacro() {
            CompileResult result = compile(
                    "define PUMP_HASH HASH(\"StructureLiquidVolumePump\")\nhcf\n");
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for HASH macro");
        }

        @Test
        @DisplayName("compile STR macro in define")
        void compileStrMacro() {
            CompileResult result = compile(
                    "define LABEL STR(\"SomeHash\")\nhcf\n");
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for STR macro");
        }

        @Test
        @DisplayName("compile multiple HASH and STR macros")
        void compileMultipleMacros() {
            String src = String.join("\n",
                    "define HASH1 HASH(\"StructureLiquidVolumePump\")",
                    "define HASH2 HASH(\"StructureGasVentSensor\")",
                    "define STR1 STR(\"Label1\")",
                    "define STR2 STR(\"Label2\")",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for multiple macros");
        }
    }

    // ============================================================
    // 设备引用测试
    // ============================================================

    @Nested
    @DisplayName("Device references")
    class DeviceReferences {

        @Test
        @DisplayName("compile program using all device references d0-d5")
        void compileAllDeviceReferences() {
            String src = String.join("\n",
                    "l r0 d0 Setting",
                    "l r1 d1 Setting",
                    "l r2 d2 Setting",
                    "l r3 d3 Setting",
                    "l r4 d4 Setting",
                    "l r5 d5 Setting",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for d0-d5");
        }

        @Test
        @DisplayName("compile program using db and dn references")
        void compileDbDnReferences() {
            String src = String.join("\n",
                    "l r0 db Setting",
                    "l r1 dn Setting",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for db/dn");
        }
    }

    // ============================================================
    // 寄存器边界测试
    // ============================================================

    @Nested
    @DisplayName("Register boundaries")
    class RegisterBoundaries {

        @Test
        @DisplayName("compile program using r0 and r15")
        void compileR0AndR15() {
            String src = String.join("\n",
                    "move r0 0",
                    "move r15 0",
                    "add r0 r0 r15",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for r0 and r15");
        }
    }

    // ============================================================
    // 数字类型测试
    // ============================================================

    @Nested
    @DisplayName("Number types")
    class NumberTypes {

        @Test
        @DisplayName("compile program with all number types")
        void compileAllNumberTypes() {
            String src = String.join("\n",
                    "move r0 42",
                    "move r1 3.14",
                    "move r2 $FF",
                    "move r3 %1010",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for all number types");
        }
    }

    // ============================================================
    // Unicode 标识符测试
    // ============================================================

    @Nested
    @DisplayName("Unicode identifiers")
    class UnicodeIdentifiers {

        @Test
        @DisplayName("compile program with Chinese identifiers")
        void compileChineseIdentifiers() {
            String src = String.join("\n",
                    "alias 计数器 r0",
                    "alias 设备 d0",
                    "主循环:",
                    "move r0 0",
                    "add r0 r0 1",
                    "blt r0 10 主循环",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for Chinese identifiers");
        }
    }

    // ============================================================
    // 大型程序测试
    // ============================================================

    @Nested
    @DisplayName("Large programs")
    class LargePrograms {

        @Test
        @DisplayName("compile program with 100+ statements")
        void compileLargeProgram() {
            java.util.List<String> lines = new java.util.ArrayList<>();
            lines.add("alias counter r0");
            lines.add("define LIMIT 100");
            lines.add("start:");
            lines.add("move r0 0");
            for (int i = 0; i < 50; i++) {
                lines.add("add r0 r0 1");
                lines.add("yield");
            }
            lines.add("blt r0 LIMIT start");
            lines.add("hcf");
            String src = String.join("\n", lines) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for large program");
            assertNotNull(result.program.toJSON());
        }

        @Test
        @DisplayName("compile program with many labels and jumps")
        void compileManyLabelsAndJumps() {
            java.util.List<String> lines = new java.util.ArrayList<>();
            for (int i = 0; i < 20; i++) {
                lines.add("label" + i + ":");
                lines.add("j label" + ((i + 1) % 20));
            }
            lines.add("hcf");
            String src = String.join("\n", lines) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for many labels");
            assertNotNull(result.program.toJSON());
        }
    }

    // ============================================================
    // 常量引用测试
    // ============================================================

    @Nested
    @DisplayName("Constants in instructions")
    class ConstantsInInstructions {

        @Test
        @DisplayName("compile program using pi and tau")
        void compilePiAndTau() {
            String src = String.join("\n",
                    "move r0 pi",
                    "move r1 tau",
                    "mul r0 r0 r1",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for pi/tau");
        }

        @Test
        @DisplayName("compile program using nan and infinities")
        void compileNanAndInfinities() {
            String src = String.join("\n",
                    "move r0 nan",
                    "move r1 pinf",
                    "move r2 ninf",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for nan/pinf/ninf");
        }

        @Test
        @DisplayName("compile program using rgas")
        void compileRgas() {
            String src = String.join("\n",
                    "move r0 rgas",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for rgas");
        }
    }

    // ============================================================
    // 注释处理测试
    // ============================================================

    @Nested
    @DisplayName("Comments in programs")
    class CommentsInPrograms {

        @Test
        @DisplayName("compile program with extensive comments")
        void compileWithExtensiveComments() {
            String src = String.join("\n",
                    "# ============================================",
                    "# 这是程序的头部注释",
                    "# 描述了程序的功能和用途",
                    "# ============================================",
                    "",
                    "// alias 定义区",
                    "alias devA d0",
                    "alias devB d1",
                    "",
                    "# 主程序入口",
                    "main:",
                    "move r0 0",
                    "add r0 r0 1",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics with extensive comments");
        }
    }

    // ============================================================
    // 文档注释与类型提示测试
    // ============================================================

    @Nested
    @DisplayName("Doc comments and type hints")
    class DocCommentsAndTypeHints {

        @Test
        @DisplayName("compile program with device doc comment")
        void compileDeviceDocComment() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Furnace",
                    "#> @desc 炉窑设备",
                    "#> @end-device",
                    "alias furnace d0 #: @type Furnace",
                    "main:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for device doc comment");
        }

        @Test
        @DisplayName("compile program with enum doc comment")
        void compileEnumDocComment() {
            String src = String.join("\n",
                    "#> @enum",
                    "#> @name GasType",
                    "#> @value Oxygen 1 氧气",
                    "#> @value Nitrogen 2 氮气",
                    "#> @end-enum",
                    "main:",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for enum doc comment");
        }

        @Test
        @DisplayName("compile program with mixed doc comments and code")
        void compileMixedDocComments() {
            String src = String.join("\n",
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
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for mixed doc comments");
        }

        @Test
        @DisplayName("compile alias with type hint")
        void compileAliasWithTypeHint() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Furnace",
                    "#> @logic Pressure r",
                    "#> @end-device",
                    "alias myDevice d0 #: @type Furnace",
                    "alias myReg r0",
                    "main:",
                    "l r0 myDevice Pressure",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for alias with type hint");
        }
    }

    // ============================================================
    // 类型推导与语义分析系统测试
    // ============================================================

    @Nested
    @DisplayName("Type inference and semantic analysis")
    class TypeInferenceAndSemanticAnalysis {

        @Test
        @DisplayName("perform full type checking with device doc comments")
        void performFullTypeChecking() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Furnace",
                    "#> @desc 炉窑设备",
                    "#> @logic Temperature r",
                    "#> @logic Active rw",
                    "#> @slot 0 fuel",
                    "#> @slot 1 ore",
                    "#> @logicSlot Occupied",
                    "#> @end-device",
                    "",
                    "alias furnace d0 #: @type Furnace",
                    "",
                    "main:",
                    "  l r0 furnace Temperature",
                    "  s furnace Active r0",
                    "  ls r1 furnace 0 Occupied",
                    "  hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics");
            assertEquals(0, result.analyser.getDiagnostics().length,
                    "analyser should produce no diagnostics for valid typed device usage");
            String symJson = result.analyser.getSymbolTable().toJSON();
            assertTrue(symJson.contains("furnace"),
                    "symbol table should contain 'furnace' alias");
            // BasicType.DEVICE = 4 (numerical serialization)
            assertTrue(symJson.contains("\"type\":4") || symJson.contains("\"type\": 4"),
                    "furnace symbol type should be DEVICE (4)");
            assertTrue(symJson.contains("Furnace"),
                    "furnace symbol typeName should be 'Furnace'");
        }

        @Test
        @DisplayName("detect invalid logic names on typed devices")
        void detectInvalidLogicNamesOnTypedDevices() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure rw",
                    "#> @end-device",
                    "alias sensor d0 #: @type Sensor",
                    "l r0 sensor InvalidLogic",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length);
            assertTrue(result.analyser.getDiagnostics().length > 0,
                    "analyser should report diagnostic for invalid logic name");
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IWA14_2"),
                    "should report IWA14_2 for invalid logic name on typed device");
        }

        @Test
        @DisplayName("handle batch mode with enum doc comment")
        void handleBatchModeWithEnumDocComment() {
            String src = String.join("\n",
                    "#> @enum",
                    "#> @name BatchMode",
                    "#> @value Greater 0",
                    "#> @value Less 1",
                    "#> @end-enum",
                    "lbn r0 0 0 Pressure Greater",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for batch mode with enum");
        }

        @Test
        @DisplayName("pass device context within single instruction only")
        void passDeviceContextWithinSingleInstruction() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure r",
                    "#> @end-device",
                    "#> @device",
                    "#> @name Furnace",
                    "#> @logic Temperature r",
                    "#> @end-device",
                    "alias sensor d0 #: @type Sensor",
                    "alias furnace d1 #: @type Furnace",
                    "l r0 sensor Pressure",
                    "l r1 furnace Temperature",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length);
            assertEquals(0, result.analyser.getDiagnostics().length,
                    "analyser should produce no diagnostics for valid device contexts");
        }

        @Test
        @DisplayName("work with device references (d0) directly without alias")
        void workWithDeviceReferencesDirectly() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure rw",
                    "#> @end-device",
                    "l r0 d0 Pressure",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length,
                    "parser should produce no diagnostics for direct d0 reference");
        }

        @Test
        @DisplayName("report reagent mode errors correctly")
        void reportReagentModeErrors() {
            String src = String.join("\n",
                    "#> @enum",
                    "#> @name ReagentMode",
                    "#> @value Contents 0",
                    "#> @end-enum",
                    "#> @device",
                    "#> @name Filter",
                    "#> @end-device",
                    "alias filter d0 #: @type Filter",
                    "lr r0 filter BadMode Oxygen",
                    "hcf"
            ) + "\n";
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length);
            assertTrue(result.analyser.getDiagnostics().length > 0,
                    "analyser should report diagnostic for invalid reagent mode");
        }
    }
}
