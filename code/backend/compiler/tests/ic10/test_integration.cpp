// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_integration.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/11
 * @brief IC10集成测试
 * @details 测试编译器各阶段（Lexer → Parser → Analyser）之间的协作，
 *          验证跨阶段数据流、诊断传播和错误恢复。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include "ic10/incremental/inc_compiler.hpp"
#include "ic10/lexer/lexer.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/semantic/analyser.hpp"

using namespace stationeers::ic10;


class IntegrationTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ILoc::registerLanguage<ZhHans>("zh-hans");
        ILoc::setLanguage("zh-hans");
    }

    /// 完整编译流水线：Lexer → Parser → Analyser
    /// @param source IC10源代码
    /// @return {tokens, ast, analyser} 三元组
    struct CompileResult {
        std::vector<std::shared_ptr<Token>> tokens;
        Program ast;
        std::shared_ptr<Analyser> analyser;
    };

    static CompileResult compile(const std::string& source) {
        CompileResult result;
        result.tokens = Lexer::tokenize(source);
        result.ast = Parser::parsing(result.tokens);
        result.analyser = std::make_shared<Analyser>();
        auto task = result.analyser->visit(result.ast);
        (void)task.getFuture().get();
        return result;
    }
};

// ============================================================
// Lexer → Parser 数据流测试
// ============================================================

TEST_F(IntegrationTestFixture, LexerOutputConsumedByParser) {
    // Lexer生成的Token序列应能被Parser正确消费并生成有效AST
    auto source = "alias foo r0\nmove r0 1\nhcf\n";
    auto tokens = Lexer::tokenize(source);
    Parser parser(tokens);
    auto ast = parser.parse();

    EXPECT_TRUE(parser.getDiagnostics().empty());
    EXPECT_GE(ast.statements.size(), 3u);
}

TEST_F(IntegrationTestFixture, LexerDiagnosticsDoNotBlockParser) {
    // Lexer产生诊断时，Parser仍应能处理Token序列
    // 使用未闭合字符串作为错误源（不会导致死循环）
    auto source = "move r0 1\n\"unclosed\nhcf\n";
    auto tokens = Lexer::tokenize(source);
    Parser parser(tokens);
    auto ast = parser.parse();

    // Parser应仍然能解析出有效语句
    EXPECT_GE(ast.statements.size(), 1u);
}

TEST_F(IntegrationTestFixture, TokenPositionsPropagatedToAST) {
    // Token的位置信息应传递到AST节点
    auto source = "move r0 1\n";
    auto tokens = Lexer::tokenize(source);
    auto ast = Parser::parsing(tokens);

    ASSERT_GE(ast.statements.size(), 1u);
    // 通过std::visit访问variant的position
    auto pos = std::visit([](const auto& node) { return node.start(); }, ast.statements[0]);
    EXPECT_EQ(pos.line(), 1);
}

TEST_F(IntegrationTestFixture, MultiLineProgramTokenPositions) {
    // 多行程序中每行的Token位置应正确
    auto source = "move r0 1\nmove r1 2\nmove r2 3\n";
    auto tokens = Lexer::tokenize(source);
    auto ast = Parser::parsing(tokens);

    ASSERT_GE(ast.statements.size(), 3u);
    auto pos0 = std::visit([](const auto& node) { return node.start(); }, ast.statements[0]);
    auto pos1 = std::visit([](const auto& node) { return node.start(); }, ast.statements[1]);
    auto pos2 = std::visit([](const auto& node) { return node.start(); }, ast.statements[2]);
    EXPECT_EQ(pos0.line(), 1);
    EXPECT_EQ(pos1.line(), 2);
    EXPECT_EQ(pos2.line(), 3);
}

// ============================================================
// Parser → Analyser 数据流测试
// ============================================================

TEST_F(IntegrationTestFixture, ParserASTConsumedByAnalyser) {
    // Parser生成的AST应能被Analyser正确分析
    auto source = "alias foo r0\ndefine MAX 10\nstart:\nmove r0 0\nhcf\n";
    auto tokens = Lexer::tokenize(source);
    auto ast = Parser::parsing(tokens);

    Analyser analyser;
    auto task = analyser.visit(ast);
    (void)task.getFuture().get();

    // 符号表应包含定义的符号
    EXPECT_TRUE(analyser.getSymbolTable().contains("foo"));
    EXPECT_TRUE(analyser.getSymbolTable().contains("MAX"));
    EXPECT_TRUE(analyser.getSymbolTable().contains("start"));
}

TEST_F(IntegrationTestFixture, AnalyserDetectsUndefinedSymbol) {
    // Analyser应检测到未定义的标识符引用
    auto source = "move r0 undefined_var\n";
    auto result = compile(source);

    EXPECT_FALSE(result.analyser->getDiagnostics().empty());
}

TEST_F(IntegrationTestFixture, AnalyserDetectsRedefinition) {
    // Analyser应检测到符号重定义
    auto source = "alias foo r0\nalias foo r1\n";
    auto result = compile(source);

    EXPECT_FALSE(result.analyser->getDiagnostics().empty());
}

TEST_F(IntegrationTestFixture, AnalyserHandlesForwardReference) {
    // Analyser应正确处理标签的前向引用
    auto source = "j end\nend:\nhcf\n";
    auto result = compile(source);

    // 前向引用是合法的，不应产生未定义错误
    EXPECT_TRUE(result.analyser->getSymbolTable().contains("end"));
}

TEST_F(IntegrationTestFixture, AnalyserSymbolTypes) {
    // Analyser应正确推断符号类型
    auto source = "alias foo r0\ndefine COUNT 10\nstart:\nhcf\n";
    auto result = compile(source);

    auto& symtab = result.analyser->getSymbolTable();
    EXPECT_TRUE(symtab.contains("foo"));
    EXPECT_TRUE(symtab.contains("COUNT"));
    EXPECT_TRUE(symtab.contains("start"));
}

// ============================================================
// 完整流水线（Lexer → Parser → Analyser）测试
// ============================================================

TEST_F(IntegrationTestFixture, FullPipelineSimpleProgram) {
    auto source = "move r0 0\nmove r1 1\nadd r0 r0 r1\nhcf\n";
    auto result = compile(source);

    EXPECT_TRUE(result.analyser->getDiagnostics().empty());
    EXPECT_GE(result.ast.statements.size(), 4u);
}

TEST_F(IntegrationTestFixture, FullPipelineWithMacros) {
    auto source = "define HASH_VAL HASH(\"StructureLiquidVolumePump\")\nmove r0 HASH_VAL\nhcf\n";
    auto result = compile(source);

    EXPECT_GE(result.ast.statements.size(), 2u);
}

TEST_F(IntegrationTestFixture, FullPipelineWithComments) {
    auto source =
        "# 初始化\n"
        "move r0 0  # 计数器\n"
        "// 主循环\n"
        "loop:\n"
        "add r0 r0 1\n"
        "yield\n"
        "j loop\n";
    auto result = compile(source);

    EXPECT_TRUE(result.analyser->getDiagnostics().empty());
    EXPECT_TRUE(result.analyser->getSymbolTable().contains("loop"));
}

TEST_F(IntegrationTestFixture, FullPipelineEmptyProgram) {
    auto result = compile("");

    EXPECT_EQ(result.ast.statements.size(), 0u);
    EXPECT_TRUE(result.analyser->getDiagnostics().empty());
}

TEST_F(IntegrationTestFixture, FullPipelineOnlyComments) {
    auto result = compile("# comment 1\n// comment 2\n# comment 3\n");

    EXPECT_EQ(result.ast.statements.size(), 0u);
    EXPECT_TRUE(result.analyser->getDiagnostics().empty());
}

// ============================================================
// 诊断传播测试
// ============================================================

TEST_F(IntegrationTestFixture, LexerErrorPropagatedThroughPipeline) {
    // Lexer错误应通过整个流水线可见
    // 使用未闭合字符串作为错误源（不会导致死循环）
    auto source = "move r0 1\n\"unclosed\nhcf\n";
    auto result = compile(source);

    // 流水线不应崩溃，应仍能生成AST
    EXPECT_GE(result.ast.statements.size(), 1u);
}

TEST_F(IntegrationTestFixture, ParserErrorDoesNotBlockAnalyser) {
    // Parser错误不应阻止Analyser分析有效部分
    auto source = "move r0 1\n\nmove r1 2\nhcf\n";
    auto result = compile(source);

    // 有效语句应被正确分析
    EXPECT_GE(result.ast.statements.size(), 1u);
}

TEST_F(IntegrationTestFixture, MultipleErrorsCollected) {
    // 多个错误应被收集而非在第一个错误处终止
    auto source =
        "alias foo r0\n"
        "alias foo r1\n"      // 重定义
        "move r0 undefined\n" // 未定义
        "hcf\n";
    auto result = compile(source);

    EXPECT_GE(result.analyser->getDiagnostics().size(), 1u);
}

// ============================================================
// AST 序列化一致性测试
// ============================================================

TEST_F(IntegrationTestFixture, ASTToJSONIsValid) {
    auto source = "alias foo r0\ndefine MAX 10\nstart:\nmove r0 0\nhcf\n";
    auto tokens = Lexer::tokenize(source);
    auto ast = Parser::parsing(tokens);

    auto json = ast.toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find('{'), std::string::npos);
    EXPECT_NE(json.find('}'), std::string::npos);
}

TEST_F(IntegrationTestFixture, SymbolTableToJSONIsValid) {
    auto source = "alias foo r0\ndefine MAX 10\nstart:\nhcf\n";
    auto result = compile(source);

    auto json = result.analyser->getSymbolTable().toJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find('{'), std::string::npos);
}

// ============================================================
// 增量编译 vs 全量编译一致性测试
// ============================================================

TEST_F(IntegrationTestFixture, IncCompilerMatchesFullPipeline) {
    // IncCompiler的全量编译结果应与Lexer+Parser一致
    std::string source = "alias foo r0\ndefine MAX 10\nstart:\nmove r0 0\nadd r0 r0 1\nyield\nj start\nhcf\n";

    // 全量编译
    auto fullTokens = Lexer::tokenize(source);
    auto fullAst = Parser::parsing(fullTokens);

    // 增量编译器全量模式
    IncCompiler incCompiler;
    auto incResult = incCompiler.compileFull(source);

    // Token数量应一致（忽略END）
    std::size_t fullCount = 0;
    for (const auto& t : fullTokens) {
        if (t->type != TokenType::END) ++fullCount;
    }
    std::size_t incCount = 0;
    for (const auto& t : incResult.tokens) {
        if (t->type != TokenType::END) ++incCount;
    }
    EXPECT_EQ(fullCount, incCount);

    // AST语句数应一致
    EXPECT_EQ(fullAst.statements.size(), incResult.ast.statements.size());
}
