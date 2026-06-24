// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file error.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:44
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_ERROR_INL
#define COMPILER_ERROR_INL
#pragma once

#include <format>
#include <sstream>

namespace stationeers {

    // ErrorBase

    template<typename Derived>
    ErrorBase<Derived>::ErrorBase(
        std::string_view message, Pos start, Pos end, const std::source_location& loc
    )
        : start(start)
        , end(end)
        , message_(std::format("[{}]: {}", std::string_view(Derived::name), message))
        , loc_(loc) {}

    template<typename Derived>
    ErrorBase<Derived>::ErrorBase(
        std::string message, Pos start, Pos end, const std::source_location& loc
    )
        : start(start)
        , end(end)
        , message_(std::format("[{}]: {}", std::string_view(Derived::name), message))
        , loc_(loc) {}

    template<typename Derived>
    std::string_view ErrorBase<Derived>::getName() const noexcept {
        return Derived::name;
    }

    template<typename Derived>
    template<typename Self>
    const std::string& ErrorBase<Derived>::message(this Self& self) noexcept {
        return self.message_;
    }

    template<typename Derived>
    std::string ErrorBase<Derived>::stackTraceInfo() const {
        std::ostringstream ss;

        ss << "    File \"" << loc_.file_name() << "\", line " << loc_.line() << ", in <"
           << loc_.function_name() << ">"
           << "\n\t" << what();

        return ss.str();
    }

    template<typename Derived>
    const char* ErrorBase<Derived>::what() const noexcept {
        return message_.c_str();
    }

    // Error::Model

    template<typename Derived>
    Error::Model<Derived>::Model(const Derived& self)
        : self_(self) {}

    template<typename Derived>
    const Pos& Error::Model<Derived>::getStart() const noexcept {
        return self_.start;
    }

    template<typename Derived>
    const Pos& Error::Model<Derived>::getEnd() const noexcept {
        return self_.end;
    }

    template<typename Derived>
    std::string_view Error::Model<Derived>::getName() const noexcept {
        return self_.getName();
    }

    template<typename Derived>
    std::string_view Error::Model<Derived>::message() const noexcept {
        return self_.message();
    }

    template<typename Derived>
    std::string Error::Model<Derived>::stackTraceInfo() const {
        return self_.stackTraceInfo();
    }

    // Error

    template<typename Derived>
    Error::Error(const Derived& self)
        : pimpl_(std::make_shared<Model<Derived>>(self)) {}

}  // namespace stationeers

#endif  // COMPILER_ERROR_INL
