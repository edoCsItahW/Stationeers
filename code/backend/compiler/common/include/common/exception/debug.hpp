// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file debug.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:29
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_DEBUG_HPP
#define COMPILER_DEBUG_HPP
#pragma once

#include <functional>
#include <optional>
#include <source_location>
#include <string>

namespace stationeers {

    // Console

    class Console {
    public:
        enum Level { NONE, INFO, WARNING, ERROR };

        static Level level;

        template<Level L = INFO>
        static void debug(const std::string& message, bool newline = true);

        template<Level L = INFO>
        static void debug(std::string_view message, bool newline = true);

        static void log(std::string_view message, bool newline = true);

        static void warning(const std::string& message, bool newline = true);

        static void warning(std::string_view message, bool newline = true);

        static void error(const std::string& message, bool newline = true);

        static void error(std::string_view message, bool newline = true);
    };

    /** @var First
     * @brief 调试器截获的错误是否为程序第一个错误
     * @details 该变量用于记录调试器截获的错误是否为程序第一个错误,默认为false.
     * @note 仅供 @ref debuger::Debuger 类使用.
     * @see debuger::Debuger
     * */
    inline bool First = true;

    // SourceLocation

    struct SourceLocation {
        std::uint_least32_t line   = 0;
        std::uint_least32_t column = 0;
        const char* file           = "";
        const char* function       = "";

        static consteval SourceLocation current(
            const std::uint_least32_t line = __builtin_LINE(),  // GCC/Clang/MSVC 均支持
            const std::uint_least32_t col  = __builtin_COLUMN(),
            const char* const file         = __builtin_FILE(),
            const char* const func = __builtin_FUNCTION()  // 或 __builtin_FUNCSIG() 在 MSVC 上
        ) noexcept {
            return SourceLocation{line, col, file, func};
        }
    };

    // Debugger

    /** @class Debugger
     * @brief 调试器模板类
     * @details
     * 调试器模板类，通过自上而下的对各层函数进行装饰，实现对错误的捕获、打印、定位、处理并自下而上的打印调用栈信息，以便调试。
     *
     * @tparam F 待调试函数类型
     *
     * @warning 如果 @a F 为成员函数，需先将该函数使用 @a std::bind
     * 或者匿名函数绑定到对象上，否则无法调用.
     *
     * @todo
     * 将loc参数从构造函数中转移或者附着到调用重载函数上，以确保调试信息的位置信息是使用函数时，而不是装饰函数时
     * @todo 记录函数信息而非实时打印，避免被其它程序输出影响，导致调试信息混乱
     *
     * @remark 程序输出信息格式形如：
     * Traceback (most recent call last):
     * File "/path/to/file.cpp", line 23, in <void level2(int, int)>
     *     level1 error
     * File "/path/to/file.cpp", line 28, in <void level3(int, int)>
     *     std::exception
     * File "/path/to/file.cpp", line 33, in <void func(int, int)>
     *     std::exception
     *
     * ## 示例
     *
     * ### 普通函数
     *
     * @code
     * int add(int a, int b) { return a + b; }
     *
     * int main() {
     *     Debugger debugger(&add);  // 位置参数loc默认值为std::source_location::current()
     *     return debugger(1, 2);
     * }
     * @endcode
     *
     * ### 成员函数
     *
     * @code
     * class A {
     * public:
     *     int add(int a, int b) { return a + b; }
     *
     *     int bindFunc(int a, int b) {
     *         // 使用std::bind绑定成员函数
     *         Debugger debugger(std::bind(&A::add, this, std::placeholders::_1,
     * std::placeholders::_2)); return debugger(1, 2);
     *     }
     *
     *     int lambdaFunc(int a, int b) {
     *         // 使用lambda表达式绑定成员函数
     *         Debugger debugger([&](int _a, int _b) { return this->add(_a, _b); });
     *         return debugger(1, 2);
     *     }
     *
     *     int specializedFunc(int a, int b) {
     *         // 使用其特化版本
     *         Debugger debugger(&A::add);
     *         return debugger(this, 1, 2);
     *     }
     * };
     *
     * int main() {
     *     A a;
     *     // 使用std::bind绑定成员函数
     *     Debugger debugger(std::bind(&A::add, &a, std::placeholders::_1, std::placeholders::_2));
     *
     *     // 使用lambda表达式绑定成员函数
     *     Debugger debugger([&](int _a, int _b) { return a.add(_a, _b); });
     *
     *     // 使用其特化版本
     *     Debugger debugger(&A::add);
     *     debugger(&a, 1, 2);  // 调用A::add函数并打印调试信息
     *     return 0;
     * }
     * @endcode
     *
     * @note 对于特化函数，有时编译器无法正确推导。
     * 例如:
     * @code
     * struct Test {
     *     template<typename T>
     *     void func(T _t, int _i) { _t do something }
     *
     *     template<typename T>
     *     void func(T _t, double _d) { _t do something else }
     * };
     * @endcode
     * 这时 @c Test::func<T> 被视为一个重载函数，这种情况下编译器无法确定具体函数类型，因为模板参数
     * @c T 可以是任意类型. 并且此时编译器看到的是一个重载函数列表，而不是具体的函数，这导致 @c T
     * 无法推导.  \n
     * **解决方案**  \n
     * 编译器无法根据调用上下文确定 @c T 的确切类型，所以需要明确指定。这通常通过强制转换来完成。 \n
     * @code
     * using FuncPtr = void (Test::*)(T, int);
     * auto fPtr = static_cast<FuncPtr>(&Test::func<T>);
     * debuger::Debugger dbg(fPtr);
     * dbg(this, 1);
     * @endcode
     * 这样就可以正确推导 @c T 的类型。
     *
     * @tip exit参数可以用来控制调试器是否退出程序，当 @a exit
     * 为true时，调试器不再打印调用栈信息，转而退出程序.  \n 例如:
     * @code
     * void level1() { throw std::runtime_error("error"); }
     * void level2() {
     *     Debugger debugger(level1);
     *     debugger();
     * }
     * void mustExit() {
     *     Debugger debugger(level2, true);  // 调试器退出程序
     *     debugger();
     * }
     * @endcode
     * */
    template<typename F>
    class Debugger {
    public:
        /**
         * @brief 调试器模板类构造函数
         * @details 构造函数初始化调试器参数，并设置调试器是否退出程序的默认值为false.
         * @param func 待调试函数
         * @param exit 调试器是否退出程序，默认为false
         * @remark @a loc 参数默认为std::source_location::current()， @a exit 参数默认为false.
         * */
        explicit Debugger(
            F&& func, bool exit = false
        );

        /**
         * @brief 调试器模板类构造函数（带回调函数）
         * @details 构造函数初始化调试器参数，并设置调试器是否退出程序的默认值为false.
         * @param func 待调试函数
         * @param callback 回调函数，用于处理异常
         * @param exit 调试器是否退出程序，默认为false
         * @remark @a loc 参数默认为std::source_location::current()， @a exit 参数默认为false.
         * */
        explicit Debugger(
            F&& func, const std::function<void(std::exception)>& callback, bool exit = false
        );

        /**
         * @brief 调试器模板类调用运算符重载
         * @details 调用调试器模板类，打印调试信息并调用待调试函数.
         * @tparam Args 待调试函数参数类型
         * @param args 待调试函数参数
         * @return 待调试函数返回值类型
         * @note 当 @a _exit 为true时，调试器将退出程序.
         * */
        template<SourceLocation L = SourceLocation::current(), typename... Args>
        auto operator()(Args&&... args) -> decltype(auto);

    private:
        /**
         * @brief 待调试函数
         * @details 待调试函数，由构造函数初始化.
         * @invariant 自初始化赋值后不应修改
         * @qualifier const
         * */
        const F func;

        /**
         * @brief 回调函数
         * @details 回调函数，由构造函数初始化.
         * @invariant 自初始化赋值后不应修改
         * @qualifier const
         * */
        std::optional<std::function<void(const std::exception&)>> callback;

        /**
         * @brief 调试器是否退出程序
         * @details 调试器是否退出程序，由构造函数初始化.
         * */
        bool exit;

        /**
         * @brief 打印调试信息
         * @details 打印调试信息，包括函数调用位置、函数参数、函数返回值、函数异常信息.
         * @tparam R 待调试函数返回值类型
         * @param e 待调试函数异常信息
         * @return 待调试函数返回值
         * */
        template<typename R>
        R handleError(const std::exception& e);
    };

    template<class C, typename F>
    class Debugger<F C::*> {
    public:
        explicit Debugger(
            F C::* func, bool exit = false
        );

        explicit Debugger(
            F C::* func, const std::function<void(std::exception)>& callback, bool exit = false
        );

        template<SourceLocation L = SourceLocation::current(), typename... Args>
        auto operator()(C* obj, Args&&... args) -> decltype(auto);

        template<typename R>
        R handleError(const std::exception& e);

    private:
        F C::* func;

        std::optional<std::function<void(const std::exception&)>> callback;

        bool exit;

    };

}  // namespace stationeers

#include "debug.inl"

#endif  // COMPILER_DEBUG_HPP
