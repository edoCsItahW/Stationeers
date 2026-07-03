// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file enum_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/03
 * @if zh
 * @brief 枚举 Node.js 导出工具
 * @details 提供模板函数 exportEnum,通过 enum_to_str 的编译期反射将 C++ enum class
 *          导出为 Node.js 对象。导出对象包含每个枚举成员的 name: value 键值对,
 *          保证与 C++ 枚举定义自动同步,无需手工维护。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Enum Node.js export utility
 * @details Provides template function exportEnum that exports a C++ enum class
 *          to a Node.js object via compile-time reflection of enum_to_str.
 *          The exported object contains name: value pairs for each enum member,
 *          automatically synchronized with the C++ enum definition, no manual maintenance needed.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_ENUM_ADAPTER_HPP
#define EXPORT_ENUM_ADAPTER_HPP
#pragma once

#include "common/utils/enum_to_str.hpp"
#include "common_node/base.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @brief 将 C++ 枚举导出为 Node.js 对象
     * @details 利用 enum_to_str 的编译期反射(enumMax + enumToStr)遍历枚举所有值,
     *          构建一个 Node.js 对象,包含每个枚举成员的 name: value 键值对。
     *
     *          工作原理:
     *          1. enumMax<E>() 编译期计算枚举有效值数量 N
     *          2. 编译期遍历 0..N-1,用 enumToStr<static_cast<E>(I)>() 获取每个值的名字
     *          3. 构建 node::Object,设置 name -> integer value
     *
     * @tparam E 枚举类型(必须从 0 开始连续)
     * @param env Node.js 环境
     * @return 包含所有枚举成员的 Node.js 对象
     *
     * @par 使用示例:
     * @code
     * auto tokenType = exportEnum<ic10::TokenType>(env);
     * (void)exports.Set("TokenType", tokenType);
     * @endcode
     *
     * @elseif en
     *
     * @brief Export a C++ enum as a Node.js object
     * @details Uses compile-time reflection of enum_to_str (enumMax + enumToStr) to iterate
     *          over all enum values, building a Node.js object with name: value pairs
     *          for each enum member.
     *
     *          How it works:
     *          1. enumMax<E>() computes the count N of valid enum values at compile-time
     *          2. Iterates 0..N-1 at compile-time, getting each value's name via
     *             enumToStr<static_cast<E>(I)>>()
     *          3. Builds a node::Object, setting name -> integer value
     *
     * @tparam E Enum type (must be contiguous starting from 0)
     * @param env Node.js environment
     * @return Node.js object containing all enum members
     *
     * @par Usage example:
     * @code
     * auto tokenType = exportEnum<ic10::TokenType>(env);
     * (void)exports.Set("TokenType", tokenType);
     * @endcode
     *
     * @endif
     */
    template<typename E>
        requires std::is_enum_v<E>
    node::Object exportEnum(node::Env env) {
        // 编译期计算枚举有效值数量
        constexpr auto N = enumMax<E>();

        auto obj = node::Object::New(env);

        // 编译期展开:为每个枚举值设置 name -> integer
        [&]<std::size_t... I>(std::index_sequence<I...>) {
            ((void)obj.Set(
                 std::string(enumToStr<static_cast<E>(I)>()),
                 node::Number::New(env, static_cast<int>(static_cast<E>(I)))
             ), ...);
        }(std::make_index_sequence<N>{});

        return obj;
    }

}  // namespace stationeers

#endif  // EXPORT_ENUM_ADAPTER_HPP
