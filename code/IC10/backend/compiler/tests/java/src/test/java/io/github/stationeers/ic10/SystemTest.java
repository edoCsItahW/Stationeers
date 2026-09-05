// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10;

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
        IC10CompilerLocal.setLanguage("zh-hans");
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
            String src = "# comment 1\n# comment 2\n# comment 3\n";
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

        

        
    }

    // ============================================================
    // 注释处理测试
    // ============================================================

    

    // ============================================================
    // 文档注释与类型提示测试
    // ============================================================

    

    // ============================================================
    // 类型推导与语义分析系统测试
    // ============================================================

    
}
