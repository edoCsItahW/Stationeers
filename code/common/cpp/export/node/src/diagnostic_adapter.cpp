// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file diagnostic_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/03
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_node/diagnostic_adapter.hpp"
#include "common_node/pos_adapter.hpp"

namespace stationeers {

    node::Object DiagnosticAdapter::init(node::Env env, node::Object exports) {
        // 当前无类需要注册，保留以便未来扩展
        (void)env;
        return exports;
    }

    const char* DiagnosticAdapter::levelToString(DiagnosticLevel level) {
        switch (level) {
            case DiagnosticLevel::Error:   return "error";
            case DiagnosticLevel::Warning: return "warning";
            case DiagnosticLevel::Info:    return "info";
        }
        return "unknown";
    }

    node::Object DiagnosticAdapter::to(node::Env env, const Diagnostic& diagnostic) {
        auto obj = node::Object::New(env);

        (void)obj.Set("level", node::String::New(env, levelToString(diagnostic.level)));
        (void)obj.Set("id", node::String::New(env, diagnostic.id));
        (void)obj.Set("start", PosAdapter::to(env, diagnostic.start));
        (void)obj.Set("end", PosAdapter::to(env, diagnostic.end));
        (void)obj.Set("message", node::String::New(env, diagnostic.message));

        return obj;
    }

}  // namespace stationeers
