// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file linker_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/21
 * @if zh
 * @brief IC10链接器Python适配器实现
 * @details 实现Linker类的pybind11绑定,支持添加编译单元、执行链接、获取诊断信息和单元信息
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 linker Python adapter implementation
 * @details Implements pybind11 bindings for Linker class, supports adding compilation units,
 *          performing linking, and retrieving diagnostics and unit information
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#include "ic10_python/link/linker_adapter.hpp"
#include "common_python/diagnostic_adapter.hpp"
#include "ic10/link/linker.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/semantic/semantic.hpp"

namespace stationeers::ic10 {

    void initLinker(py::module_& m) {
        py::class_<Linker::UnitInfo>(m, "UnitInfo")
            .def_readonly("path", &Linker::UnitInfo::path)
            .def_property_readonly("diagnostics", [](const Linker::UnitInfo& self) {
                if (self.diagnostics) {
                    py::list result;
                    for (const auto& diag : *self.diagnostics) {
                        result.append(stationeers::diagnosticToPython(diag));
                    }
                    return result;
                }
                return py::list();
            });

        py::class_<Linker>(m, "Linker")
            .def(py::init<>())
            .def(
                "addUnit", py::overload_cast<Program>(&Linker::addUnit), py::arg("program"),
                R"(
                    @if zh
                    @brief 添加编译单元(Program对象)
                    @param program 要链接的Program对象
                    @elseif en
                    @brief Add compilation unit (Program object)
                    @param program Program object to link
                    @endif
                 )"
            )
            .def(
                "addUnit", py::overload_cast<std::string_view>(&Linker::addUnit), py::arg("source"),
                R"(
                    @if zh
                    @brief 添加编译单元(源代码字符串)
                    @param source IC10源代码字符串
                    @elseif en
                    @brief Add compilation unit (source code string)
                    @param source IC10 source code string
                    @endif
                 )"
            )
            .def(
                "addUnit", py::overload_cast<Program, const std::string&>(&Linker::addUnit),
                py::arg("program"), py::arg("path"),
                R"(
                    @if zh
                    @brief 添加编译单元(带路径)
                    @param program 要链接的Program对象
                    @param path 源文件路径
                    @elseif en
                    @brief Add compilation unit with path
                    @param program Program object to link
                    @param path Source file path
                    @endif
                 )"
            )
            .def(
                "addUnit",
                py::overload_cast<std::string_view, const std::string&>(&Linker::addUnit),
                py::arg("source"), py::arg("path"),
                R"(
                    @if zh
                    @brief 添加编译单元(源代码字符串带路径)
                    @param source IC10源代码字符串
                    @param path 源文件路径
                    @elseif en
                    @brief Add compilation unit with path (source string)
                    @param source IC10 source code string
                    @param path Source file path
                    @endif
                 )"
            )
            .def(
                "link", &Linker::link,
                R"(
                    @if zh
                    @brief 执行链接操作
                    @return 合并后的SymbolTable对象
                    @elseif en
                    @brief Perform linking
                    @return Merged SymbolTable object
                    @endif
                 )"
            )
            .def_property_readonly(
                "diagnostics",
                [](const Linker& self) {
                    py::list result;
                    for (const auto& diag : self.getDiagnostics())
                        result.append(stationeers::diagnosticToPython(diag));
                    return result;
                },
                R"(
                @if zh
                @brief 获取所有诊断信息
                @return 诊断信息列表,每个元素是包含level、id、start、end、message字段的字典
                @elseif en
                @brief Get all diagnostics
                @return List of diagnostics, each is a dict with level, id, start, end, message fields
                @endif
            )"
            )
            .def_property_readonly(
                "units",
                [](const Linker& self) {
                    py::list result;
                    for (const auto& unit : self.getUnits()) result.append(py::cast(unit));
                    return result;
                },
                R"(
                @if zh
                @brief 获取所有编译单元信息
                @return UnitInfo对象列表
                @elseif en
                @brief Get all compilation units
                @return List of UnitInfo objects
                @endif
            )"
            );
    }

}  // namespace stationeers::ic10
