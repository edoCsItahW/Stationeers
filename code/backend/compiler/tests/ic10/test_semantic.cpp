// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

/**
 * @file test_semantic.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/18
 * @if zh
 * @brief IC10语义分析类型推导测试
 * @details 针对语义分析阶段的类型推导逻辑进行单元测试，覆盖：
 *          - LOGIC_TYPE / LOGIC_SLOT / REAGENT_MODE / BATCH_MODE 标识符检查
 *          - 设备上下文传递（alias 类型注解 + 后续逻辑名检查）
 *          - SLOT_IDX 设备槽索引检查
 *          - 标准库枚举缺失场景（IEA8_1）
 *          - 数字回退（LOGIC_TYPE 等接受数字字面量）
 *          测试通过在源码中内嵌 #> 文档注释来注册最小化的标准库枚举与设备类型。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 semantic analysis type deduction tests
 * @details Unit tests for the type deduction logic in the semantic analysis phase, covering:
 *          - LOGIC_TYPE / LOGIC_SLOT / REAGENT_MODE / BATCH_MODE identifier checks
 *          - Device context passing (alias type annotation + subsequent logic name check)
 *          - SLOT_IDX device slot index check
 *          - Missing standard library enum scenarios (IEA8_1)
 *          - Number fallback (LOGIC_TYPE etc. accept numeric literals)
 *          Tests register minimal standard library enums and device types by embedding
 *          #> doc comments in the source.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#include <gtest/gtest.h>

#include <algorithm>
#include <initializer_list>
#include <ranges>
#include <string>

#include "ic10/lexer/lexer.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/semantic/analyser.hpp"

using namespace stationeers::ic10;

namespace {

    /// @brief 语义分析测试夹具 / Semantic analysis test fixture
    class SemanticTestFixture : public ::testing::Test {
    protected:
        static void SetUpTestSuite() {
            Loc::registerLanguage<ZhHans>("zh-hans");
            Loc::setLanguage("zh-hans");
        }

        /// @brief 完整编译流水线结果 / Full compilation pipeline result
        struct CompileResult {
            std::vector<std::shared_ptr<Token>> tokens;
            Program ast;
            std::shared_ptr<Analyser> analyser;
            std::vector<Diagnostic> lexerDiags;
            std::vector<Diagnostic> parserDiags;
            std::vector<Diagnostic> analyserDiags;
        };

        /// @brief 运行完整编译流水线 / Run the full compilation pipeline
        static CompileResult compile(const std::string& source) {
            CompileResult result;

            Lexer lexer(source);
            result.tokens = lexer.scan();
            result.lexerDiags = lexer.getDiagnostics();

            Parser parser(result.tokens);
            result.ast = parser.parse();
            result.parserDiags = parser.getDiagnostics();

            result.analyser = std::make_shared<Analyser>();
            auto task = result.analyser->visit(result.ast);
            (void)task.getFuture().get();
            result.analyserDiags = result.analyser->getDiagnostics();

            return result;
        }

        /// @brief 判断诊断列表中是否包含指定 ID / Check if diagnostics contain the given ID
        static bool hasDiagnostic(const std::vector<Diagnostic>& diags, const std::string& id) {
            return std::ranges::any_of(diags, [&](const Diagnostic& d) { return d.id == id; });
        }

        /// @brief 统计指定 ID 的诊断数量 / Count diagnostics with the given ID
        static std::size_t countDiagnostic(const std::vector<Diagnostic>& diags, const std::string& id) {
            return static_cast<std::size_t>(
                std::ranges::count_if(diags, [&](const Diagnostic& d) { return d.id == id; })
            );
        }

        /// @brief 断言词法和语法阶段无诊断 / Assert no lexer/parser diagnostics
        static void assertNoLexerParserDiags(const CompileResult& result) {
            ASSERT_TRUE(result.lexerDiags.empty())
                << "Lexer diagnostics:\n" << formatDiags(result.lexerDiags);
            ASSERT_TRUE(result.parserDiags.empty())
                << "Parser diagnostics:\n" << formatDiags(result.parserDiags);
        }

        /// @brief 格式化诊断列表（调试用） / Format diagnostics for debugging
        static std::string formatDiags(const std::vector<Diagnostic>& diags) {
            std::string out;
            for (const auto& d : diags) {
                out += "  [" + d.id + "] L" + std::to_string(d.start.line()) + ":"
                     + std::to_string(d.start.column()) + " - " + d.message + "\n";
            }
            return out;
        }

        // ------------------------------------------------------------------
        //  最小化标准库定义（仅包含测试所需的枚举值与设备逻辑）
        //  Minimal standard library definitions (only enum values and device
        //  logics needed for tests)
        // ------------------------------------------------------------------

        static constexpr std::string_view kLogicTypeEnum =
            "#> @enum\n"
            "#> @name LogicType\n"
            "#> @value Pressure 5\n"
            "#> @value Setting 12\n"
            "#> @value Temperature 6\n"
            "#> @value On 28\n"
            "#> @value Open 21\n"
            "#> @value Color 189\n"
            "#> @value TotalMoles 66\n"
            "#> @end-enum\n";

        static constexpr std::string_view kLogicSlotTypeEnum =
            "#> @enum\n"
            "#> @name LogicSlotType\n"
            "#> @value Quantity 3\n"
            "#> @value Charge 10\n"
            "#> @value Damage 4\n"
            "#> @end-enum\n";

        static constexpr std::string_view kReagentModeEnum =
            "#> @enum\n"
            "#> @name ReagentMode\n"
            "#> @value Contents 0\n"
            "#> @value Recipe 2\n"
            "#> @value Required 1\n"
            "#> @value TotalContents 3\n"
            "#> @end-enum\n";

        static constexpr std::string_view kBatchModeEnum =
            "#> @enum\n"
            "#> @name BatchMode\n"
            "#> @value Average 0\n"
            "#> @value Sum 1\n"
            "#> @value Minimum 2\n"
            "#> @value Maximum 3\n"
            "#> @value Count 4\n"
            "#> @end-enum\n";

        /// @brief 测试设备：含 logics/logicSlots/slots / Test device with logics/logicSlots/slots
        static constexpr std::string_view kTestDevice =
            "#> @device\n"
            "#> @name TestDevice\n"
            "#> @logic Pressure r\n"
            "#> @logic Setting rw\n"
            "#> @logic On w\n"
            "#> @logicSlot Quantity\n"
            "#> @logicSlot Charge\n"
            "#> @slot 0 input\n"
            "#> @slot 1 output\n"
            "#> @end-device\n";

        /// @brief 拼接所有标准库定义 + 用户源码 / Concatenate all stdlib defs + user source
        static std::string withStdLib(const std::string& source) {
            std::string result;
            result.reserve(kLogicTypeEnum.size() + kLogicSlotTypeEnum.size()
                           + kReagentModeEnum.size() + kBatchModeEnum.size()
                           + kTestDevice.size() + source.size() + 16);
            result += kLogicTypeEnum;
            result += kLogicSlotTypeEnum;
            result += kReagentModeEnum;
            result += kBatchModeEnum;
            result += kTestDevice;
            result += "\n";
            result += source;
            return result;
        }

        /// @brief 仅拼接指定枚举定义 + 用户源码（用于测试缺失场景）
        ///        Concatenate only specified enum defs + user source (for missing scenarios)
        static std::string withEnums(
            std::initializer_list<std::string_view> enums, const std::string& source
        ) {
            std::string result;
            for (auto e : enums) {
                result += e;
            }
            result += "\n";
            result += source;
            return result;
        }
    };

}  // namespace

// ============================================================
// LOGIC_TYPE 标识符检查（无设备上下文）
// LOGIC_TYPE identifier check (no device context)
// ============================================================

/// @brief 合法 LOGIC_TYPE 标识符不应产生诊断 / Valid LOGIC_TYPE identifier produces no diagnostic
TEST_F(SemanticTestFixture, ValidLogicTypeNoDiagnostic) {
    // lb 指令: REG_IDENT, REG_NUM, LOGIC_TYPE, BATCH_MODE
    // Pressure 在 LogicType 枚举中，Average 在 BatchMode 枚举中
    auto source = withStdLib("lb r0 100 Pressure Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA15_1"))
        << "Pressure 是合法的 LogicType，不应上报 IWA15_1";
}

/// @brief 非法 LOGIC_TYPE 标识符应上报 IWA15_1 / Invalid LOGIC_TYPE identifier reports IWA15_1
TEST_F(SemanticTestFixture, InvalidLogicTypeReportsIWA15_1) {
    auto source = withStdLib("lb r0 100 BadLogicName Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IWA15_1"))
        << "BadLogicName 不在 LogicType 枚举中，应上报 IWA15_1";
}

/// @brief LOGIC_TYPE 缺失标准库枚举应上报 IEA8_1 / Missing LogicType enum reports IEA8_1
TEST_F(SemanticTestFixture, MissingLogicTypeEnumReportsIEA8_1) {
    // 仅加载 BatchMode，不加载 LogicType
    auto source = withEnums({kBatchModeEnum}, "lb r0 100 Pressure Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IEA8_1"))
        << "LogicType 枚举未定义，应上报 IEA8_1";
}

// ============================================================
// LOGIC_SLOT 标识符检查（无设备上下文）
// LOGIC_SLOT identifier check (no device context)
// ============================================================

/// @brief 合法 LOGIC_SLOT 标识符不应产生诊断 / Valid LOGIC_SLOT identifier produces no diagnostic
TEST_F(SemanticTestFixture, ValidLogicSlotNoDiagnostic) {
    // lbs 指令: REG_IDENT, REG_NUM, SLOT_IDX, LOGIC_SLOT, BATCH_MODE
    // Quantity 在 LogicSlotType 枚举中
    auto source = withStdLib("lbs r0 100 0 Quantity Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA12_1"))
        << "Quantity 是合法的 LogicSlotType，不应上报 IWA12_1";
}

/// @brief 非法 LOGIC_SLOT 标识符应上报 IWA12_1 / Invalid LOGIC_SLOT identifier reports IWA12_1
TEST_F(SemanticTestFixture, InvalidLogicSlotReportsIWA12_1) {
    auto source = withStdLib("lbs r0 100 0 BadSlotName Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IWA12_1"))
        << "BadSlotName 不在 LogicSlotType 枚举中，应上报 IWA12_1";
}

// ============================================================
// REAGENT_MODE 标识符检查
// REAGENT_MODE identifier check
// ============================================================

/// @brief 合法 REAGENT_MODE 标识符不应产生诊断 / Valid REAGENT_MODE identifier produces no diagnostic
TEST_F(SemanticTestFixture, ValidReagentModeNoDiagnostic) {
    // lr 指令: REG_IDENT, DEV_REF, REAGENT_MODE, JUMP_TARGET
    // Contents 在 ReagentMode 枚举中
    // 注意: 使用 d0 (Device 节点) 而非 db (未定义 Identifier) 作为 DEV_REF，
    //       因为 process 是 Task<void>，await resolveSymbol 的 Future 时无法注册为等待者，
    //       未定义的 DEV_REF 会导致 process 永久挂起，后续 REAGENT_MODE 检查不会执行。
    //       Note: use d0 (Device node) instead of db (undefined Identifier) as DEV_REF,
    //       because process is Task<void> and cannot register as a waiter when awaiting
    //       resolveSymbol's Future. An undefined DEV_REF would cause process to suspend
    //       forever, preventing subsequent REAGENT_MODE checks from running.
    auto source = withStdLib(
        "main:\n"
        "lr r0 d0 Contents main\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA13_1"))
        << "Contents 是合法的 ReagentMode，不应上报 IWA13_1";
}

/// @brief 非法 REAGENT_MODE 标识符应上报 IWA13_1 / Invalid REAGENT_MODE identifier reports IWA13_1
TEST_F(SemanticTestFixture, InvalidReagentModeReportsIWA13_1) {
    // 同上，使用 d0 避免 DEV_REF 挂起 / Same as above, use d0 to avoid DEV_REF suspension
    auto source = withStdLib(
        "main:\n"
        "lr r0 d0 BadReagentMode main\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IWA13_1"))
        << "BadReagentMode 不在 ReagentMode 枚举中，应上报 IWA13_1";
}

// ============================================================
// BATCH_MODE 标识符检查
// BATCH_MODE identifier check
// ============================================================

/// @brief 合法 BATCH_MODE 标识符不应产生诊断 / Valid BATCH_MODE identifier produces no diagnostic
TEST_F(SemanticTestFixture, ValidBatchModeNoDiagnostic) {
    auto source = withStdLib("lb r0 100 Pressure Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA17_1"))
        << "Average 是合法的 BatchMode，不应上报 IWA17_1";
}

/// @brief 非法 BATCH_MODE 标识符应上报 IWA17_1 / Invalid BATCH_MODE identifier reports IWA17_1
TEST_F(SemanticTestFixture, InvalidBatchModeReportsIWA17_1) {
    auto source = withStdLib("lb r0 100 Pressure BadBatchMode\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IWA17_1"))
        << "BadBatchMode 不在 BatchMode 枚举中，应上报 IWA17_1";
}

// ============================================================
// 数字回退：LOGIC_TYPE / LOGIC_SLOT / REAGENT_MODE / BATCH_MODE 接受数字
// Number fallback: these operand types accept numeric literals
// ============================================================

/// @brief LOGIC_TYPE 接受数字字面量 / LOGIC_TYPE accepts numeric literal
TEST_F(SemanticTestFixture, LogicTypeAcceptsNumber) {
    auto source = withStdLib("lb r0 100 5 Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA15_1"))
        << "数字 5 是合法的 LOGIC_TYPE 值";
}

/// @brief BATCH_MODE 接受数字字面量 / BATCH_MODE accepts numeric literal
TEST_F(SemanticTestFixture, BatchModeAcceptsNumber) {
    auto source = withStdLib("lb r0 100 Pressure 0\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA17_1"))
        << "数字 0 是合法的 BATCH_MODE 值";
}

// ============================================================
// 设备上下文：alias 带类型注解 + 后续 LOGIC_TYPE 检查
// Device context: alias with type annotation + subsequent LOGIC_TYPE check
// ============================================================

/// @brief 设备上下文下合法 LOGIC_TYPE 不应产生诊断
///        Valid LOGIC_TYPE with device context produces no diagnostic
TEST_F(SemanticTestFixture, DeviceContextValidLogicTypeNoDiagnostic) {
    // alias dev d0 #: @type TestDevice  → pendingDeviceSymbol_.type.typeName = "TestDevice"
    // l r0 dev Pressure → Pressure 在 TestDevice.logics 中
    auto source = withStdLib(
        "alias dev d0 #: @type TestDevice\n"
        "l r0 dev Pressure\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA14_2"))
        << "Pressure 在 TestDevice 的 logics 中，不应上报 IWA14_2";
}

/// @brief 设备上下文下非法 LOGIC_TYPE 应上报 IWA14_2（恰好一次）
///        Invalid LOGIC_TYPE with device context reports IWA14_2 exactly once
TEST_F(SemanticTestFixture, DeviceContextInvalidLogicTypeReportsIWA14_2) {
    auto source = withStdLib(
        "alias dev d0 #: @type TestDevice\n"
        "l r0 dev BadLogicName\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_EQ(countDiagnostic(result.analyserDiags, "IWA14_2"), 1u)
        << "BadLogicName 不在 TestDevice 的 logics 中，应恰好上报一次 IWA14_2";
}

// ============================================================
// 设备上下文：LOGIC_SLOT 检查
// Device context: LOGIC_SLOT check
// ============================================================

/// @brief 设备上下文下合法 LOGIC_SLOT 不应产生诊断
///        Valid LOGIC_SLOT with device context produces no diagnostic
TEST_F(SemanticTestFixture, DeviceContextValidLogicSlotNoDiagnostic) {
    // ls 指令: REG_IDENT, DEV_REF, SLOT_IDX, LOGIC_SLOT
    // Quantity 在 TestDevice.logicSlots 中
    auto source = withStdLib(
        "alias dev d0 #: @type TestDevice\n"
        "ls r0 dev 0 Quantity\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA11_2"))
        << "Quantity 在 TestDevice 的 logicSlots 中，不应上报 IWA11_2";
}

/// @brief 设备上下文下非法 LOGIC_SLOT 应上报 IWA11_2（恰好一次）
///        Invalid LOGIC_SLOT with device context reports IWA11_2 exactly once
TEST_F(SemanticTestFixture, DeviceContextInvalidLogicSlotReportsIWA11_2) {
    auto source = withStdLib(
        "alias dev d0 #: @type TestDevice\n"
        "ls r0 dev 0 BadSlotName\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_EQ(countDiagnostic(result.analyserDiags, "IWA11_2"), 1u)
        << "BadSlotName 不在 TestDevice 的 logicSlots 中，应恰好上报一次 IWA11_2";
}

// ============================================================
// 设备上下文：SLOT_IDX 检查
// Device context: SLOT_IDX check
// ============================================================

/// @brief 设备上下文下合法 SLOT_IDX 不应产生诊断
///        Valid SLOT_IDX with device context produces no diagnostic
TEST_F(SemanticTestFixture, DeviceContextValidSlotIdxNoDiagnostic) {
    // TestDevice 定义了 slot 0 和 slot 1
    auto source = withStdLib(
        "alias dev d0 #: @type TestDevice\n"
        "ls r0 dev 0 Quantity\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA16_2"))
        << "0 在 TestDevice 的 slots 中，不应上报 IWA16_2";
}

/// @brief 设备上下文下非法 SLOT_IDX 应上报 IWA16_2
///        Invalid SLOT_IDX with device context reports IWA16_2
TEST_F(SemanticTestFixture, DeviceContextInvalidSlotIdxReportsIWA16_2) {
    // TestDevice 只有 slot 0 和 1，99 不在范围内
    auto source = withStdLib(
        "alias dev d0 #: @type TestDevice\n"
        "ls r0 dev 99 Quantity\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IWA16_2"))
        << "99 不在 TestDevice 的 slots 中，应上报 IWA16_2";
}

// ============================================================
// 设备上下文隔离：每条指令开始时重置 pendingDeviceSymbol_
// Device context isolation: pendingDeviceSymbol_ reset at instruction boundary
// ============================================================

/// @brief 设备上下文不应跨指令泄漏 / Device context should not leak across instructions
TEST_F(SemanticTestFixture, DeviceContextResetAcrossInstructions) {
    // 第一条指令设置 dev 上下文，第二条指令不带 dev，LOGIC_TYPE 应走全局枚举检查
    auto source = withStdLib(
        "alias dev d0 #: @type TestDevice\n"
        "l r0 dev Pressure\n"
        "lb r0 100 Pressure Average\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    // 两条指令的 Pressure 都合法，不应有任何 IWA 诊断
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA14_2"));
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA15_1"));
}

// ============================================================
// 未定义符号检查（IE0_1）
// Undefined symbol check (IE0_1)
// ============================================================

/// @brief 未定义的设备别名引用应上报 IE0_1 / Undefined device alias reference reports IE0_1
TEST_F(SemanticTestFixture, UndefinedDeviceAliasReportsIE0_1) {
    auto source = withStdLib(
        "l r0 undefinedDev Pressure\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IE0_1"))
        << "undefinedDev 未定义，应上报 IE0_1";
}
