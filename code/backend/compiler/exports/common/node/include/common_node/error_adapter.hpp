// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file error_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/22 22:28
 * @if zh
 * @brief 错误类型Node.js适配器
 * @details 提供C++错误类型与Node.js Error对象之间的转换适配器。
 *        支持各种错误类型的注册和转换,实现错误类型在JS层的正确表示。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Error type Node.js adapter
 * @details Provides conversion adapter between C++ error types and Node.js Error objects.
 *        Supports registration and conversion of various error types, ensuring correct
 *        representation of error types in the JS layer.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_ERROR_ADAPTER_HPP
#define EXPORT_ERROR_ADAPTER_HPP
#pragma once

#include "base.hpp"
#include "common/exception/error.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @class ErrorAdapter
     * @brief 错误适配器类
     * @details 提供C++错误类型与Node.js Error对象之间的双向转换。
     *          使用注册机制管理各种错误类型的构造函数。
     *
     * @elseif en
     *
     * @class ErrorAdapter
     * @brief Error adapter class
     * @details Provides bidirectional conversion between C++ error types and Node.js Error objects.
     *          Uses registration mechanism to manage constructors for various error types.
     *
     * @endif
     */
    class ErrorAdapter {
    public:
        /**
         * @if zh
         *
         * @brief 初始化错误适配器
         * @param env Node.js环境
         * @param exports 导出对象
         * @return 配置后的导出对象
         *
         * @elseif en
         *
         * @brief Initialize error adapter
         * @param env Node.js environment
         * @param exports Exports object
         * @return Configured exports object
         *
         * @endif
         */
        static node::Object init(node::Env env, node::Object exports);

        /**
         * @if zh
         *
         * @brief 将C++错误转换为Node.js错误对象
         * @param env Node.js环境
         * @param error C++错误对象
         * @return Node.js错误对象
         *
         * @elseif en
         *
         * @brief Convert C++ error to Node.js error object
         * @param env Node.js environment
         * @param error C++ error object
         * @return Node.js error object
         *
         * @endif
         */
        static node::Value to(node::Env env, const Error& error);

    private:
        /**
         * @if zh
         * @brief 已注册的错误类型构造函数映射表
         * @elseif en
         * @brief Map of registered error type constructors
         * @endif
         */
        static std::unordered_map<std::string, node::FunctionReference> constructors;

        /**
         * @if zh
         *
         * @brief 注册错误类
         * @tparam Derived 派生错误类型
         * @param env Node.js环境
         * @param exports 导出对象
         *
         * @elseif en
         *
         * @brief Register error class
         * @tparam Derived Derived error type
         * @param env Node.js environment
         * @param exports Exports object
         *
         * @endif
         */
        template<typename Derived>
        static void registerErrorClass(node::Env env, node::Object exports);
    };

    extern template void ErrorAdapter::registerErrorClass<SyntaxError>(node::Env, node::Object);
    extern template void ErrorAdapter::registerErrorClass<TypeError>(node::Env, node::Object);
    extern template void ErrorAdapter::registerErrorClass<RedefinitionError>(node::Env, node::Object);
    extern template void ErrorAdapter::registerErrorClass<UndefinedSymbolError>(node::Env, node::Object);
    extern template void ErrorAdapter::registerErrorClass<RuntimeError>(node::Env, node::Object);

}  // namespace stationeers

#endif  // EXPORT_ERROR_ADAPTER_HPP
