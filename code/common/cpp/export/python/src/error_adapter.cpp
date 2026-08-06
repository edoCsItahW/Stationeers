// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file error_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_python/error_adapter.hpp"
#include "common_python/pos_adapter.hpp"

#include <unordered_map>

namespace stationeers {

    namespace {
        /// Python异常类型注册表,按错误名称索引
        std::unordered_map<std::string, py::object> exceptionRegistry;
    }  // namespace

    void initError(py::module_& m) {
        // 注册自定义异常类型(继承RuntimeError)
        // Register custom exception types(inheriting RuntimeError)
        auto redefExc = py::exception<struct RedefinitionErrorTag>(m, "RedefinitionError", PyExc_RuntimeError);
        auto undefExc = py::exception<struct UndefinedSymbolErrorTag>(m, "UndefinedSymbolError", PyExc_RuntimeError);
        auto runtimeExc = py::exception<struct RuntimeErrorTag>(m, "IC10RuntimeError", PyExc_RuntimeError);

        // 内建异常类型通过builtins模块获取(不能使用py::globals(),因为内建类型不在全局命名空间中)
        // Builtin exception types are accessed via the builtins module (not py::globals(),
        // since builtins are not in the global namespace dict)
        auto builtins = py::module_::import("builtins");

        exceptionRegistry["SyntaxError"] = builtins.attr("SyntaxError");
        exceptionRegistry["TypeError"] = builtins.attr("TypeError");
        exceptionRegistry["RedefinitionError"] = redefExc;
        exceptionRegistry["UndefinedSymbolError"] = undefExc;
        exceptionRegistry["RuntimeError"] = runtimeExc;
    }

    py::object errorToPython(const Error& error) {
        py::object excType;
        auto name = std::string(error.getName());

        if (auto it = exceptionRegistry.find(name); it != exceptionRegistry.end()) {
            excType = it->second;
        } else {
            // 回退到IC10RuntimeError
            // Fallback to IC10RuntimeError
            excType = exceptionRegistry["RuntimeError"];
        }

        // 创建异常实例
        // Create exception instance
        py::object instance = excType(error.message());

        // 设置start/end位置属性
        // Set start/end position attributes
        // 使用Pos的Python绑定创建位置对象
        // Use Pos Python binding to create position objects
        instance.attr("start") = py::cast(error.getStart());
        instance.attr("end") = py::cast(error.getEnd());
        instance.attr("name") = name;

        return instance;
    }

}  // namespace stationeers
