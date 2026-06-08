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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_ERROR_HPP
#define COMPILER_ERROR_HPP
#pragma once

#include <source_location>
#include <exception>
#include <string>
#include <memory>

#include "common/utils/position.hpp"
#include "pch/utils.hpp"


namespace stationeers {

    template<typename>
    struct ErrorBase : std::exception {
        Pos start;

        Pos end;

        ErrorBase(
            std::string_view message, Pos start, Pos end,
            const std::source_location& loc = std::source_location::current()
        );

        ErrorBase(
            std::string message, Pos start, Pos end,
            const std::source_location& loc = std::source_location::current()
        );

        template<typename Self>
        [[nodiscard]] const std::string& message(this Self& self) noexcept;

        [[nodiscard]] std::string stackTraceInfo() const;

        [[nodiscard]] const char* what() const noexcept override;

    protected:
        std::string message_;

        std::source_location loc_;
    };

    class Error : public std::exception {
    public:
        struct Base : std::exception {
            ~Base() override = default;

            [[nodiscard]] virtual std::string_view message() const noexcept = 0;

            [[nodiscard]] virtual std::string stackTraceInfo() const = 0;
        };

        template<typename Derived>
        struct Model : public Base {
        private:
            Derived self_;

        public:
            Model(const Derived& self);

            [[nodiscard]] std::string_view message() const noexcept override;

            [[nodiscard]] std::string stackTraceInfo() const override;
        };

        template<typename Derived>
        Error(const Derived& self);

        [[nodiscard]] std::string_view message() const noexcept;

        [[nodiscard]] std::string stackTraceInfo() const;

        [[nodiscard]] const char* what() const noexcept override;

        Error(const Error&)            = default;
        Error& operator=(const Error&) = default;

        Error(Error&&)            = default;
        Error& operator=(Error&&) = default;

        friend std::ostream& operator<<(std::ostream& os, const Error& e);

    private:
        std::shared_ptr<Base> pimpl_;
    };

    struct SyntaxError : public ErrorBase<SyntaxError> {
        static constexpr auto name = "SyntaxError"_fs;

        using ErrorBase::ErrorBase;
    };

    struct TypeError : ErrorBase<TypeError> {
        static constexpr auto name = "TypeError"_fs;

        using ErrorBase::ErrorBase;
    };

    struct RedefinitionError : ErrorBase<RedefinitionError> {
        static constexpr auto name = "RedefinitionError"_fs;

        using ErrorBase::ErrorBase;
    };

    struct UndefinedSymbolError : ErrorBase<UndefinedSymbolError> {
        static constexpr auto name = "UndefinedSymbolError"_fs;

        using ErrorBase::ErrorBase;
    };

    struct RuntimeError : ErrorBase<RuntimeError> {
        static constexpr auto name = "RuntimeError"_fs;

        using ErrorBase::ErrorBase;
    };

    std::ostream& operator<<(std::ostream& os, const Error& e);

}  // namespace stationeers

#include "error.inl"

#endif  // COMPILER_ERROR_HPP
