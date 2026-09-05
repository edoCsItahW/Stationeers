// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

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
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/semantic/analyser.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/lexer/lexer.hpp"

#include <gtest/gtest.h>

#include <initializer_list>
#include <algorithm>
#include <ranges>
#include <string>

using namespace stationeers::ic10;
using stationeers::Diagnostic;
using stationeers::Pos;

namespace {

    /// @brief 语义分析测试夹具 / Semantic analysis test fixture
    class SemanticTestFixture : public ::testing::Test {
    protected:
        static void SetUpTestSuite() {
            ICLoc::registerLanguage<ZhHans>("zh-hans");
            ICLoc::setLanguage("zh-hans");
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
                out += "  [" + d.id + "] L" + std::to_string((d.start ? d.start->line() : 1)) + ":"
                     + std::to_string((d.start ? d.start->column() : 1)) + " - " + d.message + "\n";
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
            "#> @value Pressure 5 \"Pressure\"\n"
            "#> @value Setting 12 \"Setting\"\n"
            "#> @value Temperature 6 \"Temperature\"\n"
            "#> @value On 28 \"On\"\n"
            "#> @value Open 21 \"Open\"\n"
            "#> @value Color 189 \"Color\"\n"
            "#> @value TotalMoles 66 \"TotalMoles\"\n"
            "#> @end-enum\n";

        static constexpr std::string_view kLogicSlotTypeEnum =
            "#> @enum\n"
            "#> @name LogicSlotType\n"
            "#> @value Quantity 3 \"Quantity\"\n"
            "#> @value Charge 10 \"Charge\"\n"
            "#> @value Damage 4 \"Damage\"\n"
            "#> @end-enum\n";

        static constexpr std::string_view kReagentModeEnum =
            "#> @enum\n"
            "#> @name ReagentMode\n"
            "#> @value Contents 0 \"Contents\"\n"
            "#> @value Recipe 2 \"Recipe\"\n"
            "#> @value Required 1 \"Required\"\n"
            "#> @value TotalContents 3 \"TotalContents\"\n"
            "#> @end-enum\n";

        static constexpr std::string_view kBatchModeEnum =
            "#> @enum\n"
            "#> @name BatchMode\n"
            "#> @value Average 0 \"Average\"\n"
            "#> @value Sum 1 \"Sum\"\n"
            "#> @value Minimum 2 \"Minimum\"\n"
            "#> @value Maximum 3 \"Maximum\"\n"
            "#> @value Count 4 \"Count\"\n"
            "#> @end-enum\n";

        /// @brief 测试设备：含 logics/logicSlots/slots / Test device with logics/logicSlots/slots
        static constexpr std::string_view kTestDevice =
            "#> @device\n"
            "#> @name TestDevice\n"
            "#> @logic Pressure\n"
            "#> @logic Setting\n"
            "#> @logic On\n"
            "#> @logic-slot Quantity\n"
            "#> @logic-slot Charge\n"
            "#> @slot 0\n"
            "#> @slot 1\n"
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

// ============================================================
// SymbolTable 单元测试
// SymbolTable unit tests
// ============================================================

/// @brief 符号表测试夹具 / Symbol table test fixture
class SymbolTableTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("zh-hans");
        ICLoc::setLanguage("zh-hans");
    }

    /// @brief 创建一个整数类型符号 / Create an integer type symbol
    static std::shared_ptr<Symbol> makeIntSymbol(const std::string& name, const std::string& value = {}) {
        auto sym = std::make_shared<Symbol>();
        sym->name = name;
        sym->type = type_of<Integer>;
        if (!value.empty()) sym->value = value;
        return sym;
    }

    /// @brief 创建一个寄存器类型符号 / Create a register type symbol
    static std::shared_ptr<Symbol> makeRegSymbol(const std::string& name) {
        auto sym = std::make_shared<Symbol>();
        sym->name = name;
        sym->type = type_of<GeneralPurposeRegister>;
        return sym;
    }

    /// @brief 创建一个设备类型符号 / Create a device type symbol
    static std::shared_ptr<Symbol> makeDeviceSymbol(const std::string& name) {
        auto sym = std::make_shared<Symbol>();
        sym->name = name;
        sym->type = type_of<StaticDevice>;
        return sym;
    }
};

/// @brief 空符号表 contains 返回 false / Empty symbol table contains returns false
TEST_F(SymbolTableTestFixture, EmptyTableContainsReturnsFalse) {
    SymbolTable st;
    EXPECT_FALSE(st.contains("anything"));
    EXPECT_FALSE(st.contains("foo"));
}

/// @brief define 后 contains 返回 true / contains returns true after define
TEST_F(SymbolTableTestFixture, DefineThenContainsReturnsTrue) {
    SymbolTable st;
    auto result = st.define("x", makeIntSymbol("x", "42"));
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(st.contains("x"));
}

/// @brief 重复定义返回错误 / Redefinition returns error
TEST_F(SymbolTableTestFixture, RedefinitionReturnsError) {
    SymbolTable st;
    (void)st.define("x", makeIntSymbol("x", "1"));
    auto result = st.define("x", makeIntSymbol("x", "2"));
    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(result.error().empty());
}

/// @brief 重复定义后符号值不变 / Symbol value unchanged after redefinition attempt
TEST_F(SymbolTableTestFixture, RedefinitionKeepsOriginalValue) {
    SymbolTable st;
    (void)st.define("x", makeIntSymbol("x", "1"));
    (void)st.define("x", makeIntSymbol("x", "2"));
    // 从 toJSON 或其他方式验证值保持不变
    auto json = st.toJSON();
    EXPECT_NE(json.find("\"1\""), std::string::npos);
}

/// @brief resolve 创建 pending 条目，contains 返回 false
///        resolve creates pending entry, contains returns false
TEST_F(SymbolTableTestFixture, ResolveCreatesPendingEntry) {
    SymbolTable st;
    Pos pos(1, 1, 0);
    auto& future = st.resolve("x", pos);
    EXPECT_FALSE(st.contains("x"));
    EXPECT_FALSE(future.isReady());
}

/// @brief resolve 后 define 填充 Future / define after resolve fills the Future
TEST_F(SymbolTableTestFixture, DefineAfterResolveFillsFuture) {
    SymbolTable st;
    Pos pos(1, 1, 0);
    auto& future = st.resolve("x", pos);
    EXPECT_FALSE(future.isReady());

    auto result = st.define("x", makeIntSymbol("x", "42"));
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(future.isReady());
    EXPECT_TRUE(st.contains("x"));

    auto sym = future.get();
    ASSERT_TRUE(sym.has_value());
    EXPECT_EQ(sym.value()->name, "x");
    EXPECT_EQ(sym.value()->value, "42");
}

/// @brief 对同一符号多次 resolve 返回同一 Future
///        Multiple resolve calls for same symbol return the same Future
TEST_F(SymbolTableTestFixture, MultipleResolveSameFuture) {
    SymbolTable st;
    Pos pos1(1, 1, 0);
    Pos pos2(2, 1, 10);
    auto& fut1 = st.resolve("x", pos1);
    auto& fut2 = st.resolve("x", pos2);
    EXPECT_EQ(&fut1, &fut2);
}

/// @brief failAllPending 标记未决符号为失败
///        failAllPending marks pending symbols as failed
TEST_F(SymbolTableTestFixture, FailAllPendingMarksFailed) {
    SymbolTable st;
    Pos pos(1, 1, 0);
    auto& future = st.resolve("undefined_sym", pos);
    EXPECT_FALSE(future.isReady());

    st.failAllPending();
    // failAllPending 设置 FAILED 状态，isReady() 只对 READY 状态返回 true
    EXPECT_FALSE(future.isReady());
    EXPECT_FALSE(st.contains("undefined_sym"));
}

/// @brief failAllPending 不影响已定义符号
///        failAllPending does not affect defined symbols
TEST_F(SymbolTableTestFixture, FailAllPendingDoesNotAffectDefined) {
    SymbolTable st;
    (void)st.define("defined", makeIntSymbol("defined", "1"));
    Pos pos(1, 1, 0);
    (void)st.resolve("pending", pos);

    st.failAllPending();
    EXPECT_TRUE(st.contains("defined"));
    EXPECT_FALSE(st.contains("pending"));
}

/// @brief 空符号表 toJSON 为空数组 / Empty symbol table toJSON is empty array
TEST_F(SymbolTableTestFixture, EmptyTableToJSONEmptyArray) {
    SymbolTable st;
    auto json = st.toJSON();
    EXPECT_EQ(json, "{}");
}

/// @brief 符号表 toJSON 包含已定义符号 / Symbol table toJSON includes defined symbols
TEST_F(SymbolTableTestFixture, ToJSONIncludesDefinedSymbols) {
    SymbolTable st;
    (void)st.define("foo", makeIntSymbol("foo", "42"));
    (void)st.define("bar", makeRegSymbol("bar"));

    auto json = st.toJSON();
    EXPECT_NE(json.find("foo"), std::string::npos);
    EXPECT_NE(json.find("bar"), std::string::npos);
    EXPECT_NE(json.find("42"), std::string::npos);
}

/// @brief 符号表 toJSON 不包含未决符号 / Symbol table toJSON excludes pending symbols
TEST_F(SymbolTableTestFixture, ToJSONExcludesPendingSymbols) {
    SymbolTable st;
    (void)st.define("defined", makeIntSymbol("defined", "1"));
    Pos pos(1, 1, 0);
    (void)st.resolve("pending", pos);

    auto json = st.toJSON();
    EXPECT_NE(json.find("defined"), std::string::npos);
    EXPECT_EQ(json.find("pending"), std::string::npos);
}

/// @brief 符号表内置 d0-d5 设备符号
///        Symbol table has builtin d0-d5 device symbols
TEST_F(SymbolTableTestFixture, BuiltinDeviceSymbolsExist) {
    SymbolTable st;

    // 1. 验证 size 不为 0
    EXPECT_GT(st.builtinSymbols.size(), 0u) << "builtinSymbols should not be empty";

    // 2. 验证 d0 存在且 name 正确
    auto it0 = st.builtinSymbols.find("d0");
    ASSERT_NE(it0, st.builtinSymbols.end()) << "d0 should exist in builtinSymbols";
    EXPECT_EQ(it0->second.name, "d0") << "d0 symbol name should be d0";

    // 3. 验证 d5 存在
    auto it5 = st.builtinSymbols.find("d5");
    ASSERT_NE(it5, st.builtinSymbols.end()) << "d5 should exist in builtinSymbols";

    // 4. 验证 d6 不存在
    EXPECT_EQ(st.builtinSymbols.find("d6"), st.builtinSymbols.end()) << "d6 should not exist";

    // 5. 验证总数为 6
    EXPECT_EQ(st.builtinSymbols.size(), 6u) << "builtinSymbols should have 6 entries";

    // 6. 验证内置符号类型是 DEVICE
    EXPECT_EQ(it0->second.type.kind, BasicType::DEVICE) << "d0 type should be DEVICE";
}

/// @brief find 能找到已定义符号 / find finds defined symbols
TEST_F(SymbolTableTestFixture, FindDefinedSymbol) {
    SymbolTable st;
    (void)st.define("x", makeIntSymbol("x"));
    auto it = st.find("x");
    EXPECT_NE(it, st.end());
    EXPECT_TRUE(it->second.ready());
}

/// @brief find 找不到未定义符号 / find returns end for undefined symbol
TEST_F(SymbolTableTestFixture, FindUndefinedReturnsEnd) {
    SymbolTable st;
    auto it = st.find("nonexistent");
    EXPECT_EQ(it, st.end());
}

/// @brief find 能找到 pending 条目（与 contains 不同）
///        find finds pending entries (unlike contains)
TEST_F(SymbolTableTestFixture, FindPendingEntryFound) {
    SymbolTable st;
    Pos pos(1, 1, 0);
    (void)st.resolve("pending", pos);
    auto it = st.find("pending");
    EXPECT_NE(it, st.end());
    EXPECT_FALSE(it->second.ready());
}

/// @brief begin/end 迭代器遍历所有条目（包括 pending）
///        begin/end iterators iterate all entries (including pending)
TEST_F(SymbolTableTestFixture, BeginEndIterateAllEntries) {
    SymbolTable st;
    (void)st.define("defined", makeIntSymbol("defined"));
    Pos pos(1, 1, 0);
    (void)st.resolve("pending", pos);

    int count = 0;
    for (auto it = st.begin(); it != st.end(); ++it) {
        ++count;
    }
    EXPECT_EQ(count, 2);
}

/// @brief Symbol::toJSON 输出正确格式 / Symbol::toJSON outputs correct format
TEST_F(SymbolTableTestFixture, SymbolToJSONFormat) {
    auto sym = makeIntSymbol("testSym", "100");
    String descStr;
    descStr.value = "test description";
    sym->desc = descStr;
    auto json = sym->toJSON();

    EXPECT_NE(json.find("\"name\""), std::string::npos);
    EXPECT_NE(json.find("testSym"), std::string::npos);
    EXPECT_NE(json.find("\"type\""), std::string::npos);
    EXPECT_NE(json.find("\"value\""), std::string::npos);
    EXPECT_NE(json.find("\"desc\""), std::string::npos);
    EXPECT_NE(json.find("test description"), std::string::npos);
}

/// @brief 符号带 typeName 的 toJSON 输出
///        Symbol with typeName toJSON output
TEST_F(SymbolTableTestFixture, SymbolWithTypeNameToJSON) {
    auto sym = makeDeviceSymbol("dev");
    sym->type.typeName = "Furnace";
    auto json = sym->toJSON();

    EXPECT_NE(json.find("typeName"), std::string::npos);
    EXPECT_NE(json.find("Furnace"), std::string::npos);
}

// ============================================================
// TypeTable 单元测试
// TypeTable unit tests
// ============================================================

/// @brief 类型表测试夹具 / Type table test fixture
class TypeTableTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("zh-hans");
        ICLoc::setLanguage("zh-hans");
    }
};

/// @brief 空类型表 find 返回 nullptr / Empty type table find returns nullptr
TEST_F(TypeTableTestFixture, EmptyTableFindReturnsNull) {
    TypeTable tt;
    EXPECT_EQ(tt.find("Anything"), nullptr);
}

/// @brief registerType 后 find 可找到 / find finds after registerType
TEST_F(TypeTableTestFixture, RegisterThenFindSucceeds) {
    TypeTable tt;
    EnumAnnotation enumType;
    enumType.name = "TestEnum";
    tt.registerType(CustomType{enumType});

    auto* found = tt.find("TestEnum");
    ASSERT_NE(found, nullptr);
    ASSERT_TRUE(std::holds_alternative<EnumAnnotation>(*found));
    EXPECT_EQ(std::get<EnumAnnotation>(*found).name, "TestEnum");
}

/// @brief 注册设备类型后可找到 / Registered device type can be found
TEST_F(TypeTableTestFixture, RegisterDeviceType) {
    TypeTable tt;
    DeviceAnnotation devType;
    devType.name = "TestDevice";
    tt.registerType(CustomType{devType});

    auto* found = tt.find("TestDevice");
    ASSERT_NE(found, nullptr);
    ASSERT_TRUE(std::holds_alternative<DeviceAnnotation>(*found));
    EXPECT_EQ(std::get<DeviceAnnotation>(*found).name, "TestDevice");
}

/// @brief 注册多个类型后都可找到 / Multiple registered types all findable
TEST_F(TypeTableTestFixture, RegisterMultipleTypes) {
    TypeTable tt;

    EnumAnnotation enumType;
    enumType.name = "EnumA";
    tt.registerType(CustomType{enumType});

    DeviceAnnotation devType;
    devType.name = "DevB";
    tt.registerType(CustomType{devType});

    EXPECT_NE(tt.find("EnumA"), nullptr);
    EXPECT_NE(tt.find("DevB"), nullptr);
    EXPECT_EQ(tt.find("NonExistent"), nullptr);
}

/// @brief 设备类型包含 slots/logics 等信息
///        Device type includes slots/logics info
TEST_F(TypeTableTestFixture, DeviceTypeHasSlotsAndLogics) {
    TypeTable tt;
    DeviceAnnotation devType;
    devType.name = "Sensor";

    DeviceAnnotationSlot slot;
    slot.value = "0";
    devType.slots.push_back(slot);

    DeviceAnnotationLogic logic;
    logic.value = "Pressure";
    devType.logics.push_back(logic);

    tt.registerType(CustomType{devType});

    auto* found = tt.find("Sensor");
    ASSERT_NE(found, nullptr);
    auto& dev = std::get<DeviceAnnotation>(*found);
    EXPECT_EQ(dev.slots.size(), 1u);
    EXPECT_EQ(dev.logics.size(), 1u);
    EXPECT_EQ(dev.slots[0].value, "0");
    EXPECT_EQ(dev.logics[0].value, "Pressure");
}

/// @brief 枚举类型包含值列表 / Enum type contains value list
TEST_F(TypeTableTestFixture, EnumTypeHasValues) {
    TypeTable tt;
    EnumAnnotation enumType;
    enumType.name = "ModeType";

    EnumAnnotationValue val1;
    val1.name = "ModeA";
    val1.value = "0";
    enumType.values.push_back(val1);

    EnumAnnotationValue val2;
    val2.name = "ModeB";
    val2.value = "1";
    enumType.values.push_back(val2);

    tt.registerType(CustomType{enumType});

    auto* found = tt.find("ModeType");
    ASSERT_NE(found, nullptr);
    auto& en = std::get<EnumAnnotation>(*found);
    EXPECT_EQ(en.values.size(), 2u);
    EXPECT_EQ(en.values[0].name, "ModeA");
    EXPECT_EQ(en.values[1].value, "1");
}

// ============================================================
// 语义分析扩展测试：更多边界场景
// Extended semantic analysis tests: more boundary scenarios
// ============================================================

/// @brief alias 重定义应上报 IEA2_1 / Alias redefinition reports IEA2_1
TEST_F(SemanticTestFixture, AliasRedefinitionReportsIEA2_1) {
    auto source = withStdLib(
        "alias foo r0\n"
        "alias foo r1\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IEA2_1"))
        << "alias foo 重复定义，应上报 IEA2_1";
}

/// @brief define 重定义应上报 IEA2_1 / Define redefinition reports IEA2_1
TEST_F(SemanticTestFixture, DefineRedefinitionReportsIEA2_1) {
    auto source = withStdLib(
        "define VAL 10\n"
        "define VAL 20\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IEA2_1"))
        << "define VAL 重复定义，应上报 IEA2_1";
}

/// @brief 标签重定义应上报 IEA2_1 / Label redefinition reports IEA2_1
TEST_F(SemanticTestFixture, LabelRedefinitionReportsIEA2_1) {
    auto source = withStdLib(
        "loop:\n"
        "loop:\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IEA2_1"))
        << "标签 loop 重复定义，应上报 IEA2_1";
}

/// @brief 未定义跳转目标应上报 IE0_1 / Undefined jump target reports IE0_1
TEST_F(SemanticTestFixture, UndefinedJumpTargetReportsIE0_1) {
    auto source = withStdLib(
        "j nonexistent\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IE0_1"))
        << "nonexistent 标签未定义，应上报 IE0_1";
}

/// @brief 前向引用标签合法，不上报错误 / Forward label reference is legal
TEST_F(SemanticTestFixture, ForwardLabelReferenceNoError) {
    auto source = withStdLib(
        "j end\n"
        "end:\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(result.analyser->getSymbolTable().contains("end"));
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IE0_1"))
        << "前向引用标签是合法的，不应上报 IE0_1";
}

/// @brief LOGIC_SLOT 缺失枚举应上报 IEA8_1 / Missing LogicSlotType enum reports IEA8_1
TEST_F(SemanticTestFixture, MissingLogicSlotTypeEnumReportsIEA8_1) {
    // 仅加载 BatchMode 和 LogicType，不加载 LogicSlotType
    auto source = withEnums(
        {kBatchModeEnum, kLogicTypeEnum},
        "lbs r0 100 0 Quantity Average\nhcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IEA8_1"))
        << "LogicSlotType 枚举未定义，应上报 IEA8_1";
}

/// @brief REAGENT_MODE 缺失枚举应上报 IEA8_1 / Missing ReagentMode enum reports IEA8_1
TEST_F(SemanticTestFixture, MissingReagentModeEnumReportsIEA8_1) {
    // 不加载 ReagentMode
    auto source = withEnums(
        {kLogicTypeEnum},
        "main:\n"
        "lr r0 d0 Contents main\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IEA8_1"))
        << "ReagentMode 枚举未定义，应上报 IEA8_1";
}

/// @brief BATCH_MODE 缺失枚举应上报 IEA8_1 / Missing BatchMode enum reports IEA8_1
TEST_F(SemanticTestFixture, MissingBatchModeEnumReportsIEA8_1) {
    // 仅加载 LogicType，不加载 BatchMode
    auto source = withEnums(
        {kLogicTypeEnum},
        "lb r0 100 Pressure Average\nhcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_TRUE(hasDiagnostic(result.analyserDiags, "IEA8_1"))
        << "BatchMode 枚举未定义，应上报 IEA8_1";
}

/// @brief LOGIC_SLOT 接受数字字面量 / LOGIC_SLOT accepts numeric literal
TEST_F(SemanticTestFixture, LogicSlotAcceptsNumber) {
    auto source = withStdLib("lbs r0 100 0 3 Average\nhcf\n");
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA12_1"))
        << "数字 3 是合法的 LOGIC_SLOT 值";
}

/// @brief REAGENT_MODE 接受数字字面量 / REAGENT_MODE accepts numeric literal
TEST_F(SemanticTestFixture, ReagentModeAcceptsNumber) {
    auto source = withStdLib(
        "main:\n"
        "lr r0 d0 0 main\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IWA13_1"))
        << "数字 0 是合法的 REAGENT_MODE 值";
}

/// @brief alias 类型提示指向不存在类型时不崩溃 / Alias with nonexistent type hint does not crash
TEST_F(SemanticTestFixture, AliasWithNonexistentTypeHintNoCrash) {
    auto source = withStdLib(
        "alias dev d0 #: @type NonexistentType\n"
        "l r0 dev Pressure\n"
        "hcf\n"
    );
    EXPECT_NO_THROW({
        auto result = compile(source);
        (void)result;
    });
}

/// @brief 多个未定义符号都应上报 / Multiple undefined symbols all reported
TEST_F(SemanticTestFixture, MultipleUndefinedSymbolsAllReported) {
    auto source = withStdLib(
        "move r0 undefA\n"
        "move r1 undefB\n"
        "j undefC\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    // 至少应有3个未定义符号相关错误
    EXPECT_GE(result.analyserDiags.size(), 3u);
}

/// @brief 符号表包含 alias 定义的符号 / Symbol table contains alias-defined symbols
TEST_F(SemanticTestFixture, SymbolTableContainsAliasSymbols) {
    auto source = withStdLib(
        "alias myReg r0\n"
        "alias myDev d0\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("myReg"));
    EXPECT_TRUE(symtab.contains("myDev"));
}

/// @brief 符号表包含 define 定义的符号 / Symbol table contains define-defined symbols
TEST_F(SemanticTestFixture, SymbolTableContainsDefineSymbols) {
    auto source = withStdLib(
        "define MAX 100\n"
        "define NAME HASH(\"test\")\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("MAX"));
    EXPECT_TRUE(symtab.contains("NAME"));
}

/// @brief 符号表包含标签定义 / Symbol table contains label definitions
TEST_F(SemanticTestFixture, SymbolTableContainsLabels) {
    auto source = withStdLib(
        "start:\n"
        "loop:\n"
        "j end\n"
        "end:\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("start"));
    EXPECT_TRUE(symtab.contains("loop"));
    EXPECT_TRUE(symtab.contains("end"));
}

/// @brief 设备别名通过 alias 定义后可被引用 / Device alias defined by alias can be referenced
TEST_F(SemanticTestFixture, DeviceAliasDefinedByAliasCanBeReferenced) {
    auto source = withStdLib(
        "alias myDev d0\n"
        "l r0 myDev Pressure\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("myDev"));
}

/// @brief 常量引用（pi/nan/rgas 等）不应产生未定义错误
///        Constant references (pi/nan/rgas etc.) should not produce undefined errors
TEST_F(SemanticTestFixture, ConstantsReferenceNoUndefinedError) {
    // 常量现在通过标准库的 define + @builtin 定义，此处模拟标准库定义后引用
    auto source = withStdLib(
        "define pi \"pi\" #: @builtin\n"
        "define nan \"nan\" #: @builtin\n"
        "define rgas \"rgas\" #: @builtin\n"
        "move r0 pi\n"
        "move r1 nan\n"
        "move r2 rgas\n"
        "hcf\n"
    );
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    assertNoLexerParserDiags(result);
    // 常量已由标准库定义，不应产生未定义错误
    EXPECT_FALSE(hasDiagnostic(result.analyserDiags, "IE0_1"));
}

/// @brief 空程序语义分析无错误 / Empty program semantic analysis has no errors
TEST_F(SemanticTestFixture, EmptyProgramNoErrors) {
    auto result = compile("");

    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    EXPECT_TRUE(result.analyserDiags.empty());
    EXPECT_EQ(result.ast.statements.size(), 0u);
}

/// @brief 仅含文档注释的程序无错误 / Program with only doc comments has no errors
TEST_F(SemanticTestFixture, DocCommentOnlyProgramNoErrors) {
    auto source = std::string(kLogicTypeEnum) + "\n" + std::string(kTestDevice);
    auto result = compile(source);

    SCOPED_TRACE(formatDiags(result.analyserDiags));
    EXPECT_TRUE(result.lexerDiags.empty());
    EXPECT_TRUE(result.parserDiags.empty());
    // 文档注释定义类型，不应有语义错误
    EXPECT_TRUE(result.analyserDiags.empty());
}
