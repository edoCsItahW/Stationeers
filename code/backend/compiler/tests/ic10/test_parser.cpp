// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_parser.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/07 23:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/lexer/lexer.hpp"


using namespace stationeers::ic10;

class ParserTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ILoc::registerLanguage<ZhHans>("zh-hans");
        ILoc::setLanguage("zh-hans");
    }

    static Program parse(const std::string& source) {
        auto tokens = Lexer::tokenize(source);
        return Parser::parsing(tokens);
    }
};

// ============================================================
// 基础解析测试
// ============================================================

TEST_F(ParserTestFixture, EmptyProgram) {
    auto ast = parse("");
    EXPECT_EQ(ast.statements.size(), 0u);
}

TEST_F(ParserTestFixture, OnlyWhitespace) {
    auto ast = parse("   \n  \n  ");
    EXPECT_EQ(ast.statements.size(), 0u);
}

TEST_F(ParserTestFixture, OnlyComments) {
    auto ast = parse("# this is a comment\n// another comment\n");
    EXPECT_EQ(ast.statements.size(), 0u);
}

// ============================================================
// 标签定义测试
// ============================================================

TEST_F(ParserTestFixture, ParseLabelSimple) {
    auto ast = parse("main:\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(ParserTestFixture, ParseLabelMultiple) {
    auto ast = parse("start:\nloop:\nend:\n");
    EXPECT_GE(ast.statements.size(), 3u);
}

// ============================================================
// 预处理指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseAlias) {
    auto ast = parse("alias foo r0\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(ParserTestFixture, ParseDefineWithInteger) {
    auto ast = parse("define MAX_COUNT 100\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 文档注释与类型提示测试
// ============================================================

TEST_F(ParserTestFixture, ParseDeviceDocComment) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Furnace\n"
        "#> @desc 炉窑\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceDocComment>(stmt));
    auto& doc = std::get<DeviceDocComment>(stmt);
    EXPECT_EQ(doc.name, "Furnace");
    EXPECT_TRUE(doc.desc.has_value());
    EXPECT_EQ(doc.desc->kind, DescValue::Kind::TEXT);
    EXPECT_EQ(doc.desc->value, "炉窑");
}

TEST_F(ParserTestFixture, ParseEnumDocComment) {
    auto ast = parse(
        "#> @enum\n"
        "#> @name GasType\n"
        "#> @desc 气体类型\n"
        "#> @value Oxygen 1 氧气\n"
        "#> @value Nitrogen 2 氮气\n"
        "#> @end-enum\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<EnumDocComment>(stmt));
    auto& doc = std::get<EnumDocComment>(stmt);
    EXPECT_EQ(doc.name, "GasType");
    EXPECT_EQ(doc.values.size(), 2u);
    EXPECT_EQ(doc.values[0].name, "Oxygen");
    EXPECT_EQ(doc.values[0].value, "1");
    EXPECT_EQ(doc.values[1].name, "Nitrogen");
    EXPECT_EQ(doc.values[1].value, "2");
}

TEST_F(ParserTestFixture, ParseAliasWithTypeHint) {
    auto ast = parse("alias myFurnace d0 #: @type Furnace\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt));
    auto& alias = std::get<AliasDirective>(stmt);
    EXPECT_TRUE(alias.type.has_value());
    EXPECT_EQ(*alias.type, "Furnace");
}

TEST_F(ParserTestFixture, ParseAliasWithoutTypeHint) {
    auto ast = parse("alias myFurnace d0\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt));
    auto& alias = std::get<AliasDirective>(stmt);
    EXPECT_FALSE(alias.type.has_value());
    EXPECT_FALSE(alias.desc.has_value());
}

TEST_F(ParserTestFixture, ParseAliasWithDescTypeHint) {
    auto ast = parse("alias myFurnace d0 #: @desc 炉窑设备\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt));
    auto& alias = std::get<AliasDirective>(stmt);
    EXPECT_FALSE(alias.type.has_value());
    EXPECT_TRUE(alias.desc.has_value());
    EXPECT_EQ(*alias.desc, "炉窑设备");
}

TEST_F(ParserTestFixture, ParseAliasWithTypeAndDesc) {
    auto ast = parse("alias myFurnace d0 #: @type Furnace @desc 炉窑\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt));
    auto& alias = std::get<AliasDirective>(stmt);
    EXPECT_TRUE(alias.type.has_value());
    EXPECT_EQ(*alias.type, "Furnace");
    EXPECT_TRUE(alias.desc.has_value());
    EXPECT_EQ(*alias.desc, "炉窑");
}

TEST_F(ParserTestFixture, ParseDefineWithDescTypeHint) {
    auto ast = parse("define MAX 100 #: @desc 最大值\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DefineDirective>(stmt));
    auto& def = std::get<DefineDirective>(stmt);
    EXPECT_FALSE(def.type.has_value());
    EXPECT_TRUE(def.desc.has_value());
    EXPECT_EQ(*def.desc, "最大值");
}

TEST_F(ParserTestFixture, ParseDefineWithTypeAndDesc) {
    auto ast = parse("define PRESSURE 101325 #: @type Pressure @desc 标准大气压\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DefineDirective>(stmt));
    auto& def = std::get<DefineDirective>(stmt);
    EXPECT_TRUE(def.type.has_value());
    EXPECT_EQ(*def.type, "Pressure");
    EXPECT_TRUE(def.desc.has_value());
    EXPECT_EQ(*def.desc, "标准大气压");
}

TEST_F(ParserTestFixture, DocCommentWithLinkDesc) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Furnace\n"
        "#> @desc $./locals/furnace.desc\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceDocComment>(stmt));
    auto& doc = std::get<DeviceDocComment>(stmt);
    EXPECT_TRUE(doc.desc.has_value());
    EXPECT_EQ(doc.desc->kind, DescValue::Kind::LINK);
    EXPECT_EQ(doc.desc->value, "./locals/furnace.desc");
}

TEST_F(ParserTestFixture, DeviceDocWithSlots) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Furnace\n"
        "#> @slot 0 input 输入槽\n"
        "#> @slot 1 output 输出槽\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceDocComment>(stmt));
    auto& doc = std::get<DeviceDocComment>(stmt);
    ASSERT_EQ(doc.slots.size(), 2u);
    EXPECT_EQ(doc.slots[0].index, "0");
    EXPECT_EQ(doc.slots[0].direction, SlotDirection::INPUT);
    EXPECT_TRUE(doc.slots[0].desc.has_value());
    EXPECT_EQ(doc.slots[0].desc->value, "输入槽");
    EXPECT_EQ(doc.slots[1].index, "1");
    EXPECT_EQ(doc.slots[1].direction, SlotDirection::OUTPUT);
    EXPECT_EQ(doc.slots[1].desc->value, "输出槽");
}

TEST_F(ParserTestFixture, DeviceDocWithLogics) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Pressure rw\n"
        "#> @logic Temperature r\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceDocComment>(stmt));
    auto& doc = std::get<DeviceDocComment>(stmt);
    ASSERT_EQ(doc.logics.size(), 2u);
    EXPECT_EQ(doc.logics[0].name, "Pressure");
    EXPECT_EQ(doc.logics[0].access, LogicAccess::RW);
    EXPECT_EQ(doc.logics[1].name, "Temperature");
    EXPECT_EQ(doc.logics[1].access, LogicAccess::R);
}

TEST_F(ParserTestFixture, DeviceDocWithModes) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Pump\n"
        "#> @mode 0 待机模式\n"
        "#> @mode 1 运行模式\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceDocComment>(stmt));
    auto& doc = std::get<DeviceDocComment>(stmt);
    ASSERT_EQ(doc.modes.size(), 2u);
    EXPECT_EQ(doc.modes[0].index, "0");
    EXPECT_EQ(doc.modes[0].desc->value, "待机模式");
    EXPECT_EQ(doc.modes[1].index, "1");
    EXPECT_EQ(doc.modes[1].desc->value, "运行模式");
}

TEST_F(ParserTestFixture, DeviceDocWithLogicSlots) {
    auto ast = parse(
        "#> @device\n"
        "#> @name IC10\n"
        "#> @logicSlot db\n"
        "#> @logicSlot r0\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceDocComment>(stmt));
    auto& doc = std::get<DeviceDocComment>(stmt);
    ASSERT_EQ(doc.logicSlots.size(), 2u);
    EXPECT_EQ(doc.logicSlots[0].name, "db");
    EXPECT_EQ(doc.logicSlots[1].name, "r0");
}

TEST_F(ParserTestFixture, DeviceDocWithConnects) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Pipe\n"
        "#> @connect 0 入口\n"
        "#> @connect 1 出口\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceDocComment>(stmt));
    auto& doc = std::get<DeviceDocComment>(stmt);
    ASSERT_EQ(doc.connects.size(), 2u);
    EXPECT_EQ(doc.connects[0].index, "0");
    EXPECT_EQ(doc.connects[0].desc->value, "入口");
    EXPECT_EQ(doc.connects[1].index, "1");
    EXPECT_EQ(doc.connects[1].desc->value, "出口");
}

TEST_F(ParserTestFixture, EnumValueWithLinkDesc) {
    auto ast = parse(
        "#> @enum\n"
        "#> @value Oxygen 1 $./locals/gas.desc\n"
        "#> @end-enum\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<EnumDocComment>(stmt));
    auto& doc = std::get<EnumDocComment>(stmt);
    ASSERT_EQ(doc.values.size(), 1u);
    EXPECT_TRUE(doc.values[0].desc.has_value());
    EXPECT_EQ(doc.values[0].desc->kind, DescValue::Kind::LINK);
    EXPECT_EQ(doc.values[0].desc->value, "./locals/gas.desc");
}

TEST_F(ParserTestFixture, TypeHintStandaloneError) {
    auto tokens = Lexer::tokenize("#: @type Furnace\n");
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<ErrorNode>(stmt));
}

TEST_F(ParserTestFixture, MixedDocCommentAndCode) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Furnace\n"
        "#> @end-device\n"
        "alias f d0\n"
    );
    EXPECT_EQ(ast.statements.size(), 2u);
}

// ============================================================
// 零元指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseHcfInstruction) {
    auto ast = parse("hcf\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(ParserTestFixture, ParseYieldInstruction) {
    auto ast = parse("yield\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 一元指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseSleepInstruction) {
    auto ast = parse("sleep 1000\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(ParserTestFixture, ParseMoveInstruction) {
    auto ast = parse("move r0 r1\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 二元指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseAddInstruction) {
    auto ast = parse("add r0 r1 r2\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(ParserTestFixture, ParseSubInstruction) {
    auto ast = parse("sub r0 r1 r2\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 三元指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseSapInstruction) {
    auto ast = parse("sap r0 d0 1\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 四元指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseLerpInstruction) {
    auto ast = parse("lerp r0 r1 r2 r3\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 五元指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseLbnInstruction) {
    auto ast = parse("lbn r0 d0 1 2 r1\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 六元指令测试
// ============================================================

TEST_F(ParserTestFixture, ParseLbnsInstruction) {
    auto ast = parse("lbns r0 d0 1 2 r1 r2\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 多语句程序测试
// ============================================================

TEST_F(ParserTestFixture, MultipleStatements) {
    auto ast = parse(
        "alias counter r0\n"
        "define LIMIT 10\n"
        "start:\n"
        "add r0 r0 1\n"
        "yield\n"
        "j start\n"
    );
    EXPECT_GE(ast.statements.size(), 5u);
}

TEST_F(ParserTestFixture, MixedLabelsAndInstructions) {
    auto ast = parse(
        "init:\n"
        "move r0 0\n"
        "loop:\n"
        "add r0 r0 1\n"
        "yield\n"
        "j loop\n"
        "end:\n"
        "hcf\n"
    );
    EXPECT_GE(ast.statements.size(), 7u);
}

// ============================================================
// 带注释的代码测试
// ============================================================

TEST_F(ParserTestFixture, CodeWithComments) {
    auto ast = parse(
        "# 初始化计数器\n"
        "move r0 0\n"
        "// 主循环\n"
        "loop:\n"
        "add r0 r0 1\n"
        "yield\n"
        "j loop\n"
    );
    EXPECT_GE(ast.statements.size(), 4u);
}

// ============================================================
// Program toString测试
// ============================================================

TEST_F(ParserTestFixture, ProgramToString) {
    auto ast = parse("hcf\n");
    auto str = ast.toString();
    EXPECT_FALSE(str.empty());
}

TEST_F(ParserTestFixture, ProgramToJSON) {
    auto ast = parse("yield\n");
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
}

// ============================================================
// 诊断信息测试
// ============================================================

TEST_F(ParserTestFixture, ValidProgramHasNoErrors) {
    Parser parser(Lexer::tokenize("hcf\nyield\n"));
    auto ast = parser.parse();
    EXPECT_GE(ast.statements.size(), 2u);
}

TEST_F(ParserTestFixture, ParserGetDiagnostics) {
    Parser parser(Lexer::tokenize("hcf\n"));
    auto ast = parser.parse();
    auto& diags = parser.getDiagnostics();
    EXPECT_GE(ast.statements.size(), 1u);
}

// ============================================================
// 语句换行分隔测试（IEP2: 语句之间缺少换行符）
// 参考IC10.g4: program : statement (NEWLINE+ statement)* NEWLINE* EOF;
// ============================================================

TEST_F(ParserTestFixture, StatementsWithoutNewlineProducesError) {
    // 两个语句在同一行，缺少换行分隔
    auto tokens = Lexer::tokenize("hcf yield\n");
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_FALSE(parser.getDiagnostics().empty());
}

TEST_F(ParserTestFixture, StatementsWithNewlineNoError) {
    // 语句间有换行分隔，不应产生错误
    auto tokens = Lexer::tokenize("hcf\nyield\n");
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_TRUE(parser.getDiagnostics().empty());
    EXPECT_GE(ast.statements.size(), 2u);
}

TEST_F(ParserTestFixture, LastStatementWithoutNewlineNoError) {
    // 最后一个语句不需要尾随换行
    auto tokens = Lexer::tokenize("hcf\nyield");
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_TRUE(parser.getDiagnostics().empty());
    EXPECT_GE(ast.statements.size(), 2u);
}

TEST_F(ParserTestFixture, StatementFollowedByCommentThenNewline) {
    // 语句后跟行内注释，然后换行
    auto tokens = Lexer::tokenize("hcf # comment\nyield\n");
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_TRUE(parser.getDiagnostics().empty());
    EXPECT_GE(ast.statements.size(), 2u);
}
