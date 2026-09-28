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
        Engine engine(ast, analyser->getSymbolTable(), cfg, analyser->getTypeTable());
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
        engine_ = std::make_unique<Engine>(
            ast_, analyser_->getSymbolTable(), cfg, analyser_->getTypeTable()
        );
    }

    /// 拼接最小标准库片段：逻辑属性名需命中 LogicType 枚举才算合法
    static std::string withLogicTypes(const std::string& source) {
        static constexpr std::string_view kLogicTypes =
            "#> @enum\n"
            "#> @name LogicType\n"
            "#> @value Setting 12\n"
            "#> @value On 28\n"
            "#> @value Color 3\n"
            "#> @value Pressure 5\n"
            "#> @end-enum\n"
            "#> @enum\n"
            "#> @name BatchMode\n"
            "#> @value Average 0\n"
            "#> @value Sum 1\n"
            "#> @end-enum\n";

        return std::string(kLogicTypes) + source;
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
// v3 放宽语法：别名写入目标、设备引用、写入值形态
// ============================================================

TEST_F(IntegrationTestFixture, AliasAsWriteTarget) {
    // 目标操作数可为指向寄存器的别名（v3 语法放宽）
    std::string src =
        "alias tmp r3\n"
        "move tmp 7\n"
        "move r4 tmp\n"
        "hcf\n";
    compile(src);
    engine_->runFull();
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r3"), 7.0);
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r4"), 7.0);
}

TEST_F(IntegrationTestFixture, DeviceAliasWithValueForms) {
    // 设备别名 + 写入值的三种合法形态：常量别名、字面量、寄存器
    std::string src =
        "alias led d0\n"
        "define Level 3\n"
        "move r1 5\n"
        "s led Setting Level\n"
        "s led On r1\n"
        "s led Color 7\n"
        "l r0 led Setting\n"
        "hcf\n";

    compile(withLogicTypes(src));

    auto device     = std::make_unique<VirtualDevice>();
    auto* devicePtr = device.get();
    engine_->getContext().manager.setExternalDevice("d0", std::move(device));

    engine_->runFull();

    EXPECT_DOUBLE_EQ(devicePtr->readLogic("Setting"), 3.0);
    EXPECT_DOUBLE_EQ(devicePtr->readLogic("On"), 5.0);
    EXPECT_DOUBLE_EQ(devicePtr->readLogic("Color"), 7.0);
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r0"), 3.0);
}

TEST_F(IntegrationTestFixture, DynamicDevicePort) {
    // dr0：端口号存于 r0，运行时按 d0..d5 查找
    std::string src =
        "move r0 1\n"
        "s dr0 Setting 9\n"
        "hcf\n";

    compile(withLogicTypes(src));

    auto device     = std::make_unique<VirtualDevice>();
    auto* devicePtr = device.get();
    engine_->getContext().manager.setExternalDevice("d1", std::move(device));

    engine_->runFull();

    EXPECT_DOUBLE_EQ(devicePtr->readLogic("Setting"), 9.0);
}

TEST_F(IntegrationTestFixture, DynamicDevicePortOutOfRangeReportsError) {
    // 边界：端口号越界时设备引用无法解析，应上报运行期诊断而不是静默写入
    std::string src =
        "move r0 9\n"
        "s dr0 Setting 1\n"
        "hcf\n";

    compile(withLogicTypes(src));
    engine_->runFull();

    EXPECT_FALSE(engine_->getDiagnostics().empty());
}

TEST_F(IntegrationTestFixture, EnumConstantAsWriteValue) {
    // 枚举常量作写入值：取值来自类型表的枚举注解
    std::string src =
        "#> @enum\n"
        "#> @name Color\n"
        "#> @value Green 2\n"
        "#> @end-enum\n"
        "alias led d0\n"
        "s led Color Color.Green\n"
        "hcf\n";

    compile(withLogicTypes(src));

    auto device     = std::make_unique<VirtualDevice>();
    auto* devicePtr = device.get();
    engine_->getContext().manager.setExternalDevice("d0", std::move(device));

    engine_->runFull();

    EXPECT_DOUBLE_EQ(devicePtr->readLogic("Color"), 2.0);
}

TEST_F(IntegrationTestFixture, DeviceHashFromRegister) {
    // sb 的设备哈希可存放在寄存器中（v3 语法放宽）：此处设备类型哈希为 0
    std::string src =
        "alias Hash r2\n"
        "move Hash 0\n"
        "sb Hash Setting 4\n"
        "hcf\n";

    compile(withLogicTypes(src));

    auto device     = std::make_unique<VirtualDevice>();
    auto* devicePtr = device.get();
    engine_->getContext().manager.setExternalDevice("d0", std::move(device));

    engine_->runFull();

    EXPECT_DOUBLE_EQ(devicePtr->readLogic("Setting"), 4.0);
}

TEST_F(IntegrationTestFixture, AggregateModeBareEnumMember) {
    // 裸枚举成员名（Average）不是符号表条目，运行时据类型表在 BatchMode 中求值
    std::string src =
        "define DevType 0\n"
        "lb r0 DevType Pressure Average\n"
        "hcf\n";

    compile(withLogicTypes(src));

    auto first  = std::make_unique<VirtualDevice>();
    auto second = std::make_unique<VirtualDevice>();
    first->writeLogic("Pressure", 6.0);
    second->writeLogic("Pressure", 12.0);

    auto& manager = engine_->getContext().manager;
    manager.setExternalDevice("d0", std::move(first));
    manager.setExternalDevice("d1", std::move(second));
    // 芯片自身也是类型哈希为 0 的设备，lb 的设备集合包含它
    manager.getDevice("db")->writeLogic("Pressure", 3.0);

    engine_->runFull();

    // Average = (6 + 12 + 3) / 3：既证明成员名解析为模式 0，也证明三台设备都参与了聚合
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r0"), 7.0);
}

TEST_F(IntegrationTestFixture, NumericLogicPropLegacySyntax) {
    // 旧语法：逻辑属性写成数字，按十进制文本作为属性名传递
    std::string src =
        "alias led d0\n"
        "s led 3 8\n"
        "hcf\n";

    compile(withLogicTypes(src));

    auto device     = std::make_unique<VirtualDevice>();
    auto* devicePtr = device.get();
    engine_->getContext().manager.setExternalDevice("d0", std::move(device));

    engine_->runFull();

    EXPECT_DOUBLE_EQ(devicePtr->readLogic("3"), 8.0);
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
