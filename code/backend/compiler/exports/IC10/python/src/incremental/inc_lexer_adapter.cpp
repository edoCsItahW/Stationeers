// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_lexer_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_python/incremental/inc_lexer_adapter.hpp"
#include "ic10/incremental/inc_lexer.hpp"

namespace stationeers::ic10 {

    void initIncLexer(py::module_& m) {
        py::class_<IncLexerResult>(m, "IncLexerResult")
            .def(py::init<>())
            .def_readonly("tokens", &IncLexerResult::tokens)
            .def_readonly("incremental", &IncLexerResult::incremental)
            .def_readonly("relexedLines", &IncLexerResult::relexedLines)
            .def_readonly("changedStartLine", &IncLexerResult::changedStartLine)
            .def_readonly("oldChangedEndLine", &IncLexerResult::oldChangedEndLine)
            .def_readonly("newChangedEndLine", &IncLexerResult::newChangedEndLine);

        py::class_<IncLexer>(m, "IncLexer")
            .def(py::init<>())
            .def("tokenizeFull", &IncLexer::tokenizeFull, py::arg("source"))
            .def("tokenizeInc", &IncLexer::tokenizeInc, py::arg("newSource"))
            .def("hasCache", &IncLexer::hasCache)
            .def("clear", &IncLexer::clear);
    }

}  // namespace stationeers::ic10
