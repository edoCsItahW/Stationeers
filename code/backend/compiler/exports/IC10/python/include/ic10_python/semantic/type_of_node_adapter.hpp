// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file type_of_node_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22
 * @if zh
 * @brief AST节点类型映射 Python 适配器
 * @details 导出 TypeOfNode 映射对象，将节点类型名映射到对应的 BasicType 和 TypeCategory。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief AST node type mapping Python adapter
 * @details Exports TypeOfNode mapping object, mapping node type names to corresponding BasicType and TypeCategory.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_TYPE_OF_NODE_ADAPTER_HPP
#define EXPORT_TYPE_OF_NODE_ADAPTER_HPP
#pragma once

#include "common_python/base.hpp"
#include "ic10/semantic/types.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @brief 导出 TypeOfNode 映射对象到 Python
     * @param scope Python 模块作用域
     *
     * @elseif en
     *
     * @brief Export TypeOfNode mapping object to Python
     * @param scope Python module scope
     *
     * @endif
     */
    inline void exportTypeOfNode(pybind11::module_& scope) {
        // 创建 TypeOfNode 字典
        pybind11::dict typeOfNode;

        // Integer -> { kind: INTEGER, category: NUMBER }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::INTEGER);
            entry["category"] = static_cast<int>(TypeCategory::NUMBER);
            typeOfNode["Integer"] = entry;
        }

        // Float -> { kind: FLOAT, category: NUMBER }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::FLOAT);
            entry["category"] = static_cast<int>(TypeCategory::NUMBER);
            typeOfNode["Float"] = entry;
        }

        // String -> { kind: STRING, category: BASIC }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::STRING);
            entry["category"] = static_cast<int>(TypeCategory::BASIC);
            typeOfNode["String"] = entry;
        }

        // StrCall -> { kind: INTEGER, category: STR_CALL }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::INTEGER);
            entry["category"] = static_cast<int>(TypeCategory::STR_CALL);
            typeOfNode["StrCall"] = entry;
        }

        // HashCall -> { kind: INTEGER, category: HASH_CALL }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::INTEGER);
            entry["category"] = static_cast<int>(TypeCategory::HASH_CALL);
            typeOfNode["HashCall"] = entry;
        }

        // Register -> { kind: REGISTER, category: BASIC }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::REGISTER);
            entry["category"] = static_cast<int>(TypeCategory::BASIC);
            typeOfNode["Register"] = entry;
        }

        // Device -> { kind: DEVICE, category: BASIC }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::DEVICE);
            entry["category"] = static_cast<int>(TypeCategory::BASIC);
            typeOfNode["Device"] = entry;
        }

        // LabelDef -> { kind: INTEGER, category: LABEL }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::INTEGER);
            entry["category"] = static_cast<int>(TypeCategory::LABEL);
            typeOfNode["LabelDef"] = entry;
        }

        // ErrorNode -> { kind: UNKNOWN, category: BASIC }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::UNKNOWN);
            entry["category"] = static_cast<int>(TypeCategory::BASIC);
            typeOfNode["ErrorNode"] = entry;
        }

        // Constant -> { kind: UNKNOWN, category: CONSTANT }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::UNKNOWN);
            entry["category"] = static_cast<int>(TypeCategory::CONSTANT);
            typeOfNode["Constant"] = entry;
        }

        // BinaryNumber -> { kind: INTEGER, category: NUMBER }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::INTEGER);
            entry["category"] = static_cast<int>(TypeCategory::NUMBER);
            typeOfNode["BinaryNumber"] = entry;
        }

        // HexNumber -> { kind: INTEGER, category: NUMBER }
        {
            pybind11::dict entry;
            entry["kind"] = static_cast<int>(BasicType::INTEGER);
            entry["category"] = static_cast<int>(TypeCategory::NUMBER);
            typeOfNode["HexNumber"] = entry;
        }

        scope.attr("TypeOfNode") = typeOfNode;
    }

}  // namespace stationeers::ic10

#endif  // EXPORT_TYPE_OF_NODE_ADAPTER_HPP