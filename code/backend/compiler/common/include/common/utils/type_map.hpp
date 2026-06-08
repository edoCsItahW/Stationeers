// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file type_map.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:55
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_TYPE_MAP_HPP
#define COMPILER_TYPE_MAP_HPP
#pragma once

#include <array>
#include <tuple>
#include <variant>

namespace stationeers {

    template<auto K>
    struct TypeFor;

    template<typename T, T... Ks>
    struct TypeMap {
    private:
        template<T K>
        using MappedType = TypeFor<K>::type;

        using Variant = std::variant<MappedType<Ks>...>;

        using TypeList = std::tuple<MappedType<Ks>...>;

        static constexpr std::array<T, sizeof...(Ks)> keys{Ks...};

        static constexpr std::size_t operator[](T key) noexcept;

// reslove MSVC C3577 C3528 C2977 error
#ifdef _MSC_VER
        template<typename, typename, typename...>
        struct MakerArray;

        template<std::size_t I, typename... Args>
        static Variant construct(Args&&... args) noexcept {
            return Variant{std::in_place_index<I>, std::forward<Args>(args)...};
        }

        template<std::size_t... Is, typename... Args>
        struct MakerArray<std::index_sequence<Is...>, Variant, Args...> {
            static constexpr std::array<Variant (*)(Args&&...), sizeof...(Is)> value = {
                &TypeMap::construct<Is, Args...>...
            };
        };
#endif

    public:
        template<typename... Args>
        static Variant make(T e, Args&&... args) noexcept;

        static bool contains(T key) noexcept;

        template<typename U>
        static constexpr bool contains_v = (... || std::is_same_v<U, MappedType<Ks>>);
    };

}  // namespace stationeers

#include "type_map.inl"

#endif  // COMPILER_TYPE_MAP_HPP
