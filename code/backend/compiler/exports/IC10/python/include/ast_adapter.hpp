// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief IC10 AST Program节点Python适配器
 * @details 提供IC10 Program AST根节点的pybind11绑定注册函数。
 *          statements属性通过JSON转换返回Python字典列表。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 AST Program node Python adapter
 * @details Provides pybind11 binding registration function for IC10 Program AST root node.
 *          statements property returns Python dict list via JSON conversion.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_AST_ADAPTER_HPP
#define EXPORT_PYTHON_AST_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 注册Program类到Python模块
     * @details 绑定Program类,暴露nodeName静态属性、statements/end只读属性
     *          和toString/toJSON方法
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register Program class to Python module
     * @details Binds Program class, exposes nodeName static property, statements/end readonly properties
     *          and toString/toJSON methods
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initProgram(py::module_& m);

}  // namespace stationeers::ic10

#endif  // EXPORT_PYTHON_AST_ADAPTER_HPP
