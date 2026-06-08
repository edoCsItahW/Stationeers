// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

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

#include <iostream>

namespace stationeers::ic10 {

    template<typename T, typename... Args>
    void Analyser::reportError(Args&&... args) {
        errors_.emplace_back(T{std::forward<Args>(args)...});
    }

    template<template<auto, typename...> class Ins, FString V, typename... Args>
    Task<> Analyser::operator()(const Ins<V, Args...>& ins) {
        auto process = [&](const auto& variant) -> Task<> {
            (void)co_await std::visit(
                [&]<typename T>(const T& arg) -> Task<> {
                    if constexpr (std::is_same_v<std::remove_cvref_t<T>, Identifier>)
                        (void)co_await resolveSymbol(arg.value, arg.position);

                    else
                        (void)co_await this->operator()(arg);

                    co_return;
                },
                variant
            );
        };

        std::apply(
            [&](const auto&... args) -> Task<> {
                (((void)co_await process(args)), ...);
                co_return;
            },
            ins.args
        );

        co_return;
    }

    template<typename T>
    Task<> Analyser::operator()(T&&) {
        using U = std::remove_cvref_t<T>;
        std::cout << "Unknown Grammar Type: " << std::string(U::nodeName) << std::endl;

        co_return;
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_ANALYSER_INL
