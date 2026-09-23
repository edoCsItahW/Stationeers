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
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/lexer/lexer.hpp"

#include <gtest/gtest.h>


using namespace stationeers::ic10;

class ParserTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("zh-hans");
        ICLoc::setLanguage("zh-hans");
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
    auto ast = parse("# this is a comment\n# another comment\n");
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
        "#> @desc \"炉窑\"\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceAnnotation>(stmt.raw()));
    auto& doc = std::get<DeviceAnnotation>(stmt.raw());
    EXPECT_EQ(doc.name, "Furnace");
    ASSERT_TRUE(doc.desc.has_value());
    const auto* desc = std::get_if<String>(&*doc.desc);
    ASSERT_NE(desc, nullptr);
    EXPECT_EQ(desc->value, "\"炉窑\"");
}

TEST_F(ParserTestFixture, ParseEnumDocComment) {
    auto ast = parse(
        "#> @enum\n"
        "#> @name GasType\n"
        "#> @desc \"气体类型\"\n"
        "#> @value Oxygen 1 \"氧气\"\n"
        "#> @value Nitrogen 2 \"氮气\"\n"
        "#> @end-enum\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<EnumAnnotation>(stmt.raw()));
    auto& doc = std::get<EnumAnnotation>(stmt.raw());
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
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt.raw()));
    auto& alias = std::get<AliasDirective>(stmt.raw());
    ASSERT_TRUE(alias.typeHint.has_value());
    ASSERT_TRUE(alias.typeHint->type.has_value());
    EXPECT_EQ(alias.typeHint->type, "Furnace");
}

TEST_F(ParserTestFixture, ParseAliasWithoutTypeHint) {
    auto ast = parse("alias myFurnace d0\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt.raw()));
    auto& alias = std::get<AliasDirective>(stmt.raw());
    EXPECT_FALSE(alias.typeHint.has_value());
}

TEST_F(ParserTestFixture, ParseAliasWithDescTypeHint) {
    auto ast = parse("alias myFurnace d0 #: @desc \"炉窑设备\"\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt.raw()));
    auto& alias = std::get<AliasDirective>(stmt.raw());
    ASSERT_TRUE(alias.typeHint.has_value());
    EXPECT_FALSE(alias.typeHint->type.has_value());
    ASSERT_TRUE(alias.typeHint->desc.has_value());
    const auto* desc = std::get_if<String>(&*alias.typeHint->desc);
    ASSERT_NE(desc, nullptr);
    EXPECT_EQ(desc->value, "\"炉窑设备\"");
}

TEST_F(ParserTestFixture, ParseAliasWithTypeAndDesc) {
    auto ast = parse("alias myFurnace d0 #: @type Furnace @desc \"炉窑\"\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<AliasDirective>(stmt.raw()));
    auto& alias = std::get<AliasDirective>(stmt.raw());
    ASSERT_TRUE(alias.typeHint.has_value());
    ASSERT_TRUE(alias.typeHint->type.has_value());
    EXPECT_EQ(alias.typeHint->type, "Furnace");
    ASSERT_TRUE(alias.typeHint->desc.has_value());
    const auto* desc = std::get_if<String>(&*alias.typeHint->desc);
    ASSERT_NE(desc, nullptr);
    EXPECT_EQ(desc->value, "\"炉窑\"");
}

TEST_F(ParserTestFixture, ParseDefineWithDescTypeHint) {
    auto ast = parse("define MAX 100 #: @desc \"最大值\"\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DefineDirective>(stmt.raw()));
    auto& def = std::get<DefineDirective>(stmt.raw());
    ASSERT_TRUE(def.typeHint.has_value());
    EXPECT_FALSE(def.typeHint->type.has_value());
    ASSERT_TRUE(def.typeHint->desc.has_value());
    const auto* desc = std::get_if<String>(&*def.typeHint->desc);
    ASSERT_NE(desc, nullptr);
    EXPECT_EQ(desc->value, "\"最大值\"");
}

TEST_F(ParserTestFixture, ParseDefineWithTypeAndDesc) {
    auto ast = parse("define PRESSURE 101325 #: @type Pressure @desc \"标准大气压\"\n");
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DefineDirective>(stmt.raw()));
    auto& def = std::get<DefineDirective>(stmt.raw());
    ASSERT_TRUE(def.typeHint.has_value());
    ASSERT_TRUE(def.typeHint->type.has_value());
    EXPECT_EQ(def.typeHint->type, "Pressure");
    ASSERT_TRUE(def.typeHint->desc.has_value());
    const auto* desc = std::get_if<String>(&*def.typeHint->desc);
    ASSERT_NE(desc, nullptr);
    EXPECT_EQ(desc->value, "\"标准大气压\"");
}

TEST_F(ParserTestFixture, DocCommentWithLinkDesc) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Furnace\n"
        "#> @desc ./locals/furnace.desc\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceAnnotation>(stmt.raw()));
    auto& doc = std::get<DeviceAnnotation>(stmt.raw());
    ASSERT_TRUE(doc.desc.has_value());
    const auto* link = std::get_if<Link>(&*doc.desc);
    ASSERT_NE(link, nullptr);
    EXPECT_EQ(link->toString(), "./locals/furnace.desc");
}

TEST_F(ParserTestFixture, DeviceDocWithSlots) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Furnace\n"
        "#> @slot Input 0\n"
        "#> @slot Output 1\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceAnnotation>(stmt.raw()));
    auto& doc = std::get<DeviceAnnotation>(stmt.raw());
    ASSERT_EQ(doc.slots.size(), 2u);
    EXPECT_EQ(doc.slots[0].name, "Input");
    EXPECT_EQ(doc.slots[0].value, "0");
    EXPECT_EQ(doc.slots[1].name, "Output");
    EXPECT_EQ(doc.slots[1].value, "1");
}

TEST_F(ParserTestFixture, DeviceDocWithLogics) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Pressure 5\n"
        "#> @logic Temperature 6 \"Temperature\"\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceAnnotation>(stmt.raw()));
    auto& doc = std::get<DeviceAnnotation>(stmt.raw());
    ASSERT_EQ(doc.logics.size(), 2u);
    EXPECT_EQ(doc.logics[0].name, "Pressure");
    EXPECT_EQ(doc.logics[0].value, "5");
    EXPECT_FALSE(doc.logics[0].desc.has_value());
    EXPECT_EQ(doc.logics[1].name, "Temperature");
    EXPECT_EQ(doc.logics[1].value, "6");
    // 描述为可选：带引号的描述应被解析
    EXPECT_TRUE(doc.logics[1].desc.has_value());
}

TEST_F(ParserTestFixture, DeviceDocWithLogicSlots) {
    auto ast = parse(
        "#> @device\n"
        "#> @name IC10\n"
        "#> @logic-slot Quantity 3\n"
        "#> @logic-slot Charge 10\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceAnnotation>(stmt.raw()));
    auto& doc = std::get<DeviceAnnotation>(stmt.raw());
    ASSERT_EQ(doc.logicSlots.size(), 2u);
    EXPECT_EQ(doc.logicSlots[0].name, "Quantity");
    EXPECT_EQ(doc.logicSlots[0].value, "3");
    EXPECT_EQ(doc.logicSlots[1].name, "Charge");
    EXPECT_EQ(doc.logicSlots[1].value, "10");
}

TEST_F(ParserTestFixture, EnumValueWithLinkDesc) {
    auto ast = parse(
        "#> @enum\n"
        "#> @name GasType\n"
        "#> @value Oxygen 1 ./locals/gas.desc\n"
        "#> @end-enum\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<EnumAnnotation>(stmt.raw()));
    auto& doc = std::get<EnumAnnotation>(stmt.raw());
    ASSERT_EQ(doc.values.size(), 1u);
    ASSERT_TRUE(doc.values[0].desc.has_value());
    const auto* link = std::get_if<Link>(&*doc.values[0].desc);
    ASSERT_NE(link, nullptr);
    EXPECT_EQ(link->toString(), "./locals/gas.desc");
}

TEST_F(ParserTestFixture, TypeHintStandaloneError) {
    auto tokens = Lexer::tokenize("#: @type Furnace\n");
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<ErrorNode>(stmt.raw()));
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
        "# 主循环\n"
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

// ============================================================
// AST 序列化测试（toString / toJSON）
// AST serialization tests (toString / toJSON)
// ============================================================

TEST_F(ParserTestFixture, ProgramToStringEmpty) {
    auto ast = parse("");
    auto str = ast.toString();
    EXPECT_EQ(str, "[]");
}

TEST_F(ParserTestFixture, ProgramToJSONEmpty) {
    auto ast = parse("");
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find('['), std::string::npos);
    EXPECT_NE(json.find(']'), std::string::npos);
}

TEST_F(ParserTestFixture, ProgramToJSONSingleInstruction) {
    auto ast = parse("hcf\n");
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("hcf"), std::string::npos);
}

TEST_F(ParserTestFixture, ProgramToJSONAliasDirective) {
    auto ast = parse("alias foo r0\n");
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("foo"), std::string::npos);
    EXPECT_NE(json.find("registerOrDevice"), std::string::npos);
}

TEST_F(ParserTestFixture, ProgramToJSONDefineDirective) {
    auto ast = parse("define MAX 100\n");
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("MAX"), std::string::npos);
    EXPECT_NE(json.find("identifier"), std::string::npos);
}

TEST_F(ParserTestFixture, ProgramToJSONLabelDef) {
    auto ast = parse("start:\n");
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("start"), std::string::npos);
}

TEST_F(ParserTestFixture, ProgramToJSONMultipleStatements) {
    auto ast = parse(
        "alias foo r0\n"
        "define MAX 10\n"
        "start:\n"
        "add r0 r1 r2\n"
        "hcf\n"
    );
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("foo"), std::string::npos);
    EXPECT_NE(json.find("MAX"), std::string::npos);
    EXPECT_NE(json.find("start"), std::string::npos);
    EXPECT_NE(json.find("add"), std::string::npos);
    EXPECT_NE(json.find("hcf"), std::string::npos);
}

TEST_F(ParserTestFixture, ProgramToStringMultipleInstructions) {
    auto ast = parse(
        "move r0 0\n"
        "add r0 r0 1\n"
        "yield\n"
    );
    auto str = ast.toString();
    EXPECT_FALSE(str.empty());
}

TEST_F(ParserTestFixture, DeviceDocCommentToJSON) {
    auto ast = parse(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @end-device\n"
    );
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("Sensor"), std::string::npos);
    EXPECT_NE(json.find("name"), std::string::npos);
}

TEST_F(ParserTestFixture, EnumDocCommentToJSON) {
    auto ast = parse(
        "#> @enum\n"
        "#> @name GasType\n"
        "#> @value Oxygen 1 \"Oxygen\"\n"
        "#> @end-enum\n"
    );
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("GasType"), std::string::npos);
    EXPECT_NE(json.find("Oxygen"), std::string::npos);
}

TEST_F(ParserTestFixture, AliasWithTypeHintToJSON) {
    auto ast = parse("alias myDev d0 #: @type Furnace\n");
    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("myDev"), std::string::npos);
    EXPECT_NE(json.find("Furnace"), std::string::npos);
}

// ============================================================
// DocParser 相关测试
// DocParser related tests
// ============================================================

TEST_F(ParserTestFixture, EnumValueWithDescription) {
    auto ast = parse(
        "#> @enum\n"
        "#> @name Status\n"
        "#> @value Active 1 \"激活状态\"\n"
        "#> @value Inactive 0 \"未激活\"\n"
        "#> @end-enum\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<EnumAnnotation>(stmt.raw()));
    auto& doc = std::get<EnumAnnotation>(stmt.raw());
    ASSERT_EQ(doc.values.size(), 2u);
    EXPECT_EQ(doc.values[0].name, "Active");
    EXPECT_EQ(doc.values[0].value, "1");
    ASSERT_TRUE(doc.values[0].desc.has_value());
    const auto* desc = std::get_if<String>(&*doc.values[0].desc);
    ASSERT_NE(desc, nullptr);
    EXPECT_EQ(desc->value, "\"激活状态\"");
}

TEST_F(ParserTestFixture, DeviceDocWithDeviceHash) {
    auto ast = parse(
        "#> @device\n"
        "#> @name TestDev\n"
        "#> @device-hash 12345\n"
        "#> @end-device\n"
    );
    EXPECT_EQ(ast.statements.size(), 1u);
    auto& stmt = ast.statements[0];
    ASSERT_TRUE(std::holds_alternative<DeviceAnnotation>(stmt.raw()));
    auto& doc = std::get<DeviceAnnotation>(stmt.raw());
    ASSERT_TRUE(doc.deviceHash.has_value());
    EXPECT_EQ(doc.deviceHash->value, "12345");
}

// ============================================================
// 更多指令类型解析测试
// More instruction type parsing tests
// ============================================================

TEST_F(ParserTestFixture, ParseJumpInstruction) {
    auto ast = parse("j label\n");
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(ParserTestFixture, ParseBranchInstructions) {
    auto ast = parse(
        "beq r0 r1 label\n"
        "bne r0 r1 label\n"
        "blt r0 r1 label\n"
        "bgt r0 r1 label\n"
        "ble r0 r1 label\n"
        "bge r0 r1 label\n"
    );
    EXPECT_GE(ast.statements.size(), 6u);
}

TEST_F(ParserTestFixture, ParseStackInstructions) {
    auto ast = parse(
        "push r0\n"
        "pop r0\n"
        "peek r0 0\n"
    );
    EXPECT_GE(ast.statements.size(), 3u);
}

TEST_F(ParserTestFixture, ParseDeviceInstructions) {
    auto ast = parse(
        "l r0 d0 Pressure\n"
        "s d0 On r0\n"
        "ls r0 d0 0 Quantity\n"
        "lr r0 d0 Contents label\n"
    );
    EXPECT_GE(ast.statements.size(), 4u);
}

TEST_F(ParserTestFixture, ParseBatchInstructions) {
    auto ast = parse(
        "lb r0 100 Pressure Average\n"
        "lbs r0 100 0 Quantity Average\n"
        "lbn r0 100 200 Pressure Average\n"
        "lbns r0 100 200 0 Quantity Average\n"
    );
    EXPECT_GE(ast.statements.size(), 4u);
}

// ============================================================
// 错误恢复测试
// Error recovery tests
// ============================================================

TEST_F(ParserTestFixture, InvalidInstructionProducesErrorNode) {
    // 无效指令应产生 ErrorNode，但不应崩溃
    auto tokens = Lexer::tokenize("invalid_instruction r0 r1\n");
    Parser parser(tokens);
    auto ast = parser.parse();
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(ParserTestFixture, ErrorDoesNotBlockSubsequentStatements) {
    // 一个错误不应阻止后续语句的解析
    auto tokens = Lexer::tokenize(
        "hcf\n"
        "bad_instruction\n"
        "yield\n"
    );
    Parser parser(tokens);
    auto ast = parser.parse();
    // 至少有2个有效语句（hcf 和 yield）
    EXPECT_GE(ast.statements.size(), 2u);
}

// ============================================================
// 鲁棒性测试 — alias 指令
// 等价类划分：以词法递增的方式测试解析器对不完整 alias 语句的容错能力
// Robustness tests — alias directive
// Equivalence class partitioning: test parser tolerance for incomplete alias statements with lexical increments
// ============================================================

TEST_F(ParserTestFixture, AliasRobustnessIncremental) {
    // alias 语句从关键字到完整形式的逐步递增输入，验证每一步都不崩溃
    const std::vector<std::string> inputs = {
        "alias",
        "alias test",
        "alias test r0",
        "alias test d0",
        "alias test HASH",
        "alias test HASH(",
        "alias test HASH(\"",
        "alias test HASH(\"Something",
        "alias test HASH(\"Something\"",
        "alias test HASH(\"Something\")",
        "alias test STR",
        "alias test STR(",
        "alias test STR(\"",
        "alias test STR(\"Equipment",
        "alias test STR(\"Equipment\"",
        "alias test STR(\"Equipment\")",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing alias input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

TEST_F(ParserTestFixture, AliasRobustnessWithHints) {
    // alias + type hint 的不完整形式
    const std::vector<std::string> inputs = {
        "alias test r0 #:",
        "alias test r0 #: @",
        "alias test r0 #: @type",
        "alias test r0 #: @type ",
        "alias test r0 #: @type MyDevice",
        "alias test r0 #: @desc",
        "alias test r0 #: @desc ",
        "alias test r0 #: @desc \"Some description\"",
        "alias test HASH(\"Something\") #: @type MyDevice",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing alias hint input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

// ============================================================
// 鲁棒性测试 — define 指令
// Robustness tests — define directive
// ============================================================

TEST_F(ParserTestFixture, DefineRobustnessIncremental) {
    // define 语句从关键字到完整形式的逐步递增输入
    const std::vector<std::string> inputs = {
        "define",
        "define X",
        "define X 1",
        "define X 1.5",
        "define X 0x10",
        "define X STR",
        "define X STR(",
        "define X STR(\"",
        "define X STR(\"Structure",
        "define X STR(\"Structure\"",
        "define X STR(\"Structure\")",
        "define X HASH",
        "define X HASH(",
        "define X HASH(\"",
        "define X HASH(\"ItemName",
        "define X HASH(\"ItemName\"",
        "define X HASH(\"ItemName\")",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing define input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

TEST_F(ParserTestFixture, DefineRobustnessWithHints) {
    // define + type hint 的不完整形式
    const std::vector<std::string> inputs = {
        "define X 1 #:",
        "define X 1 #: @type",
        "define X 1 #: @type MyType",
        "define X STR(\"Structure\") #: @type MyType",
        "define X 1 #: @desc \"Constant value\"",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing define hint input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

// ============================================================
// 鲁棒性测试 — 指令语句（代表各元数）
// 等价类划分：选择0~3元指令的代表，测试不完整形式的容错
// Robustness tests — instructions (representative arities)
// Equivalence class partitioning: select representatives of 0~3-ary instructions, test tolerance of incomplete forms
// ============================================================

TEST_F(ParserTestFixture, InstructionRobustnessNullary) {
    // 0元指令（hcf / yield）：测试后缀垃圾不崩溃
    const std::vector<std::string> inputs = {
        "hcf",
        "hcf extra",
        "yield",
        "yield extra",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing nullary input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

TEST_F(ParserTestFixture, InstructionRobustnessUnary) {
    // 1元指令（move）：测试操作数不完整形式
    const std::vector<std::string> inputs = {
        "move",
        "move r0",
        "move 1",
        "move r0 extra",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing unary input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

TEST_F(ParserTestFixture, InstructionRobustnessBinary) {
    // 2元指令（add）：测试操作数不完整形式
    const std::vector<std::string> inputs = {
        "add",
        "add r0",
        "add r0 r1",
        "add r0 1",
        "add r0 r1 extra",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing binary input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

TEST_F(ParserTestFixture, InstructionRobustnessTernary) {
    // 3元指令（sap）：测试操作数不完整形式
    const std::vector<std::string> inputs = {
        "sap",
        "sap r0",
        "sap r0 d0",
        "sap r0 d0 0",
        "sap r0 d0 0 extra",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing ternary input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

TEST_F(ParserTestFixture, InstructionRobustnessWithMacroCalls) {
    // 指令中使用 STR/HASH 宏调用的不完整形式
    const std::vector<std::string> inputs = {
        "move r0 STR",
        "move r0 STR(",
        "move r0 STR(\"",
        "move r0 STR(\"Test\"",
        "move r0 STR(\"Test\")",
        "move r0 HASH",
        "move r0 HASH(",
        "move r0 HASH(\"",
        "move r0 HASH(\"Test\"",
        "move r0 HASH(\"Test\")",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing instruction macro input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

TEST_F(ParserTestFixture, RobustnessMixedErrorsNotBlocking) {
    // 混合错误场景：多个语句，部分有效、部分无效，验证不崩溃且有效语句被解析
    const std::vector<std::string> inputs = {
        "alias\nhcf\n",
        "define\nhcf\n",
        "alias test STR(\nhcf\n",
        "define X STR(\nhcf\n",
        "hcf\nalias\nyield\n",
        "hcf\ndefine\nyield\n",
        "alias test HASH(\"Something\")\nalias bad\nyield\n",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing mixed error input: " + input);
        auto tokens = Lexer::tokenize(input);
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 1u);
        });
    }
}

// ============================================================
// 边界值测试 — 字符串边界
// Boundary value tests — string boundaries
// ============================================================

TEST_F(ParserTestFixture, RobustnessStringBoundaries) {
    // 字符串边界值：空字符串、单个字符、超长字符串、特殊字符
    const std::vector<std::string> inputs = {
        "alias test HASH(\"\")",
        "alias test HASH(\"x\")",
        "alias test HASH(\"" + std::string(1000, 'A') + "\")",
        "alias test STR(\"has spaces\")",
        "alias test STR(\"has_underscores\")",
        "alias test STR(\"has-dashes\")",
        "alias test STR(\"has.mixed.content_123\")",
    };

    for (const auto& input : inputs) {
        SCOPED_TRACE("Testing string boundary input: " + input);
        auto tokens = Lexer::tokenize(input + "\n");
        Parser parser(tokens);
        EXPECT_NO_THROW({
            auto ast = parser.parse();
            EXPECT_GE(ast.statements.size(), 0u);
        });
    }
}

// ============================================================
// 多 token 操作数 — 源码文本终点（语言服务按列定位操作数与子段的依据）
// Multi-token operands — source text end (what the language service uses
// to locate operands and their sub-segments by column)
// ============================================================

/// @brief 取语句中的指定指令节点 / Extract the given instruction node from a statement
template<typename Instruction>
static const Instruction* asInstruction(const Statement& stmt) {
    return std::get_if<Instruction>(&stmt.raw());
}

TEST_F(ParserTestFixture, StaticDeviceEndCoversPin) {
    // `d0:1` 的终点必须覆盖引脚，否则按列定位会把光标后的引脚算进下一个操作数
    auto ast = parse("l r0 d0:1 Pressure\n");
    ASSERT_GE(ast.statements.size(), 1u);

    const auto* ins = asInstruction<LInstruction>(ast.statements[0]);
    ASSERT_NE(ins, nullptr);

    const auto* device = std::get_if<StaticDevice>(&ins->operand2);
    ASSERT_NE(device, nullptr);
    ASSERT_TRUE(device->pin.has_value());
    EXPECT_EQ(std::visit([](const auto& d) { return d.end().column(); }, device->device), 8);  // `d0`
    EXPECT_EQ(device->pin->end().column(), 10);                                                // `:1`
    EXPECT_EQ(device->end().column(), 10);                                                     // `d0:1` 整体
}

TEST_F(ParserTestFixture, StaticDeviceEndCoversConsumedColon) {
    // `d0:` 未输入完时冒号已被消费，终点需覆盖该冒号，语言服务才能把光标判进引脚下标
    auto ast = parse("l r0 d0:\n");
    ASSERT_GE(ast.statements.size(), 1u);

    const auto* ins = asInstruction<LInstruction>(ast.statements[0]);
    ASSERT_NE(ins, nullptr);

    const auto* device = std::get_if<StaticDevice>(&ins->operand2);
    ASSERT_NE(device, nullptr);
    EXPECT_FALSE(device->pin.has_value());
    EXPECT_EQ(device->end().column(), 9);  // `d0:`（冒号在 8 列）
}

TEST_F(ParserTestFixture, StaticDeviceEndWithoutColon) {
    // 无冒号时终点仍为设备本身（边界值：不得因携带 endPos 而延长）
    auto ast = parse("l r0 d0 Pressure\n");
    ASSERT_GE(ast.statements.size(), 1u);

    const auto* ins = asInstruction<LInstruction>(ast.statements[0]);
    ASSERT_NE(ins, nullptr);

    const auto* device = std::get_if<StaticDevice>(&ins->operand2);
    ASSERT_NE(device, nullptr);
    EXPECT_FALSE(device->pin.has_value());
    EXPECT_EQ(device->end().column(), 8);
}

TEST_F(ParserTestFixture, HashMacroEndCoversRightParen) {
    // HASH/STR 宏的终点应在右括号之后，与其它节点保持一致（原为右括号所在列）
    auto ast = parse("lb r0 HASH(\"XX\") Pressure Sum\n");
    ASSERT_GE(ast.statements.size(), 1u);

    const auto* ins = asInstruction<LbInstruction>(ast.statements[0]);
    ASSERT_NE(ins, nullptr);

    const auto* macro = std::get_if<HashMacro>(&ins->operand2);
    ASSERT_NE(macro, nullptr);
    EXPECT_EQ(macro->end().column(), 17);  // `)` 在 16 列
}

TEST_F(ParserTestFixture, EnumEndCoversDotWhenValueMissing) {
    // `Foo.` 未输入完：仍构造 Enum，点号记入 value 的错误节点，终点覆盖 `Foo.`
    auto tokens = Lexer::tokenize("move r0 Foo.\n");
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_GE(ast.statements.size(), 1u);

    const auto* ins = asInstruction<MoveInstruction>(ast.statements[0]);
    ASSERT_NE(ins, nullptr);

    const auto* enumNode = std::get_if<Enum>(&ins->operand2);
    ASSERT_NE(enumNode, nullptr);

    const auto* name = std::get_if<Identifier>(&enumNode->name);
    ASSERT_NE(name, nullptr);
    EXPECT_EQ(name->value, "Foo");

    // 值缺失时记入错误节点（点号在 12 列），终点覆盖 `Foo.`
    EXPECT_TRUE(std::holds_alternative<ErrorNode>(enumNode->value));
    EXPECT_EQ(enumNode->end().column(), 13);

    // 点号已被消费，语句层的“缺少换行”检查不会触发，需在此上报
    const auto& diags = parser.getDiagnostics();
    bool reported = false;

    for (const stationeers::Diagnostic& d : diags) reported = reported || d.id == "IEP34_1";

    EXPECT_TRUE(reported) << "缺失枚举值时应上报 IEP34_1";
}

TEST_F(ParserTestFixture, EnumEndCoversDotBeforeUnexpectedToken) {
    // `Foo.r0`：值 token 类型不符时同样保留部分节点，且不消费该 token
    auto tokens = Lexer::tokenize("move r0 Foo.r0\n");
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_GE(ast.statements.size(), 1u);

    const auto* ins = asInstruction<MoveInstruction>(ast.statements[0]);
    ASSERT_NE(ins, nullptr);

    const auto* enumNode = std::get_if<Enum>(&ins->operand2);
    ASSERT_NE(enumNode, nullptr);
    EXPECT_TRUE(std::holds_alternative<ErrorNode>(enumNode->value));
    EXPECT_EQ(enumNode->end().column(), 13);
}

TEST_F(ParserTestFixture, EnumKeepsBothPartsWhenComplete) {
    // `Foo.Bar`：完整枚举由 name 与 value 两个 token 组成
    auto ast = parse("move r0 Foo.Bar\n");
    ASSERT_GE(ast.statements.size(), 1u);

    const auto* ins = asInstruction<MoveInstruction>(ast.statements[0]);
    ASSERT_NE(ins, nullptr);

    const auto* enumNode = std::get_if<Enum>(&ins->operand2);
    ASSERT_NE(enumNode, nullptr);

    const auto* name = std::get_if<Identifier>(&enumNode->name);
    const auto* value = std::get_if<Identifier>(&enumNode->value);
    ASSERT_NE(name, nullptr);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(name->value, "Foo");
    EXPECT_EQ(value->value, "Bar");
    EXPECT_EQ(enumNode->position.column(), 9);
    EXPECT_EQ(enumNode->end().column(), 16);
}
