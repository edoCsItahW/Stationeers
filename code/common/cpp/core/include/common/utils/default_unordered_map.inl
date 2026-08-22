/**
 * @file default_unordered_map.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 10:59
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMMON_DEFAULT_UNORDERED_MAP_INL
#define COMMON_DEFAULT_UNORDERED_MAP_INL
#pragma once
#include "default_unordered_map.hpp"

namespace stationeers {

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::DefaultUnorderedMap(F factory)
        : factory_(std::move(factory)) {}

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    template<typename... Args>
    DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::DefaultUnorderedMap(
        F factory, Args&&... args
    )
        : map_(std::forward<Args>(args)...)
        , factory_(std::move(factory)) {}

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    template<typename Self>
    auto DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::find(this Self&& self, const K& key) noexcept {
        return self.map_.find(key);
    }

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    template<typename Self>
    auto&& DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::at(this Self&& self, const K& key) {
        return self.map_.at(key);
    }

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    template<typename Self>
    bool DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::contains(
        this Self&& self, const K& key
    ) noexcept {
        return self.map_.contains(key);
    }

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    V& DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::operator[](const K& key) {
        if (auto it = map_.find(key); it == map_.end()) {
            auto [inserted_it, inserted] = map_.try_emplace(key, factory_());

            return inserted_it->second;
        } else
            return it->second;
    }

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    V& DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::operator[](K&& key) {
        if (auto it = map_.find(key); it == map_.end()) {
            auto [inserted_it, inserted] = map_.try_emplace(std::move(key), factory_());

            return inserted_it->second;
        } else
            return it->second;
    }

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    template<typename Self>
    auto DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::begin(this Self&& self) noexcept {
        return self.map_.begin();
    }

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    template<typename Self>
    auto DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::end(this Self&& self) noexcept {
        return self.map_.end();
    }

    template<typename K, typename V, typename Hash, typename Eq, typename Allocator, typename F>
    template<typename Self>
    auto& DefaultUnorderedMap<K, V, Hash, Eq, Allocator, F>::getMap(this Self&& self) noexcept {
        return self.map_;
    }

}  // namespace stationeers

#endif  // COMMON_DEFAULT_UNORDERED_MAP_INL
