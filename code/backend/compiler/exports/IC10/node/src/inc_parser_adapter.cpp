// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_parser_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "inc_parser_adapter.hpp"
#include "ast_adapter.hpp"
#include "token_adapter.hpp"

namespace stationeers::ic10 {

    IncParserAdapter::IncParserAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object IncParserAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "IncParser",
            {
                InstanceMethod<&IncParserAdapter::parseFull>("parseFull"),
                InstanceMethod<&IncParserAdapter::parseInc>("parseInc"),
                InstanceMethod<&IncParserAdapter::hasCache>("hasCache"),
                InstanceMethod<&IncParserAdapter::clear>("clear")
            }
        );
        auto constructor = std::make_unique<node::FunctionReference>();
        *constructor = node::Persistent(func);
        constructor->SuppressDestruct();
        (void)exports.Set("IncParser", func);
        return exports;
    }

    node::Value IncParserAdapter::parseFull(const node::CallbackInfo& info) {
        Arguments args(info);
        auto array = args.getWithCheck<node::Array>(0);

        std::vector<std::shared_ptr<Token>> tokens;
        for (auto&& [fst, snd] : array)
            tokens.push_back(std::make_shared<Token>(TokenAdapter::from(snd.AsValue().As<node::Object>())));

        auto result = parser_.parseFull(tokens);
        return resultToJS(info.Env(), result);
    }

    node::Value IncParserAdapter::parseInc(const node::CallbackInfo& info) {
        Arguments args(info);
        auto array = args.getWithCheck<node::Array>(0);

        std::vector<std::shared_ptr<Token>> tokens;
        for (auto&& [fst, snd] : array)
            tokens.push_back(std::make_shared<Token>(TokenAdapter::from(snd.AsValue().As<node::Object>())));

        auto changedStartLine = args.getWithCheck<node::Number>(1).Int32Value();

        auto result = parser_.parseInc(tokens, changedStartLine);
        return resultToJS(info.Env(), result);
    }

    node::Value IncParserAdapter::hasCache(const node::CallbackInfo& info) {
        return node::Boolean::New(info.Env(), parser_.hasCache());
    }

    void IncParserAdapter::clear(const node::CallbackInfo& info) {
        parser_.clear();
    }

    node::Object IncParserAdapter::resultToJS(node::Env env, const IncParserResult& result) {
        auto obj = node::Object::New(env);

        obj["ast"] = ProgramAdapter::to(env, result.ast);
        obj["incremental"] = node::Boolean::New(env, result.incremental);
        obj["reparsedStmts"] = node::Number::New(env, static_cast<double>(result.reparsedStmts));
        obj["affectedStmtStart"] = node::Number::New(env, static_cast<double>(result.affectedStmtStart));

        return obj;
    }

}  // namespace stationeers::ic10
