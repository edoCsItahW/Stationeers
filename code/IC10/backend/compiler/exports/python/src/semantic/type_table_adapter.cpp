// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file type_table_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/26
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/semantic/type_table_adapter.hpp"
#include "ic10_compiler/semantic/types.hpp"

namespace stationeers::ic10 {

    void initTypeTable(py::module_& m) {
        py::class_<TypeTable>(m, "TypeTable")
            .def(py::init<>())
            /**
             * @if zh
             * @brief 转换为JSON格式
             * @return JSON格式字符串
             * @elseif en
             * @brief Convert to JSON format
             * @return JSON format string
             * @endif
             */
            .def("toJSON", &TypeTable::toJSON);
    }

}  // namespace stationeers::ic10
