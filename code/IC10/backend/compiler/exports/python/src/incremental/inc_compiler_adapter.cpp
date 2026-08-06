// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_compiler_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/incremental/inc_compiler_adapter.hpp"
#include "ic10_compiler/incremental/inc_compiler.hpp"

namespace stationeers::ic10 {

    void initIncCompiler(py::module_& m) {
        py::class_<IncCompileResult>(m, "IncCompileResult")
            .def(py::init<>())
            .def_readonly("tokens", &IncCompileResult::tokens)
            .def_readonly("ast", &IncCompileResult::ast)
            .def_readonly("incremental", &IncCompileResult::incremental)
            .def_readonly("relexedLines", &IncCompileResult::relexedLines)
            .def_readonly("reparsedStmts", &IncCompileResult::reparsedStmts);

        py::class_<IncCompiler>(m, "IncCompiler")
            .def(py::init<>())
            .def("compileFull", &IncCompiler::compileFull, py::arg("source"))
            .def("compileInc", &IncCompiler::compileInc, py::arg("source"))
            .def("hasCache", &IncCompiler::hasCache)
            .def("clear", &IncCompiler::clear);
    }

}  // namespace stationeers::ic10
