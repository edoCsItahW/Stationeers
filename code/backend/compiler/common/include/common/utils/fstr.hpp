// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file fstr.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:48
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_FSTR_HPP
#define COMPILER_FSTR_HPP
#pragma once

#include <array>


namespace stationeers {

    template<std::size_t N, typename T = char>
    struct FStr {
        using value_type = T;

        std::array<T, N - 1> value;

        static constexpr std::size_t size = N - 1;

        constexpr FStr(const T (&value)[N]);

        constexpr FStr(const std::array<T, N - 1>& value);

        template<std::size_t M>
        constexpr bool operator==(const FStr<M, T>& other) const;

        constexpr bool operator==(const FStr& other) const;

        template<std::size_t M>
        constexpr bool operator==(const T (&other)[M]) const;

        constexpr bool equals(const T* other, std::size_t len) const;

        template<std::size_t M>
        constexpr FStr<N + M + 1, T> concat(FStr<M, T>& other);

        operator std::basic_string<T>() const;

        operator std::basic_string_view<T>() const;
    };

    template<std::size_t N>
    using FString = FStr<N>;

    template<typename>
    struct isFStr : std::false_type {};

    template<std::size_t N, typename T>
    struct isFStr<FStr<N, T>> : std::true_type {};

    template<FStr V1, FStr V2>
    static constexpr FStr<V1.size + V2.size + 1, typename decltype(V1)::value_type> fstr_concat_v =
        {[] {
            std::array<typename decltype(V1)::value_type, V1.size + V2.size> result{};

            for (std::size_t i = 0; i < V1.size; ++i) result[i] = V1.value[i];

            for (std::size_t i = 0; i < V2.size; ++i) result[V1.size + i] = V2.value[i];

            return result;
        }()};

    template<typename T>
    inline constexpr bool isFStr_v = isFStr<T>::value;

    template<typename T>
    concept is_fs = isFStr_v<T>;

    template<typename>
    struct isFString : std::false_type {};

    template<std::size_t N>
    struct isFString<FStr<N>> : std::true_type {};

    template<typename T>
    inline constexpr bool isFString_v = isFString<T>::value;

    template<typename T>
    concept is_fstring = isFString_v<T>;

    template<FString V>
    constexpr auto operator""_fs();

}  // namespace stationeers

#include "fstr.inl"

#endif  // COMPILER_FSTR_HPP
