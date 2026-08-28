// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/parser/ast_adapter.hpp"
#include "ic10_compiler/pch/ast.hpp"

#include <variant>

namespace stationeers::ic10 {

    void initProgram(py::module_& m) {
        // 绑定 OperandType 枚举,AST JSON 序列化中使用数值表示
        // Bind OperandType enum, used in AST JSON serialization as numeric values
        py::enum_<OperandType>(m, "OperandType")
            .value("REG_TARGET", OperandType::REG_TARGET)
            .value("REG_OR_DEV", OperandType::REG_OR_DEV)
            .value("NUM_VALUE", OperandType::NUM_VALUE)
            .value("JUMP_LINE", OperandType::JUMP_LINE)
            .value("ADDRESS", OperandType::ADDRESS)
            .value("SLOT_IDX", OperandType::SLOT_IDX)
            .value("HARDWARE_ID", OperandType::HARDWARE_ID)
            .value("REAGENT_HASH", OperandType::REAGENT_HASH)
            .value("DEVICE_REF", OperandType::DEVICE_REF)
            .value("DEVICE_REF_STRICT", OperandType::DEVICE_REF_STRICT)
            .value("LOGIC_PROP", OperandType::LOGIC_PROP)
            .value("LOGIC_SLOT_PROP", OperandType::LOGIC_SLOT_PROP)
            .value("AGG_MODE", OperandType::AGG_MODE)
            .value("REAGENT_MODE", OperandType::REAGENT_MODE)
            .value("DEVICE_HASH", OperandType::DEVICE_HASH)
            .value("NAME_HASH", OperandType::NAME_HASH)
            .value("CONST_NUM", OperandType::CONST_NUM)
            .export_values();

        py::class_<Program>(m, "Program")
            .def(py::init<>())
            /**
             * @if zh
             * @brief 节点名称(静态属性)
             * @return "Program"
             * @elseif en
             * @brief Node name(static property)
             * @return "Program"
             * @endif
             */
            .def_property_readonly_static("nodeName", [](py::object) {
                return std::string(Program::nodeName);
            })
            /**
             * @if zh
             * @brief 获取语句列表
             * @details 将每个Statement通过JSON转换返回为Python字典列表
             * @return Python字典列表
             * @elseif en
             * @brief Get statement list
             * @details Converts each Statement to Python dict via JSON conversion
             * @return Python dict list
             * @endif
             */
            .def_property_readonly("statements", [](const Program& self) {
                py::list list;
                for (const auto& stmt : self.statements) {
                    // 使用std::visit调用各AST节点的toJSON方法
                    // Use std::visit to call toJSON on each AST node
                    std::string jsonStr = std::visit([](const auto& node) {
                        return node.toJSON();
                    }, stmt);
                    list.append(jsonLoads(jsonStr));
                }
                return list;
            })
            /**
             * @if zh
             * @brief 获取程序结束位置
             * @return Pos位置对象
             * @elseif en
             * @brief Get program end position
             * @return Pos position object
             * @endif
             */
            .def_property_readonly("end", [](const Program& self) {
                return self.position;
            })
            /**
             * @if zh
             * @brief 转换为字符串表示
             * @return 字符串
             * @elseif en
             * @brief Convert to string representation
             * @return String
             * @endif
             */
            .def("toString", &Program::toString)
            /**
             * @if zh
             * @brief 转换为JSON格式
             * @return JSON格式字符串
             * @elseif en
             * @brief Convert to JSON format
             * @return JSON format string
             * @endif
             */
            .def("toJSON", &Program::toJSON)
            .def("__repr__", [](const Program& self) {
                return self.toString();
            });
    }

}  // namespace stationeers::ic10
