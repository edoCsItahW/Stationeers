// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file fstr.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:49
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_FSTR_INL
#define COMPILER_FSTR_INL
#pragma once

namespace stationeers {

    template<std::size_t N, typename T>
    constexpr FStr<N, T>::FStr(const T (&value)[N]) {
        for (std::size_t i = 0; i < N - 1; ++i) this->value[i] = static_cast<T>(value[i]);
    }

    template<std::size_t N, typename T>
    constexpr FStr<N, T>::FStr(const std::array<T, N - 1>& value) {
        for (std::size_t i = 0; i < N - 1; ++i) this->value[i] = value[i];
    }

    template<std::size_t N, typename T>
    template<std::size_t M>
    constexpr bool FStr<N, T>::operator==(const FStr<M, T>& other) const {
        if constexpr (N != M) return false;

        return std::equal(value.begin(), value.end(), other.data.begin());
    }

    template<std::size_t N, typename T>
    constexpr bool FStr<N, T>::operator==(const FStr& other) const {
        return size != other.size ? false : value == other.value;  // c++20
    }

    template<std::size_t N, typename T>
    template<std::size_t M>
    constexpr bool FStr<N, T>::operator==(const T (&other)[M]) const {
        if constexpr (N != M) return false;

        return std::equal(value.begin(), value.end(), other);
    }

    template<std::size_t N, typename T>
    constexpr bool FStr<N, T>::equals(const T* other, std::size_t len) const {
        if (len != size) return false;

        return std::equal(value.begin(), value.end(), other);
    }

    template<std::size_t N, typename T>
    template<std::size_t M>
    constexpr FStr<N + M + 1, T> FStr<N, T>::concat(FStr<M, T>& other) {
        std::array<T, N + M + 1> result{};

        for (std::size_t i = 0; i < N - 1; ++i) result[i] = value[i];

        for (std::size_t i = 0; i < M; ++i) result[N + i] = other.value[i];

        result[N + M] = '\0';

        return result;
    }

    template<std::size_t N, typename T>
    FStr<N, T>::operator std::basic_string<T>() const {
        return std::basic_string<T>(value.data(), size);
    }

    template<std::size_t N, typename T>
    FStr<N, T>::operator std::basic_string_view<T>() const {
        return std::basic_string_view<T>(value.data(), size);
    }

    template<FString V>
    constexpr auto operator""_fs() {
        return V;
    }

}  // namespace stationeers

#endif  // COMPILER_FSTR_INL
