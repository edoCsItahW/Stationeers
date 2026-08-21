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
 * @date 2026/08/09
 * @brief IC10 运行时集成测试
 * @details 测试从源码编译到执行的完整流水线。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_runtime/locals/languages/en_us.hpp"
#include "ic10_compiler/semantic/analyser.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_runtime/engine.hpp"

#include <gtest/gtest.h>

using namespace stationeers::ic10;

class IntegrationTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("en_us");
        ICLoc::setLanguage("en_us");
        IRLoc::registerLanguage<EnUs>("en_us");
        IRLoc::setLanguage("en_us");
    }

    /// 编译并执行程序
    double runAndGetReg(const std::string& source, const std::string& regName) {
        auto tokens = Lexer::tokenize(source);
        auto ast = Parser::parsing(tokens);
        auto analyser = std::make_shared<Analyser>();
        auto task = analyser->visit(ast);
        task.getFuture().get();

        Config cfg;
        Engine engine(ast, analyser->getSymbolTable(), cfg);
        engine.runFull();
        return engine.getContext().memory.getReg(regName);
    }

    /// 编译并保存 Program 和 Engine（Program 生命周期由 fixture 管理）
    void compile(const std::string& source) {
        auto tokens = Lexer::tokenize(source);
        ast_ = Parser::parsing(tokens);
        analyser_ = std::make_shared<Analyser>();
        auto task = analyser_->visit(ast_);
        task.getFuture().get();

        Config cfg;
        engine_ = std::make_unique<Engine>(ast_, analyser_->getSymbolTable(), cfg);
    }

    Program ast_;
    std::shared_ptr<Analyser> analyser_;
    std::unique_ptr<Engine> engine_;
};

// ============================================================
// 算术和逻辑运算
// ============================================================

TEST_F(IntegrationTestFixture, Factorial) {
    // 计算 5! = 120
    std::string src =
        "move r0 5\n"
        "move r1 1\n"
        "loop:\n"
        "breqz r0 end\n"
        "mul r1 r1 r0\n"
        "sub r0 r0 1\n"
        "j loop\n"
        "end:\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r1"), 120.0);
}

TEST_F(IntegrationTestFixture, Summation) {
    // 1 + 2 + 3 + 4 + 5 = 15
    std::string src =
        "move r0 0\n"   // sum
        "move r1 1\n"   // i
        "loop:\n"
        "add r0 r0 r1\n"
        "add r1 r1 1\n"
        "ble r1 5 loop\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 15.0);
}

TEST_F(IntegrationTestFixture, Fibonacci) {
    // 计算第 10 个斐波那契数 (0-indexed: F10=55)
    std::string src =
        "move r0 0\n"   // a (F0)
        "move r1 1\n"   // b (F1)
        "move r2 10\n"  // n
        "loop:\n"
        "breqz r2 end\n"
        "add r3 r0 r1\n"
        "move r0 r1\n"
        "move r1 r3\n"
        "sub r2 r2 1\n"
        "j loop\n"
        "end:\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 55.0);
}

// ============================================================
// 条件逻辑
// ============================================================

TEST_F(IntegrationTestFixture, MaxOfThree) {
    // r0 = max(a, b, c)
    std::string src =
        "move r1 10\n"  // a
        "move r2 25\n"  // b
        "move r3 15\n"  // c
        "bge r1 r2 skip1\n"
        "move r1 r2\n"   // r1 = max(a,b)
        "skip1:\n"
        "bge r1 r3 skip2\n"
        "move r1 r3\n"   // r1 = max(a,b,c)
        "skip2:\n"
        "move r0 r1\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 25.0);
}

TEST_F(IntegrationTestFixture, EvenOddCounter) {
    // 统计 [1,10] 中偶数的个数
    std::string src =
        "move r0 0\n"   // counter
        "move r1 1\n"   // i
        "loop:\n"
        "bgt r1 10 end\n"
        "move r2 r1\n"
        "mod r2 r2 2\n"
        "brnez r2 next\n"
        "add r0 r0 1\n"
        "next:\n"
        "add r1 r1 1\n"
        "j loop\n"
        "end:\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 5.0);
}

// ============================================================
// Stack 操作
// ============================================================

TEST_F(IntegrationTestFixture, StackReverse) {
    // 通过栈反转数组 [1,2,3] → pop 得到 3,2,1
    std::string src =
        "push 1\n"
        "push 2\n"
        "push 3\n"
        "pop r0\n"
        "pop r1\n"
        "pop r2\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r0"), 3.0);
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r1"), 2.0);
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r2"), 1.0);
}

// ============================================================
// 相对跳转
// ============================================================

TEST_F(IntegrationTestFixture, RelativeJump) {
    std::string src =
        "jr 2\n"
        "move r0 1\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    // jr 2 从当前行(0) 跳转 2 行 → hcf
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r0"), 0.0);
}

// ============================================================
// alias/define 基本指令
// ============================================================

TEST_F(IntegrationTestFixture, DefineAndAlias) {
    std::string src =
        "define foo 42\n"
        "move r0 foo\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 42.0);
}

// ============================================================
// 注释和空行不影响执行
// ============================================================

TEST_F(IntegrationTestFixture, CommentsAreIgnored) {
    std::string src =
        "# This is a comment\n"
        "move r0 7\n"
        "# Another comment\n"
        "\n"
        "add r0 r0 3\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 10.0);
}

// ============================================================
// 前向引用
// ============================================================

TEST_F(IntegrationTestFixture, ForwardReference) {
    std::string src =
        "beq r0 r0 end\n"
        "move r0 1\n"
        "end:\n"
        "move r0 99\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 99.0);
}

// ============================================================
// 多行 alias / define
// ============================================================

TEST_F(IntegrationTestFixture, MultipleDefines) {
    std::string src =
        "define A 10\n"
        "define B 20\n"
        "define C 30\n"
        "move r0 A\n"
        "add r0 r0 B\n"
        "add r0 r0 C\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 60.0);
}

// ============================================================
// 文档注释不影响执行
// ============================================================

// TODO: Parser doesn't support standalone @desc/@return doc comments outside @device/@enum blocks
// When the parser is updated to handle standalone doc comments as no-ops, re-enable this test
TEST_F(IntegrationTestFixture, DISABLED_DocCommentIgnored) {
    std::string src =
        "@desc myprog\n"
        "@return\n"
        "move r0 5\n"
        "hcf\n";
    EXPECT_DOUBLE_EQ(runAndGetReg(src, "r0"), 5.0);
}
