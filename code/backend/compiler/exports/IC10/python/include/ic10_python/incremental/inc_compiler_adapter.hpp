// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_compiler_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @if zh
 * @brief 增量编译器Python适配器
 * @details 将IncCompiler绑定为Python类
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Incremental compiler Python adapter
 * @details Binds IncCompiler as a Python class
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_IC10_PYTHON_INC_COMPILER_ADAPTER_HPP
#define COMPILER_IC10_PYTHON_INC_COMPILER_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"

namespace stationeers::ic10 {

    void initIncCompiler(py::module_& m);

}  // namespace stationeers::ic10

#endif  // COMPILER_IC10_PYTHON_INC_COMPILER_ADAPTER_HPP
