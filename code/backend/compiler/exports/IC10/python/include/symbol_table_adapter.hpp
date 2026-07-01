// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file symbol_table_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief IC10符号表Python适配器
 * @details 提供IC10 SymbolTable类的pybind11绑定注册函数。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 symbol table Python adapter
 * @details Provides pybind11 binding registration function for IC10 SymbolTable class.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_SYMBOL_TABLE_ADAPTER_HPP
#define EXPORT_PYTHON_SYMBOL_TABLE_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 注册SymbolTable类到Python模块
     * @details 绑定SymbolTable类,暴露toJSON方法
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register SymbolTable class to Python module
     * @details Binds SymbolTable class, exposes toJSON method
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initSymbolTable(py::module_& m);

}  // namespace stationeers::ic10

#endif  // EXPORT_PYTHON_SYMBOL_TABLE_ADAPTER_HPP
