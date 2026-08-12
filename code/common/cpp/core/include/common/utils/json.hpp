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
 * @if zh
 * @brief JSON格式化工具
 * @details 提供编译期安全的JSON字符串构造工具,支持键值对形式的JSON对象构建。
 *        通过FStr类型约束实现编译期键名验证,避免运行时字符串错误。
 * @note 键名必须为FStr类型,值支持字符串、数字和optional类型
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief JSON formatting utilities
 * @details Provides compile-time safe JSON string construction tools, supporting key-value pair
 * JSON object building. Achieves compile-time key name validation through FStr type constraints,
 * avoiding runtime string errors.
 * @note Key names must be FStr type, values support strings, numbers, and optional types
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_JSON_HPP
#define COMPILER_JSON_HPP
#pragma once

#include "common.hpp"
#include "fstr.hpp"

#include <unordered_map>
#include <optional>
#include <string>
#include <vector>

namespace stationeers {

    /**
     * @if zh
     *
     * @brief JSON字符串可转换概念
     * @details 约束类型T可以转换为JSON字符串表示
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief JSON string convertible concept
     * @details Constrains type T to be convertible to JSON string representation
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T, typename U = std::decay_t<T>>
    concept JsonStringAble =
        std::is_constructible_v<std::string, U> || std::is_arithmetic_v<U>
        || (IsOptional<U>
            && std::is_constructible_v<std::string, std::decay_t<optional_trait_t<U>>>);

    /**
     * @if zh
     *
     * @brief JSON参数可接受概念
     * @details 约束类型T可以作为JSON值参数
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief JSON argument acceptable concept
     * @details Constrains type T to be acceptable as JSON value argument
     * @tparam T Type to check
     *
     * @endif
     */
    template<typename T>
    concept JsonArgsAble = JsonStringAble<T> || std::is_same_v<std::decay_t<T>, std::nullopt_t>;

    /**
     * @if zh
     *
     * @brief 将值转换为JSON字符串
     * @tparam T JSON可转换类型
     * @param value 要转换的值
     * @return JSON格式的字符串
     *
     * @elseif en
     *
     * @brief Convert value to JSON string
     * @tparam T JSON convertible type
     * @param value Value to convert
     * @return JSON-formatted string
     *
     * @endif
     */
    template<JsonStringAble T>
    std::string toJsonString(T value);

    /**
     * @if zh
     *
     * @brief 构建JSON对象字符串
     * @tparam Vs FStr键名包
     * @tparam Args 值参数包
     * @param args 键值对参数
     * @return 格式化的JSON对象字符串
     * @note 键名数量必须与值参数数量相等
     *
     * @par 示例:
     * @code
     * std::string json = toJson<"name", "value">("test", 42);
     * // 结果: {"name": "test", "value": 42}
     * @endcode
     *
     * @elseif en
     *
     * @brief Build JSON object string
     * @tparam Vs FStr key name pack
     * @tparam Args Value argument pack
     * @param args Key-value pair arguments
     * @return Formatted JSON object string
     * @note Number of key names must equal number of value arguments
     *
     * @par Example:
     * @code
     * std::string json = toJson<"name", "value">("test", 42);
     * // Result: {"name": "test", "value": 42}
     * @endcode
     *
     * @endif
     */
    template<FString... Vs, JsonArgsAble... Args>
        requires(sizeof...(Vs) == sizeof...(Args))
    std::string toJson(Args&&... args);

    template<JsonArgsAble T>
    std::string toJson(std::unordered_map<std::string, T> map);

    template<JsonArgsAble T>
    std::string toJson(std::vector<T> vec);

}  // namespace stationeers

#include "json.inl"

#endif  // COMPILER_JSON_HPP