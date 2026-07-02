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
#include "ast_adapter.hpp"
#include "ic10/parser/ast/ast.hpp"

#include <variant>

namespace stationeers::ic10 {

    void initProgram(py::module_& m) {
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
