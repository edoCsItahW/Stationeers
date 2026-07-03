// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file token_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief IC10 Token Python适配器
 * @details 提供IC10 Token结构、TokenType枚举和TokenCategory枚举的pybind11绑定注册函数。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 Token Python adapter
 * @details Provides pybind11 binding registration functions for IC10 Token struct,
 *          TokenType enum and TokenCategory enum.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_TOKEN_ADAPTER_HPP
#define EXPORT_PYTHON_TOKEN_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 注册Token类和枚举到Python模块
     * @details 绑定Token类(使用shared_ptr持有)、TokenType枚举和TokenCategory枚举,
     *          暴露type/pos/lexeme/category属性和toString/toJSON方法
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register Token class and enums to Python module
     * @details Binds Token class(with shared_ptr holder), TokenType enum and TokenCategory enum,
     *          exposes type/pos/lexeme/category properties and toString/toJSON methods
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initToken(py::module_& m);

}  // namespace stationeers::ic10

#endif  // EXPORT_PYTHON_TOKEN_ADAPTER_HPP
