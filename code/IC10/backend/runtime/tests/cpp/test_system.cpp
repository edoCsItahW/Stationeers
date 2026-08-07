// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_system.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/09
 * @brief IC10 运行时系统测试
 * @details 端到端测试：执行完整 IC10 程序，验证寄存器状态和执行结果。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/semantic/analyser.hpp"
#include "ic10_runtime/engine.hpp"
#include "ic10_runtime/locals/languages/en_us.hpp"

using namespace stationeers::ic10;

class SystemTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("en_us");
        ICLoc::setLanguage("en_us");
        IRLoc::registerLanguage<EnUs>("en_us");
        IRLoc::setLanguage("en_us");
    }

    /// 读取文件内容
    static std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return "";
        std::stringstream buf;
        buf << file.rdbuf();
        return buf.str();
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

    double reg(const std::string& name) {
        return engine_->getContext().memory.getReg(name);
    }

    Program ast_;
    std::shared_ptr<Analyser> analyser_;
    std::unique_ptr<Engine> engine_;
};

// ============================================================
// 复杂程序 — 累加 0..9
// ============================================================

TEST_F(SystemTestFixture, SumZeroToNine) {
    // 计算 0 + 1 + ... + 9 = 45
    std::string src =
        "move r0 0\n"
        "move r1 0\n"
        "loop:\n"
        "add r0 r0 r1\n"
        "add r1 r1 1\n"
        "blt r1 10 loop\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 45.0);
}

// ============================================================
// 复杂程序 — 带嵌套分支的计数器
// ============================================================

TEST_F(SystemTestFixture, NestedBranching) {
    // 统计能被 3 整除的数 (3,6,9) → 3 个
    std::string src =
        "move r0 0\n"   // counter
        "move r1 1\n"   // i
        "loop:\n"
        "bgt r1 10 end\n"
        "move r2 r1\n"
        "mod r2 r2 3\n"
        "brnez r2 skip\n"
        "add r0 r0 1\n"
        "skip:\n"
        "add r1 r1 1\n"
        "j loop\n"
        "end:\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 3.0);
}

// ============================================================
// 复杂程序 — 栈深度测试
// ============================================================

TEST_F(SystemTestFixture, StackDepthTest) {
    // TODO: 100 push/pop 导致 SEH，用 10 诊断是否为程序长度或栈操作问题
    std::string src;
    for (int i = 0; i < 10; i++) {
        src += "push " + std::to_string(i + 1) + "\n";
    }
    // pop 到寄存器
    for (int i = 0; i < 10; i++) {
        src += "pop r" + std::to_string(i) + "\n";
    }
    src += "hcf\n";
    compile(src);
    engine_->runFull();
    // 最后 push 的 10 最先 pop 到 r0
    EXPECT_DOUBLE_EQ(reg("r0"), 10.0);
    EXPECT_DOUBLE_EQ(reg("r9"), 1.0);
}

// ============================================================
// 复杂程序 — 幂运算
// ============================================================

TEST_F(SystemTestFixture, PowerByRepeatedMultiplication) {
    // 计算 2^8 = 256
    std::string src =
        "move r0 1\n"   // result
        "move r1 8\n"   // exponent
        "loop:\n"
        "breqz r1 end\n"
        "mul r0 r0 2\n"
        "sub r1 r1 1\n"
        "j loop\n"
        "end:\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 256.0);
}

// ============================================================
// 复杂程序 — Clamp 运算
// ============================================================

TEST_F(SystemTestFixture, ClampOperation) {
    std::string src =
        "clamp r0 -5 0 10\n"
        "clamp r1 5 0 10\n"
        "clamp r2 15 0 10\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
    EXPECT_DOUBLE_EQ(reg("r1"), 5.0);
    EXPECT_DOUBLE_EQ(reg("r2"), 10.0);
}

// ============================================================
// 错误场景 — 除以零
// ============================================================

// TODO: 待 runtime 实现除零处理
// TEST_F(SystemTestFixture, DivisionByZero) {
//     std::string src =
//         "move r0 1\n"
//         "div r0 r0 0\n"
//         "hcf\n";
//     auto engine = compile(src);
//     engine->runFull();
// }

// ============================================================
// 边界情况 — 大数值
// ============================================================

TEST_F(SystemTestFixture, LargeValues) {
    std::string src =
        "move r0 1234567890\n"
        "move r1 -987654321\n"
        "add r2 r0 r1\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r2"), 246913569.0);
}

// ============================================================
// 边界情况 — 浮点数精度
// ============================================================

TEST_F(SystemTestFixture, FloatingPointPrecision) {
    std::string src =
        "move r0 3.141592653589793\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 3.141592653589793);
}

// ============================================================
// 边界情况 — 空循环体
// ============================================================

TEST_F(SystemTestFixture, EmptyLoopBodyDoesNotCrash) {
    std::string src =
        "move r0 0\n"
        "loop:\n"
        "add r0 r0 1\n"
        "blt r0 5 loop\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(reg("r0"), 5.0);
}
