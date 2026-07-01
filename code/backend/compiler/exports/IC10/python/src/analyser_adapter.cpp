// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "analyser_adapter.hpp"
#include "common_python/base.hpp"
#include "common_python/error_adapter.hpp"
#include "ic10/semantic/analyser.hpp"

namespace stationeers::ic10 {

    void initAnalyser(py::module_& m) {
        // 显式指定shared_ptr作为holder类型,因为Analyser继承自enable_shared_from_this
        // Explicitly specify shared_ptr as holder type, since Analyser inherits from enable_shared_from_this
        py::class_<Analyser, std::shared_ptr<Analyser>>(m, "Analyser")
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
             * @brief 静态语义分析入口
             * @details 阻塞执行协程任务,释放GIL以允许其他Python线程运行。
             *          注意:静态方法不会填充实例的symbolTable和errors。
             * @param program 待分析的Program对象
             * @elseif en
             * @brief Static semantic analysis entry point
             * @details Blocks executing coroutine task, releases GIL to allow other Python threads.
             *          Note: static method does not populate instance's symbolTable and errors.
             * @param program Program object to analyze
             * @endif
             */
            .def_static("analyse", [](const Program& program) {
                runCoroutine(Analyser::analyse(program));
            }, py::arg("program"))
            /**
             * @if zh
             * @brief 访问程序节点进行语义分析
             * @details 阻塞执行协程任务,释放GIL以允许其他Python线程运行。
             *          分析完成后可通过symbolTable和errors属性获取结果。
             * @param program 待分析的Program对象
             * @elseif en
             * @brief Visit program node for semantic analysis
             * @details Blocks executing coroutine task, releases GIL to allow other Python threads.
             *          After analysis, results can be accessed via symbolTable and errors properties.
             * @param program Program object to analyze
             * @endif
             */
            .def("visit", [](Analyser& self, const Program& program) {
                runCoroutine(self.visit(program));
            }, py::arg("program"))
            /**
             * @if zh
             * @brief 获取符号表
             * @return SymbolTable对象
             * @elseif en
             * @brief Get symbol table
             * @return SymbolTable object
             * @endif
             */
            .def_property_readonly("symbolTable", [](Analyser& self) {
                return self.getSymbolTable();
            })
            /**
             * @if zh
             * @brief 获取错误列表
             * @return Python异常对象列表
             * @elseif en
             * @brief Get error list
             * @return Python exception object list
             * @endif
             */
            .def_property_readonly("errors", [](Analyser& self) {
                py::list list;
                for (const auto& err : self.getErrors()) {
                    list.append(errorToPython(err));
                }
                return list;
            });
    }

}  // namespace stationeers::ic10
