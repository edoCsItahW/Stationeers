// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file fstr.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:48
 * @if zh
 * @brief 格式化字符串字面量工具
 * @details 提供编译期固定的字符串类型(FStr),支持字符串字面量操作、连接、比较等功能。
 *        主要用于需要在编译期处理字符串字面量的场景。
 * @note 字符串长度不包括null终止符
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Formatted string literal utility
 * @details Provides compile-time fixed string type (FStr), supporting string literal operations,
 *        concatenation, comparison and other functionalities. Mainly used for scenarios requiring
 *        compile-time string literal processing.
 * @note String length does not include null terminator
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_FSTR_HPP
#define COMPILER_FSTR_HPP
#pragma once

#include <array>


namespace stationeers {

    /**
     * @if zh
     *
     * @class FStr
     * @brief 编译期固定长度字符串类型
     * @details 存储编译期确定的固定长度字符串,不含null终止符。
     *        提供编译期字符串比较、连接和运行时转换功能。
     *
     * @tparam N 字符串长度(不包括null终止符)
     * @tparam T 字符类型,默认为char
     *
     * @par 示例:
     * @code
     * using MyStr = FStr<5>;
     * constexpr MyStr s1 = "hello"_fs;
     * constexpr MyStr s2 = {'h', 'e', 'l', 'l', 'o'};
     * static_assert(s1 == s2);
     * @endcode
     *
     * @elseif en
     *
     * @class FStr
     * @brief Compile-time fixed-length string type
     * @details Stores compile-time determined fixed-length strings, without null terminator.
     *        Provides compile-time string comparison, concatenation and runtime conversion.
     *
     * @tparam N String length (excluding null terminator)
     * @tparam T Character type, defaults to char
     *
     * @par Example:
     * @code
     * using MyStr = FStr<5>;
     * constexpr MyStr s1 = "hello"_fs;
     * constexpr MyStr s2 = {'h', 'e', 'l', 'l', 'o'};
     * static_assert(s1 == s2);
     * @endcode
     *
     * @endif
     */
    template<std::size_t N, typename T = char>
    struct FStr {
        /**
         * @if zh
         *
         * @brief 字符类型别名
         *
         * @elseif en
         *
         * @brief Character type alias
         *
         * @endif
         *
         * @public @memberof FStr
         */
        using value_type = T;

        /**
         * @if zh
         *
         * @brief 存储字符串值的数组
         *
         * @elseif en
         *
         * @brief Array storing string value
         *
         * @endif
         *
         * @public @memberof FStr
         */
        std::array<T, N - 1> value;

        /**
         * @if zh
         *
         * @brief 字符串长度(不包括null终止符)
         *
         * @elseif en
         *
         * @brief String length (excluding null terminator)
         *
         * @endif
         *
         * @public @memberof FStr
         */
        static constexpr std::size_t size = N - 1;

        /**
         * @if zh
         *
         * @brief C风格字符串构造函数
         * @param value C风格字符串数组
         *
         * @elseif en
         *
         * @brief C-style string constructor
         * @param value C-style string array
         *
         * @endif
         *
         * @public @memberof FStr
         */
        constexpr FStr(const T (&value)[N]);

        /**
         * @if zh
         *
         * @brief std::array构造函数
         * @param value std::array数组引用
         *
         * @elseif en
         *
         * @brief std::array constructor
         * @param value std::array reference
         *
         * @endif
         *
         * @public @memberof FStr
         */
        constexpr FStr(const std::array<T, N - 1>& value);

        /**
         * @if zh
         *
         * @brief 相等比较运算符(不同长度)
         * @tparam M 其他字符串的长度
         * @param other 要比较的FStr
         * @return 如果长度和内容都相等返回true
         *
         * @elseif en
         *
         * @brief Equality comparison operator (different lengths)
         * @tparam M Length of other string
         * @param other FStr to compare
         * @return true if both length and content are equal
         *
         * @endif
         *
         * @public @memberof FStr
         */
        template<std::size_t M>
        constexpr bool operator==(const FStr<M, T>& other) const;

        /**
         * @if zh
         *
         * @brief 相等比较运算符(相同长度)
         * @param other 要比较的FStr
         * @return 如果内容相等返回true
         *
         * @elseif en
         *
         * @brief Equality comparison operator (same length)
         * @param other FStr to compare
         * @return true if content is equal
         *
         * @endif
         *
         * @public @memberof FStr
         */
        constexpr bool operator==(const FStr& other) const;

        /**
         * @if zh
         *
         * @brief 与C风格字符串比较
         * @tparam M C字符串长度
         * @param other C风格字符串
         * @return 如果内容相等返回true
         *
         * @elseif en
         *
         * @brief Compare with C-style string
         * @tparam M C string length
         * @param other C-style string
         * @return true if content is equal
         *
         * @endif
         *
         * @public @memberof FStr
         */
        template<std::size_t M>
        constexpr bool operator==(const T (&other)[M]) const;

        /**
         * @if zh
         *
         * @brief 与外部字符串比较
         * @param other 外部字符串指针
         * @param len 字符串长度
         * @return 如果内容相等返回true
         *
         * @elseif en
         *
         * @brief Compare with external string
         * @param other External string pointer
         * @param len String length
         * @return true if content is equal
         *
         * @endif
         *
         * @public @memberof FStr
         */
        constexpr bool equals(const T* other, std::size_t len) const;

        /**
         * @if zh
         *
         * @brief 连接另一个FStr
         * @tparam M 其他字符串的长度
         * @param other 要连接的FStr
         * @return 连接后的新FStr
         *
         * @elseif en
         *
         * @brief Concatenate with another FStr
         * @tparam M Length of other string
         * @param other FStr to concatenate
         * @return New FStr after concatenation
         *
         * @endif
         *
         * @public @memberof FStr
         */
        template<std::size_t M>
        constexpr FStr<N + M + 1, T> concat(FStr<M, T>& other);

        /**
         * @if zh
         *
         * @brief 转换为std::basic_string
         * @return 转换后的std::basic_string
         *
         * @elseif en
         *
         * @brief Convert to std::basic_string
         * @return Converted std::basic_string
         *
         * @endif
         *
         * @public @memberof FStr
         */
        operator std::basic_string<T>() const;

        /**
         * @if zh
         *
         * @brief 转换为std::basic_string_view
         * @return 转换后的std::basic_string_view
         *
         * @elseif en
         *
         * @brief Convert to std::basic_string_view
         * @return Converted std::basic_string_view
         *
         * @endif
         *
         * @public @memberof FStr
         */
        operator std::basic_string_view<T>() const;
    };

    /**
     * @if zh
     *
     * @brief FStr类型别名
     * @tparam N 字符串长度
     *
     * @elseif en
     *
     * @brief FStr type alias
     * @tparam N String length
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<std::size_t N>
    using FString = FStr<N>;

    /**
     * @if zh
     *
     * @brief FStr类型检测特化
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief FStr type detection specialization
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename>
    struct isFStr : std::false_type {};

    template<std::size_t N, typename T>
    struct isFStr<FStr<N, T>> : std::true_type {};

    /**
     * @if zh
     *
     * @brief 编译期FStr连接结果变量模板
     * @details 在编译期计算两个FStr的连接结果
     * @tparam V1 第一个FStr
     * @tparam V2 第二个FStr
     *
     * @elseif en
     *
     * @brief Compile-time FStr concatenation result variable template
     * @details Computes the concatenation result of two FStr at compile-time
     * @tparam V1 First FStr
     * @tparam V2 Second FStr
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<FStr V1, FStr V2>
    static constexpr FStr<V1.size + V2.size + 1, typename decltype(V1)::value_type> fstr_concat_v =
        {[] {
            std::array<typename decltype(V1)::value_type, V1.size + V2.size> result{};

            for (std::size_t i = 0; i < V1.size; ++i) result[i] = V1.value[i];

            for (std::size_t i = 0; i < V2.size; ++i) result[V1.size + i] = V2.value[i];

            return result;
        }()};

    /**
     * @if zh
     *
     * @brief FStr类型检测变量模板
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief FStr type detection variable template
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    inline constexpr bool isFStr_v = isFStr<T>::value;

    /**
     * @if zh
     *
     * @brief FStr约束概念
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief FStr constraint concept
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    concept is_fs = isFStr_v<T>;

    /**
     * @if zh
     *
     * @brief FString类型检测特化
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief FString type detection specialization
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename>
    struct isFString : std::false_type {};

    template<std::size_t N>
    struct isFString<FStr<N>> : std::true_type {};

    /**
     * @if zh
     *
     * @brief FString类型检测变量模板
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief FString type detection variable template
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    inline constexpr bool isFString_v = isFString<T>::value;

    /**
     * @if zh
     *
     * @brief FString约束概念
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief FString constraint concept
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    concept is_fstring = isFString_v<T>;

    /**
     * @if zh
     *
     * @brief FStr用户自定义字面量运算符
     * @details 将字符串字面量转换为FStr类型
     * @tparam N 字符串长度
     * @param lit 字符串字面量
     * @return FStr类型实例
     *
     * @par 示例:
     * @code
     * constexpr auto s = "hello"_fs;
     * @endcode
     *
     * @elseif en
     *
     * @brief FStr user-defined literal operator
     * @details Converts string literal to FStr type
     * @tparam N String length
     * @param lit String literal
     * @return FStr type instance
     *
     * @par Example:
     * @code
     * constexpr auto s = "hello"_fs;
     * @endcode
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<FString V>
    constexpr auto operator""_fs();

}  // namespace stationeers

#include "fstr.inl"

#endif  // COMPILER_FSTR_HPP
