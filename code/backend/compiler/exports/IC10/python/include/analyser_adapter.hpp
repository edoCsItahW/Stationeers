// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file analyser_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief IC10语义分析器Python适配器
 * @details 提供IC10 Analyser类的pybind11绑定注册函数。
 *          analyse和visit方法通过阻塞协程实现同步调用。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 semantic analyser Python adapter
 * @details Provides pybind11 binding registration function for IC10 Analyser class.
 *          analyse and visit methods implement synchronous calls via blocking coroutine.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_ANALYSER_ADAPTER_HPP
#define EXPORT_PYTHON_ANALYSER_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 注册Analyser类到Python模块
     * @details 绑定Analyser类,暴露构造函数、静态方法analyse(program)、实例方法visit(program)
     *          和只读属性symbolTable/errors
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register Analyser class to Python module
     * @details Binds Analyser class, exposes constructor, static method analyse(program),
     *          instance method visit(program) and readonly properties symbolTable/errors
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initAnalyser(py::module_& m);

}  // namespace stationeers::ic10

#endif  // EXPORT_PYTHON_ANALYSER_ADAPTER_HPP
