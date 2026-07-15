// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:49
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_ANALYSER_INL
#define COMPILER_ANALYSER_INL
#pragma once

#include "ic10/locals/local.hpp"
#include <iostream>

namespace stationeers::ic10 {

    // 通用指令访问器：遍历指令的所有操作数（args 元组），按操作数类型分派处理
    template<template<auto, auto...> class Ins, FString V, OperandType... Vs>
    Task<> Analyser::operator()(const Ins<V, Vs...>& ins) {
        // 单个操作数处理：对 variant 分派
        // - Identifier: 解析符号（可能为前向引用，需等待 Future）
        // - ErrorNode : Parser 已上报，跳过避免重复诊断
        // - 其他类型  : 递归访问（走叶节点访问器或其他专用访问器）
        auto process = [&]<OperandType Type>(const auto& variant) -> Task<> {
            (void)co_await std::visit(
                [&]<typename T>(const T& arg) -> Task<> {
                    using U = std::remove_cvref_t<T>;

                    // Identifier: 需要解析符号
                    if constexpr (std::is_same_v<U, Identifier>) {
                        // 标签检查
                        if constexpr (Type == OperandType::JUMP_TARGET) {
                            if (auto expected = co_await resolveSymbol(arg.value, arg.position);
                                expected.has_value()
                                && expected.value()->category != TypeCategory::LABEL)
                                reporter_.warnWith<IC10MsgId::IWA1_1>(arg.start(), arg.end(), expected.value()->name);
                        }

                        else
                            (void)co_await resolveSymbol(arg.value, arg.position);
                    }

                    // ErrorNode: Parser 已报错，跳过
                    else if constexpr (std::is_same_v<U, ErrorNode>)
                        co_return;

                    // 其他类型: 递归访问
                    else
                        (void)co_await this->operator()(arg);

                    co_return;
                },
                variant
            );
        };

        // 折叠表达式依次处理指令的所有操作数
        std::apply(
            [&](const auto&... args) -> Task<> {
                (((void)co_await (process.template operator()<Vs>(args))), ...);
                co_return;
            },
            ins.args
        );

        co_return;
    }

    // 泛型 fallback 访问器：未匹配到专用访问器的节点走此分支，统一上报 IEA5（未知语法类型）
    template<typename T>
    Task<> Analyser::operator()(T&& arg) {
        using U = std::remove_cvref_t<T>;

        // 如果是 tuple-like 类型，尝试提取内部元素
        if constexpr (requires { std::get<0>(arg); })
            std::apply(
                [this](const auto& innerArg) {
                    reporter_.errorWith<MsgId::IEA6>(
                        innerArg.start(), innerArg.end(), std::string(U::nodeName)
                    );
                },
                arg
            );
        else
            reporter_.errorWith<MsgId::IEA6>(arg.start(), arg.end(), std::string(U::nodeName));

        co_return;
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_ANALYSER_INL
