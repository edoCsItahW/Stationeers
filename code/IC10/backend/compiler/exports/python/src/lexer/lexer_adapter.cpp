// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file lexer_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/lexer/lexer_adapter.hpp"
#include "common_python/diagnostic_adapter.hpp"
#include "ic10_compiler/lexer/lexer.hpp"

namespace stationeers::ic10 {

    void initLexer(py::module_& m) {
        py::class_<Lexer>(m, "Lexer")
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
             * @param src 源代码字符串
             * @param debug 是否启用调试模式
             * @elseif en
             * @brief Constructor
             * @param src Source code string
             * @param debug Whether to enable debug mode
             * @endif
             */
            .def(py::init<std::string_view, bool>(), py::arg("src"), py::arg("debug") = false)
            /**
             * @if zh
             * @brief 扫描源代码生成标记序列
             * @return Token列表
             * @elseif en
             * @brief Scan source code to generate token sequence
             * @return Token list
             * @endif
             */
            .def("scan", &Lexer::scan)
            /**
             * @if zh
             * @brief 静态词法分析入口
             * @param src 源代码字符串
             * @param debug 是否启用调试模式
             * @return Token列表
             * @elseif en
             * @brief Static lexical analysis entry point
             * @param src Source code string
             * @param debug Whether to enable debug mode
             * @return Token list
             * @endif
             */
            .def_static("tokenize", &Lexer::tokenize,
                        py::arg("src"), py::arg("debug") = false)
            /**
             * @if zh
             * @brief 获取诊断列表
             * @details 返回词法分析过程中收集到的所有诊断信息(包含错误、警告、提示)。
             *          每个诊断信息为字典,包含level/id/start/end/message字段。
             * @return Python字典列表
             * @elseif en
             * @brief Get diagnostic list
             * @details Returns all diagnostics collected during lexical analysis
             *          (including errors, warnings, info).
             *          Each diagnostic is a dict with level/id/start/end/message fields.
             * @return List of Python dicts
             * @endif
             */
            .def_property_readonly("diagnostics", [](Lexer& self) {
                py::list list;
                for (const auto& diag : self.getDiagnostics()) {
                    list.append(diagnosticToPython(diag));
                }
                return list;
            });
    }

}  // namespace stationeers::ic10
