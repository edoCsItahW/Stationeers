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
 * @date 2026/07/22
 * @if zh
 * @brief 枚举 Python 导出工具
 * @details 提供模板函数 exportEnum,通过 enum_to_str 的编译期反射将 C++ enum class
 *          导出为 Python 枚举。导出对象包含每个枚举成员的 name: value 键值对,
 *          保证与 C++ 枚举定义自动同步,无需手工维护。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Enum Python export utility
 * @details Provides template function exportEnum that exports a C++ enum class
 *          to a Python enum via compile-time reflection of enum_to_str.
 *          The exported object contains name: value pairs for each enum member,
 *          automatically synchronized with the C++ enum definition, no manual maintenance needed.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_ENUM_ADAPTER_HPP
#define EXPORT_ENUM_ADAPTER_HPP
#pragma once

#include "common/utils/enum_to_str.hpp"
#include "common_python/base.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @brief 将 C++ 枚举导出为 Python 枚举
     * @details 利用 enum_to_str 的编译期反射(enumMax + enumToStr)遍历枚举所有值,
     *          构建一个 pybind11 枚举类型。
     *
     *          工作原理:
     *          1. enumMax<E>() 编译期计算枚举有效值数量 N
     *          2. 编译期遍历 0..N-1,用 enumToStr<static_cast<E>(I)>() 获取每个值的名字
     *          3. 通过 py::enum_ 导出枚举
     *
     * @tparam E 枚举类型(必须从 0 开始连续)
     * @param m Python 模块
     * @param name 枚举在 Python 中的名称
     *
     * @par 使用示例:
     * @code
     * exportEnum<ic10::BasicType>(m, "BasicType");
     * @endcode
     *
     * @elseif en
     *
     * @brief Export a C++ enum as a Python enum
     * @details Uses compile-time reflection of enum_to_str (enumMax + enumToStr) to iterate
     *          over all enum values, building a pybind11 enum type.
     *
     *          How it works:
     *          1. enumMax<E>() computes the count N of valid enum values at compile-time
     *          2. Iterates 0..N-1 at compile-time, getting each value's name via
     *             enumToStr<static_cast<E>(I)>>()
     *          3. Exports via py::enum_
     *
     * @tparam E Enum type (must be contiguous starting from 0)
     * @param m Python module
     * @param name Name of the enum in Python
     *
     * @par Usage example:
     * @code
     * exportEnum<ic10::BasicType>(m, "BasicType");
     * @endcode
     *
     * @endif
     */
    template<typename E>
        requires std::is_enum_v<E>
    void exportEnum(py::module_& m, const std::string& name) {
        // 编译期计算枚举有效值数量
        constexpr auto N = enumMax<E>();

        auto enum_obj = py::enum_<E>(m, name.c_str());

        // 编译期展开:为每个枚举值设置 name -> value
        [&]<std::size_t... I>(std::index_sequence<I...>) {
            ((void)enum_obj.value(
                std::string(enumToStr<static_cast<E>(I)>()).c_str(),
                static_cast<E>(I)
            ), ...);
        }(std::make_index_sequence<N>{});
    }

}  // namespace stationeers

#endif  // EXPORT_ENUM_ADAPTER_HPP