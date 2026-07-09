// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_compiler_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_node/incremental/inc_compiler_adapter.hpp"
#include "ic10_node/lexer/token_adapter.hpp"
#include "ic10_node/parser/ast_adapter.hpp"

namespace stationeers::ic10 {

    IncCompilerAdapter::IncCompilerAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object IncCompilerAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "IncCompiler",
            {
                InstanceMethod<&IncCompilerAdapter::compileFull>("compileFull"),
                InstanceMethod<&IncCompilerAdapter::compileInc>("compileInc"),
                InstanceMethod<&IncCompilerAdapter::hasCache>("hasCache"),
                InstanceMethod<&IncCompilerAdapter::clear>("clear")
            }
        );
        auto constructor = std::make_unique<node::FunctionReference>();
        *constructor = node::Persistent(func);
        constructor->SuppressDestruct();
        (void)exports.Set("IncCompiler", func);
        return exports;
    }

    node::Value IncCompilerAdapter::compileFull(const node::CallbackInfo& info) {
        Arguments args(info);
        auto source = args.getWithCheck<node::String>(0).Utf8Value();
        auto result = compiler_.compileFull(source);
        return resultToJS(info.Env(), result);
    }

    node::Value IncCompilerAdapter::compileInc(const node::CallbackInfo& info) {
        Arguments args(info);
        auto source = args.getWithCheck<node::String>(0).Utf8Value();
        auto result = compiler_.compileInc(source);
        return resultToJS(info.Env(), result);
    }

    node::Value IncCompilerAdapter::hasCache(const node::CallbackInfo& info) {
        return node::Boolean::New(info.Env(), compiler_.hasCache());
    }

    void IncCompilerAdapter::clear(const node::CallbackInfo& info) {
        compiler_.clear();
    }

    node::Object IncCompilerAdapter::resultToJS(node::Env env, const IncCompileResult& result) {
        auto obj = node::Object::New(env);

        auto tokArr = node::Array::New(env, result.tokens.size());
        for (std::size_t i = 0; i < result.tokens.size(); ++i)
            tokArr[i] = TokenAdapter::to(env, *result.tokens[i]);
        obj["tokens"] = tokArr;

        obj["ast"] = ProgramAdapter::to(env, result.ast);
        obj["incremental"] = node::Boolean::New(env, result.incremental);
        obj["relexedLines"] = node::Number::New(env, static_cast<double>(result.relexedLines));
        obj["reparsedStmts"] = node::Number::New(env, static_cast<double>(result.reparsedStmts));

        return obj;
    }

}  // namespace stationeers::ic10
