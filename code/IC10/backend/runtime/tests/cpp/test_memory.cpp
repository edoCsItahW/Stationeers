// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_memory.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/09
 * @brief IC10 运行时 Memory 单元测试
 * @details 测试寄存器读写、栈操作（push/pop/peek/poke）、sp/ra 字段。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/memory/memory.hpp"
#include "ic10_runtime/config.hpp"
#include <gtest/gtest.h>

using namespace stationeers::ic10;

// ============================================================
// 寄存器操作
// ============================================================

TEST(MemoryTest, RegisterSetAndGet) {
    Config cfg;
    Memory mem(cfg);
    mem.setReg("r0", 42.0);
    EXPECT_DOUBLE_EQ(mem.getReg("r0"), 42.0);
}

TEST(MemoryTest, RegisterDefaultZero) {
    Config cfg;
    Memory mem(cfg);
    EXPECT_DOUBLE_EQ(mem.getReg("r0"), 0.0);
    EXPECT_DOUBLE_EQ(mem.getReg("r15"), 0.0);
}

TEST(MemoryTest, RegisterOverwrite) {
    Config cfg;
    Memory mem(cfg);
    mem.setReg("r0", 10.0);
    mem.setReg("r0", 20.0);
    EXPECT_DOUBLE_EQ(mem.getReg("r0"), 20.0);
}

TEST(MemoryTest, DeviceRegistersDefaultZero) {
    Config cfg;
    Memory mem(cfg);
    EXPECT_DOUBLE_EQ(mem.getReg("d0"), 0.0);
    EXPECT_DOUBLE_EQ(mem.getReg("d5"), 0.0);
}

TEST(MemoryTest, DeviceRegisterSetAndGet) {
    Config cfg;
    Memory mem(cfg);
    mem.setReg("d0", -1.5);
    EXPECT_DOUBLE_EQ(mem.getReg("d0"), -1.5);
}

TEST(MemoryTest, NegativeRegisterValue) {
    Config cfg;
    Memory mem(cfg);
    mem.setReg("r1", -100.0);
    EXPECT_DOUBLE_EQ(mem.getReg("r1"), -100.0);
}

TEST(MemoryTest, ZeroRegisterValue) {
    Config cfg;
    Memory mem(cfg);
    mem.setReg("r0", 0.0);
    EXPECT_DOUBLE_EQ(mem.getReg("r0"), 0.0);
}

// ============================================================
// 栈操作
// ============================================================

TEST(MemoryTest, PushAndPop) {
    Config cfg;
    Memory mem(cfg);
    mem.push(3.14);
    EXPECT_DOUBLE_EQ(mem.pop(), 3.14);
}

TEST(MemoryTest, PushMultipleAndPopAll) {
    Config cfg;
    Memory mem(cfg);
    mem.push(1.0);
    mem.push(2.0);
    mem.push(3.0);
    EXPECT_DOUBLE_EQ(mem.pop(), 3.0);
    EXPECT_DOUBLE_EQ(mem.pop(), 2.0);
    EXPECT_DOUBLE_EQ(mem.pop(), 1.0);
}

TEST(MemoryTest, PeekDoesNotModifyStack) {
    Config cfg;
    Memory mem(cfg);
    mem.push(42.0);
    EXPECT_DOUBLE_EQ(mem.peek(), 42.0);
    EXPECT_DOUBLE_EQ(mem.peek(), 42.0);  // 重复 peek 不变
    EXPECT_DOUBLE_EQ(mem.pop(), 42.0);   // pop 后才会移除
}

TEST(MemoryTest, PokeReplacesElement) {
    Config cfg;
    Memory mem(cfg);
    mem.push(1.0);
    mem.push(2.0);
    mem.push(3.0);
    mem.poke(1, 99.0);  // index 1 → 倒数第二个
    EXPECT_DOUBLE_EQ(mem.pop(), 3.0);
    EXPECT_DOUBLE_EQ(mem.pop(), 99.0);
    EXPECT_DOUBLE_EQ(mem.pop(), 1.0);
}

TEST(MemoryTest, PokeTopElement) {
    Config cfg;
    Memory mem(cfg);
    mem.push(1.0);
    mem.poke(0, 99.0);
    EXPECT_DOUBLE_EQ(mem.peek(), 99.0);
    EXPECT_DOUBLE_EQ(mem.pop(), 99.0);
}

TEST(MemoryTest, PushNegativeValues) {
    Config cfg;
    Memory mem(cfg);
    mem.push(-1.0);
    mem.push(-3.14);
    EXPECT_DOUBLE_EQ(mem.pop(), -3.14);
    EXPECT_DOUBLE_EQ(mem.pop(), -1.0);
}

// ============================================================
// sp / ra
// ============================================================

TEST(MemoryTest, SpInitialValue) {
    Config cfg;
    Memory mem(cfg);
    EXPECT_DOUBLE_EQ(mem.getSP(), 0.0);
}

TEST(MemoryTest, RaInitialValue) {
    Config cfg;
    Memory mem(cfg);
    EXPECT_DOUBLE_EQ(mem.getReg("ra"), 0.0);
}

TEST(MemoryTest, SpAndRaCanBeModified) {
    Config cfg;
    Memory mem(cfg);
    mem.setSP(16.0);
    mem.setReg("ra", 32.0);
    EXPECT_DOUBLE_EQ(mem.getSP(), 16.0);
    EXPECT_DOUBLE_EQ(mem.getReg("ra"), 32.0);
}
