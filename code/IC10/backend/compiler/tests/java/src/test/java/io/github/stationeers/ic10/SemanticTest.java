// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Semantic analysis tests for the IC10 Java binding.
 * <p>
 * Mirrors the C++ {@code test_semantic.cpp} test suite at matching granularity.
 * Because the Java binding exposes only {@link SymbolTable#toJSON()} and
 * {@link Analyser#getDiagnostics()} for inspection, assertions focus on
 * precise diagnostic IDs (IWA11_2/IWA12_1/IWA13_1/IWA14_2/IWA15_1/IWA16_2/
 * IWA17_1/IEA8_1/IEA2_1/IE0_1) and on the presence of expected fields in
 * the serialized symbol/type tables. Direct SymbolTable/TypeTable internal
 * structure unit tests (present in C++) are intentionally omitted because
 * the Java binding does not expose the underlying {@code define/resolve/find}
 * APIs.
 *
 * <p>Coverage:
 * <ul>
 *   <li>LOGIC_TYPE / LOGIC_SLOT / REAGENT_MODE / BATCH_MODE identifier checks</li>
 *   <li>Number fallback (these operand types accept numeric literals)</li>
 *   <li>Device context passing (alias type annotation + subsequent logic/slot
 *       name check, including SLOT_IDX bounds)</li>
 *   <li>Device context isolation across instructions</li>
 *   <li>Undefined symbol / redefinition diagnostics</li>
 *   <li>Missing standard library enum scenarios (IEA8_1)</li>
 *   <li>Robustness: alias with nonexistent type hint, constants, empty program,
 *       doc-comment-only program</li>
 *   <li>Symbol table contents: contains alias/define/label symbols</li>
 * </ul>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("Semantic analysis tests")
class SemanticTest {

    @BeforeAll
    static void setupLanguage() {
        IC10Local.setLanguage("zh-hans");
    }

    // ============================================================
    // 最小化标准库定义（仅包含测试所需的枚举值与设备逻辑）
    // Minimal standard library definitions
    // ============================================================

    private static final String K_LOGIC_TYPE_ENUM = String.join("\n",
            "#> @enum",
            "#> @name LogicType",
            "#> @value Pressure 5",
            "#> @value Setting 12",
            "#> @value Temperature 6",
            "#> @value On 28",
            "#> @value Open 21",
            "#> @value Color 189",
            "#> @value TotalMoles 66",
            "#> @end-enum"
    );

    private static final String K_LOGIC_SLOT_TYPE_ENUM = String.join("\n",
            "#> @enum",
            "#> @name LogicSlotType",
            "#> @value Quantity 3",
            "#> @value Charge 10",
            "#> @value Damage 4",
            "#> @end-enum"
    );

    private static final String K_REAGENT_MODE_ENUM = String.join("\n",
            "#> @enum",
            "#> @name ReagentMode",
            "#> @value Contents 0",
            "#> @value Recipe 2",
            "#> @value Required 1",
            "#> @value TotalContents 3",
            "#> @end-enum"
    );

    private static final String K_BATCH_MODE_ENUM = String.join("\n",
            "#> @enum",
            "#> @name BatchMode",
            "#> @value Average 0",
            "#> @value Sum 1",
            "#> @value Minimum 2",
            "#> @value Maximum 3",
            "#> @value Count 4",
            "#> @end-enum"
    );

    private static final String K_TEST_DEVICE = String.join("\n",
            "#> @device",
            "#> @name TestDevice",
            "#> @logic Pressure r",
            "#> @logic Setting rw",
            "#> @logic On w",
            "#> @logicSlot Quantity",
            "#> @logicSlot Charge",
            "#> @slot 0 input",
            "#> @slot 1 output",
            "#> @end-device"
    );

    // ---- Helpers ----

    /** 编译结果载体 / Compile result carrier. */
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

    /** 拼接所有标准库定义 + 用户源码 / Concatenate all stdlib defs + user source. */
    private static String withStdLib(String source) {
        return String.join("\n", K_LOGIC_TYPE_ENUM, K_LOGIC_SLOT_TYPE_ENUM,
                K_REAGENT_MODE_ENUM, K_BATCH_MODE_ENUM, K_TEST_DEVICE, "", source);
    }

    /** 仅拼接指定枚举定义 + 用户源码（用于测试缺失场景） / Concatenate only specified enum defs + source. */
    private static String withEnums(java.util.List<String> enums, String source) {
        java.util.List<String> all = new java.util.ArrayList<>(enums);
        all.add("");
        all.add(source);
        return String.join("\n", all);
    }

    /** 完整编译流水线 / Full compile pipeline. */
    private static CompileResult compile(String source) {
        Token[] tokens = Lexer.tokenize(source, false);
        Parser parser = new Parser();
        parser.init(tokens, false);
        Program program = parser.parse();
        Analyser analyser = new Analyser();
        analyser.visit(program);
        return new CompileResult(tokens, program, parser, analyser);
    }

    /** 断言词法和语法阶段无诊断 / Assert no lexer/parser diagnostics. */
    private static void assertNoLexerParserDiags(CompileResult result) {
        // Java 绑定不直接暴露 lexer 的 getDiagnostics()（lexer 是实例方法且我们用的是静态 tokenize），
        // 这里仅校验 parser 诊断。lexer 诊断在静态 tokenize 路径下不收集。
        assertEquals(0, result.parser.getDiagnostics().length,
                "parser should produce no diagnostics");
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

    /** 统计指定 ID 的诊断数量 / Count diagnostics with the given ID. */
    private static long countDiagnostic(Diagnostic[] diags, String id) {
        long count = 0;
        for (Diagnostic d : diags) {
            if (id.equals(d.id)) {
                count++;
            }
        }
        return count;
    }

    // ============================================================
    // LOGIC_TYPE 标识符检查（无设备上下文）
    // ============================================================

    @Nested
    @DisplayName("LOGIC_TYPE identifier check (no device context)")
    class LogicTypeCheck {

        @Test
        @DisplayName("valid LOGIC_TYPE produces no IWA15_1 diagnostic")
        void validLogicTypeNoDiagnostic() {
            // lb: REG_IDENT, REG_NUM, LOGIC_TYPE, BATCH_MODE
            CompileResult result = compile(withStdLib("lb r0 100 Pressure Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA15_1"),
                    "Pressure is a valid LogicType, should not report IWA15_1");
        }

        @Test
        @DisplayName("invalid LOGIC_TYPE reports IWA15_1")
        void invalidLogicTypeReportsIWA15_1() {
            CompileResult result = compile(withStdLib("lb r0 100 BadLogicName Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IWA15_1"),
                    "BadLogicName is not in LogicType enum, should report IWA15_1");
        }

        @Test
        @DisplayName("missing LogicType enum reports IEA8_1")
        void missingLogicTypeEnumReportsIEA8_1() {
            // 仅加载 BatchMode，不加载 LogicType
            CompileResult result = compile(
                    withEnums(java.util.Collections.singletonList(K_BATCH_MODE_ENUM),
                            "lb r0 100 Pressure Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IEA8_1"),
                    "LogicType enum undefined, should report IEA8_1");
        }
    }

    // ============================================================
    // LOGIC_SLOT 标识符检查
    // ============================================================

    @Nested
    @DisplayName("LOGIC_SLOT identifier check")
    class LogicSlotCheck {

        @Test
        @DisplayName("valid LOGIC_SLOT produces no IWA12_1 diagnostic")
        void validLogicSlotNoDiagnostic() {
            // lbs: REG_IDENT, REG_NUM, SLOT_IDX, LOGIC_SLOT, BATCH_MODE
            CompileResult result = compile(withStdLib("lbs r0 100 0 Quantity Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA12_1"),
                    "Quantity is a valid LogicSlotType, should not report IWA12_1");
        }

        @Test
        @DisplayName("invalid LOGIC_SLOT reports IWA12_1")
        void invalidLogicSlotReportsIWA12_1() {
            CompileResult result = compile(withStdLib("lbs r0 100 0 BadSlotName Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IWA12_1"),
                    "BadSlotName is not in LogicSlotType enum, should report IWA12_1");
        }
    }

    // ============================================================
    // REAGENT_MODE 标识符检查
    // ============================================================

    @Nested
    @DisplayName("REAGENT_MODE identifier check")
    class ReagentModeCheck {

        @Test
        @DisplayName("valid REAGENT_MODE produces no IWA13_1 diagnostic")
        void validReagentModeNoDiagnostic() {
            // lr: REG_IDENT, DEV_REF, REAGENT_MODE, JUMP_TARGET
            // NOTE: 使用 d0 (Device 节点) 而非 db (未定义 Identifier) 作为 DEV_REF，
            //       因为 process 是 Task<void>，await resolveSymbol 的 Future 时无法注册为等待者，
            //       未定义的 DEV_REF 会导致 process 永久挂起，后续 REAGENT_MODE 检查不会执行。
            CompileResult result = compile(withStdLib(
                    "main:\n" +
                    "lr r0 d0 Contents main\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA13_1"),
                    "Contents is a valid ReagentMode, should not report IWA13_1");
        }

        @Test
        @DisplayName("invalid REAGENT_MODE reports IWA13_1")
        void invalidReagentModeReportsIWA13_1() {
            // 同上，使用 d0 避免 DEV_REF 挂起
            CompileResult result = compile(withStdLib(
                    "main:\n" +
                    "lr r0 d0 BadReagentMode main\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IWA13_1"),
                    "BadReagentMode is not in ReagentMode enum, should report IWA13_1");
        }
    }

    // ============================================================
    // BATCH_MODE 标识符检查
    // ============================================================

    @Nested
    @DisplayName("BATCH_MODE identifier check")
    class BatchModeCheck {

        @Test
        @DisplayName("valid BATCH_MODE produces no IWA17_1 diagnostic")
        void validBatchModeNoDiagnostic() {
            CompileResult result = compile(withStdLib("lb r0 100 Pressure Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA17_1"),
                    "Average is a valid BatchMode, should not report IWA17_1");
        }

        @Test
        @DisplayName("invalid BATCH_MODE reports IWA17_1")
        void invalidBatchModeReportsIWA17_1() {
            CompileResult result = compile(withStdLib("lb r0 100 Pressure BadBatchMode\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IWA17_1"),
                    "BadBatchMode is not in BatchMode enum, should report IWA17_1");
        }
    }

    // ============================================================
    // 数字回退：LOGIC_TYPE / LOGIC_SLOT / REAGENT_MODE / BATCH_MODE 接受数字
    // ============================================================

    @Nested
    @DisplayName("Number fallback")
    class NumberFallback {

        @Test
        @DisplayName("LOGIC_TYPE accepts numeric literal")
        void logicTypeAcceptsNumber() {
            CompileResult result = compile(withStdLib("lb r0 100 5 Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA15_1"),
                    "numeric 5 is a valid LOGIC_TYPE value");
        }

        @Test
        @DisplayName("BATCH_MODE accepts numeric literal")
        void batchModeAcceptsNumber() {
            CompileResult result = compile(withStdLib("lb r0 100 Pressure 0\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA17_1"),
                    "numeric 0 is a valid BATCH_MODE value");
        }

        @Test
        @DisplayName("LOGIC_SLOT accepts numeric literal")
        void logicSlotAcceptsNumber() {
            CompileResult result = compile(withStdLib("lbs r0 100 0 3 Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA12_1"),
                    "numeric 3 is a valid LOGIC_SLOT value");
        }

        @Test
        @DisplayName("REAGENT_MODE accepts numeric literal")
        void reagentModeAcceptsNumber() {
            CompileResult result = compile(withStdLib(
                    "main:\n" +
                    "lr r0 d0 0 main\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA13_1"),
                    "numeric 0 is a valid REAGENT_MODE value");
        }
    }

    // ============================================================
    // 设备上下文：LOGIC_TYPE 检查
    // ============================================================

    @Nested
    @DisplayName("Device context: LOGIC_TYPE check")
    class DeviceContextLogicType {

        @Test
        @DisplayName("valid LOGIC_TYPE with device context produces no IWA14_2")
        void validLogicTypeNoDiagnostic() {
            CompileResult result = compile(withStdLib(
                    "alias dev d0 #: @type TestDevice\n" +
                    "l r0 dev Pressure\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA14_2"),
                    "Pressure is in TestDevice.logics, should not report IWA14_2");
        }

        @Test
        @DisplayName("invalid LOGIC_TYPE with device context reports IWA14_2 exactly once")
        void invalidLogicTypeReportsIWA14_2() {
            CompileResult result = compile(withStdLib(
                    "alias dev d0 #: @type TestDevice\n" +
                    "l r0 dev BadLogicName\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertEquals(1L, countDiagnostic(result.analyser.getDiagnostics(), "IWA14_2"),
                    "BadLogicName not in TestDevice.logics, should report IWA14_2 exactly once");
        }
    }

    // ============================================================
    // 设备上下文：LOGIC_SLOT 检查
    // ============================================================

    @Nested
    @DisplayName("Device context: LOGIC_SLOT check")
    class DeviceContextLogicSlot {

        @Test
        @DisplayName("valid LOGIC_SLOT with device context produces no IWA11_2")
        void validLogicSlotNoDiagnostic() {
            // ls: REG_IDENT, DEV_REF, SLOT_IDX, LOGIC_SLOT
            CompileResult result = compile(withStdLib(
                    "alias dev d0 #: @type TestDevice\n" +
                    "ls r0 dev 0 Quantity\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA11_2"),
                    "Quantity is in TestDevice.logicSlots, should not report IWA11_2");
        }

        @Test
        @DisplayName("invalid LOGIC_SLOT with device context reports IWA11_2 exactly once")
        void invalidLogicSlotReportsIWA11_2() {
            CompileResult result = compile(withStdLib(
                    "alias dev d0 #: @type TestDevice\n" +
                    "ls r0 dev 0 BadSlotName\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertEquals(1L, countDiagnostic(result.analyser.getDiagnostics(), "IWA11_2"),
                    "BadSlotName not in TestDevice.logicSlots, should report IWA11_2 exactly once");
        }
    }

    // ============================================================
    // 设备上下文：SLOT_IDX 检查
    // ============================================================

    @Nested
    @DisplayName("Device context: SLOT_IDX check")
    class DeviceContextSlotIdx {

        @Test
        @DisplayName("valid SLOT_IDX with device context produces no IWA16_2")
        void validSlotIdxNoDiagnostic() {
            // TestDevice 定义了 slot 0 和 slot 1
            CompileResult result = compile(withStdLib(
                    "alias dev d0 #: @type TestDevice\n" +
                    "ls r0 dev 0 Quantity\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA16_2"),
                    "0 is in TestDevice.slots, should not report IWA16_2");
        }

        @Test
        @DisplayName("invalid SLOT_IDX with device context reports IWA16_2")
        void invalidSlotIdxReportsIWA16_2() {
            // TestDevice 只有 slot 0 和 1，99 不在范围内
            CompileResult result = compile(withStdLib(
                    "alias dev d0 #: @type TestDevice\n" +
                    "ls r0 dev 99 Quantity\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IWA16_2"),
                    "99 not in TestDevice.slots, should report IWA16_2");
        }
    }

    // ============================================================
    // 设备上下文隔离：每条指令开始时重置 pendingDeviceSymbol_
    // ============================================================

    @Nested
    @DisplayName("Device context isolation")
    class DeviceContextIsolation {

        @Test
        @DisplayName("device context should not leak across instructions")
        void deviceContextResetAcrossInstructions() {
            // 第一条指令设置 dev 上下文，第二条指令不带 dev，LOGIC_TYPE 应走全局枚举检查
            CompileResult result = compile(withStdLib(
                    "alias dev d0 #: @type TestDevice\n" +
                    "l r0 dev Pressure\n" +
                    "lb r0 100 Pressure Average\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            // 两条指令的 Pressure 都合法，不应有任何 IWA 诊断
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA14_2"),
                    "device context should not leak across instructions");
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IWA15_1"),
                    "global LogicType check should also pass for valid Pressure");
        }
    }

    // ============================================================
    // 未定义符号检查（IE0_1）
    // ============================================================

    @Nested
    @DisplayName("Undefined symbol check (IE0_1)")
    class UndefinedSymbolCheck {

        @Test
        @DisplayName("undefined device alias reference reports IE0_1")
        void undefinedDeviceAliasReportsIE0_1() {
            CompileResult result = compile(withStdLib(
                    "l r0 undefinedDev Pressure\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IE0_1"),
                    "undefinedDev is undefined, should report IE0_1");
        }

        @Test
        @DisplayName("undefined jump target reports IE0_1")
        void undefinedJumpTargetReportsIE0_1() {
            CompileResult result = compile(withStdLib(
                    "j nonexistent\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IE0_1"),
                    "nonexistent label is undefined, should report IE0_1");
        }

        @Test
        @DisplayName("multiple undefined symbols all reported")
        void multipleUndefinedSymbolsAllReported() {
            CompileResult result = compile(withStdLib(
                    "move r0 undefA\n" +
                    "move r1 undefB\n" +
                    "j undefC\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(result.analyser.getDiagnostics().length >= 3,
                    "at least 3 undefined symbols should be reported");
        }
    }

    // ============================================================
    // 重定义检查（IEA2_1）
    // ============================================================

    @Nested
    @DisplayName("Redefinition check (IEA2_1)")
    class RedefinitionCheck {

        @Test
        @DisplayName("alias redefinition reports IEA2_1")
        void aliasRedefinitionReportsIEA2_1() {
            CompileResult result = compile(withStdLib(
                    "alias foo r0\n" +
                    "alias foo r1\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IEA2_1"),
                    "alias foo redefinition, should report IEA2_1");
        }

        @Test
        @DisplayName("define redefinition reports IEA2_1")
        void defineRedefinitionReportsIEA2_1() {
            CompileResult result = compile(withStdLib(
                    "define VAL 10\n" +
                    "define VAL 20\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IEA2_1"),
                    "define VAL redefinition, should report IEA2_1");
        }

        @Test
        @DisplayName("label redefinition reports IEA2_1")
        void labelRedefinitionReportsIEA2_1() {
            CompileResult result = compile(withStdLib(
                    "loop:\n" +
                    "loop:\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IEA2_1"),
                    "label loop redefinition, should report IEA2_1");
        }
    }

    // ============================================================
    // 前向引用合法
    // ============================================================

    @Nested
    @DisplayName("Forward reference legality")
    class ForwardReference {

        @Test
        @DisplayName("forward label reference is legal (no IE0_1)")
        void forwardLabelReferenceNoError() {
            CompileResult result = compile(withStdLib(
                    "j end\n" +
                    "end:\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            String symJson = result.analyser.getSymbolTable().toJSON();
            assertTrue(symJson.contains("end"),
                    "symbol table should contain forward-referenced label 'end'");
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IE0_1"),
                    "forward label reference is legal, should not report IE0_1");
        }
    }

    // ============================================================
    // 缺失枚举场景（IEA8_1）
    // ============================================================

    @Nested
    @DisplayName("Missing enum scenarios (IEA8_1)")
    class MissingEnumScenarios {

        @Test
        @DisplayName("missing LogicSlotType enum reports IEA8_1")
        void missingLogicSlotTypeEnumReportsIEA8_1() {
            // 仅加载 BatchMode 和 LogicType，不加载 LogicSlotType
            CompileResult result = compile(
                    withEnums(java.util.Arrays.asList(K_BATCH_MODE_ENUM, K_LOGIC_TYPE_ENUM),
                            "lbs r0 100 0 Quantity Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IEA8_1"),
                    "LogicSlotType enum undefined, should report IEA8_1");
        }

        @Test
        @DisplayName("missing ReagentMode enum reports IEA8_1")
        void missingReagentModeEnumReportsIEA8_1() {
            // 不加载 ReagentMode
            CompileResult result = compile(
                    withEnums(java.util.Collections.singletonList(K_LOGIC_TYPE_ENUM),
                            "main:\n" +
                            "lr r0 d0 Contents main\n" +
                            "hcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IEA8_1"),
                    "ReagentMode enum undefined, should report IEA8_1");
        }

        @Test
        @DisplayName("missing BatchMode enum reports IEA8_1")
        void missingBatchModeEnumReportsIEA8_1() {
            // 仅加载 LogicType，不加载 BatchMode
            CompileResult result = compile(
                    withEnums(java.util.Collections.singletonList(K_LOGIC_TYPE_ENUM),
                            "lb r0 100 Pressure Average\nhcf\n"));
            assertNoLexerParserDiags(result);
            assertTrue(hasDiagnostic(result.analyser.getDiagnostics(), "IEA8_1"),
                    "BatchMode enum undefined, should report IEA8_1");
        }
    }

    // ============================================================
    // 鲁棒性测试
    // ============================================================

    @Nested
    @DisplayName("Robustness")
    class Robustness {

        @Test
        @DisplayName("alias with nonexistent type hint does not crash")
        void aliasWithNonexistentTypeHintNoCrash() {
            String src = withStdLib(
                    "alias dev d0 #: @type NonexistentType\n" +
                    "l r0 dev Pressure\n" +
                    "hcf\n");
            // 应正常返回，不抛出异常
            CompileResult result = compile(src);
            assertNotNull(result.program);
            assertNotNull(result.analyser);
        }

        @Test
        @DisplayName("constant references (pi/nan/rgas) should not produce IE0_1")
        void constantsReferenceNoUndefinedError() {
            CompileResult result = compile(withStdLib(
                    "move r0 pi\n" +
                    "move r1 nan\n" +
                    "move r2 rgas\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            assertFalse(hasDiagnostic(result.analyser.getDiagnostics(), "IE0_1"),
                    "constants are builtin, should not report IE0_1");
        }

        @Test
        @DisplayName("empty program semantic analysis has no errors")
        void emptyProgramNoErrors() {
            CompileResult result = compile("");
            assertNotNull(result.program);
            assertEquals(0, result.parser.getDiagnostics().length);
            assertEquals(0, result.analyser.getDiagnostics().length,
                    "empty program should produce no analyser diagnostics");
        }

        @Test
        @DisplayName("program with only doc comments has no errors")
        void docCommentOnlyProgramNoErrors() {
            String src = K_LOGIC_TYPE_ENUM + "\n" + K_TEST_DEVICE;
            CompileResult result = compile(src);
            assertEquals(0, result.parser.getDiagnostics().length);
            // 文档注释定义类型，不应有语义错误
            assertEquals(0, result.analyser.getDiagnostics().length,
                    "program with only doc comments should produce no analyser diagnostics");
        }
    }

    // ============================================================
    // 符号表内容验证
    // ============================================================

    @Nested
    @DisplayName("Symbol table contents")
    class SymbolTableContents {

        @Test
        @DisplayName("symbol table contains alias-defined symbols")
        void containsAliasSymbols() {
            CompileResult result = compile(withStdLib(
                    "alias myReg r0\n" +
                    "alias myDev d0\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            String json = result.analyser.getSymbolTable().toJSON();
            assertTrue(json.contains("myReg"), "symbol table should contain 'myReg'");
            assertTrue(json.contains("myDev"), "symbol table should contain 'myDev'");
        }

        @Test
        @DisplayName("symbol table contains define-defined symbols")
        void containsDefineSymbols() {
            CompileResult result = compile(withStdLib(
                    "define MAX 100\n" +
                    "define NAME HASH(\"test\")\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            String json = result.analyser.getSymbolTable().toJSON();
            assertTrue(json.contains("MAX"), "symbol table should contain 'MAX'");
            assertTrue(json.contains("NAME"), "symbol table should contain 'NAME'");
        }

        @Test
        @DisplayName("symbol table contains label definitions")
        void containsLabelSymbols() {
            CompileResult result = compile(withStdLib(
                    "start:\n" +
                    "loop:\n" +
                    "j end\n" +
                    "end:\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            String json = result.analyser.getSymbolTable().toJSON();
            assertTrue(json.contains("start"), "symbol table should contain 'start' label");
            assertTrue(json.contains("loop"), "symbol table should contain 'loop' label");
            assertTrue(json.contains("end"), "symbol table should contain 'end' label");
        }

        @Test
        @DisplayName("device alias defined by alias can be referenced")
        void deviceAliasCanBeReferenced() {
            CompileResult result = compile(withStdLib(
                    "alias myDev d0\n" +
                    "l r0 myDev Pressure\n" +
                    "hcf\n"));
            assertNoLexerParserDiags(result);
            String json = result.analyser.getSymbolTable().toJSON();
            assertTrue(json.contains("myDev"),
                    "symbol table should contain device alias 'myDev'");
        }
    }
}
