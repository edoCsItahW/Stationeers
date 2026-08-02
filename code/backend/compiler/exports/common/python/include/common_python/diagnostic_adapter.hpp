// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file diagnostic_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/03
 * @if zh
 * @brief 诊断信息 Python 适配器
 * @details 提供诊断信息(Diagnostic)的pybind11绑定注册函数和转换工具函数。
 *          将C++ Diagnostic结构转换为Python字典,包含level/id/start/end/message字段。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Diagnostic Python adapter
 * @details Provides pybind11 binding registration function and conversion utility
 *          for diagnostic information. Converts C++ Diagnostic struct into a Python dict
 *          with level/id/start/end/message fields.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_PYTHON_DIAGNOSTIC_ADAPTER_HPP
#define EXPORT_PYTHON_DIAGNOSTIC_ADAPTER_HPP
#pragma once

#include "common/exception/diagnostic.hpp"
#include "common_python/base.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @brief 注册诊断相关类型到Python模块
     * @details 当前为空实现,保留以便未来扩展(如注册DiagnosticLevel枚举到Python层)
     *
     * @param m pybind11模块对象
     *
     * @elseif en
     *
     * @brief Register diagnostic types to Python module
     * @details Currently an empty implementation, reserved for future extension
     *          (e.g. registering DiagnosticLevel enum to Python layer)
     *
     * @param m pybind11 module object
     *
     * @endif
     */
    void initDiagnostic(py::module_& m);

    /**
     * @if zh
     *
     * @brief 将C++ Diagnostic转换为Python字典
     * @details 将诊断信息转换为包含以下字段的Python字典:
     *          - level: 诊断级别字符串("error" | "warning" | "info")
     *          - id: 诊断ID字符串(如"IEA1_2")
     *          - start: 起始位置(Pos对象)
     *          - end: 结束位置(Pos对象)
     *          - message: 诊断消息文本
     *
     * @param diagnostic C++ Diagnostic结构
     * @return 包含诊断信息的Python字典
     *
     * @elseif en
     *
     * @brief Convert C++ Diagnostic to Python dict
     * @details Converts diagnostic information into a Python dict with the following fields:
     *          - level: diagnostic level string ("error" | "warning" | "info")
     *          - id: diagnostic ID string (e.g. "IEA1_2")
     *          - start: start position (Pos object)
     *          - end: end position (Pos object)
     *          - message: diagnostic message text
     *
     * @param diagnostic C++ Diagnostic struct
     * @return Python dict containing diagnostic information
     *
     * @endif
     */
    py::dict diagnosticToPython(const Diagnostic& diagnostic);

}  // namespace stationeers

#endif  // EXPORT_PYTHON_DIAGNOSTIC_ADAPTER_HPP
