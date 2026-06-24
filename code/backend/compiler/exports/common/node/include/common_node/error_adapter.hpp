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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_ERROR_ADAPTER_HPP
#define EXPORT_ERROR_ADAPTER_HPP
#pragma once

#include "base.hpp"
#include "common/exception/error.hpp"

namespace stationeers {

    class ErrorAdapter {
    public:
        static node::Object init(node::Env env, node::Object exports);

        static node::Value to(node::Env env, const Error& error);

    private:
        static std::unordered_map<std::string, node::FunctionReference> constructors;

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
