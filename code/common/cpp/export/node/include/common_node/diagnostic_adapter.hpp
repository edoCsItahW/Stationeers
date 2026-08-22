// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw

/**
 * @file diagnostic_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/03
 * @if zh
 * @brief 诊断信息 Node.js 适配器
 * @details 提供 C++ Diagnostic 结构与 Node.js 对象之间的转换适配器。
 *          将诊断信息（级别、ID、位置、消息）序列化为 JS 层可访问的普通对象。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Diagnostic Node.js adapter
 * @details Provides conversion adapter between C++ Diagnostic struct and Node.js objects.
 *          Serializes diagnostic information (level, id, position, message) into plain objects
 *          accessible from the JS layer.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_DIAGNOSTIC_ADAPTER_HPP
#define EXPORT_DIAGNOSTIC_ADAPTER_HPP
#pragma once

#include "base.hpp"
#include "common/exception/diagnostic.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @class DiagnosticAdapter
     * @brief 诊断信息适配器类
     * @details 提供 C++ Diagnostic 结构到 Node.js 对象的转换。
     *          诊断信息以普通 JS 对象形式暴露，包含以下字段：
     *          - level: 诊断级别字符串（"error" | "warning" | "info"）
     *          - id: 诊断 ID 字符串（如 "IEA1_2"）
     *          - start: 起始位置（Pos 对象）
     *          - end: 结束位置（Pos 对象）
     *          - message: 诊断消息文本
     *
     * @elseif en
     *
     * @class DiagnosticAdapter
     * @brief Diagnostic adapter class
     * @details Provides conversion from C++ Diagnostic struct to Node.js objects.
     *          Diagnostics are exposed as plain JS objects with the following fields:
     *          - level: diagnostic level string ("error" | "warning" | "info")
     *          - id: diagnostic ID string (e.g. "IEA1_2")
     *          - start: start position (Pos object)
     *          - end: end position (Pos object)
     *          - message: diagnostic message text
     *
     * @endif
     */
    class DiagnosticAdapter {
    public:
        /**
         * @if zh
         *
         * @brief 初始化诊断适配器
         * @details 当前为空实现，保留以便未来扩展（如注册诊断级别枚举到 JS 层）
         * @param env Node.js 环境
         * @param exports 导出对象
         * @return 传入的导出对象
         *
         * @elseif en
         *
         * @brief Initialize diagnostic adapter
         * @details Currently an empty implementation, reserved for future extension
         *          (e.g. registering diagnostic level enum to JS layer)
         * @param env Node.js environment
         * @param exports Exports object
         * @return The passed-in exports object
         *
         * @endif
         */
        static node::Object init(node::Env env, node::Object exports);

        /**
         * @if zh
         *
         * @brief 将 C++ Diagnostic 转换为 Node.js 对象
         * @param env Node.js 环境
         * @param diagnostic C++ Diagnostic 结构
         * @return 包含诊断信息的 Node.js 对象
         *
         * @elseif en
         *
         * @brief Convert C++ Diagnostic to Node.js object
         * @param env Node.js environment
         * @param diagnostic C++ Diagnostic struct
         * @return Node.js object containing diagnostic information
         *
         * @endif
         */
        static node::Object to(node::Env env, const Diagnostic& diagnostic);

    private:
        /**
         * @if zh
         * @brief 将 DiagnosticLevel 枚举转换为字符串
         * @elseif en
         * @brief Convert DiagnosticLevel enum to string
         * @endif
         */
        static const char* levelToString(DiagnosticLevel level) noexcept;
    };

}  // namespace stationeers

#endif  // EXPORT_DIAGNOSTIC_ADAPTER_HPP
