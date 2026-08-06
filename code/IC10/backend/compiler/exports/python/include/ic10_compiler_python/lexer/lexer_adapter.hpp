// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file lexer_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief IC10词法分析器Python适配器
 * @details 提供IC10 Lexer类的pybind11绑定注册函数。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 lexer Python adapter
 * @details Provides pybind11 binding registration function for IC10 Lexer class.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_LEXER_ADAPTER_HPP
#define EXPORT_PYTHON_LEXER_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 注册Lexer类到Python模块
     * @details 绑定Lexer类,暴露构造函数(src, debug)、静态方法tokenize(src, debug)
     *          和实例方法scan()
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register Lexer class to Python module
     * @details Binds Lexer class, exposes constructor(src, debug), static method tokenize(src, debug)
     *          and instance method scan()
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initLexer(py::module_& m);

}  // namespace stationeers::ic10

#endif  // EXPORT_PYTHON_LEXER_ADAPTER_HPP
