// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file value.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/08 16:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_VALUE_HPP
#define IC10_RUNTIME_VALUE_HPP
#pragma once

#include "ic10_compiler/parser/ast/ast.hpp"
#include "common/utils/fstr.hpp"
#include <optional>
#include <cstdint>
#include <string>

namespace stationeers::ic10 {

    extern const std::unordered_map<std::string, double> CONSTANTS;

    /// @brief 最小正次正规数 * 8，对应 Stationeers 中 epsilon 的游戏定义
    inline constexpr double EPSILON_TIMES_8 = std::numeric_limits<double>::denorm_min() * 8.0;

    template<typename T, IsVariant U>
        requires requires {
            { T::nodeName } -> is_fstring;
        }
    auto getValue(U& operand) -> std::conditional_t<std::is_const_v<U>, const T&, T&>;

    namespace detail {

        uint32_t crc32(const std::string& str);

    }  // namespace detail

    int32_t hashValue(const std::string& content);

    double strValue(const std::string& content);

    std::optional<double> constantValue(const std::string& keyword);

    std::optional<double> macroCall(const MacroCall& macroCall);

    std::optional<double> directionValue(const Operand& operand);

    template<typename U, typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    U arithmeticTrans(T&& value);

}  // namespace stationeers::ic10

#include "value.inl"

#endif  // IC10_RUNTIME_VALUE_HPP
