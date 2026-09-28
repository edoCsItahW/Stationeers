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

#include "ic10_compiler/pch/ast.hpp"
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

        uint32_t crc32(const std::string& str) noexcept;

    }  // namespace detail

    int32_t hashValue(const std::string& content) noexcept;

    double strValue(const std::string& content) noexcept;

    std::optional<double> constantValue(const std::string& keyword) noexcept;

    std::optional<double> macroCall(const Macro& macroCall);

    /**
     * @if zh
     * @brief 解析数值文本
     * @details 语法阶段的字面量保留原文本（`-5`、`$FF`、`%1010`），符号表里的常量值同样以文本存放，
     *          因此按前缀分派：`$` 为十六进制、`%` 为二进制，其余交给 `std::stod`。
     * @param text 数值文本
     * @return 解析成功返回数值，文本非法时返回空值
     *
     * @else
     * @brief Parse numeric text
     * @details Literals keep their source text (`-5`, `$FF`, `%1010`) and the values of constants are
     *          stored as text in the symbol table as well, so the prefix decides the base: `$` is
     *          hexadecimal, `%` is binary and anything else goes to `std::stod`.
     * @param text Numeric text
     * @return The value, or an empty optional when the text is malformed
     *
     * @endif
     */
    std::optional<double> numericText(const std::string& text) noexcept;

    /**
     * @if zh
     * @brief 字面量/宏调用 → 数值
     * @details 只处理无需执行上下文即可求值的形态：整数、浮点、进制字面量以及 `HASH`/`STR` 宏调用；
     *          寄存器、标识符、设备等需要符号表或内存的形态由 Executor 处理。
     * @tparam T 叶子节点类型
     * @param arg 叶子节点
     * @return 数值；形态不支持时返回空值
     *
     * @else
     * @brief Literal/macro call → value
     * @details Only handles the forms evaluable without execution context: integers, floats, based
     *          literals and `HASH`/`STR` macro calls. Registers, identifiers and devices need the
     *          symbol table or memory and are handled by Executor.
     * @tparam T Leaf node type
     * @param arg The leaf node
     * @return The value, or an empty optional for an unsupported form
     *
     * @endif
     */
    template<typename T>
    std::optional<double> directionValue(const T& arg);

    template<typename U, typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    U arithmeticTrans(T&& value) noexcept;

}  // namespace stationeers::ic10

#include "value.inl"

#endif  // IC10_RUNTIME_VALUE_HPP
