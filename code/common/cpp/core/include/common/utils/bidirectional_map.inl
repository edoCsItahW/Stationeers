// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file bidirectional_map.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_BIDIRECTIONAL_MAP_INL
#define COMPILER_BIDIRECTIONAL_MAP_INL
#pragma once

namespace stationeers {

    // BiMapFindResult

    template<typename Iterator>
    BiMapFindResult<Iterator>::BiMapFindResult() noexcept(std::is_nothrow_default_constructible_v<Iterator>)
        : iter_()
        , endFlag_(true) {}

    template<typename Iterator>
    BiMapFindResult<Iterator>::BiMapFindResult(Iterator iter) noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        : iter_(iter)
        , endFlag_(false) {}

    template<typename Iterator>
    BiMapFindResult<Iterator>::operator bool() const noexcept {
        return !endFlag_;
    }

    template<typename Iterator>
    auto& BiMapFindResult<Iterator>::operator*() const {
        using ValueType = std::iterator_traits<Iterator>::value_type;
        using Reference = std::iterator_traits<Iterator>::reference;

        if (endFlag_) {
            static ValueType fallback{};
            return const_cast<Reference>(fallback);
        }

        return *iter_;
    }

    template<typename Iterator>
    auto BiMapFindResult<Iterator>::operator->() const {
        using ValueType = std::iterator_traits<Iterator>::value_type;
        using Pointer = std::iterator_traits<Iterator>::pointer;

        if (endFlag_) {
            static ValueType fallback{};
            return const_cast<Pointer>(&fallback);
        }

        return Pointer{&*iter_};
    }

    template<typename Iterator>
    bool BiMapFindResult<Iterator>::operator==(const BiMapFindResult& other) const
        noexcept(noexcept(std::declval<const Iterator&>() == std::declval<const Iterator&>())) {
        return (endFlag_ && other.endFlag_) || iter_ == other.iter_;
    }

    template<typename Iterator>
    bool BiMapFindResult<Iterator>::operator==(const BiMapEndTag&) const noexcept {
        return endFlag_;
    }

    template<typename Iterator>
    BiMapFindResult<Iterator>::operator Iterator() const
        noexcept(std::is_nothrow_default_constructible_v<Iterator> &&
                 std::is_nothrow_copy_constructible_v<Iterator>) {
        if (endFlag_) return Iterator{};

        return Iterator{iter_};
    }

    // BiMap

    template<typename K, typename V>
    BiMap<K, V>::BiMap(std::initializer_list<ContentType> list) {
        for (auto& item : list) insert(item.first, item.second);
    }

    template<typename K, typename V>
    void BiMap<K, V>::insert(const K& key, const V& value) {
        if (forwardMap_.contains(key) || backwardMap_.contains(value))
            throw std::runtime_error("Key already exists");

        forwardMap_[key]    = value;
        backwardMap_[value] = key;
    }

    template<typename K, typename V>
    template<IsBiMapKeyOrValue<K, V> T, typename Self>
    auto& BiMap<K, V>::at(this Self& self, const T& keyOrValue) {
        if constexpr (IsBiMapKey<T, K>)
            return self.forwardMap_.at(keyOrValue);
        else
            return self.backwardMap_.at(keyOrValue);
    }

    template<typename K, typename V>
    template<IsBiMapKeyOrValue<K, V> T, typename Self>
    auto& BiMap<K, V>::operator[](this Self& self, const T& keyOrValue) {
        if constexpr (IsBiMapKey<T, K>)
            return self.forwardMap_[keyOrValue];
        else
            return self.backwardMap_[keyOrValue];
    }

    template<typename K, typename V>
    template<IsBiMapKeyOrValue<K, V> T>
    bool BiMap<K, V>::contains(const T& keyOrValue) const noexcept {
        if constexpr (IsBiMapKey<T, K>)
            return forwardMap_.contains(keyOrValue);
        else
            return backwardMap_.contains(keyOrValue);
    }

    template<typename K, typename V>
    template<IsBiMapKeyOrValue<K, V> T, typename Self>
    auto BiMap<K, V>::find(this Self& self, const T& keyOrValue) noexcept {
        if constexpr (IsBiMapKey<T, K>)
            return self.findValue(keyOrValue);
        else
            return self.findKey(keyOrValue);
    }

    template<typename K, typename V>
    template<typename Self>
    auto BiMap<K, V>::findValue(this Self& self, const K& key) noexcept {
        if (auto it = self.forwardMap_.find(key); it != self.forwardMap_.end())
            return BiMapFindResult<decltype(it)>(it);

        using IteratorType = std::conditional_t<
            std::is_const_v<Self>, typename std::unordered_map<K, V>::const_iterator,
            typename std::unordered_map<K, V>::iterator>;

        return BiMapFindResult<IteratorType>{};
    }

    template<typename K, typename V>
    template<typename Self>
    auto BiMap<K, V>::findKey(this Self& self, const V& value) noexcept {
        if (auto it = self.backwardMap_.find(value); it != self.backwardMap_.end())
            return BiMapFindResult<decltype(it)>(it);

        using IteratorType = std::conditional_t<
            std::is_const_v<Self>, typename std::unordered_map<V, K>::const_iterator,
            typename std::unordered_map<V, K>::iterator>;

        return BiMapFindResult<IteratorType>{};
    }

    template<typename K, typename V>
    auto BiMap<K, V>::end() noexcept {
        return BiMapEndTag{};
    }

    template<typename K, typename V>
    std::size_t BiMap<K, V>::size() const noexcept {
        return forwardMap_.size();
    }

    template<typename K, typename V>
    void BiMap<K, V>::clear() noexcept {
        forwardMap_.clear();
        backwardMap_.clear();
    }

}  // namespace stationeers

#endif  // COMPILER_BIDIRECTIONAL_MAP_INL
