// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

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

#ifdef _MSC_VER

    #include <excpt.h>

#elif defined(__GNUC__) || defined(__clang__)

    #include <csetjmp>
    #include <csignal>

#endif

namespace stationeers {
    // Debugger

    template<typename F>
    Debugger<F>::Debugger(
        F &&func, const bool exit
    ) noexcept(std::is_nothrow_move_constructible_v<F>)
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
    Debugger<F C::*>::Debugger(F C::*func, const bool exit) noexcept
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

            std::cerr << "    File \"" << L.file << "\", line " << L.line << ", in <" << L.function
                      << ">"
                      << "\n\t" << e.what() << std::endl;

            return handleError<R>(e);
        }
    }

    // SafeExecutor

#ifdef _MSC_VER

    template<typename F>
    auto safeExecute(F &&func) noexcept -> SafeExecuteResult<decltype(func())> {
        using R = decltype(func());

        SafeExecuteResult<R> result;

        __try {
            result.result    = func();
            result.success   = true;
            result.errorCode = 0;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            result.success   = false;
            result.errorCode = GetExceptionCode();
        }

        return result;
    }

#elif defined(__GNUC__) || defined(__clang__)

    inline thread_local sigjmp_buf g_jmpBuf;
    inline thread_local bool g_jmpBufValid = false;

    static void SafeExecuteSignalHandler(int sig) {
        if (g_jmpBufValid) siglongjmp(g_jmpBuf, sig);

        std::signal(sig, SIG_DFL);

        raise(sig);
    }

    template<typename F>
    auto safeExecute(F &&func) noexcept -> SafeExecuteResult<decltype(func())> {
        using R = decltype(func());
        SafeExecuteResult<R> result;

        struct sigaction oldSegv, oldFpe, oldIll, oldBus;

        bool installed = false;

        struct sigaction sa;

        sa.sa_handler = SafeExecuteSignalHandler;

        sigemptyset(&sa.sa_mask);

        sa.sa_flags = 0;

        if (sigaction(SIGSEGV, &sa, &oldSegv) == 0 && sigaction(SIGFPE, &sa, &oldFpe) == 0
            && sigaction(SIGILL, &sa, &oldIll) == 0 && sigaction(SIGBUS, &sa, &oldBus) == 0)
            installed = true;
        else {
            result.result    = func();
            result.success   = true;
            result.errorCode = 0;

            return result;
        }

        int sig = sigsetjmp(g_jmpBuf, 1);
        if (sig == 0) {
            // 正常路径：标记缓冲区有效，执行用户函数
            g_jmpBufValid    = true;
            result.result    = func();
            result.success   = true;
            result.errorCode = 0;
        } else {
            // 信号跳转路径：设置错误信息
            result.success   = false;
            result.errorCode = sig;  // 信号编号作为错误码
        }

        g_jmpBufValid = false;

        // 恢复旧的信号处理器
        if (installed) {
            sigaction(SIGSEGV, &oldSegv, nullptr);
            sigaction(SIGFPE, &oldFpe, nullptr);
            sigaction(SIGILL, &oldIll, nullptr);
            sigaction(SIGBUS, &oldBus, nullptr);
        }

        return result;
    }

#else

    #error "SafeExecute cannot be implemented in unsupported compilers"

#endif

}  // namespace stationeers

#endif  // COMPILER_DEBUG_INL
