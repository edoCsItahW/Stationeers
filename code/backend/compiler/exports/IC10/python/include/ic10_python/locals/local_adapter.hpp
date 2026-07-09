// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file local_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief IC10本地化Python适配器
 * @details 提供IC10Local本地化管理器的pybind11绑定注册函数。
 *        支持zh-hans(简体中文)和en-us(美式英语)两种语言。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 localization Python adapter
 * @details Provides pybind11 binding registration function for IC10Local localization manager.
 *        Supports zh-hans(Simplified Chinese) and en-us(American English) languages.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_LOCAL_ADAPTER_HPP
#define EXPORT_PYTHON_LOCAL_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 注册IC10Local到Python模块
     * @details 注册zh-hans和en-us语言并设置默认语言为en-us,
     *          暴露IC10Local类及静态方法setLanguage
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register IC10Local to Python module
     * @details Registers zh-hans and en-us languages and sets default language to en-us,
     *          exposes IC10Local class with static method setLanguage
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initLocal(py::module_& m);

}  // namespace stationeers::ic10

#endif  // EXPORT_PYTHON_LOCAL_ADAPTER_HPP
