// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file error_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief Error错误类Python适配器
 * @details 提供stationeers::Error类的pybind11绑定注册函数和错误转换工具函数。
 *        将C++错误类型(SyntaxError/TypeError/RedefinitionError/UndefinedSymbolError/RuntimeError)
 *        映射到对应的Python异常类型。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Error class Python adapter
 * @details Provides pybind11 binding registration function and error conversion utility
 *        for stationeers::Error class. Maps C++ error types
 *        (SyntaxError/TypeError/RedefinitionError/UndefinedSymbolError/RuntimeError)
 *        to corresponding Python exception types.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_ERROR_ADAPTER_HPP
#define EXPORT_PYTHON_ERROR_ADAPTER_HPP
#pragma once

#include "common/exception/error.hpp"
#include "common_python/base.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @brief 注册错误异常类型到Python模块
     * @details 将C++错误类型注册为Python异常类型:
     *          - SyntaxError -> Python内置SyntaxError
     *          - TypeError -> Python内置TypeError
     *          - RedefinitionError -> 自定义异常(继承RuntimeError)
     *          - UndefinedSymbolError -> 自定义异常(继承RuntimeError)
     *          - RuntimeError -> 自定义异常(继承RuntimeError)
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register error exception types to Python module
     * @details Registers C++ error types as Python exception types:
     *          - SyntaxError -> Python builtin SyntaxError
     *          - TypeError -> Python builtin TypeError
     *          - RedefinitionError -> custom exception(inherits RuntimeError)
     *          - UndefinedSymbolError -> custom exception(inherits RuntimeError)
     *          - RuntimeError -> custom exception(inherits RuntimeError)
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initError(py::module_& m);

    /**
     * @if zh
     *
     * @brief 将C++ Error转换为Python异常对象
     * @details 根据错误名称查找对应的Python异常类型,创建异常实例并设置start/end属性。
     *          用于Analyser的errors属性返回错误列表。
     *
     * @param error C++错误对象
     * @return Python异常对象
     *
     * @elseif en
     *
     * @brief Convert C++ Error to Python exception object
     * @details Looks up corresponding Python exception type by error name,
     *          creates exception instance and sets start/end attributes.
     *          Used by Analyser's errors property to return error list.
     *
     * @param error C++ error object
     * @return Python exception object
     *
     * @endif
     */
    py::object errorToPython(const Error& error);

}  // namespace stationeers

#endif  // EXPORT_PYTHON_ERROR_ADAPTER_HPP
