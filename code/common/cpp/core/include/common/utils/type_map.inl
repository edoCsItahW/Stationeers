// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file type_map.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_TYPE_MAP_INL
#define COMPILER_TYPE_MAP_INL
#pragma once

#include <algorithm>
#include <utility>

namespace stationeers {

    template<typename T, T... Ks>
    constexpr std::size_t TypeMap<T, Ks...>::operator[](T key) noexcept {
        for (std::size_t i{0}; i < keys.size(); ++i)
            if (keys[i] == key) return i;

        std::unreachable();
    }

// reslove MSVC C3577 C3528 C2977 error
#ifndef _MSC_VER
    template<typename T, T... Ks>
    template<typename... Args>
    TypeMap<T, Ks...>::Variant TypeMap<T, Ks...>::make(const T e, Args&&... args) noexcept {
        constexpr auto size = sizeof...(Ks);

        return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> Variant {
            static constexpr std::array<Variant (*)(Args&&...), size> makers = {
                +[](Args&&... _args) -> Variant {
                    return Variant{std::in_place_index<Is>, std::forward<Args>(_args)...};
                }...
            };

            return makers[operator[](e)](std::forward<Args>(args)...);
        }(std::make_index_sequence<size>{});
    }
#else
    template<typename T, T... Ks>
    template<typename... Args>
    TypeMap<T, Ks...>::Variant TypeMap<T, Ks...>::make(T e, Args&&... args) noexcept {
        constexpr std::size_t size = sizeof...(Ks);

        // 获取函数指针数组
        static constexpr auto makers =
            MakerArray<std::make_index_sequence<size>, Variant, Args...>::value;
        return makers[operator[](e)](std::forward<Args>(args)...);
    }
#endif

    template<typename T, T... Ks>
    bool TypeMap<T, Ks...>::contains(const T key) noexcept {
        return std::ranges::any_of(keys, [key](T k) { return k == key; });
    }

}  // namespace stationeers

#endif  // COMPILER_TYPE_MAP_INL
