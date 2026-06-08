// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file common.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:34
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COMMON_HPP
#define COMPILER_COMMON_HPP
#pragma once

#include <variant>


namespace stationeers {

    bool isAsciiSpace(unsigned char c) noexcept;

    bool isAsciiDigit(unsigned char c) noexcept;

    bool isAsciiHexDigit(unsigned char c) noexcept;

    bool isAsciiBinDigit(unsigned char c) noexcept;

    bool isAsciiAlpha(unsigned char c) noexcept;

    bool isAsciiAlnum(unsigned char c) noexcept;

    template<typename>
    struct is_variant : std::false_type {};

    template<typename... Ts>
    struct is_variant<std::variant<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_variant_v =
        is_variant<std::remove_cv_t<std::remove_reference_t<T>>>::value;

    template<typename T>
    concept IsVariant = is_variant_v<T>;

    template<IsVariant WideType, IsVariant NarrowType>
    WideType wide_cast(NarrowType&& value) {
        return std::visit(
            []<typename T>(T&& arg) -> WideType { return std::forward<T>(arg); },
            std::forward<NarrowType>(value)
        );
    }

    template<IsVariant T, typename F>
    auto call(T&& value, F&& f) {
        return std::visit(
            [&]<typename U>(U&& arg) { return f(std::forward<U>(arg)); }, std::forward<T>(value)
        );
    }

    template<typename Variant, typename T>
    inline constexpr bool variant_contains_v = []<typename... Ts>(std::variant<Ts...>*) {
        return (std::is_same_v<Ts, T> || ...);
    }(static_cast<Variant*>(nullptr));

    template<typename T>
    concept IsEnum = std::is_enum_v<T>;

}  // namespace stationeers

#endif  // COMPILER_COMMON_HPP
