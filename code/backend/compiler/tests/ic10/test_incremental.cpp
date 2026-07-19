// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_incremental.cpp
 * @brief 增量编译功能单元测试
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "../../IC10/include/ic10/incremental/inc_compiler.hpp"
#include "ic10/lexer/lexer.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include <gtest/gtest.h>

using namespace stationeers::ic10;

namespace {

std::string tokenPosStr(const std::shared_ptr<Token>& tok) {
    return "line=" + std::to_string(tok->pos.line())
         + ", col=" + std::to_string(tok->pos.column())
         + ", off=" + std::to_string(tok->pos.offset())
         + ", type=" + std::to_string(static_cast<int>(tok->type))
         + ", lex=" + tok->lexeme;
}

// 辅助：用全量 Lexer 产生 token 作为参照基准
std::vector<std::shared_ptr<Token>> baselineTokens(std::string_view src) {
    return Lexer::tokenize(src);
}

// 辅助：比较两个 token 序列的位置信息
void expectTokensMatch(
    const std::vector<std::shared_ptr<Token>>& actual,
    const std::vector<std::shared_ptr<Token>>& expected
) {
    ASSERT_EQ(actual.size(), expected.size()) << "token count mismatch";
    for (std::size_t i = 0; i < actual.size(); ++i) {
        SCOPED_TRACE("token index " + std::to_string(i)
                     + "\n  actual:   " + tokenPosStr(actual[i])
                     + "\n  expected: " + tokenPosStr(expected[i]));
        EXPECT_EQ(actual[i]->type, expected[i]->type);
        EXPECT_EQ(actual[i]->lexeme, expected[i]->lexeme);
        int aLine = actual[i]->pos.line();
        int eLine = expected[i]->pos.line();
        EXPECT_EQ(aLine, eLine);
        int aCol = actual[i]->pos.column();
        int eCol = expected[i]->pos.column();
        EXPECT_EQ(aCol, eCol);
        std::size_t aOff = actual[i]->pos.offset();
        std::size_t eOff = expected[i]->pos.offset();
        EXPECT_EQ(aOff, eOff);
    }
}

// 辅助：比较两个 Program 的语句数量和类型
void expectProgramsMatch(const Program& actual, const Program& expected) {
    std::size_t aSize = actual.statements.size();
    std::size_t eSize = expected.statements.size();
    ASSERT_EQ(aSize, eSize);
    for (std::size_t i = 0; i < actual.statements.size(); ++i) {
        SCOPED_TRACE("statement index " + std::to_string(i));
        std::size_t aIdx = actual.statements[i].index();
        std::size_t eIdx = expected.statements[i].index();
        EXPECT_EQ(aIdx, eIdx);
        int aLine = 0, aCol = 0;
        std::size_t aOff = 0;
        int eLine = 0, eCol = 0;
        std::size_t eOff = 0;
        std::visit([&](const auto& n) {
            aLine = n.position.line();
            aCol = n.position.column();
            aOff = n.position.offset();
        }, actual.statements[i]);
        std::visit([&](const auto& n) {
            eLine = n.position.line();
            eCol = n.position.column();
            eOff = n.position.offset();
        }, expected.statements[i]);
        EXPECT_EQ(aLine, eLine);
        EXPECT_EQ(aCol, eCol);
        EXPECT_EQ(aOff, eOff);
    }
}

}  // namespace

// ============================================================
// 增量词法分析测试
// ============================================================

TEST(IncrementalLexerTest, FullLexSingleLineMatchesBaseline) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42";
    IncLexer lexer;
    auto result = lexer.tokenizeFull(src);
    auto baseline = baselineTokens(src);
    // 去掉 END 之前应该一样
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, FullLexMultiLinePositionsMatchBaseline) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42\nsub r2 r3 100\nyield";
    IncLexer lexer;
    auto result = lexer.tokenizeFull(src);
    auto baseline = baselineTokens(src);
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, NoChangeReturnsIncremental) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1\nsub r2 r3";
    IncLexer lexer;
    lexer.tokenizeFull(src);
    auto result = lexer.tokenizeInc(src);
    EXPECT_TRUE(result.incremental);
    EXPECT_EQ(result.relexedLines, 0u);
    auto baseline = baselineTokens(src);
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, ModifyMiddleLinePositionsCorrect) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1\nsub r2 r3 2\nmul r4 r5 3";
    IncLexer lexer;
    lexer.tokenizeFull(src);

    std::string newSrc = "add r0 r1 1\nsdiv r2 r3 999\nmul r4 r5 3";
    auto result = lexer.tokenizeInc(newSrc);
    EXPECT_TRUE(result.incremental);
    EXPECT_GT(result.relexedLines, 0u);

    auto baseline = baselineTokens(newSrc);
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, InsertLinePositionsCorrect) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1\nmul r4 r5";
    IncLexer lexer;
    lexer.tokenizeFull(src);

    std::string newSrc = "add r0 r1\nsub r2 r3\nmul r4 r5";
    auto result = lexer.tokenizeInc(newSrc);
    EXPECT_TRUE(result.incremental);

    auto baseline = baselineTokens(newSrc);
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, DeleteLinePositionsCorrect) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1\nsub r2 r3\nmul r4 r5";
    IncLexer lexer;
    lexer.tokenizeFull(src);

    std::string newSrc = "add r0 r1\nmul r4 r5";
    auto result = lexer.tokenizeInc(newSrc);
    EXPECT_TRUE(result.incremental);

    auto baseline = baselineTokens(newSrc);
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, AppendLinePositionsCorrect) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1";
    IncLexer lexer;
    lexer.tokenizeFull(src);

    std::string newSrc = "add r0 r1\nsub r2 r3";
    auto result = lexer.tokenizeInc(newSrc);
    EXPECT_TRUE(result.incremental);

    auto baseline = baselineTokens(newSrc);
    expectTokensMatch(result.tokens, baseline);
}

// ============================================================
// 增量语法分析测试
// ============================================================

TEST(IncrementalParserTest, FullParseMatchesBaseline) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42\nyield";
    auto tokens = Lexer::tokenize(src);
    IncParser parser;
    auto result = parser.parseFull(tokens);
    auto baseline = Parser::parsing(tokens);
    expectProgramsMatch(result.ast, baseline);
}

TEST(IncrementalParserTest, ModifyMiddleStatementMatchesBaseline) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1\nmul r2 r3 2\ndiv r4 r5 3";
    auto fullTokens = Lexer::tokenize(src);
    IncParser parser;
    parser.parseFull(fullTokens);

    std::string newSrc = "add r0 r1 1\ndiv r2 r3 999\nmul r4 r5 3";
    auto newTokens = Lexer::tokenize(newSrc);

    auto result = parser.parseInc(newTokens, 2);
    EXPECT_TRUE(result.incremental);

    auto baseline = Parser::parsing(newTokens);
    expectProgramsMatch(result.ast, baseline);
}

TEST(IncrementalParserTest, InsertStatementMatchesBaseline) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1\nmul r4 r5 3";
    auto fullTokens = Lexer::tokenize(src);
    IncParser parser;
    parser.parseFull(fullTokens);

    std::string newSrc = "add r0 r1 1\ndiv r2 r3 2\nmul r4 r5 3";
    auto newTokens = Lexer::tokenize(newSrc);

    auto result = parser.parseInc(newTokens, 2);
    EXPECT_TRUE(result.incremental);

    auto baseline = Parser::parsing(newTokens);
    expectProgramsMatch(result.ast, baseline);
}

TEST(IncrementalParserTest, DeleteStatementMatchesBaseline) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1\ndiv r2 r3 2\nmul r4 r5 3";
    auto fullTokens = Lexer::tokenize(src);
    IncParser parser;
    parser.parseFull(fullTokens);

    std::string newSrc = "add r0 r1 1\nmul r4 r5 3";
    auto newTokens = Lexer::tokenize(newSrc);

    auto result = parser.parseInc(newTokens, 2);
    EXPECT_TRUE(result.incremental);

    auto baseline = Parser::parsing(newTokens);
    expectProgramsMatch(result.ast, baseline);
}

TEST(IncrementalParserTest, AppendStatementMatchesBaseline) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1";
    auto fullTokens = Lexer::tokenize(src);
    IncParser parser;
    parser.parseFull(fullTokens);

    std::string newSrc = "add r0 r1 1\ndiv r2 r3 2";
    auto newTokens = Lexer::tokenize(newSrc);

    auto result = parser.parseInc(newTokens, 2);
    EXPECT_TRUE(result.incremental);

    auto baseline = Parser::parsing(newTokens);
    expectProgramsMatch(result.ast, baseline);
}

TEST(IncrementalParserTest, LabelAndAliasIncremental) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "label1:\nalias foo r0\nadd foo r1 42";
    auto fullTokens = Lexer::tokenize(src);
    IncParser parser;
    parser.parseFull(fullTokens);

    std::string newSrc = "label1:\nalias bar r0\nadd bar r1 42";
    auto newTokens = Lexer::tokenize(newSrc);

    auto result = parser.parseInc(newTokens, 2);
    EXPECT_TRUE(result.incremental);

    auto baseline = Parser::parsing(newTokens);
    expectProgramsMatch(result.ast, baseline);
}

// ============================================================
// 增量编译器整合测试
// ============================================================

TEST(IncrementalCompilerTest, FullCompileWorks) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42\nmul r2 r3 10";
    IncCompiler compiler;
    auto result = compiler.compileFull(src);
    EXPECT_FALSE(result.incremental);
    EXPECT_GT(result.ast.statements.size(), 0u);
}

TEST(IncrementalCompilerTest, IncrementalModifyMatchesFull) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1\nmul r2 r3 2\ndiv r4 r5 3";
    IncCompiler compiler;
    compiler.compileFull(src);

    std::string newSrc = "add r0 r1 1\ndiv r2 r3 999\nmul r4 r5 3";
    auto result = compiler.compileInc(newSrc);
    EXPECT_TRUE(result.incremental);

    // 和全量编译结果比较
    IncCompiler compiler2;
    auto fullResult = compiler2.compileFull(newSrc);

    expectProgramsMatch(result.ast, fullResult.ast);
}

TEST(IncrementalCompilerTest, IncrementalInsertMatchesFull) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1\nmul r4 r5 3";
    IncCompiler compiler;
    compiler.compileFull(src);

    std::string newSrc = "add r0 r1 1\ndiv r2 r3 2\nmul r4 r5 3";
    auto result = compiler.compileInc(newSrc);
    EXPECT_TRUE(result.incremental);

    IncCompiler compiler2;
    auto fullResult = compiler2.compileFull(newSrc);
    expectProgramsMatch(result.ast, fullResult.ast);
}

TEST(IncrementalCompilerTest, IncrementalDeleteMatchesFull) {
    ILoc::registerLanguage<ZhHans>("zh-hans");
    ILoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 1\ndiv r2 r3 2\nmul r4 r5 3";
    IncCompiler compiler;
    compiler.compileFull(src);

    std::string newSrc = "add r0 r1 1\nmul r4 r5 3";
    auto result = compiler.compileInc(newSrc);
    EXPECT_TRUE(result.incremental);

    IncCompiler compiler2;
    auto fullResult = compiler2.compileFull(newSrc);
    expectProgramsMatch(result.ast, fullResult.ast);
}
