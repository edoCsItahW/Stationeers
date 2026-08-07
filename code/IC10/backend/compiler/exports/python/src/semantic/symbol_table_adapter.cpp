// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file symbol_table_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/semantic/symbol_table_adapter.hpp"
#include "ic10_compiler/semantic/semantic.hpp"
#include "ic10_compiler/semantic/types.hpp"

namespace stationeers::ic10 {

    void initSymbolTable(py::module_& m) {
        // 绑定 BasicType 枚举,Symbol JSON 序列化中使用数值表示
        // Bind BasicType enum, used in Symbol JSON serialization as numeric values
        py::enum_<BasicType>(m, "BasicType")
            .value("STRING", BasicType::STRING)
            .value("INTEGER", BasicType::INTEGER)
            .value("FLOAT", BasicType::FLOAT)
            .value("REGISTER", BasicType::REGISTER)
            .value("DEVICE", BasicType::DEVICE)
            .value("UNKNOWN", BasicType::UNKNOWN)
            .value("ENUM", BasicType::ENUM)
            .export_values();

        // 绑定 TypeCategory 枚举,Symbol JSON 序列化中使用数值表示
        // Bind TypeCategory enum, used in Symbol JSON serialization as numeric values
        py::enum_<TypeCategory>(m, "TypeCategory")
            .value("LABEL", TypeCategory::LABEL)
            .value("STR_CALL", TypeCategory::STR_CALL)
            .value("HASH_CALL", TypeCategory::HASH_CALL)
            .value("CONSTANT", TypeCategory::CONSTANT)
            .value("NUMBER", TypeCategory::NUMBER)
            .value("BASIC", TypeCategory::BASIC)
            .export_values();

        py::class_<SymbolTable>(m, "SymbolTable")
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
            .def("toJSON", &SymbolTable::toJSON);
    }

}  // namespace stationeers::ic10
