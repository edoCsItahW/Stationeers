// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/20 03:15
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_JSON_HPP
#define COMPILER_JSON_HPP
#pragma once

#include "common.hpp"
#include "fstr.hpp"
#include <optional>
#include <string>

namespace stationeers::ic10 {

    template<typename T>
    concept JsonStringAble =
        std::is_constructible_v<std::string, std::decay_t<T>> || std::is_arithmetic_v<T>
        || (IsOptional<T>
            && std::is_constructible_v<std::string, std::decay_t<optional_trait_t<T>>>);

    template<typename T>
    concept JsonArgsAble = JsonStringAble<T> || std::is_same_v<std::decay_t<T>, std::nullopt_t>;

    template<JsonStringAble T>
    std::string toJsonString(T value);

    template<FString... Vs, JsonArgsAble... Args>
        requires(sizeof...(Vs) == sizeof...(Args))
    std::string toJson(Args&&... args);

}  // namespace stationeers::ic10

#include "json.inl"

#endif  // COMPILER_JSON_HPP
