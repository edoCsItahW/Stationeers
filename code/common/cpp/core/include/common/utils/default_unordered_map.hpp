// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file default_unordered_map.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 10:48
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMMON_DEFAULT_UNORDERED_MAP_HPP
#define COMMON_DEFAULT_UNORDERED_MAP_HPP
#pragma once
#include <concepts>
#include <functional>

namespace stationeers {

    template<typename F, typename V>
    concept DefaultFactory = std::invocable<F> && std::convertible_to<std::invoke_result_t<F>, V>;

    template<
        typename K, typename V, typename Hash = std::hash<K>, typename Eq = std::equal_to<K>,
        typename Allocator = std::allocator<std::pair<const K, V>>, typename F = std::function<V()>>
    class DefaultUnorderedMap {
    public:
        using map_type       = std::unordered_map<K, V, Hash, Eq, Allocator>;
        using key_type       = map_type::key_type;
        using mapped_type    = map_type::mapped_type;
        using value_type     = map_type::value_type;
        using size_type      = map_type::size_type;
        using iterator       = map_type::iterator;
        using const_iterator = map_type::const_iterator;

        explicit DefaultUnorderedMap(F factory = F{});

        template<typename... Args>
        DefaultUnorderedMap(F factory, Args&&... args);

        V& operator[](const K& key);

        V& operator[](K&& key);

        template<typename Self>
        auto find(this Self&& self, const K& key);

        template<typename Self>
        auto&& at(this Self&& self, const K& key);

        template<typename Self>
        bool contains(this Self&& self, const K& key);

        template<typename Self>
        auto begin(this Self&& self);

        template<typename Self>
        auto end(this Self&& self);

        template<typename Self>
        auto& getMap(this Self&& self);

    private:
        map_type map_;

        F factory_;
    };

}  // namespace stationeers

#include "default_unordered_map.inl"

#endif  // COMMON_DEFAULT_UNORDERED_MAP_HPP
