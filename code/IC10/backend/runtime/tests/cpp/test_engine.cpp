// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_engine.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/09
 * @brief IC10 运行时 Engine 集成测试
 * @details 测试 Engine::runTick/runFull 执行循环。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/semantic/analyser.hpp"
#include "ic10_runtime/engine.hpp"
#include "ic10_runtime/locals/languages/en_us.hpp"

using namespace stationeers::ic10;

class EngineTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("en_us");
        ICLoc::setLanguage("en_us");
        IRLoc::registerLanguage<EnUs>("en_us");
        IRLoc::setLanguage("en_us");
    }

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

    double reg(const std::string& name) { return engine_->getContext().memory.getReg(name); }
};

// ============================================================
// 基本程序执行
// ============================================================

TEST_F(EngineTestFixture, SimpleProgramRunsToHalt) {
    compile("move r0 42\nhcf\n");
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 42.0);
}

TEST_F(EngineTestFixture, ArithmeticProgram) {
    compile(
        "move r0 10\n"
        "move r1 20\n"
        "add r2 r0 r1\n"
        "sub r3 r2 5\n"
        "mul r4 r3 2\n"
        "div r5 r4 5\n"
        "hcf\n"
    );
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r2"), 30.0);
    EXPECT_DOUBLE_EQ(reg("r3"), 25.0);
    EXPECT_DOUBLE_EQ(reg("r4"), 50.0);
    EXPECT_DOUBLE_EQ(reg("r5"), 10.0);
}

TEST_F(EngineTestFixture, JumpAndLabel) {
    compile(
        "j skip\n"
        "move r0 1\n"
        "skip:\n"
        "move r0 2\n"
        "hcf\n"
    );
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 2.0);
}

// ============================================================
// 循环
// ============================================================

TEST_F(EngineTestFixture, ForLoop) {
    compile(
        "move r0 0\n"
        "loop:\n"
        "add r0 r0 1\n"
        "blt r0 5 loop\n"
        "hcf\n"
    );
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 5.0);
}

TEST_F(EngineTestFixture, WhileLoop) {
    compile(
        "move r0 10\n"
        "loop:\n"
        "breqz r0 end\n"
        "sub r0 r0 1\n"
        "j loop\n"
        "end:\n"
        "hcf\n"
    );
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

// ============================================================
// Stack
// ============================================================

TEST_F(EngineTestFixture, StackPushPop) {
    compile(
        "push 1\n"
        "push 2\n"
        "pop r0\n"
        "pop r1\n"
        "hcf\n"
    );
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 2.0);
    EXPECT_DOUBLE_EQ(reg("r1"), 1.0);
}

// ============================================================
// 空程序
// ============================================================

TEST_F(EngineTestFixture, EmptyProgramHaltsImmediately) {
    compile("");
    engine_->runFull();
    // 空程序应该完成执行
}

// ============================================================
// runTick 单步执行
// ============================================================

TEST_F(EngineTestFixture, RunTickAdvancesOneStatement) {
    // runTick 一次执行 maxInstructions 条指令（直到 hcf）
    compile(
        "move r0 1\n"
        "move r0 2\n"
        "hcf\n"
    );
    engine_->runTick();
    // 一次 runTick 执行完所有指令：r0 最终值为 2
    EXPECT_DOUBLE_EQ(reg("r0"), 2.0);
}
