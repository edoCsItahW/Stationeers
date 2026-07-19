// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

/**
 * @file test_linker.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/19
 * @if zh
 * @brief IC10 连接器测试
 * @details 针对接器（Linker）进行单元测试，覆盖：
 *          - 单单元链接（addUnit(string) / addUnit(Program) 两种重载）
 *          - 多单元符号合并
 *          - 跨单元前向引用解析
 *          - 类型表跨单元共享（文档注释定义的设备/枚举类型）
 *          - 未定义符号处理（failAllPending 不崩溃）
 *          - 空链接器
 *          - 重复调用 link()
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 linker tests
 * @details Unit tests for the Linker, covering:
 *          - Single-unit linking (both addUnit overloads)
 *          - Multi-unit symbol merging
 *          - Cross-unit forward reference resolution
 *          - Type table sharing across units (doc-comment-defined device/enum types)
 *          - Undefined symbol handling (failAllPending does not crash)
 *          - Empty linker
 *          - Repeated link() calls
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "ic10/lexer/lexer.hpp"
#include "ic10/link/linker.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/semantic/analyser.hpp"

using namespace stationeers::ic10;

namespace {

    /// @brief 连接器测试夹具 / Linker test fixture
    class LinkerTestFixture : public ::testing::Test {
    protected:
        static void SetUpTestSuite() {
            ILoc::registerLanguage<ZhHans>("zh-hans");
            ILoc::setLanguage("zh-hans");
        }

        /// @brief 手动构建 Program（绕过 addUnit(string)） / Build a Program manually
        static Program parse(const std::string& source) {
            auto tokens = Lexer::tokenize(source);
            Parser parser(tokens);
            return parser.parse();
        }
    };

}  // namespace

// ============================================================
// 单单元链接测试
// ============================================================

TEST_F(LinkerTestFixture, SingleUnitStringAddsAliasAndDefine) {
    // addUnit(string) 应将源码编译为 Program 并在 link() 后将符号注册到符号表
    Linker linker;
    linker.addUnit("alias dev d0\ndefine MAX 100\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("dev"));
    EXPECT_TRUE(symtab.contains("MAX"));
}

TEST_F(LinkerTestFixture, SingleUnitStringAddsLabel) {
    // 标签也应注册到符号表
    Linker linker;
    linker.addUnit("main:\nmove r0 1\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("main"));
}

TEST_F(LinkerTestFixture, SingleUnitProgramOverload) {
    // addUnit(Program) 重载应与 addUnit(string) 产生相同结果
    auto program = parse("alias dev d0\ndefine COUNT 42\nhcf\n");

    Linker linker;
    linker.addUnit(std::move(program));

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("dev"));
    EXPECT_TRUE(symtab.contains("COUNT"));
}

// ============================================================
// 多单元符号合并测试
// ============================================================

TEST_F(LinkerTestFixture, MultipleUnitsMergeSymbols) {
    // 两个单元的符号应合并到同一符号表
    Linker linker;
    linker.addUnit("alias devA d0\nhcf\n");
    linker.addUnit("alias devB d1\ndefine MAX 100\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("devA"));
    EXPECT_TRUE(symtab.contains("devB"));
    EXPECT_TRUE(symtab.contains("MAX"));
}

TEST_F(LinkerTestFixture, MultipleUnitsMergeLabels) {
    // 两个单元的标签应合并到同一符号表
    Linker linker;
    linker.addUnit("start:\nmove r0 1\nj end\n");
    linker.addUnit("end:\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("start"));
    EXPECT_TRUE(symtab.contains("end"));
}

// ============================================================
// 跨单元前向引用测试
// ============================================================

TEST_F(LinkerTestFixture, CrossUnitForwardLabelReference) {
    // 单元1引用单元2中定义的标签，link() 后标签应被解析
    Linker linker;
    linker.addUnit("j target\nhcf\n");
    linker.addUnit("target:\nmove r0 1\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("target"));
}

TEST_F(LinkerTestFixture, CrossUnitForwardAliasReference) {
    // 单元1引用单元2中定义的别名，link() 后别名应被解析
    Linker linker;
    linker.addUnit("l r0 dev Setting\nhcf\n");
    linker.addUnit("alias dev d0\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("dev"));
}

TEST_F(LinkerTestFixture, CrossUnitDefineReference) {
    // 单元1引用单元2中定义的常量
    Linker linker;
    linker.addUnit("move r0 LIMIT\nhcf\n");
    linker.addUnit("define LIMIT 100\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("LIMIT"));
}

// ============================================================
// 类型表跨单元共享测试
// ============================================================

TEST_F(LinkerTestFixture, SharedTypeTableDeviceType) {
    // 单元1定义设备类型，单元2使用该类型
    Linker linker;
    linker.addUnit(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Pressure rw\n"
        "#> @end-device\n"
    );
    linker.addUnit(
        "alias sensor d0 #: @type Sensor\n"
        "l r0 sensor Pressure\n"
        "hcf\n"
    );

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("sensor"));
}

TEST_F(LinkerTestFixture, SharedTypeTableEnumType) {
    // 单元1定义枚举类型，单元2使用该枚举
    Linker linker;
    linker.addUnit(
        "#> @enum\n"
        "#> @name ReagentMode\n"
        "#> @value Contents 0\n"
        "#> @value Required 1\n"
        "#> @end-enum\n"
    );
    linker.addUnit(
        "alias filter d0\n"
        "lr r0 filter Contents Oxygen\n"
        "hcf\n"
    );

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("filter"));
}

// ============================================================
// 未定义符号处理测试
// ============================================================

TEST_F(LinkerTestFixture, UndefinedSymbolDoesNotCrash) {
    // 引用未定义符号时，failAllPending 应安全处理，不崩溃
    Linker linker;
    linker.addUnit("move r0 UNDEFINED\nhcf\n");

    EXPECT_NO_THROW({
        auto& symtab = linker.link();
        (void)symtab;
    });
}

TEST_F(LinkerTestFixture, UndefinedSymbolNotInSymbolTable) {
    // 未定义符号不应出现在符号表中
    Linker linker;
    linker.addUnit("move r0 UNDEFINED\nhcf\n");

    auto& symtab = linker.link();

    EXPECT_FALSE(symtab.contains("UNDEFINED"));
}

// ============================================================
// 空链接器测试
// ============================================================

TEST_F(LinkerTestFixture, EmptyLinkerReturnsEmptySymbolTable) {
    // 没有 addUnit 直接 link() 应返回空符号表
    Linker linker;

    auto& symtab = linker.link();

    EXPECT_FALSE(symtab.contains("anything"));
}

// ============================================================
// 重复调用 link() 测试
// ============================================================

TEST_F(LinkerTestFixture, LinkReturnsSameSymbolTableReference) {
    // 多次调用 link() 应返回同一符号表引用
    Linker linker;
    linker.addUnit("alias dev d0\nhcf\n");

    auto& symtab1 = linker.link();
    auto& symtab2 = linker.link();

    EXPECT_EQ(&symtab1, &symtab2);
}

TEST_F(LinkerTestFixture, LinkAfterAddingMoreUnits) {
    // 第一次 link() 后，符号表已有符号；再添加单元并 link()，新符号应被加入
    Linker linker;
    linker.addUnit("alias devA d0\nhcf\n");

    auto& symtab1 = linker.link();
    EXPECT_TRUE(symtab1.contains("devA"));

    // 注意：link() 会遍历所有 units_，包括之前已处理过的单元
    // 这里验证 link() 可被重复调用而不崩溃
    linker.addUnit("alias devB d1\nhcf\n");
    auto& symtab2 = linker.link();
    EXPECT_TRUE(symtab2.contains("devA"));
    EXPECT_TRUE(symtab2.contains("devB"));
}

// ============================================================
// 混合 addUnit 重载测试
// ============================================================

TEST_F(LinkerTestFixture, MixedAddUnitOverloads) {
    // 混合使用 addUnit(string) 和 addUnit(Program)
    auto program = parse("alias devB d1\nhcf\n");

    Linker linker;
    linker.addUnit("alias devA d0\nhcf\n");
    linker.addUnit(std::move(program));

    auto& symtab = linker.link();

    EXPECT_TRUE(symtab.contains("devA"));
    EXPECT_TRUE(symtab.contains("devB"));
}

// ============================================================
// 符号表内容验证测试
// ============================================================

TEST_F(LinkerTestFixture, SymbolTableToJSONNotEmpty) {
    // link() 后符号表应可序列化为非空 JSON
    Linker linker;
    linker.addUnit("alias dev d0\ndefine MAX 100\nmain:\nhcf\n");

    auto& symtab = linker.link();

    auto json = symtab.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find('{'), std::string::npos);
    EXPECT_NE(json.find("dev"), std::string::npos);
    EXPECT_NE(json.find("MAX"), std::string::npos);
    EXPECT_NE(json.find("main"), std::string::npos);
}
