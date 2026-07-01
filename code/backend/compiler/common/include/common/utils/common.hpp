// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file common.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:34
 * @if zh
 * @brief 公共工具函数与类型定义
 * @details 本文件提供编译器各模块共享的基础工具函数和类型约束,
 *        包括ASCII字符分类、Variant类型操作、枚举约束等常用工具。
 * @note 所有函数均遵循无异常规范(noexcept)
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Common utility functions and type definitions
 * @details This file provides foundational utility functions and type constraints shared across
 *        compiler modules, including ASCII character classification, Variant type operations,
 *        enum constraints, and other commonly used tools.
 * @note All functions follow noexcept specification
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_COMMON_HPP
#define COMPILER_COMMON_HPP
#pragma once

#include <variant>


namespace stationeers {

    /**
     * @if zh
     *
     * @brief 检查字符是否为ASCII空白字符
     * @details 判断给定字符是否为ASCII标准空白字符,包括:空格(0x20)、制表符(\\t)、换行符(\\n)、回车符(\\r)、垂直制表符(\\v)、翻页符(\\f)
     * @param c 待检测的字符(通常为unsigned char以避免符号扩展问题)
     * @return 是空白字符返回true,否则返回false
     * @note 该函数不处理Unicode空白字符,仅针对ASCII范围
     *
     * @elseif en
     *
     * @brief Check if a character is an ASCII whitespace character
     * @details Determines if the given character is an ASCII standard whitespace, including: space(0x20),
     *        tab(\\t), newline(\\n), carriage return(\\r), vertical tab(\\v), form feed(\\f)
     * @param c Character to check (typically unsigned char to avoid sign extension issues)
     * @return true if whitespace, false otherwise
     * @note This function does not handle Unicode whitespace characters, only ASCII range
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    bool isAsciiSpace(unsigned char c) noexcept;

    /**
     * @if zh
     *
     * @brief 检查字符是否为ASCII十进制数字
     * @details 判断给定字符是否为ASCII十进制数字字符('0'-'9')
     * @param c 待检测的字符
     * @return 是数字返回true,否则返回false
     * @note 不验证字符是否为数字的文本表示形式,仅做ASCII码范围检查
     *
     * @elseif en
     *
     * @brief Check if a character is an ASCII decimal digit
     * @details Determines if the given character is an ASCII decimal digit character ('0'-'9')
     * @param c Character to check
     * @return true if digit, false otherwise
     * @note Does not verify if the character is a textual representation of a digit,
     *       only performs ASCII code range checking
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    bool isAsciiDigit(unsigned char c) noexcept;

    /**
     * @if zh
     *
     * @brief 检查字符是否为ASCII十六进制数字
     * @details 判断给定字符是否为ASCII十六进制数字字符('0'-'9', 'A'-'F', 'a'-'f')
     * @param c 待检测的字符
     * @return 是十六进制数字返回true,否则返回false
     *
     * @elseif en
     *
     * @brief Check if a character is an ASCII hexadecimal digit
     * @details Determines if the given character is an ASCII hexadecimal digit character
     *        ('0'-'9', 'A'-'F', 'a'-'f')
     * @param c Character to check
     * @return true if hexadecimal digit, false otherwise
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    bool isAsciiHexDigit(unsigned char c) noexcept;

    /**
     * @if zh
     *
     * @brief 检查字符是否为ASCII二进制数字
     * @details 判断给定字符是否为ASCII二进制数字字符('0'或'1')
     * @param c 待检测的字符
     * @return 是二进制数字返回true,否则返回false
     *
     * @elseif en
     *
     * @brief Check if a character is an ASCII binary digit
     * @details Determines if the given character is an ASCII binary digit character ('0' or '1')
     * @param c Character to check
     * @return true if binary digit, false otherwise
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    bool isAsciiBinDigit(unsigned char c) noexcept;

    /**
     * @if zh
     *
     * @brief 检查字符是否为ASCII字母
     * @details 判断给定字符是否为ASCII字母字符('A'-'Z'或'a'-'z')
     * @param c 待检测的字符
     * @return 是字母返回true,否则返回false
     *
     * @elseif en
     *
     * @brief Check if a character is an ASCII alphabetic character
     * @details Determines if the given character is an ASCII alphabetic character ('A'-'Z' or 'a'-'z')
     * @param c Character to check
     * @return true if alphabetic, false otherwise
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    bool isAsciiAlpha(unsigned char c) noexcept;

    /**
     * @if zh
     *
     * @brief 检查字符是否为ASCII字母或数字
     * @details 判断给定字符是否为ASCII字母或数字字符(等同于isAsciiAlpha||isAsciiDigit)
     * @param c 待检测的字符
     * @return 是字母或数字返回true,否则返回false
     *
     * @elseif en
     *
     * @brief Check if a character is an ASCII alphanumeric character
     * @details Determines if the given character is an ASCII alphabetic or digit character
     *        (equivalent to isAsciiAlpha||isAsciiDigit)
     * @param c Character to check
     * @return true if alphanumeric, false otherwise
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    bool isAsciiAlnum(unsigned char c) noexcept;

    /**
     * @if zh
     *
     * @brief Variant类型检测特化
     * @details 当T为std::variant类型时,is_variant<T>特化派生自std::true_type,否则派生自std::false_type
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Variant type detection specialization
     * @details When T is std::variant type, is_variant<T> specialization derives from std::true_type,
     *        otherwise derives from std::false_type
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename>
    struct is_variant : std::false_type {};

    template<typename... Ts>
    struct is_variant<std::variant<Ts...>> : std::true_type {};

    /**
     * @if zh
     *
     * @brief 移除cv限定符和引用后的Variant类型检测变量
     * @details 通过is_variant检测T(移除const/volatile限定符和引用后)是否为Variant类型
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Variable template for cv-ref removed variant type detection
     * @details Uses is_variant to check if T (after removing cv-qualifiers and references) is a Variant type
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    inline constexpr bool is_variant_v =
        is_variant<std::remove_cv_t<std::remove_reference_t<T>>>::value;

    /**
     * @if zh
     *
     * @brief Variant类型约束概念
     * @details 用于约束模板参数必须为std::variant类型
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Variant type constraint concept
     * @details Used to constrain template parameters to be std::variant types
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    concept IsVariant = is_variant_v<T>;

    /**
     * @if zh
     *
     * @brief 窄类型到宽类型的Variant转换
     * @details 将 NarrowType variant 转换为 WideType variant,要求 WideType 包含 NarrowType 的所有类型
     * @tparam WideType 目标宽类型(必须为Variant)
     * @tparam NarrowType 源窄类型(必须为Variant)
     * @param value 要转换的variant值
     * @return 转换后的WideType variant
     *
     * @par 示例:
     * @code
     * std::variant<int, double> narrow = 42;
     * std::variant<int, double, std::string> wide = wide_cast<std::variant<int, double, std::string>>(std::move(narrow));
     * @endcode
     *
     * @elseif en
     *
     * @brief Narrow type to wide type Variant conversion
     * @details Converts NarrowType variant to WideType variant, requires WideType to contain all types in NarrowType
     * @tparam WideType Target wide type (must be a Variant)
     * @tparam NarrowType Source narrow type (must be a Variant)
     * @param value The variant value to convert
     * @return Converted WideType variant
     *
     * @par Example:
     * @code
     * std::variant<int, double> narrow = 42;
     * std::variant<int, double, std::string> wide = wide_cast<std::variant<int, double, std::string>>(std::move(narrow));
     * @endcode
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsVariant WideType, IsVariant NarrowType>
    WideType wide_cast(NarrowType&& value) {
        return std::visit(
            []<typename T>(T&& arg) -> WideType { return std::forward<T>(arg); },
            std::forward<NarrowType>(value)
        );
    }

    /**
     * @if zh
     *
     * @brief 对Variant值调用函数
     * @details 使用std::visit对Variant中的值调用给定的函数对象
     * @tparam T Variant类型
     * @tparam F 函数对象类型
     * @param value 要处理的Variant值
     * @param f 要应用的函数对象
     * @return 函数对象的返回值
     *
     * @par 示例:
     * @code
     * std::variant<int, double> v = 3.14;
     * auto result = call(v, [](auto x) { return x * 2; }); // result = 6.28
     * @endcode
     *
     * @elseif en
     *
     * @brief Call a function on a Variant value
     * @details Uses std::visit to call a given function object on the value in the Variant
     * @tparam T Variant type
     * @tparam F Function object type
     * @param value The Variant value to process
     * @param f The function object to apply
     * @return Return value of the function object
     *
     * @par Example:
     * @code
     * std::variant<int, double> v = 3.14;
     * auto result = call(v, [](auto x) { return x * 2; }); // result = 6.28
     * @endcode
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<IsVariant T, typename F>
    auto call(T&& value, F&& f) {
        return std::visit(
            [&]<typename U>(U&& arg) { return f(std::forward<U>(arg)); }, std::forward<T>(value)
        );
    }

    /**
     * @if zh
     *
     * @brief 检查Variant是否包含指定类型
     * @details 编译期检测Variant模板实例中是否包含类型T
     * @tparam Variant 要检查的Variant类型
     * @tparam T 要查找的类型
     * @return 如果Variant包含T则返回true,否则返回false
     *
     * @elseif en
     *
     * @brief Check if Variant contains a specified type
     * @details Compile-time check if type T is contained in the Variant template instantiation
     * @tparam Variant The Variant type to check
     * @tparam T The type to find
     * @return true if Variant contains T, false otherwise
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename Variant, typename T>
    inline constexpr bool variant_contains_v = []<typename... Ts>(std::variant<Ts...>*) {
        return (std::is_same_v<Ts, T> || ...);
    }(static_cast<Variant*>(nullptr));

    /**
     * @if zh
     *
     * @brief 枚举类型约束概念
     * @details 用于约束模板参数必须为枚举类型(std::is_enum_v<T>)
     * @tparam T 待检测的类型
     *
     * @elseif en
     *
     * @brief Enum type constraint concept
     * @details Used to constrain template parameters to be enum types (std::is_enum_v<T>)
     * @tparam T Type to check
     *
     * @endif
     *
     * @public @memberof stationeers
     */
    template<typename T>
    concept IsEnum = std::is_enum_v<T>;

}  // namespace stationeers

#endif  // COMPILER_COMMON_HPP
