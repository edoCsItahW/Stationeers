// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file build.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief common模块Python导出初始化
 * @details 提供common模块在Python环境中的初始化入口函数。
 *        该函数注册Pos和Error等基础类型的绑定。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief common module Python export initialization
 * @details Provides initialization entry function for common module in Python environment.
 *        This function registers bindings for basic types like Pos and Error.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_BUILD_HPP
#define EXPORT_PYTHON_BUILD_HPP
#pragma once

#include "common_python/base.hpp"

/**
 * @if zh
 *
 * @brief common模块初始化函数
 * @details 注册Pos类和Error异常类型到pybind11模块,由IC10 Python模块的
 *          PYBIND11_MODULE入口调用
 *
 * @param m pybind11模块对象
 *
 * @elseif en
 *
 * @brief common module initialization function
 * @details Registers Pos class and Error exception types to pybind11 module,
 *          called by IC10 Python module's PYBIND11_MODULE entry
 *
 * @param m pybind11 module object
 *
 * @endif
 */
void commonInit(py::module_& m);

#endif  // EXPORT_PYTHON_BUILD_HPP
