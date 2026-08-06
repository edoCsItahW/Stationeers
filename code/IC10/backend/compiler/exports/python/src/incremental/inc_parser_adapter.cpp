// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_parser_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/incremental/inc_parser_adapter.hpp"
#include "ic10_compiler/incremental/inc_parser.hpp"

namespace stationeers::ic10 {

    void initIncParser(py::module_& m) {
        py::class_<IncParserResult>(m, "IncParserResult")
            .def(py::init<>())
            .def_readonly("ast", &IncParserResult::ast)
            .def_readonly("incremental", &IncParserResult::incremental)
            .def_readonly("reparsedStmts", &IncParserResult::reparsedStmts)
            .def_readonly("affectedStmtStart", &IncParserResult::affectedStmtStart);

        py::class_<IncParser>(m, "IncParser")
            .def(py::init<>())
            .def("parseFull", &IncParser::parseFull, py::arg("tokens"))
            .def("parseInc", &IncParser::parseInc,
                 py::arg("tokens"), py::arg("changedStartLine"))
            .def("hasCache", &IncParser::hasCache)
            .def("clear", &IncParser::clear);
    }

}  // namespace stationeers::ic10
