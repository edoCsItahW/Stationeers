// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file bidirectional_map.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:09
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_BIDIRECTIONAL_MAP_HPP
#define COMPILER_BIDIRECTIONAL_MAP_HPP
#pragma once

#include <unordered_map>
#include <stdexcept>


namespace stationeers {

    template<typename T, typename K>
    concept IsBiMapKey = std::is_same_v<T, K>;

    template<typename T, typename V>
    concept IsBiMapValue = std::is_same_v<T, V>;

    template<typename T, typename K, typename V>
    concept IsBiMapKeyOrValue = IsBiMapKey<T, K> || IsBiMapValue<T, V>;

    struct BiMapEndTag {};

    // BiMapFindResult

    template<typename Iterator>
    class BiMapFindResult {
    public:
        BiMapFindResult();

        BiMapFindResult(Iterator iter);

        explicit operator bool() const;

        auto &operator*() const;

        auto operator->() const;

        bool operator==(const BiMapFindResult &other) const;

        bool operator==(const BiMapEndTag &) const;

        explicit operator Iterator() const;

    private:
        Iterator iter_;
        bool endFlag_;
    };

    // BiMap

    template<typename K, typename V>
    class BiMap {
    public:
        using ContentType = std::pair<const K, V>;

        BiMap() = default;

        BiMap(std::initializer_list<ContentType> list);

        void insert(const K &key, const V &value);

        template<IsBiMapKeyOrValue<K, V> T, typename Self>
        auto &at(this Self &self, const T &keyOrValue);

        template<IsBiMapKeyOrValue<K, V> T>
        bool contains(const T &keyOrValue) const;

        template<IsBiMapKeyOrValue<K, V> T, typename Self>
        auto &operator[](this Self &self, const T &keyOrValue);

        template<IsBiMapKeyOrValue<K, V> T, typename Self>
        auto find(this Self &self, const T &keyOrValue);

        static auto end();

        [[nodiscard]] std::size_t size() const;

        void clear();

    private:
        std::unordered_map<K, V> forwardMap_;
        std::unordered_map<V, K> backwardMap_;
    };

}  // namespace stationeers

#include "bidirectional_map.inl"

#endif  // COMPILER_BIDIRECTIONAL_MAP_HPP
