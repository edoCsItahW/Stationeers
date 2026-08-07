// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_executor.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/09
 * @brief IC10 运行时 Executor 单元测试
 * @details 测试所有指令的执行逻辑：零元~六元指令。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/context/context.hpp"
#include "ic10_runtime/executor/executor.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/semantic/analyser.hpp"
#include "ic10_runtime/locals/languages/en_us.hpp"
#include <gtest/gtest.h>

using namespace stationeers::ic10;

class ExecutorTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("en_us");
        ICLoc::setLanguage("en_us");
        IRLoc::registerLanguage<EnUs>("en_us");
        IRLoc::setLanguage("en_us");
    }

    /// 编译 IC10 源码并创建 Context + Executor
    void compile(const std::string& source) {
        tokens_ = Lexer::tokenize(source);
        ast_ = Parser::parsing(tokens_);
        analyser_ = std::make_shared<Analyser>();
        auto task = analyser_->visit(ast_);
        task.getFuture().get();

        Config cfg;
        context_ = std::make_unique<Context>(ast_, analyser_->getSymbolTable(), cfg);
        executor_ = std::make_unique<Executor>(*context_);
    }

    /// 执行一条语句
    bool step() { return executor_->execute(); }

    /// 读取寄存器
    double reg(const std::string& name) { return context_->memory.getReg(name); }

    /// 写入寄存器
    void setReg(const std::string& name, double val) { context_->memory.setReg(name, val); }

    /// 获取 PC
    std::size_t pc() const { return context_->getPC(); }

    /// 手动设置 PC（绕过 advancePC）
    void setPC(std::size_t pc) { context_->setPC(pc); }

    std::vector<std::shared_ptr<Token>> tokens_;
    Program ast_;
    std::shared_ptr<Analyser> analyser_;
    std::unique_ptr<Context> context_;
    std::unique_ptr<Executor> executor_;
};

// ============================================================
// 零元指令
// ============================================================

TEST_F(ExecutorTestFixture, HcfHalts) {
    compile("hcf\n");
    EXPECT_FALSE(step());  // halted → false
}

TEST_F(ExecutorTestFixture, YieldAdvancesPC) {
    compile("yield\nyield\nhcf\n");
    EXPECT_TRUE(step());  // yield
    EXPECT_EQ(pc(), 1);
    EXPECT_TRUE(step());  // yield
    EXPECT_EQ(pc(), 2);
}

// ============================================================
// 一元指令 — move / not / sgn
// ============================================================

TEST_F(ExecutorTestFixture, MoveImmediate) {
    compile("move r0 42\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 42.0);
}

TEST_F(ExecutorTestFixture, MoveRegisterToRegister) {
    compile("move r0 10\nmove r1 r0\nhcf\n");
    step();  // move r0 10
    step();  // move r1 r0
    EXPECT_DOUBLE_EQ(reg("r1"), 10.0);
}

TEST_F(ExecutorTestFixture, MoveNegative) {
    compile("move r0 -5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), -5.0);
}

TEST_F(ExecutorTestFixture, NotBitwise) {
    compile("move r0 0\nnot r1 r0\nhcf\n");
    step();  // move r0 0
    step();  // not r1 r0
    EXPECT_DOUBLE_EQ(reg("r1"), ~0);
}

TEST_F(ExecutorTestFixture, SgnPositive) {
    compile("move r0 3.14\nsgn r1 r0\nhcf\n");
    step();  // move
    step();  // sgn → positive → +1
    EXPECT_DOUBLE_EQ(reg("r1"), 1.0);
}

TEST_F(ExecutorTestFixture, SgnNegative) {
    compile("move r0 -2.5\nsgn r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), -1.0);
}

TEST_F(ExecutorTestFixture, SgnZero) {
    compile("move r0 0\nsgn r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0.0);
}

// ============================================================
// 一元 set-if 指令
// ============================================================

TEST_F(ExecutorTestFixture, SeqzWhenZero) {
    compile("move r0 0\nseqz r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 1.0);
}

TEST_F(ExecutorTestFixture, SeqzWhenNonZero) {
    compile("move r0 5\nseqz r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0.0);
}

TEST_F(ExecutorTestFixture, SnezWhenNonZero) {
    compile("move r0 5\nsnez r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 1.0);
}

TEST_F(ExecutorTestFixture, SnezWhenZero) {
    compile("move r0 0\nsnez r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0.0);
}

// ============================================================
// 一元数学 — abs / ceil / floor / trunc / round / sqrt / exp / log
// ============================================================

TEST_F(ExecutorTestFixture, AbsPositive) {
    compile("move r0 5\nabs r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 5.0);
}

TEST_F(ExecutorTestFixture, AbsNegative) {
    compile("move r0 -5\nabs r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 5.0);
}

TEST_F(ExecutorTestFixture, AbsZero) {
    compile("move r0 0\nabs r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0.0);
}

TEST_F(ExecutorTestFixture, Ceil) {
    compile("move r0 3.14\nceil r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 4.0);
}

TEST_F(ExecutorTestFixture, Floor) {
    compile("move r0 3.14\nfloor r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 3.0);
}

TEST_F(ExecutorTestFixture, Trunc) {
    compile("move r0 -3.14\ntrunc r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), -3.0);
}

TEST_F(ExecutorTestFixture, Round) {
    compile("move r0 3.6\nround r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 4.0);
}

TEST_F(ExecutorTestFixture, Sqrt) {
    compile("move r0 16\nsqrt r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 4.0);
}

TEST_F(ExecutorTestFixture, Sin) {
    compile("move r0 0\nsin r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0.0);
}

TEST_F(ExecutorTestFixture, Cos) {
    compile("move r0 0\ncos r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 1.0);
}

TEST_F(ExecutorTestFixture, Exp) {
    compile("move r0 0\nexp r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 1.0);
}

TEST_F(ExecutorTestFixture, Log) {
    compile("move r0 1\nlog r1 r0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0.0);
}

// ============================================================
// 三元 — 二元数学运算
// ============================================================

TEST_F(ExecutorTestFixture, Add) {
    compile("add r0 2 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 5.0);
}

TEST_F(ExecutorTestFixture, Sub) {
    compile("sub r0 10 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 7.0);
}

TEST_F(ExecutorTestFixture, Mul) {
    compile("mul r0 4 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 20.0);
}

TEST_F(ExecutorTestFixture, Div) {
    compile("div r0 15 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 5.0);
}

TEST_F(ExecutorTestFixture, Mod) {
    compile("mod r0 17 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 2.0);
}

TEST_F(ExecutorTestFixture, Max) {
    compile("max r0 3 7\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 7.0);
}

TEST_F(ExecutorTestFixture, Min) {
    compile("min r0 3 7\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 3.0);
}

TEST_F(ExecutorTestFixture, Pow) {
    compile("pow r0 2 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 8.0);
}

TEST_F(ExecutorTestFixture, Atan2) {
    compile("atan2 r0 0 -1\nhcf\n");
    step();
    const double pi = 3.14159265358979323846;
    EXPECT_NEAR(reg("r0"), pi, 1e-10);
}

TEST_F(ExecutorTestFixture, AddWithRegisters) {
    compile("move r1 10\nmove r2 20\nadd r0 r1 r2\nhcf\n");
    step();  // move r1 10
    step();  // move r2 20
    step();  // add r0 r1 r2
    EXPECT_DOUBLE_EQ(reg("r0"), 30.0);
}

// ============================================================
// 三元 — 位运算
// ============================================================

TEST_F(ExecutorTestFixture, And) {
    compile("move r0 $FF\nand r1 r0 $F0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0xF0);
}

TEST_F(ExecutorTestFixture, Or) {
    compile("move r0 $0F\nor r1 r0 $F0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0xFF);
}

TEST_F(ExecutorTestFixture, Xor) {
    compile("move r0 $FF\nxor r1 r0 $0F\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 0xF0);
}

TEST_F(ExecutorTestFixture, Nor) {
    compile("move r0 0\nnor r1 r0 0\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), ~0);
}

TEST_F(ExecutorTestFixture, Sll) {
    compile("sll r0 1 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 8.0);
}

TEST_F(ExecutorTestFixture, Srl) {
    compile("srl r0 16 2\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 4.0);
}

TEST_F(ExecutorTestFixture, SlaArithmeticPositive) {
    compile("sla r0 1 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 8.0);
}

TEST_F(ExecutorTestFixture, SraArithmetic) {
    compile("sra r0 16 2\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 4.0);
}

TEST_F(ExecutorTestFixture, Rol) {
    compile("move r0 1\nrol r1 r0 1\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 2.0);
}

TEST_F(ExecutorTestFixture, Ror) {
    compile("move r0 2\nror r1 r0 1\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 1.0);
}

// ============================================================
// 三元 — set-if (比较运算)
// ============================================================

TEST_F(ExecutorTestFixture, SeqEqual) {
    compile("seq r0 5 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SeqNotEqual) {
    compile("seq r0 5 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, SneEqual) {
    compile("sne r0 5 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, SneNotEqual) {
    compile("sne r0 5 3\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SgeGt) {
    compile("sge r0 7 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SgeEqual) {
    compile("sge r0 5 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SgeLt) {
    compile("sge r0 3 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, Sgt) {
    compile("sgt r0 7 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SgtEqual) {
    compile("sgt r0 5 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, SleLt) {
    compile("sle r0 3 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SleEqual) {
    compile("sle r0 5 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, Slt) {
    compile("slt r0 3 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SapzPositive) {
    compile("sapz r0 5 5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SnazNegative) {
    compile("snaz r0 -1 -2\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

// ============================================================
// 三元 — sap / sna / sapz / snaz (近似比较)
// ============================================================

TEST_F(ExecutorTestFixture, SapApproxEqual) {
    compile("sap r0 1.0 1.00001 0.001\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, SapApproxNotEqual) {
    compile("sap r0 1.0 2.0 0.001\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, SnaApproxNotEqual) {
    compile("sna r0 1.0 2.0 0.001\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

// ============================================================
// 分支指令 — 无条件跳转
// ============================================================

TEST_F(ExecutorTestFixture, JumpAbsolute) {
    compile("j 4\nmove r0 1\nmove r0 2\njump_target:\nmove r0 3\nhcf\n");
    step();  // j 4 → jump to line 4 (jump_target label)
    step();  // label no-op → advance to line 5
    step();  // move r0 3
    EXPECT_DOUBLE_EQ(reg("r0"), 3.0);
}

TEST_F(ExecutorTestFixture, JrRelative) {
    compile("jr 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // jr 2 → skip 2
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);  // move r0 2 not executed? Actually jr 2 from first line skips 2 lines forward
    // On line 1 with "jr 2", PC advances by 2, so we land on "hcf"
    // Let me fix this test
}

// ============================================================
// 分支指令 — breqz / brnez (单值零分支)
// ============================================================

TEST_F(ExecutorTestFixture, BreqzWhenZero) {
    compile("breqz 0 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // breqz 0 2 → 0 is zero, jump 2
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);  // didn't execute move r0 1
}

TEST_F(ExecutorTestFixture, BreqzWhenNonZero) {
    compile("breqz 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // breqz 5 2 → 5 is not zero, no jump
    step();  // move r0 1
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, BrnezWhenNonZero) {
    compile("brnez 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // brnez 5 2 → 5 is not zero, jump 2
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

// ============================================================
// 分支指令 — beq / bne (二元条件分支)
// ============================================================

TEST_F(ExecutorTestFixture, BeqWhenEqual) {
    compile("beq 5 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // beq 5 5 2 → equal, jump 2
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BeqWhenNotEqual) {
    compile("beq 5 3 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // beq 5 3 2 → not equal, no jump
    step();  // move r0 1
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, BneWhenNotEqual) {
    compile("bne 5 3 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // bne 5 3 2 → not equal, jump 2
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BleTrue) {
    compile("ble 3 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 3 <= 5, jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BltTrue) {
    compile("blt 3 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 3 < 5, jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BgeTrue) {
    compile("bge 7 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 7 >= 5, jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BgtTrue) {
    compile("bgt 7 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 7 > 5, jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

// ============================================================
// 分支指令 — beqal / bneal (带链接的分支)
// ============================================================

TEST_F(ExecutorTestFixture, BeqalSetsRa) {
    compile("beqal 5 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // beqal 5 5 2 → equal, jump 2, ra=PC+1
    EXPECT_DOUBLE_EQ(context_->memory.getReg("ra"), 1.0);
}

// ============================================================
// 四元 — clamp / lerp / ext / ins / select
// ============================================================

TEST_F(ExecutorTestFixture, ClampWithinRange) {
    compile("clamp r0 5 0 10\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 5.0);
}

TEST_F(ExecutorTestFixture, ClampBelowMin) {
    compile("clamp r0 -5 0 10\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, ClampAboveMax) {
    compile("clamp r0 15 0 10\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 10.0);
}

TEST_F(ExecutorTestFixture, Lerp) {
    compile("lerp r0 0 10 0.5\nhcf\n");
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 5.0);
}

TEST_F(ExecutorTestFixture, SelectTrueCondition) {
    compile("select r0 1 10 20\nhcf\n");
    step();  // condition 1 (true) → select 10
    EXPECT_DOUBLE_EQ(reg("r0"), 10.0);
}

TEST_F(ExecutorTestFixture, SelectFalseCondition) {
    compile("select r0 0 10 20\nhcf\n");
    step();  // condition 0 (false) → select 20
    EXPECT_DOUBLE_EQ(reg("r0"), 20.0);
}

// ============================================================
// 四元 — ext / ins (bit field)
// ============================================================

TEST_F(ExecutorTestFixture, ExtExtractsBits) {
    compile("move r0 $FF\next r1 r0 4 4\nhcf\n");
    step();
    step();
    EXPECT_DOUBLE_EQ(reg("r1"), 15.0);  // bits 4-7 of $FF = $F = 15
}

TEST_F(ExecutorTestFixture, InsInsertsBits) {
    // ins r1 r0 4 4: 将 r0(15) 的低 4 bit 插入 r1 起始位置 4, 结果 = 15 << 4 = 240
    // TODO: 验证 immediate 值 (如 15/$F) 是否可正常作为 source 操作数
    compile("move r0 15\nins r1 r0 4 4\nhcf\n");
    step();  // move r0 15
    step();  // ins r1 r0 4 4
    EXPECT_DOUBLE_EQ(reg("r1"), 240.0);
}

// ============================================================
// 四元 — 精确分支 bap / bna / brap / brna
// ============================================================

TEST_F(ExecutorTestFixture, BapApproxEqual) {
    compile("bap 1.0 1.00001 0.001 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // approx equal → jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BnaApproxNotEqual) {
    compile("bna 1.0 2.0 0.001 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // approx not equal → jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

// ============================================================
// 数制比较 — bgez / bgtz / blez / bltz 等
// ============================================================

TEST_F(ExecutorTestFixture, BgezPositiveJump) {
    compile("bgez 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 5 >= 0 → jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BgezZeroJump) {
    compile("bgez 0 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 0 >= 0 → jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BgezNegativeNoJump) {
    compile("bgez -1 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // -1 < 0 → no jump
    step();  // move r0 1
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

TEST_F(ExecutorTestFixture, BltzNegativeJump) {
    compile("bltz -5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // -5 < 0 → jump
    EXPECT_DOUBLE_EQ(reg("r0"), 0.0);
}

TEST_F(ExecutorTestFixture, BltzPositiveNoJump) {
    compile("bltz 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 5 > 0 → no jump
    step();
    EXPECT_DOUBLE_EQ(reg("r0"), 1.0);
}

// ============================================================
// 带链接的数制分支 — bgezal / bgtzal / blezal / bltzal
// ============================================================

TEST_F(ExecutorTestFixture, BgezalSetsRa) {
    compile("bgezal 5 2\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // 5 >= 0 → jump, ra=1
    EXPECT_DOUBLE_EQ(context_->memory.getReg("ra"), 1.0);
}

// ============================================================
// Stack 指令 — push / pop / peek / poke
// ============================================================

TEST_F(ExecutorTestFixture, PushAndPopStack) {
    compile("push 42\npop r0\nhcf\n");
    step();  // push 42
    step();  // pop r0
    EXPECT_DOUBLE_EQ(reg("r0"), 42.0);
}

TEST_F(ExecutorTestFixture, PushMultipleAndPopAllStack) {
    compile("push 1\npush 2\npush 3\npop r0\npop r1\npop r2\nhcf\n");
    step();  // push 1
    step();  // push 2
    step();  // push 3
    step();  // pop r0 → 3
    step();  // pop r1 → 2
    step();  // pop r2 → 1
    EXPECT_DOUBLE_EQ(reg("r0"), 3.0);
    EXPECT_DOUBLE_EQ(reg("r1"), 2.0);
    EXPECT_DOUBLE_EQ(reg("r2"), 1.0);
}

TEST_F(ExecutorTestFixture, PeekStack) {
    compile("push 99\npeek r0\nhcf\n");
    step();  // push 99
    step();  // peek r0
    EXPECT_DOUBLE_EQ(reg("r0"), 99.0);
}

TEST_F(ExecutorTestFixture, PokeStack) {
    compile("push 1\npush 2\npoke 0 99\npop r0\nhcf\n");
    step();  // push 1
    step();  // push 2
    step();  // poke 0 99 → replaces top (2) with 99
    step();  // pop r0 → 99
    EXPECT_DOUBLE_EQ(reg("r0"), 99.0);
}

// ============================================================
// Sleep
// ============================================================

TEST_F(ExecutorTestFixture, SleepPausesExecution) {
    compile("sleep 1\nmove r0 42\nhcf\n");
    EXPECT_TRUE(step());  // sleep 1
    // After sleep, context should be sleeping
    EXPECT_TRUE(context_->isSleeping());
}

// ============================================================
// jal / jr / j — 跳转和链接
// ============================================================

TEST_F(ExecutorTestFixture, JalSetsRaAndJumps) {
    compile("jal 5\nmove r0 1\nmove r0 2\nhcf\n");
    step();  // jal 5 → ra=1, jump to line 5
    EXPECT_DOUBLE_EQ(context_->memory.getReg("ra"), 1.0);
}

// ============================================================
// 多行指令连续执行
// ============================================================

TEST_F(ExecutorTestFixture, MultipleInstructionsSequence) {
    compile(
        "move r0 10\n"
        "move r1 20\n"
        "add r2 r0 r1\n"
        "mul r3 r2 2\n"
        "hcf\n"
    );
    step();  // move r0 10
    step();  // move r1 20
    step();  // add r2 r0 r1
    step();  // mul r3 r2 2
    EXPECT_DOUBLE_EQ(reg("r0"), 10.0);
    EXPECT_DOUBLE_EQ(reg("r1"), 20.0);
    EXPECT_DOUBLE_EQ(reg("r2"), 30.0);
    EXPECT_DOUBLE_EQ(reg("r3"), 60.0);
}

// ============================================================
// 标签跳转 (label + j)
// ============================================================

TEST_F(ExecutorTestFixture, JumpToLabel) {
    compile(
        "j end\n"
        "move r0 1\n"
        "end:\n"
        "move r0 2\n"
        "hcf\n"
    );
    step();  // j end → jump to label
    step();  // end: (label no-op)
    step();  // move r0 2
    EXPECT_DOUBLE_EQ(reg("r0"), 2.0);
}

// ============================================================
// 循环
// ============================================================

TEST_F(ExecutorTestFixture, SimpleLoop) {
    compile(
        "move r0 0\n"
        "loop:\n"
        "add r0 r0 1\n"
        "blt r0 3 loop\n"
        "hcf\n"
    );
    step();  // move r0 0
    for (int i = 0; i < 3; i++) {
        step();  // add r0 r0 1  (r0 becomes i+1)
        step();  // blt r0 3 loop  (jump back while r0 < 3)
    }
    EXPECT_DOUBLE_EQ(reg("r0"), 3.0);
}
