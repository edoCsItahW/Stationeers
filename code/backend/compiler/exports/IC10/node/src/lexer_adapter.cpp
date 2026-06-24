// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file lexer_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 17:38
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "lexer_adapter.hpp"
#include "common/exception/debug.hpp"
#include "token_adapter.hpp"


namespace stationeers::ic10 {

    node::Object LexerAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Lexer",
            {StaticMethod("tokenize", &tokenize), InstanceMethod("scan", &LexerAdapter::scan)}
        );

        node::FunctionReference* constructor = new node::FunctionReference();

        *constructor = node::Persistent(func);

        constructor->SuppressDestruct();

        (void)exports.Set("Lexer", func);

        return exports;
    }

    LexerAdapter::LexerAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {

        Arguments args(info);

        source_ = args.getWithCheck<node::String>(0).Utf8Value();

        if (auto optArg = args.get(1); optArg.IsBoolean())
            lexer_ = Lexer(source_, optArg);
        else
            lexer_ = Lexer(source_);
    }

    node::Value LexerAdapter::tokenize(const node::CallbackInfo& info) {
        Arguments args(info);

        auto tokens = Lexer::tokenize(args.getWithCheck<node::String>(0).Utf8Value());

        auto size   = tokens.size();
        auto result = node::Array::New(info.Env(), size);

        for (std::size_t i = 0; i < size; ++i) result[i] = TokenAdapter::to(info.Env(), *tokens[i]);

        return result;
    }

    node::Value LexerAdapter::scan(const node::CallbackInfo& info) {
        auto tokens = lexer_.scan();

        auto size   = tokens.size();
        auto result = node::Array::New(info.Env(), size);

        for (std::size_t i = 0; i < size; ++i) result[i] = TokenAdapter::to(info.Env(), *tokens[i]);

        return result;
    }

}  // namespace stationeers::ic10
