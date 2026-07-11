// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_lexer.cpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/10
 * @brief IC10词法分析器单元测试
 * @details 全面测试词法分析器的各项功能，包括数字、标识符、寄存器、设备、
 *          字符串、注释、关键字、符号、位置信息及边界情况。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/lexer/lexer.hpp"


using namespace stationeers::ic10;

namespace {

void initLocale() {
    static bool initialized = false;
    if (!initialized) {
        Loc::registerLanguage<ZhHans>("zh-hans");
        Loc::setLanguage("zh-hans");
        initialized = true;
    }
}

std::vector<std::shared_ptr<Token>> tokenize(std::string_view src) {
    return Lexer::tokenize(src);
}

// 辅助函数：获取除END外的所有token
std::vector<std::shared_ptr<Token>> tokensWithoutEnd(
    const std::vector<std::shared_ptr<Token>>& tokens
) {
    std::vector<std::shared_ptr<Token>> result;
    for (const auto& tok : tokens) {
        if (tok->type != TokenType::END) {
            result.push_back(tok);
        }
    }
    return result;
}

}  // namespace

// ============================================================
// 空输入测试
// ============================================================

TEST(LexerTest, EmptyInputProducesOnlyEnd) {
    initLocale();
    auto tokens = tokenize("");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0]->type, TokenType::END);
}

TEST(LexerTest, WhitespaceOnlyInput) {
    initLocale();
    auto tokens = tokenize("   \t  ");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0]->type, TokenType::END);
}

// ============================================================
// 数字测试
// ============================================================

TEST(LexerTest, IntegerSimple) {
    initLocale();
    auto tokens = tokenize("123");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::INTEGER);
    EXPECT_EQ(toks[0]->lexeme, "123");
    EXPECT_EQ(toks[0]->category, TokenCategory::LITERAL);
}

TEST(LexerTest, IntegerZero) {
    initLocale();
    auto tokens = tokenize("0");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::INTEGER);
    EXPECT_EQ(toks[0]->lexeme, "0");
}

TEST(LexerTest, IntegerLargeNumber) {
    initLocale();
    auto tokens = tokenize("999999999");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::INTEGER);
    EXPECT_EQ(toks[0]->lexeme, "999999999");
}


TEST(LexerTest, FloatSimple) {
    initLocale();
    auto tokens = tokenize("3.14");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::FLOAT);
    EXPECT_EQ(toks[0]->lexeme, "3.14");
}

TEST(LexerTest, FloatLeadingZero) {
    initLocale();
    auto tokens = tokenize("0.5");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::FLOAT);
    EXPECT_EQ(toks[0]->lexeme, "0.5");
}

TEST(LexerTest, FloatTrailingDigits) {
    initLocale();
    auto tokens = tokenize("42.0");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::FLOAT);
    EXPECT_EQ(toks[0]->lexeme, "42.0");
}

TEST(LexerTest, FloatScientificNotation) {
    initLocale();
    auto tokens = tokenize("1.5e10");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::FLOAT);
    EXPECT_EQ(toks[0]->lexeme, "1.5e10");
}

TEST(LexerTest, FloatScientificWithPlus) {
    initLocale();
    auto tokens = tokenize("2.0e+5");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::FLOAT);
    EXPECT_EQ(toks[0]->lexeme, "2.0e+5");
}

TEST(LexerTest, FloatScientificWithMinus) {
    initLocale();
    auto tokens = tokenize("3.14e-2");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::FLOAT);
    EXPECT_EQ(toks[0]->lexeme, "3.14e-2");
}

TEST(LexerTest, FloatScientificUpperCaseE) {
    initLocale();
    auto tokens = tokenize("1.0E100");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::FLOAT);
    EXPECT_EQ(toks[0]->lexeme, "1.0E100");
}

TEST(LexerTest, HexNumberSimple) {
    initLocale();
    auto tokens = tokenize("$FF");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::HEX_NUMBER);
    EXPECT_EQ(toks[0]->lexeme, "$FF");
}

TEST(LexerTest, HexNumberLowercase) {
    initLocale();
    auto tokens = tokenize("$a1b2c3");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::HEX_NUMBER);
    EXPECT_EQ(toks[0]->lexeme, "$a1b2c3");
}

TEST(LexerTest, HexNumberMixedCase) {
    initLocale();
    auto tokens = tokenize("$AbCdEf");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::HEX_NUMBER);
    EXPECT_EQ(toks[0]->lexeme, "$AbCdEf");
}

TEST(LexerTest, HexNumberSingleDigit) {
    initLocale();
    auto tokens = tokenize("$0");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::HEX_NUMBER);
    EXPECT_EQ(toks[0]->lexeme, "$0");
}

TEST(LexerTest, BinaryNumberSimple) {
    initLocale();
    auto tokens = tokenize("%1010");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::BINARY_NUMBER);
    EXPECT_EQ(toks[0]->lexeme, "%1010");
}

TEST(LexerTest, BinaryNumberAllZeros) {
    initLocale();
    auto tokens = tokenize("%0000");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::BINARY_NUMBER);
    EXPECT_EQ(toks[0]->lexeme, "%0000");
}

TEST(LexerTest, BinaryNumberAllOnes) {
    initLocale();
    auto tokens = tokenize("%11111111");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::BINARY_NUMBER);
    EXPECT_EQ(toks[0]->lexeme, "%11111111");
}

// ============================================================
// 标识符测试
// ============================================================

TEST(LexerTest, IdentifierSimple) {
    initLocale();
    auto tokens = tokenize("myVar");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "myVar");
}

TEST(LexerTest, IdentifierStartingWithUnderscore) {
    initLocale();
    auto tokens = tokenize("_temp");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "_temp");
}

TEST(LexerTest, IdentifierWithNumbers) {
    initLocale();
    auto tokens = tokenize("var123");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "var123");
}

TEST(LexerTest, IdentifierSingleLetter) {
    initLocale();
    auto tokens = tokenize("x");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "x");
}

TEST(LexerTest, IdentifierUnderscoreOnly) {
    initLocale();
    auto tokens = tokenize("_");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "_");
}

// ============================================================
// 寄存器测试
// ============================================================

TEST(LexerTest, RegisterSingleDigit) {
    initLocale();
    auto tokens = tokenize("r0");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[0]->lexeme, "r0");
}

TEST(LexerTest, RegisterR9) {
    initLocale();
    auto tokens = tokenize("r9");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[0]->lexeme, "r9");
}

TEST(LexerTest, RegisterR10) {
    initLocale();
    auto tokens = tokenize("r10");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[0]->lexeme, "r10");
}

TEST(LexerTest, RegisterR15) {
    initLocale();
    auto tokens = tokenize("r15");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[0]->lexeme, "r15");
}

// 验证：寄存器在上下文中（后面跟空格）能被正确识别
// 注意：最后一个寄存器后面没有空格时可能不被识别（见上述bug）
TEST(LexerTest, RegisterInContext) {
    initLocale();
    auto tokens = tokenize("r0 r1 r2");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 3u);
    // 前两个寄存器后面有空格，应被正确识别
    EXPECT_EQ(toks[0]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[0]->lexeme, "r0");
    EXPECT_EQ(toks[1]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[1]->lexeme, "r1");
    EXPECT_EQ(toks[2]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[2]->lexeme, "r2");
}


TEST(LexerTest, RegisterR16IsIdentifier) {
    initLocale();
    auto tokens = tokenize("r16");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    // 按g4语法r16应该是标识符，但当前实现可能会解析为寄存器
    // 这里记录实际行为
    EXPECT_TRUE(toks[0]->type == TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "r16");
}

// ============================================================
// 设备测试
// ============================================================

TEST(LexerTest, DeviceD0) {
    initLocale();
    auto tokens = tokenize("d0");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::DEVICE);
    EXPECT_EQ(toks[0]->lexeme, "d0");
}

TEST(LexerTest, DeviceD5) {
    initLocale();
    auto tokens = tokenize("d5");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::DEVICE);
    EXPECT_EQ(toks[0]->lexeme, "d5");
}


TEST(LexerTest, DeviceD6IsIdentifier) {
    initLocale();
    auto tokens = tokenize("d6");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    // d6超出了设备范围，应该被识别为标识符
    EXPECT_EQ(toks[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "d6");
}

// ============================================================
// 字符串测试
// ============================================================

TEST(LexerTest, StringSimple) {
    initLocale();
    auto tokens = tokenize("\"hello\"");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::STRING);
    EXPECT_EQ(toks[0]->lexeme, "\"hello\"");
}

TEST(LexerTest, StringEmpty) {
    initLocale();
    auto tokens = tokenize("\"\"");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::STRING);
    EXPECT_EQ(toks[0]->lexeme, "\"\"");
}

TEST(LexerTest, StringWithSpaces) {
    initLocale();
    auto tokens = tokenize("\"hello world\"");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::STRING);
    EXPECT_EQ(toks[0]->lexeme, "\"hello world\"");
}

TEST(LexerTest, StringWithNumbers) {
    initLocale();
    auto tokens = tokenize("\"test123\"");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::STRING);
    EXPECT_EQ(toks[0]->lexeme, "\"test123\"");
}

// 未闭合字符串的错误处理 - 遇到换行符作为同步点，产生错误诊断并恢复解析
TEST(LexerTest, StringUnclosedProducesError) {
    initLocale();
    Lexer lexer("\"unclosed");
    auto tokens = lexer.scan();
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_GE(toks.size(), 1u);
    // 未闭合字符串应该产生UNKNOWN类型
    EXPECT_EQ(toks[0]->type, TokenType::UNKNOWN);
    // 验证诊断信息存在
    EXPECT_FALSE(lexer.getDiagnostics().empty());
}

// 未闭合字符串在多行输入中的恢复：遇到换行符停止，后续行正常解析
TEST(LexerTest, StringUnclosedMultiLineRecovery) {
    initLocale();
    Lexer lexer("\"unclosed\nadd r0 r1 r2\n");
    auto tokens = lexer.scan();
    auto toks = tokensWithoutEnd(tokens);
    // 第一个token是未闭合字符串（UNKNOWN），不消耗换行符
    EXPECT_EQ(toks[0]->type, TokenType::UNKNOWN);
    // 换行符保留
    EXPECT_EQ(toks[1]->type, TokenType::NEWLINE);
    // 后续行正常解析
    EXPECT_EQ(toks[2]->type, TokenType::KEYWORD_ADD);
    EXPECT_EQ(toks[3]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[4]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[5]->type, TokenType::REGISTER);
    // 应有错误诊断
    EXPECT_FALSE(lexer.getDiagnostics().empty());
}

// ============================================================
// 注释测试
// ============================================================

TEST(LexerTest, HexCommentSimple) {
    initLocale();
    auto tokens = tokenize("# this is a comment");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::HEX_COMMENT);
    EXPECT_EQ(toks[0]->lexeme, "# this is a comment");
    EXPECT_EQ(toks[0]->category, TokenCategory::COMMENT);
}

TEST(LexerTest, HexCommentEmpty) {
    initLocale();
    auto tokens = tokenize("#");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::HEX_COMMENT);
    EXPECT_EQ(toks[0]->lexeme, "#");
}

TEST(LexerTest, SlashCommentSimple) {
    initLocale();
    auto tokens = tokenize("// this is a comment");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::SLASH_COMMENT);
    EXPECT_EQ(toks[0]->lexeme, "// this is a comment");
    EXPECT_EQ(toks[0]->category, TokenCategory::COMMENT);
}

TEST(LexerTest, SlashCommentEmpty) {
    initLocale();
    auto tokens = tokenize("//");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::SLASH_COMMENT);
    EXPECT_EQ(toks[0]->lexeme, "//");
}

TEST(LexerTest, CommentAfterCode) {
    initLocale();
    auto tokens = tokenize("add r0 r1 r2 # comment");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 5u);  // add + r0 + r1 + r2 + comment
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_ADD);
    EXPECT_EQ(toks[1]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[2]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[3]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[4]->type, TokenType::HEX_COMMENT);
}

// ============================================================
// 关键字测试
// ============================================================

TEST(LexerTest, KeywordAlias) {
    initLocale();
    auto tokens = tokenize("alias");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_ALIAS);
    EXPECT_EQ(toks[0]->lexeme, "alias");
}

TEST(LexerTest, KeywordDefine) {
    initLocale();
    auto tokens = tokenize("define");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_DEFINE);
    EXPECT_EQ(toks[0]->lexeme, "define");
}

TEST(LexerTest, KeywordHcf) {
    initLocale();
    auto tokens = tokenize("hcf");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_HCF);
    EXPECT_EQ(toks[0]->lexeme, "hcf");
}

TEST(LexerTest, KeywordYield) {
    initLocale();
    auto tokens = tokenize("yield");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_YIELD);
    EXPECT_EQ(toks[0]->lexeme, "yield");
}

TEST(LexerTest, KeywordAdd) {
    initLocale();
    auto tokens = tokenize("add");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_ADD);
    EXPECT_EQ(toks[0]->lexeme, "add");
}

TEST(LexerTest, KeywordSub) {
    initLocale();
    auto tokens = tokenize("sub");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_SUB);
    EXPECT_EQ(toks[0]->lexeme, "sub");
}

TEST(LexerTest, KeywordMove) {
    initLocale();
    auto tokens = tokenize("move");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_MOVE);
    EXPECT_EQ(toks[0]->lexeme, "move");
}

TEST(LexerTest, KeywordConstantNan) {
    initLocale();
    auto tokens = tokenize("nan");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_NAN);
    EXPECT_EQ(toks[0]->lexeme, "nan");
}

TEST(LexerTest, KeywordConstantPi) {
    initLocale();
    auto tokens = tokenize("pi");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_PI);
    EXPECT_EQ(toks[0]->lexeme, "pi");
}

TEST(LexerTest, KeywordHash) {
    initLocale();
    auto tokens = tokenize("HASH");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_HASH);
    EXPECT_EQ(toks[0]->lexeme, "HASH");
}

TEST(LexerTest, KeywordStr) {
    initLocale();
    auto tokens = tokenize("STR");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_STR);
    EXPECT_EQ(toks[0]->lexeme, "STR");
}


TEST(LexerTest, KeywordRgas) {
    initLocale();
    auto tokens = tokenize("rgas");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_RGAS);
    EXPECT_EQ(toks[0]->lexeme, "rgas");
}

// ============================================================
// 符号测试
// ============================================================

TEST(LexerTest, SymbolLeftParen) {
    initLocale();
    auto tokens = tokenize("(");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::LPAREN);
    EXPECT_EQ(toks[0]->lexeme, "(");
    EXPECT_EQ(toks[0]->category, TokenCategory::SYMBOL);
}

TEST(LexerTest, SymbolRightParen) {
    initLocale();
    auto tokens = tokenize(")");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::RPAREN);
    EXPECT_EQ(toks[0]->lexeme, ")");
}

TEST(LexerTest, SymbolColon) {
    initLocale();
    auto tokens = tokenize(":");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::COLON);
    EXPECT_EQ(toks[0]->lexeme, ":");
}

// ============================================================
// 换行测试
// ============================================================

TEST(LexerTest, NewlineSingle) {
    initLocale();
    auto tokens = tokenize("\n");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->type, TokenType::NEWLINE);
    EXPECT_EQ(toks[0]->category, TokenCategory::WHITESPACE);
}

TEST(LexerTest, NewlineMultiple) {
    initLocale();
    auto tokens = tokenize("\n\n\n");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 3u);
    for (const auto& tok : toks) {
        EXPECT_EQ(tok->type, TokenType::NEWLINE);
    }
}

// ============================================================
// 位置信息测试
// ============================================================

TEST(LexerTest, PositionFirstToken) {
    initLocale();
    auto tokens = tokenize("add");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->pos.line(), 1);
    EXPECT_EQ(toks[0]->pos.column(), 1);
    EXPECT_EQ(toks[0]->pos.offset(), 0u);
}

TEST(LexerTest, PositionAfterSpaces) {
    initLocale();
    auto tokens = tokenize("   add");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0]->pos.line(), 1);
    EXPECT_EQ(toks[0]->pos.column(), 4);
    EXPECT_EQ(toks[0]->pos.offset(), 3u);
}

TEST(LexerTest, PositionSecondLine) {
    initLocale();
    auto tokens = tokenize("add\nsub");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 3u);  // add + newline + sub
    EXPECT_EQ(toks[0]->pos.line(), 1);
    EXPECT_EQ(toks[2]->pos.line(), 2);
    EXPECT_EQ(toks[2]->pos.column(), 1);
}

// ============================================================
// 多token组合测试
// ============================================================

TEST(LexerTest, MultipleIntegers) {
    initLocale();
    auto tokens = tokenize("1 2 3");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 3u);
    EXPECT_EQ(toks[0]->type, TokenType::INTEGER);
    EXPECT_EQ(toks[0]->lexeme, "1");
    EXPECT_EQ(toks[1]->type, TokenType::INTEGER);
    EXPECT_EQ(toks[1]->lexeme, "2");
    EXPECT_EQ(toks[2]->type, TokenType::INTEGER);
    EXPECT_EQ(toks[2]->lexeme, "3");
}

TEST(LexerTest, SimpleInstructionAdd) {
    initLocale();
    // 末尾加换行确保最后一个寄存器后面有空白字符
    auto tokens = tokenize("add r0 r1 r2\n");
    auto toks = tokensWithoutEnd(tokens);
    // 减去末尾的newline
    ASSERT_EQ(toks.size(), 5u);  // add + r0 + r1 + r2 + newline
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_ADD);
    EXPECT_EQ(toks[0]->lexeme, "add");
    EXPECT_EQ(toks[1]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[1]->lexeme, "r0");
    EXPECT_EQ(toks[2]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[2]->lexeme, "r1");
    EXPECT_EQ(toks[3]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[3]->lexeme, "r2");
    EXPECT_EQ(toks[4]->type, TokenType::NEWLINE);
}

TEST(LexerTest, AliasDirective) {
    initLocale();
    // 末尾加换行确保最后一个寄存器后面有空白字符
    auto tokens = tokenize("alias foo r0\n");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 4u);  // alias + foo + r0 + newline
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_ALIAS);
    EXPECT_EQ(toks[1]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[2]->type, TokenType::REGISTER);
    EXPECT_EQ(toks[3]->type, TokenType::NEWLINE);
}

TEST(LexerTest, LabelDefinition) {
    initLocale();
    auto tokens = tokenize("myLabel:");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 2u);
    EXPECT_EQ(toks[0]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(toks[0]->lexeme, "myLabel");
    EXPECT_EQ(toks[1]->type, TokenType::COLON);
    EXPECT_EQ(toks[1]->lexeme, ":");
}

TEST(LexerTest, HashCallSyntax) {
    initLocale();
    auto tokens = tokenize("HASH(\"test\")");
    auto toks = tokensWithoutEnd(tokens);
    ASSERT_EQ(toks.size(), 4u);
    EXPECT_EQ(toks[0]->type, TokenType::KEYWORD_HASH);
    EXPECT_EQ(toks[1]->type, TokenType::LPAREN);
    EXPECT_EQ(toks[2]->type, TokenType::STRING);
    EXPECT_EQ(toks[3]->type, TokenType::RPAREN);
}

// ============================================================
// END token测试
// ============================================================

TEST(LexerTest, EndTokenAlwaysPresent) {
    initLocale();
    auto tokens = tokenize("add r0 r1 r2");
    ASSERT_GE(tokens.size(), 1u);
    EXPECT_EQ(tokens.back()->type, TokenType::END);
    EXPECT_EQ(tokens.back()->category, TokenCategory::END);
}

TEST(LexerTest, EndTokenPosition) {
    initLocale();
    auto tokens = tokenize("abc");
    ASSERT_GE(tokens.size(), 1u);
    auto endTok = tokens.back();
    EXPECT_EQ(endTok->type, TokenType::END);
    // END token应该在输入末尾
    EXPECT_EQ(endTok->pos.offset(), 3u);
}

// ============================================================
// 令牌边界检查测试（IEL3_2: 令牌间缺少空白分隔）
// 参考IC10.g4: 词法规则间默认需要至少一个空格（词和注释间除外）
// ============================================================

TEST(LexerTest, IntegerFollowedByIdentifierNoSpace) {
    initLocale();
    Lexer lexer("100abc");
    (void)lexer.scan();
    EXPECT_FALSE(lexer.getDiagnostics().empty());
}

TEST(LexerTest, IntegerFollowedByIdentifierWithSpace) {
    initLocale();
    Lexer lexer("100 abc");
    (void)lexer.scan();
    EXPECT_TRUE(lexer.getDiagnostics().empty());
}

TEST(LexerTest, HexNumberFollowedByNonHexChar) {
    initLocale();
    Lexer lexer("$ffg");
    (void)lexer.scan();
    EXPECT_FALSE(lexer.getDiagnostics().empty());
}

TEST(LexerTest, BinaryNumberFollowedByIdentifier) {
    initLocale();
    Lexer lexer("%101abc");
    (void)lexer.scan();
    EXPECT_FALSE(lexer.getDiagnostics().empty());
}

TEST(LexerTest, StringFollowedByIdentifierNoSpace) {
    initLocale();
    Lexer lexer("\"hello\"world");
    (void)lexer.scan();
    EXPECT_FALSE(lexer.getDiagnostics().empty());
}

TEST(LexerTest, NumberFollowedBySymbolNoError) {
    initLocale();
    // 符号（如冒号、括号）不需要空格分隔
    Lexer lexer("main:");
    (void)lexer.scan();
    EXPECT_TRUE(lexer.getDiagnostics().empty());
}
