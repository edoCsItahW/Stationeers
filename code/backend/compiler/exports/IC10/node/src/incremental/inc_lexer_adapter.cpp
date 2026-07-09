// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_lexer_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_node/incremental/inc_lexer_adapter.hpp"
#include "ic10_node/lexer/token_adapter.hpp"

namespace stationeers::ic10 {

    IncLexerAdapter::IncLexerAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object IncLexerAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "IncLexer",
            {
                InstanceMethod<&IncLexerAdapter::tokenizeFull>("tokenizeFull"),
                InstanceMethod<&IncLexerAdapter::tokenizeInc>("tokenizeInc"),
                InstanceMethod<&IncLexerAdapter::hasCache>("hasCache"),
                InstanceMethod<&IncLexerAdapter::clear>("clear")
            }
        );
        auto constructor = std::make_unique<node::FunctionReference>();
        *constructor = node::Persistent(func);
        constructor->SuppressDestruct();
        (void)exports.Set("IncLexer", func);
        return exports;
    }

    node::Value IncLexerAdapter::tokenizeFull(const node::CallbackInfo& info) {
        Arguments args(info);
        auto source = args.getWithCheck<node::String>(0).Utf8Value();
        auto result = lexer_.tokenizeFull(source);
        return resultToJS(info.Env(), result);
    }

    node::Value IncLexerAdapter::tokenizeInc(const node::CallbackInfo& info) {
        Arguments args(info);
        auto source = args.getWithCheck<node::String>(0).Utf8Value();
        auto result = lexer_.tokenizeInc(source);
        return resultToJS(info.Env(), result);
    }

    node::Value IncLexerAdapter::hasCache(const node::CallbackInfo& info) {
        return node::Boolean::New(info.Env(), lexer_.hasCache());
    }

    void IncLexerAdapter::clear(const node::CallbackInfo& info) {
        lexer_.clear();
    }

    node::Object IncLexerAdapter::resultToJS(node::Env env, const IncLexerResult& result) {
        auto obj = node::Object::New(env);

        auto tokArr = node::Array::New(env, result.tokens.size());
        for (std::size_t i = 0; i < result.tokens.size(); ++i)
            tokArr[i] = TokenAdapter::to(env, *result.tokens[i]);
        obj["tokens"] = tokArr;

        obj["incremental"] = node::Boolean::New(env, result.incremental);
        obj["relexedLines"] = node::Number::New(env, static_cast<double>(result.relexedLines));
        obj["changedStartLine"] = node::Number::New(env, result.changedStartLine);
        obj["oldChangedEndLine"] = node::Number::New(env, result.oldChangedEndLine);
        obj["newChangedEndLine"] = node::Number::New(env, result.newChangedEndLine);

        return obj;
    }

}  // namespace stationeers::ic10
