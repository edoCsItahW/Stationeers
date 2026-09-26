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
#include "ic10_compiler/incremental/inc_compiler.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include <algorithm>
#include <gtest/gtest.h>

using namespace stationeers::ic10;
using stationeers::Diagnostic;

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
        std::size_t aIdx = actual.statements[i].raw().index();
        std::size_t eIdx = expected.statements[i].raw().index();
        EXPECT_EQ(aIdx, eIdx);
        int aLine = 0, aCol = 0;
        std::size_t aOff = 0;
        int eLine = 0, eCol = 0;
        std::size_t eOff = 0;
        std::visit([&](const auto& n) {
            aLine = n.position.line();
            aCol = n.position.column();
            aOff = n.position.offset();
        }, actual.statements[i].raw());
        std::visit([&](const auto& n) {
            eLine = n.position.line();
            eCol = n.position.column();
            eOff = n.position.offset();
        }, expected.statements[i].raw());
        EXPECT_EQ(aLine, eLine);
        EXPECT_EQ(aCol, eCol);
        EXPECT_EQ(aOff, eOff);
    }
}

// 辅助：诊断列表中是否存在指定 id 的诊断
bool hasDiagnostic(const std::vector<Diagnostic>& diagnostics, std::string_view id) {
    return std::ranges::any_of(diagnostics, [&](const Diagnostic& d) { return d.id == id; });
}

// 辅助：统计指定 id 的诊断条数（增量路径复用前缀诊断时用于确认没有重复报告）
std::size_t countDiagnostic(const std::vector<Diagnostic>& diagnostics, std::string_view id) {
    return static_cast<std::size_t>(std::ranges::count_if(
        diagnostics, [&](const Diagnostic& d) { return d.id == id; }
    ));
}

// 辅助：诊断列表中是否存在指定 id 且起始行等于 line 的诊断（增量路径下验证位置平移）
bool hasDiagnosticAtLine(const std::vector<Diagnostic>& diagnostics, std::string_view id, int line) {
    return std::ranges::any_of(diagnostics, [&](const Diagnostic& d) {
        return d.id == id && d.start.has_value() && d.start->line() == line;
    });
}

// 辅助：把诊断压成 "id@line:col" 列表，断言失败时便于排查
std::string formatDiagnostics(const std::vector<Diagnostic>& diagnostics) {
    std::string result;

    for (const auto& diagnostic : diagnostics) {
        if (!result.empty()) result += ", ";

        result += diagnostic.id;

        if (diagnostic.start)
            result += "@" + std::to_string(diagnostic.start->line()) + ":"
                    + std::to_string(diagnostic.start->column());
    }

    return result.empty() ? "<empty>" : result;
}

}  // namespace

// ============================================================
// 增量词法分析测试
// ============================================================

TEST(IncrementalLexerTest, FullLexSingleLineMatchesBaseline) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42";
    IncLexer lexer;
    auto result = lexer.tokenizeFull(src);
    auto baseline = baselineTokens(src);
    // 去掉 END 之前应该一样
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, FullLexMultiLinePositionsMatchBaseline) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42\nsub r2 r3 100\nyield";
    IncLexer lexer;
    auto result = lexer.tokenizeFull(src);
    auto baseline = baselineTokens(src);
    expectTokensMatch(result.tokens, baseline);
}

TEST(IncrementalLexerTest, NoChangeReturnsIncremental) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42\nyield";
    auto tokens = Lexer::tokenize(src);
    IncParser parser;
    auto result = parser.parseFull(tokens);
    auto baseline = Parser::parsing(tokens);
    expectProgramsMatch(result.ast, baseline);
}

TEST(IncrementalParserTest, ModifyMiddleStatementMatchesBaseline) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    std::string src = "add r0 r1 42\nmul r2 r3 10";
    IncCompiler compiler;
    auto result = compiler.compileFull(src);
    EXPECT_FALSE(result.incremental);
    EXPECT_GT(result.ast.statements.size(), 0u);
}

TEST(IncrementalCompilerTest, IncrementalModifyMatchesFull) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
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

// ============================================================
// 增量诊断传递测试
//
// 增量路径下的 diagnostics 必须覆盖整份源码：未变化的行/语句沿用缓存时的诊断，
// 变化的行/语句用本次诊断，后缀位置平移后诊断也要跟着平移。否则编辑器在编辑
// 期间会停留在旧文本的错误上（这正是 LSP 端曾出现「改了属性却没有任何提示」的原因）。
// ============================================================

TEST(IncrementalDiagnosticsTest, LexerDiagnosticsCoverWholeSourceOnFullLex) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    IncLexer lexer;

    // 等价类：全量 + 无错
    auto clean = lexer.tokenizeFull("add r0 r1 42\nsub r2 r3 100");
    EXPECT_TRUE(clean.diagnostics.empty())
        << "合法源码不应有词法诊断，实际: " << formatDiagnostics(clean.diagnostics);

    // 等价类：全量 + 有错（IEL3_1：令牌后缺少空白分隔，如 `100` 与 `abc` 粘连），错误不在首行也要报出
    auto dirty = lexer.tokenizeFull("add r0 r1 42\n100abc");
    EXPECT_TRUE(hasDiagnosticAtLine(dirty.diagnostics, "IEL3_1", 2))
        << "第 2 行的词法错误应报出，实际: " << formatDiagnostics(dirty.diagnostics);
}

TEST(IncrementalDiagnosticsTest, IncrementalLexRefreshesChangedLine) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    IncLexer lexer;
    lexer.tokenizeFull("add r0 r1 42\nsub r2 r3 100");

    // 只改动第 2 行：错误必须立即出现，而不能停留在上一次的诊断上
    auto broken = lexer.tokenizeInc("add r0 r1 42\n100abc");
    EXPECT_TRUE(broken.incremental);
    EXPECT_TRUE(hasDiagnosticAtLine(broken.diagnostics, "IEL3_1", 2))
        << "改动后的词法错误应立即出现，实际: " << formatDiagnostics(broken.diagnostics);

    // 再改回合法内容：诊断必须随之消失
    auto fixed = lexer.tokenizeInc("add r0 r1 42\nsub r2 r3 100");
    EXPECT_TRUE(fixed.diagnostics.empty())
        << "改回合法内容后诊断应消失，实际: " << formatDiagnostics(fixed.diagnostics);
}

TEST(IncrementalDiagnosticsTest, InsertedLineShiftsCachedDiagnostics) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    IncLexer lexer;

    auto full = lexer.tokenizeFull("add r0 r1 42\n100abc");
    ASSERT_TRUE(hasDiagnosticAtLine(full.diagnostics, "IEL3_1", 2));

    // 边界：在错误行之前插入一行 —— 该行本身未被重新词法分析，其诊断来自缓存，
    // 必须随行号/偏移差值一起平移
    auto shifted = lexer.tokenizeInc("add r0 r1 42\nmul r4 r5 3\n100abc");
    EXPECT_TRUE(shifted.incremental);
    EXPECT_TRUE(hasDiagnosticAtLine(shifted.diagnostics, "IEL3_1", 3))
        << "插入行后缓存诊断应平移到第 3 行，实际: " << formatDiagnostics(shifted.diagnostics);
}

TEST(IncrementalDiagnosticsTest, ParserDiagnosticsCarriedForPrefixAndRefreshedForSuffix) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    IncParser parser;

    auto full = parser.parseFull(Lexer::tokenize("hcf yield\nhcf\nhcf\n"));
    ASSERT_TRUE(hasDiagnosticAtLine(full.diagnostics, "IEP26", 1))
        << "第 1 行缺少换行分隔应报 IEP26，实际: " << formatDiagnostics(full.diagnostics);

    // 边界：变化行在第 4 行 —— 第 1 行属于复用的前缀语句，其诊断应保留且不重复
    auto carried = parser.parseInc(Lexer::tokenize("hcf yield\nhcf\nhcf\nyield\n"), 4);
    EXPECT_TRUE(carried.incremental);
    EXPECT_TRUE(hasDiagnosticAtLine(carried.diagnostics, "IEP26", 1))
        << "复用前缀的语法诊断应保留，实际: " << formatDiagnostics(carried.diagnostics);
    EXPECT_EQ(countDiagnostic(carried.diagnostics, "IEP26"), 1u)
        << "复用前缀的诊断不应重复，实际: " << formatDiagnostics(carried.diagnostics);

    // 修好出错的第 1 行：变化行即错误行，陈旧诊断必须被整体替换
    auto fixed = parser.parseInc(Lexer::tokenize("hcf\nyield\nhcf\nhcf\nyield\n"), 1);
    EXPECT_TRUE(fixed.diagnostics.empty())
        << "修好错误行后诊断应消失，实际: " << formatDiagnostics(fixed.diagnostics);
}

TEST(IncrementalDiagnosticsTest, CompileResultCombinesLexicalAndSyntax) {
    ICLoc::registerLanguage<ZhHans>("zh-hans");
    ICLoc::setLanguage("zh-hans");
    IncCompiler compiler;

    // 第 1 行是词法错误，第 2 行是语法错误，二者都应出现在结果里
    auto result = compiler.compileFull("100abc\nhcf yield\n");
    EXPECT_TRUE(hasDiagnosticAtLine(result.diagnostics, "IEL3_1", 1))
        << "结果应含词法诊断，实际: " << formatDiagnostics(result.diagnostics);
    EXPECT_TRUE(hasDiagnosticAtLine(result.diagnostics, "IEP26", 2))
        << "结果应含语法诊断，实际: " << formatDiagnostics(result.diagnostics);

    // 语义诊断不属于增量结果（需要另行执行 Linker），未定义标识符不应在此报出
    auto semanticOnly = compiler.compileFull("move r0 notDefined\n");
    EXPECT_FALSE(hasDiagnostic(semanticOnly.diagnostics, "IEA3_1"))
        << "增量结果不应包含语义诊断，实际: " << formatDiagnostics(semanticOnly.diagnostics);
}
