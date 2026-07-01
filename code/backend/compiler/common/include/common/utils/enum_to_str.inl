// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file enum_to_str.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:21
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_ENUM_TO_STR_INL
#define COMPILER_ENUM_TO_STR_INL
#pragma once

#include <string_view>
#include <array>


namespace stationeers {

    template<auto V>
    constexpr auto enumToStr() {
        std::string_view name;

#if __GNUC__ || __clang__
        name              = __PRETTY_FUNCTION__;
        std::size_t start = name.find_first_of('=') + 2;
        name              = std::string_view(name.data() + start, name.size() - start - 1);
        start             = name.rfind("::");

#elif _MSC_VER
        name              = __FUNCSIG__;
        std::size_t start = name.find_first_of('<') + 1;
        name              = std::string_view(name.data() + start, name.rfind('>') - start);
        start             = name.rfind("::");

#endif

        return start == std::string_view::npos
                 ? name
                 : std::string_view{name.data() + start + 2, name.size() - start - 2};
    }

    template<typename T, std::size_t N>
    constexpr auto enumMax() {
        if constexpr (
            constexpr auto Value = static_cast<T>(N);
            enumToStr<Value>().find(")") == std::string_view::npos
        )
            return enumMax<T, N + 1>();

        else
            return N;
    }

    template<typename T>
        requires std::is_enum_v<T>
    constexpr auto enumToStr(T value) {
        constexpr auto num = enumMax<T>();

        static constexpr auto names = []<std::size_t... I>(std::index_sequence<I...>) {
            return std::array<std::string_view, num>{enumToStr<static_cast<T>(I)>()...};
        }(std::make_index_sequence<num>{});

        return names[static_cast<std::size_t>(value)];
    }

}  // namespace stationeers

#endif  // COMPILER_ENUM_TO_STR_INL
