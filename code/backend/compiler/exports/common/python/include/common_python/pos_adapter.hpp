// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file pos_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief Pos位置类Python适配器
 * @details 提供stationeers::Pos类的pybind11绑定注册函数。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Pos position class Python adapter
 * @details Provides pybind11 binding registration function for stationeers::Pos class.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_POS_ADAPTER_HPP
#define EXPORT_PYTHON_POS_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @brief 注册Pos类到Python模块
     * @details 将stationeers::Pos类绑定到pybind11模块,暴露line/column/offset只读属性
     *          和newline/next/move方法
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register Pos class to Python module
     * @details Binds stationeers::Pos class to pybind11 module, exposing line/column/offset
     *          readonly properties and newline/next/move methods
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initPos(py::module_& m);

}  // namespace stationeers

#endif  // EXPORT_PYTHON_POS_ADAPTER_HPP
