// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/18 22:21
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_node/parser/parser_adapter.hpp"
#include "ic10_compiler_node/lexer/token_adapter.hpp"
#include "ic10_compiler_node/parser/ast_adapter.hpp"
#include "common_node/diagnostic_adapter.hpp"


namespace stationeers::ic10 {

    ParserAdapter::ParserAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {
        Arguments args(info);

        auto array = args.getWithCheck<node::Array>(0);
        std::vector<std::shared_ptr<Token>> tokens;

        for (auto&& [fst, snd] : array)
            tokens.push_back(std::make_shared<Token>(TokenAdapter::from(snd.AsValue().As<node::Object>())));

        std::tuple<decltype(tokens), bool> params;

        std::get<0>(params) = tokens;

        if (auto debug = args.get(1); debug.IsBoolean())
            std::get<1>(params) = debug.As<node::Boolean>();

        std::apply([this](auto&... tp) { parser_ = Parser(tp...); }, params);
    }

    node::Object ParserAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Parser",
            {
// reslove MSVC ICE C1001
#ifdef _MSC_VER
                InstanceAccessor("diagnostics", &ParserAdapter::getDiagnostics, nullptr),
#else
                InstanceAccessor<&ParserAdapter::getDiagnostics>("diagnostics"),
#endif
                InstanceMethod<&ParserAdapter::parse>("parse"), StaticMethod<&parsing>("parsing")
            }
        );

        auto constructor = std::make_unique<node::FunctionReference>();

        *constructor = node::Persistent(func);

        constructor->SuppressDestruct();

        (void)exports.Set("Parser", func);

        return exports;
    }

    node::Value ParserAdapter::parse(const node::CallbackInfo& info) {
        return ProgramAdapter::to(info.Env(), parser_.parse());
    }

    node::Value ParserAdapter::parsing(const node::CallbackInfo& info) {
        Arguments args(info);

        auto array = args.getWithCheck<node::Array>(0);
        std::vector<std::shared_ptr<Token>> tokens;

        for (auto&& [fst, sec] : array)
            tokens.push_back(std::make_shared<Token>(TokenAdapter::from(fst.As<node::Object>())));

        std::tuple<decltype(tokens), bool> params;

        std::get<0>(params) = tokens;

        if (auto debug = args.get(1); debug.IsBoolean())
            std::get<1>(params) = debug.As<node::Boolean>();

        auto result = std::apply([](auto&... tp) { return Parser::parsing(tp...); }, params);

        return ProgramAdapter::to(info.Env(), result);
    }

    node::Value ParserAdapter::getDiagnostics(const node::CallbackInfo& info) {
        auto diagnostics = parser_.getDiagnostics();

        auto size   = diagnostics.size();
        auto result = node::Array::New(info.Env(), size);

        for (std::size_t i = 0; i < size; i++) result[i] = DiagnosticAdapter::to(info.Env(), diagnostics[i]);

        return result;
    }

}  // namespace stationeers::ic10
