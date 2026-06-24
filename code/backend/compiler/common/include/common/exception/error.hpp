// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file error.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:40
 * @if zh
 * @brief 编译器错误类型定义
 * @details 提供编译器各阶段错误类型,包括语法错误、类型错误、符号重定义错误等。
 *        支持错误位置信息追踪和调用栈信息输出。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Compiler error type definitions
 * @details Provides compiler phase error types, including syntax errors, type errors,
 *        symbol redefinition errors, etc. Supports error location tracking and stack trace output.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_ERROR_HPP
#define COMPILER_ERROR_HPP
#pragma once

#include <source_location>
#include <exception>
#include <string>
#include <memory>

#include "common/utils/position.hpp"
#include "common/utils/fstr.hpp"


namespace stationeers {

    /**
     * @if zh
     *
     * @class ErrorBase
     * @brief 错误基类模板
     * @details 所有编译器错误的基类,存储错误位置信息和错误消息
     *
     * @tparam T 派生类类型,用于CRTP模式
     *
     * @elseif en
     *
     * @class ErrorBase
     * @brief Error base class template
     * @details Base class for all compiler errors, stores error location information and error message
     *
     * @tparam T Derived class type, used in CRTP pattern
     *
     * @endif
     */
    template<typename>
    struct ErrorBase : std::exception {
        /**
         * @if zh
         * @brief 错误起始位置
         * @elseif en
         * @brief Error start position
         * @endif
         */
        Pos start;

        /**
         * @if zh
         * @brief 错误结束位置
         * @elseif en
         * @brief Error end position
         * @endif
         */
        Pos end;

        /**
         * @if zh
         *
         * @brief 构造函数(string_view版本)
         * @param message 错误消息
         * @param start 错误起始位置
         * @param end 错误结束位置
         * @param loc 源代码位置信息
         *
         * @elseif en
         *
         * @brief Constructor (string_view version)
         * @param message Error message
         * @param start Error start position
         * @param end Error end position
         * @param loc Source code location information
         *
         * @endif
         */
        ErrorBase(
            std::string_view message, Pos start, Pos end,
            const std::source_location& loc = std::source_location::current()
        );

        /**
         * @if zh
         *
         * @brief 构造函数(std::string版本)
         * @param message 错误消息
         * @param start 错误起始位置
         * @param end 错误结束位置
         * @param loc 源代码位置信息
         *
         * @elseif en
         *
         * @brief Constructor (std::string version)
         * @param message Error message
         * @param start Error start position
         * @param end Error end position
         * @param loc Source code location information
         *
         * @endif
         */
        ErrorBase(
            std::string message, Pos start, Pos end,
            const std::source_location& loc = std::source_location::current()
        );

        /**
         * @if zh
         *
         * @brief 获取错误类型名称
         * @return 错误类型的字符串表示
         *
         * @elseif en
         *
         * @brief Get error type name
         * @return String representation of error type
         *
         * @endif
         */
        [[nodiscard]] std::string_view getName() const noexcept;

        /**
         * @if zh
         *
         * @brief 获取错误消息
         * @return 错误消息的引用
         *
         * @elseif en
         *
         * @brief Get error message
         * @return Reference to error message
         *
         * @endif
         */
        template<typename Self>
        [[nodiscard]] const std::string& message(this Self& self) noexcept;

        /**
         * @if zh
         *
         * @brief 获取调用栈信息
         * @return 格式化后的调用栈字符串
         *
         * @elseif en
         *
         * @brief Get stack trace information
         * @return Formatted stack trace string
         *
         * @endif
         */
        [[nodiscard]] std::string stackTraceInfo() const;

        /**
         * @if zh
         *
         * @brief 获取错误描述
         * @return C风格字符串错误描述
         *
         * @elseif en
         *
         * @brief Get error description
         * @return C-style string error description
         *
         * @endif
         */
        [[nodiscard]] const char* what() const noexcept override;

    protected:
        /**
         * @if zh
         * @brief 错误消息内容
         * @elseif en
         * @brief Error message content
         * @endif
         */
        std::string message_;

        /**
         * @if zh
         * @brief 构造位置信息
         * @elseif en
         * @brief Construction location information
         * @endif
         */
        std::source_location loc_;
    };

    /**
     * @if zh
     *
     * @class Error
     * @brief 错误类型擦除包装类
     * @details 使用类型擦除模式存储任意错误类型,支持运行时多态错误处理
     *
     * @elseif en
     *
     * @class Error
     * @brief Error type-erased wrapper class
     * @details Uses type erasure pattern to store arbitrary error types, supporting runtime polymorphic error handling
     *
     * @endif
     */
    class Error : public std::exception {
    public:
        /**
         * @if zh
         *
         * @class Base
         * @brief 错误接口基类
         * @details 定义错误类型的公共接口
         *
         * @elseif en
         *
         * @class Base
         * @brief Error interface base class
         * @details Defines the public interface for error types
         *
         * @endif
         */
        struct Base : std::exception {
            ~Base() override = default;

            /**
             * @if zh
             *
             * @brief 获取错误起始位置
             * @return 起始位置引用
             *
             * @elseif en
             *
             * @brief Get error start position
             * @return Reference to start position
             *
             * @endif
             */
            [[nodiscard]] virtual const Pos& getStart() const noexcept = 0;

            /**
             * @if zh
             *
             * @brief 获取错误结束位置
             * @return 结束位置引用
             *
             * @elseif en
             *
             * @brief Get error end position
             * @return Reference to end position
             *
             * @endif
             */
            [[nodiscard]] virtual const Pos& getEnd() const noexcept = 0;

            /**
             * @if zh
             *
             * @brief 获取错误类型名称
             * @return 错误类型名称
             *
             * @elseif en
             *
             * @brief Get error type name
             * @return Error type name
             *
             * @endif
             */
            [[nodiscard]] virtual std::string_view getName() const noexcept = 0;

            /**
             * @if zh
             *
             * @brief 获取错误消息
             * @return 错误消息
             *
             * @elseif en
             *
             * @brief Get error message
             * @return Error message
             *
             * @endif
             */
            [[nodiscard]] virtual std::string_view message() const noexcept = 0;

            /**
             * @if zh
             *
             * @brief 获取调用栈信息
             * @return 调用栈字符串
             *
             * @elseif en
             *
             * @brief Get stack trace information
             * @return Stack trace string
             *
             * @endif
             */
            [[nodiscard]] virtual std::string stackTraceInfo() const = 0;
        };

        /**
         * @if zh
         *
         * @class Model
         * @brief 错误模型模板
         * @details CRTP模式实现的错误模型,特化自具体错误类型
         *
         * @tparam Derived 具体错误类型
         *
         * @elseif en
         *
         * @class Model
         * @brief Error model template
         * @details Error model implemented with CRTP pattern, specialized for concrete error types
         *
         * @tparam Derived Concrete error type
         *
         * @endif
         */
        template<typename Derived>
        struct Model : public Base {
        private:
            Derived self_;

        public:
            /**
             * @if zh
             *
             * @brief 构造函数
             * @param self 派生类实例引用
             *
             * @elseif en
             *
             * @brief Constructor
             * @param self Derived class instance reference
             *
             * @endif
             */
            Model(const Derived& self);

            [[nodiscard]] const Pos& getStart() const noexcept override;

            [[nodiscard]] const Pos& getEnd() const noexcept override;

            [[nodiscard]] std::string_view getName() const noexcept override;

            [[nodiscard]] std::string_view message() const noexcept override;

            [[nodiscard]] std::string stackTraceInfo() const override;
        };

        /**
         * @if zh
         *
         * @brief 构造函数
         * @tparam Derived 具体错误类型
         * @param self 错误实例引用
         *
         * @elseif en
         *
         * @brief Constructor
         * @tparam Derived Concrete error type
         * @param self Error instance reference
         *
         * @endif
         */
        template<typename Derived>
        Error(const Derived& self);

        [[nodiscard]] const Pos& getStart() const noexcept;

        [[nodiscard]] const Pos& getEnd() const noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;

        [[nodiscard]] std::string_view message() const noexcept;

        [[nodiscard]] std::string stackTraceInfo() const;

        [[nodiscard]] const char* what() const noexcept override;

        Error(const Error&)            = default;
        Error& operator=(const Error&) = default;

        Error(Error&&)            = default;
        Error& operator=(Error&&) = default;

        /**
         * @if zh
         *
         * @brief 流输出运算符重载
         * @param os 输出流
         * @param e 错误引用
         * @return 输出流引用
         *
         * @elseif en
         *
         * @brief Stream output operator overload
         * @param os Output stream
         * @param e Error reference
         * @return Output stream reference
         *
         * @endif
         */
        friend std::ostream& operator<<(std::ostream& os, const Error& e);

    private:
        /**
         * @if zh
         * @brief 类型擦除的错误实现
         * @elseif en
         * @brief Type-erased error implementation
         * @endif
         */
        std::shared_ptr<Base> pimpl_;
    };

    /**
     * @if zh
     *
     * @struct SyntaxError
     * @brief 语法错误
     * @details 词法或语法分析阶段检测到的错误
     *
     * @elseif en
     *
     * @struct SyntaxError
     * @brief Syntax error
     * @details Errors detected during lexical or syntactic analysis phase
     *
     * @endif
     */
    struct SyntaxError : public ErrorBase<SyntaxError> {
        /**
         * @if zh
         * @brief 错误类型名称
         * @elseif en
         * @brief Error type name
         * @endif
         */
        static constexpr auto name = "SyntaxError"_fs;

        using ErrorBase::ErrorBase;
    };

    /**
     * @if zh
     *
     * @struct TypeError
     * @brief 类型错误
     * @details 类型检查阶段检测到的类型不匹配错误
     *
     * @elseif en
     *
     * @struct TypeError
     * @brief Type error
     * @details Type mismatch errors detected during type checking phase
     *
     * @endif
     */
    struct TypeError : ErrorBase<TypeError> {
        /**
         * @if zh
         * @brief 错误类型名称
         * @elseif en
         * @brief Error type name
         * @endif
         */
        static constexpr auto name = "TypeError"_fs;

        using ErrorBase::ErrorBase;
    };

    /**
     * @if zh
     *
     * @struct RedefinitionError
     * @brief 符号重定义错误
     * @details 同一作用域内重复定义符号时产生的错误
     *
     * @elseif en
     *
     * @struct RedefinitionError
     * @brief Symbol redefinition error
     * @details Error when redefining a symbol in the same scope
     *
     * @endif
     */
    struct RedefinitionError : public ErrorBase<RedefinitionError> {
        /**
         * @if zh
         * @brief 错误类型名称
         * @elseif en
         * @brief Error type name
         * @endif
         */
        static constexpr auto name = "RedefinitionError"_fs;

        using ErrorBase::ErrorBase;
    };

    /**
     * @if zh
     *
     * @struct UndefinedSymbolError
     * @brief 未定义符号错误
     * @details 引用未声明的符号时产生的错误
     *
     * @elseif en
     *
     * @struct UndefinedSymbolError
     * @brief Undefined symbol error
     * @details Error when referencing an undeclared symbol
     *
     * @endif
     */
    struct UndefinedSymbolError : public ErrorBase<UndefinedSymbolError> {
        /**
         * @if zh
         * @brief 错误类型名称
         * @elseif en
         * @brief Error type name
         * @endif
         */
        static constexpr auto name = "UndefinedSymbolError"_fs;

        using ErrorBase::ErrorBase;
    };

    /**
     * @if zh
     *
     * @struct RuntimeError
     * @brief 运行时错误
     * @details 程序运行阶段产生的错误
     *
     * @elseif en
     *
     * @struct RuntimeError
     * @brief Runtime error
     * @details Errors occurring during program runtime
     *
     * @endif
     */
    struct RuntimeError : public ErrorBase<RuntimeError> {
        /**
         * @if zh
         * @brief 错误类型名称
         * @elseif en
         * @brief Error type name
         * @endif
         */
        static constexpr auto name = "RuntimeError"_fs;

        using ErrorBase::ErrorBase;
    };

    /**
     * @if zh
     *
     * @brief 流输出运算符重载
     * @param os 输出流
     * @param e 错误引用
     * @return 输出流引用
     *
     * @elseif en
     *
     * @brief Stream output operator overload
     * @param os Output stream
     * @param e Error reference
     * @return Output stream reference
     *
     * @endif
     */
    std::ostream& operator<<(std::ostream& os, const Error& e);

}  // namespace stationeers

#include "error.inl"

#endif  // COMPILER_ERROR_HPP
