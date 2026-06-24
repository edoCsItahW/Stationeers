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
 * @if zh
 * @brief 调试工具与调试器实现
 * @details 提供控制台输出管理、源码位置追踪和函数调用栈调试功能。
 *        通过装饰器模式实现对函数的封装,自动捕获异常并输出调用栈信息。
 * @note 仅在非MSVC编译器下生效
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Debug utilities and debugger implementation
 * @details Provides console output management, source location tracking, and function call stack debugging.
 *        Implements function wrapping through decorator pattern to automatically catch exceptions
 *        and output call stack information.
 * @note Only effective for non-MSVC compilers
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_DEBUG_HPP
#define COMPILER_DEBUG_HPP
#pragma once

#include <functional>
#include <optional>
#include <source_location>
#include <string>

#ifndef _MSC_VER

#include <cstdint>

#endif


namespace stationeers {

    // Console

    /**
     * @if zh
     *
     * @class Console
     * @brief 控制台输出管理类
     * @details 提供分级日志输出功能,支持INFO、WARNING、ERROR等级别控制
     *
     * @elseif en
     *
     * @class Console
     * @brief Console output management class
     * @details Provides level-based log output functionality, supporting INFO, WARNING, ERROR levels
     *
     * @endif
     */
    class Console {
    public:
        /**
         * @if zh
         *
         * @enum Level
         * @brief 日志级别枚举
         *
         * @elseif en
         *
         * @enum Level
         * @brief Log level enumeration
         *
         * @endif
         */
        enum Level { NONE, INFO, WARNING, ERROR };

        /**
         * @if zh
         * @brief 当前日志级别
         * @elseif en
         * @brief Current log level
         * @endif
         */
        static Level level;

        /**
         * @if zh
         *
         * @brief 调试信息输出
         * @tparam L 日志级别
         * @param message 消息内容
         * @param newline 是否换行,默认为true
         *
         * @elseif en
         *
         * @brief Debug information output
         * @tparam L Log level
         * @param message Message content
         * @param newline Whether to add newline, defaults to true
         *
         * @endif
         */
        template<Level L = INFO>
        static void debug(const std::string& message, bool newline = true);

        /**
         * @if zh
         *
         * @brief 调试信息输出(string_view版本)
         * @tparam L 日志级别
         * @param message 消息内容
         * @param newline 是否换行
         *
         * @elseif en
         *
         * @brief Debug information output (string_view version)
         * @tparam L Log level
         * @param message Message content
         * @param newline Whether to add newline
         *
         * @endif
         */
        template<Level L = INFO>
        static void debug(std::string_view message, bool newline = true);

        /**
         * @if zh
         *
         * @brief 信息日志输出
         * @param message 消息内容
         * @param newline 是否换行
         *
         * @elseif en
         *
         * @brief Info log output
         * @param message Message content
         * @param newline Whether to add newline
         *
         * @endif
         */
        static void log(std::string_view message, bool newline = true);

        /**
         * @if zh
         *
         * @brief 警告信息输出
         * @param message 消息内容
         * @param newline 是否换行
         *
         * @elseif en
         *
         * @brief Warning information output
         * @param message Message content
         * @param newline Whether to add newline
         *
         * @endif
         */
        static void warning(const std::string& message, bool newline = true);

        /**
         * @if zh
         *
         * @brief 警告信息输出(string_view版本)
         * @param message 消息内容
         * @param newline 是否换行
         *
         * @elseif en
         *
         * @brief Warning information output (string_view version)
         * @param message Message content
         * @param newline Whether to add newline
         *
         * @endif
         */
        static void warning(std::string_view message, bool newline = true);

        /**
         * @if zh
         *
         * @brief 错误信息输出
         * @param message 消息内容
         * @param newline 是否换行
         *
         * @elseif en
         *
         * @brief Error information output
         * @param message Message content
         * @param newline Whether to add newline
         *
         * @endif
         */
        static void error(const std::string& message, bool newline = true);

        /**
         * @if zh
         *
         * @brief 错误信息输出(string_view版本)
         * @param message 消息内容
         * @param newline 是否换行
         *
         * @elseif en
         *
         * @brief Error information output (string_view version)
         * @param message Message content
         * @param newline Whether to add newline
         *
         * @endif
         */
        static void error(std::string_view message, bool newline = true);
    };

    /**
     * @if zh
     *
     * @brief 调试器截获的错误是否为程序第一个错误
     * @details 该变量用于记录调试器截获的错误是否为程序第一个错误,默认为false
     * @note 仅供 @ref debuger::Debuger 类使用
     * @see debuger::Debuger
     *
     * @elseif en
     *
     * @brief Whether the error caught by debugger is the first error of the program
     * @details This variable records whether the error caught by the debugger is the first error of the program,
     *        defaults to false
     * @note Only for use by @ref debuger::Debuger class
     * @see debuger::Debuger
     *
     * @endif
     */
    inline bool First = true;

    // SourceLocation

    /**
     * @if zh
     *
     * @struct SourceLocation
     * @brief 源代码位置信息结构
     * @details 存储源代码的位置信息,包括行号、列号、文件名和函数名
     *
     * @elseif en
     *
     * @struct SourceLocation
     * @brief Source code location information structure
     * @details Stores source code location information, including line number, column number,
     *        file name and function name
     *
     * @endif
     */
    struct SourceLocation {
        /**
         * @if zh
         * @brief 行号
         * @elseif en
         * @brief Line number
         * @endif
         */
        std::uint_least32_t line   = 0;

        /**
         * @if zh
         * @brief 列号
         * @elseif en
         * @brief Column number
         * @endif
         */
        std::uint_least32_t column = 0;

        /**
         * @if zh
         * @brief 文件名
         * @elseif en
         * @brief File name
         * @endif
         */
        const char* file           = "";

        /**
         * @if zh
         * @brief 函数名
         * @elseif en
         * @brief Function name
         * @endif
         */
        const char* function       = "";

        /**
         * @if zh
         *
         * @brief 获取当前源代码位置
         * @details 使用编译器内置函数获取当前位置信息
         * @param line 行号
         * @param col 列号
         * @param file 文件名
         * @param func 函数名
         * @return SourceLocation实例
         *
         * @elseif en
         *
         * @brief Get current source code location
         * @details Uses compiler builtin functions to get current location information
         * @param line Line number
         * @param col Column number
         * @param file File name
         * @param func Function name
         * @return SourceLocation instance
         *
         * @endif
         */
        static consteval SourceLocation current(
            const std::uint_least32_t line = __builtin_LINE(),
            #ifdef _MSC_VER
            const std::uint_least32_t col  = __builtin_COLUMN(),
            #else
            const std::uint_least32_t col  = 0,
            #endif
            const char* const file         = __builtin_FILE(),
            const char* const func = __builtin_FUNCTION()
        ) noexcept {
            return SourceLocation{line, col, file, func};
        }
    };

    // Debugger

    /**
     * @if zh
     *
     * @class Debugger
     * @brief 调试器模板类
     * @details 通过装饰器模式对函数进行封装,实现对错误的捕获、打印、定位、处理,
     *        并自下而上打印调用栈信息,便于调试。
     *
     * @tparam F 待调试函数类型
     *
     * @warning 如果 @a F 为成员函数,需先使用 @a std::bind 或匿名函数绑定到对象上
     *
     * @par 输出格式示例:
     * @code
     * Traceback (most recent call last):
     * File "/path/to/file.cpp", line 23, in <void level2(int, int)>
     *     level1 error
     * File "/path/to/file.cpp", line 28, in <void level3(int, int)>
     *     std::exception
     * @endcode
     *
     * @elseif en
     *
     * @class Debugger
     * @brief Debugger template class
     * @details Wraps functions through decorator pattern to capture, print, locate, and handle errors,
     *        printing call stack information from bottom to top for easy debugging.
     *
     * @tparam F Function type to debug
     *
     * @warning If @a F is a member function, use @a std::bind or lambda to bind to an object first
     *
     * @par Output format example:
     * @code
     * Traceback (most recent call last):
     * File "/path/to/file.cpp", line 23, in <void level2(int, int)>
     *     level1 error
     * File "/path/to/file.cpp", line 28, in <void level3(int, int)>
     *     std::exception
     * @endcode
     *
     * @endif
     */
    template<typename F>
    class Debugger {
    public:
        /**
         * @if zh
         *
         * @brief 调试器构造函数
         * @param func 待调试函数
         * @param exit 是否退出程序,默认为false
         *
         * @elseif en
         *
         * @brief Debugger constructor
         * @param func Function to debug
         * @param exit Whether to exit program, defaults to false
         *
         * @endif
         */
        explicit Debugger(
            F&& func, bool exit = false
        );

        /**
         * @if zh
         *
         * @brief 调试器构造函数(带回调函数)
         * @param func 待调试函数
         * @param callback 回调函数,用于处理异常
         * @param exit 是否退出程序,默认为false
         *
         * @elseif en
         *
         * @brief Debugger constructor (with callback)
         * @param func Function to debug
         * @param callback Callback function for exception handling
         * @param exit Whether to exit program, defaults to false
         *
         * @endif
         */
        explicit Debugger(
            F&& func, const std::function<void(std::exception)>& callback, bool exit = false
        );

        /**
         * @if zh
         *
         * @brief 调试器调用运算符
         * @tparam L 源码位置
         * @tparam Args 参数类型包
         * @param args 函数参数
         * @return 函数返回值
         * @note 当exit为true时,调试器将退出程序
         *
         * @elseif en
         *
         * @brief Debugger call operator
         * @tparam L Source location
         * @tparam Args Argument type pack
         * @param args Function arguments
         * @return Function return value
         * @note When exit is true, debugger will exit the program
         *
         * @endif
         */
        template<SourceLocation L = SourceLocation::current(), typename... Args>
        auto operator()(Args&&... args) -> decltype(auto);

    private:
        /**
         * @if zh
         *
         * @brief 待调试函数
         * @invariant 初始化后不应修改
         *
         * @elseif en
         *
         * @brief Function to debug
         * @invariant Should not be modified after initialization
         *
         * @endif
         */
        const F func;

        /**
         * @if zh
         *
         * @brief 回调函数
         * @invariant 初始化后不应修改
         *
         * @elseif en
         *
         * @brief Callback function
         * @invariant Should not be modified after initialization
         *
         * @endif
         */
        std::optional<std::function<void(const std::exception&)>> callback;

        /**
         * @if zh
         * @brief 是否退出程序标志
         * @elseif en
         * @brief Whether to exit program flag
         * @endif
         */
        bool exit;

        /**
         * @if zh
         *
         * @brief 错误处理函数
         * @tparam R 返回值类型
         * @param e 异常引用
         * @return 返回值
         *
         * @elseif en
         *
         * @brief Error handling function
         * @tparam R Return value type
         * @param e Exception reference
         * @return Return value
         *
         * @endif
         */
        template<typename R>
        R handleError(const std::exception& e);
    };

    template<class C, typename F>
    class Debugger<F C::*> {
    public:
        /**
         * @if zh
         *
         * @brief 成员函数调试器构造函数
         * @param func 成员函数指针
         * @param exit 是否退出程序
         *
         * @elseif en
         *
         * @brief Member function debugger constructor
         * @param func Member function pointer
         * @param exit Whether to exit program
         *
         * @endif
         */
        explicit Debugger(
            F C::* func, bool exit = false
        );

        /**
         * @if zh
         *
         * @brief 成员函数调试器构造函数(带回调)
         * @param func 成员函数指针
         * @param callback 回调函数
         * @param exit 是否退出程序
         *
         * @elseif en
         *
         * @brief Member function debugger constructor (with callback)
         * @param func Member function pointer
         * @param callback Callback function
         * @param exit Whether to exit program
         *
         * @endif
         */
        explicit Debugger(
            F C::* func, const std::function<void(std::exception)>& callback, bool exit = false
        );

        /**
         * @if zh
         *
         * @brief 成员函数调试器调用运算符
         * @tparam L 源码位置
         * @tparam Args 参数类型包
         * @param obj 对象指针
         * @param args 函数参数
         * @return 函数返回值
         *
         * @elseif en
         *
         * @brief Member function debugger call operator
         * @tparam L Source location
         * @tparam Args Argument type pack
         * @param obj Object pointer
         * @param args Function arguments
         * @return Function return value
         *
         * @endif
         */
        template<SourceLocation L = SourceLocation::current(), typename... Args>
        auto operator()(C* obj, Args&&... args) -> decltype(auto);

        /**
         * @if zh
         *
         * @brief 错误处理函数
         * @tparam R 返回值类型
         * @param e 异常引用
         * @return 返回值
         *
         * @elseif en
         *
         * @brief Error handling function
         * @tparam R Return value type
         * @param e Exception reference
         * @return Return value
         *
         * @endif
         */
        template<typename R>
        R handleError(const std::exception& e);

    private:
        /**
         * @if zh
         * @brief 成员函数指针
         * @elseif en
         * @brief Member function pointer
         * @endif
         */
        F C::* func;

        /**
         * @if zh
         * @brief 回调函数
         * @elseif en
         * @brief Callback function
         * @endif
         */
        std::optional<std::function<void(const std::exception&)>> callback;

        /**
         * @if zh
         * @brief 是否退出程序标志
         * @elseif en
         * @brief Whether to exit program flag
         * @endif
         */
        bool exit;

    };

}  // namespace stationeers

#include "debug.inl"

#endif  // COMPILER_DEBUG_HPP
