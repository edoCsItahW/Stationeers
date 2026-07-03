// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file diagnostic_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/03
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_python/diagnostic_adapter.hpp"

namespace stationeers {

    namespace {
        /// 将DiagnosticLevel枚举转换为字符串
        const char* levelToString(ic10::DiagnosticLevel level) {
            switch (level) {
                case ic10::DiagnosticLevel::Error:   return "error";
                case ic10::DiagnosticLevel::Warning: return "warning";
                case ic10::DiagnosticLevel::Info:    return "info";
            }
            return "unknown";
        }
    }  // namespace

    void initDiagnostic(py::module_& m) {
        // 当前无类型需要注册,保留以便未来扩展
        (void)m;
    }

    py::dict diagnosticToPython(const ic10::Diagnostic& diagnostic) {
        py::dict d;
        d["level"] = py::str(levelToString(diagnostic.level));
        d["id"] = py::str(diagnostic.id);
        d["start"] = py::cast(diagnostic.start);
        d["end"] = py::cast(diagnostic.end);
        d["message"] = py::str(diagnostic.message);
        return d;
    }

}  // namespace stationeers
