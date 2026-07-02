// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "parser_adapter.hpp"
#include "ic10/parser/parser.hpp"

namespace stationeers::ic10 {

    void initParser(py::module_& m) {
        py::class_<Parser>(m, "Parser")
            /**
             * @if zh
             * @brief 默认构造函数
             * @elseif en
             * @brief Default constructor
             * @endif
             */
            .def(py::init<>())
            /**
             * @if zh
             * @brief 构造函数
             * @param tokens 词法标记列表
             * @param debug 是否启用调试模式
             * @elseif en
             * @brief Constructor
             * @param tokens Token list
             * @param debug Whether to enable debug mode
             * @endif
             */
            .def(py::init<const std::vector<std::shared_ptr<Token>>&, bool>(),
                 py::arg("tokens"), py::arg("debug") = false)
            /**
             * @if zh
             * @brief 解析整个程序
             * @return 解析后的Program节点
             * @elseif en
             * @brief Parse the whole program
             * @return Parsed Program node
             * @endif
             */
            .def("parse", &Parser::parse)
            /**
             * @if zh
             * @brief 静态解析入口
             * @param tokens 词法标记列表
             * @param debug 是否启用调试模式
             * @return 解析后的Program节点
             * @elseif en
             * @brief Static parse entry point
             * @param tokens Token list
             * @param debug Whether to enable debug mode
             * @return Parsed Program node
             * @endif
             */
            .def_static("parsing", &Parser::parsing,
                        py::arg("tokens"), py::arg("debug") = false);
    }

}  // namespace stationeers::ic10
