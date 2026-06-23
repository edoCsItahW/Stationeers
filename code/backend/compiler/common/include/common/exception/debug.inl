// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file debug.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/17 20:02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_DEBUG_INL
#define COMPILER_DEBUG_INL
#pragma once

#include <iostream>

namespace stationeers {

    // Debugger

    template<typename F>
    Debugger<F>::Debugger(F &&func, const bool exit)
        : func(std::move(func))
        , exit(exit) {}

    template<typename F>
    Debugger<F>::Debugger(
        F &&func, const std::function<void(std::exception)> &callback, const bool exit
    )
        : func(func)
        , callback(callback)
        , exit(exit) {}

    template<typename F>
    template<typename R>
    R Debugger<F>::handleError(const std::exception &e) {
        if (callback.has_value()) callback.value()(e);

        if constexpr (!std::is_same_v<R, void>) {
            if (exit) std::terminate();
        } else
            throw e;

        return R{};
    }

    template<typename F>
    template<SourceLocation L, typename... Args>
    auto Debugger<F>::operator()(Args &&...args) -> decltype(auto) {
        using R = decltype(func(std::forward<Args>(args)...));

        try {
            return func(std::forward<Args>(args)...);

        } catch (const std::exception &e) {
            if (First) {
                std::cerr << "Traceback (most recent call last):" << std::endl;

                First = false;
            }

            std::cerr << "    File \"" << L.file << "\", line " << L.line << ", in <" << L.function
                      << ">"
                      << "\n\t" << e.what() << std::endl;

            return handleError<R>(e);
        }
    }

    template<class C, typename F>
    Debugger<F C::*>::Debugger(F C::*func, const bool exit)
        : func(func)
        , exit(exit) {}

    template<class C, typename F>
    Debugger<F C::*>::Debugger(
        F C::*func, const std::function<void(std::exception)> &callback, const bool exit
    )
        : func(func)
        , callback(callback)
        , exit(exit) {}

    template<class C, typename F>
    template<typename R>
    R Debugger<F C::*>::handleError(const std::exception &e) {
        if (callback.has_value()) callback.value()(e);

        if constexpr (!std::is_same_v<R, void>) {
            if (exit) std::terminate();
        } else
            throw e;

        return R{};
    }

    template<class C, typename F>
    template<SourceLocation L, typename... Args>
    auto Debugger<F C::*>::operator()(C *obj, Args &&...args) -> decltype(auto) {
        using R = decltype((obj->*func)(std::forward<Args>(args)...));

        try {
            return (obj->*func)(std::forward<Args>(args)...);

        } catch (const std::exception &e) {
            if (First) {
                std::cerr << "Traceback (most recent call last):" << std::endl;

                First = false;
            }

            std::cerr << "    File \"" << L.file << "\", line " << L.line << ", in <"
                      << L.function << ">"
                      << "\n\t" << e.what() << std::endl;

            return handleError<R>(e);
        }
    }

}  // namespace stationeers

#endif  // COMPILER_DEBUG_INL
