// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file token_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 18:25
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "token_adapter.hpp"
#include "common/utils/enum_to_str.hpp"

namespace stationeers::ic10 {

    node::FunctionReference TokenAdapter::constructor;

    TokenAdapter::TokenAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {
        Arguments args(info);

        token_.type = static_cast<TokenType>(args.getWithCheck<node::Number>(0).Uint32Value());

        token_.pos = PosAdapter::from(args.getWithCheck<node::Object>(1));

        if (auto lexemeVal = args.get(2); lexemeVal.IsString())
            token_.lexeme = lexemeVal.As<node::String>().Utf8Value();

        if (auto catVal = args.get(3); catVal.IsNumber())
            token_.category = static_cast<TokenCategory>(catVal.As<node::Number>().Uint32Value());
    }

    node::Object TokenAdapter::init(node::Env env, node::Object exports) {
        node::Function func =
            DefineClass(
                env, "Token",
                {
// reslove MSVC ICE C1001
#ifdef _MSC_VER
                    InstanceAccessor("type", &TokenAdapter::getType, &TokenAdapter::setType),
                    InstanceAccessor("pos", &TokenAdapter::getPos, &TokenAdapter::setPos),
                    InstanceAccessor("lexeme", &TokenAdapter::getLexeme, &TokenAdapter::setLexeme),
                    InstanceAccessor("categ", &TokenAdapter::getCategory, &TokenAdapter::setCategory),
#else
                    InstanceAccessor<&TokenAdapter::getType, &TokenAdapter::setType>("type"),
                    InstanceAccessor<&TokenAdapter::getPos, &TokenAdapter::setPos>("pos"),
                    InstanceAccessor<&TokenAdapter::getLexeme, &TokenAdapter::setLexeme>("lexeme"),
                    InstanceAccessor<&TokenAdapter::getCategory, &TokenAdapter::setCategory>("categ"),
#endif
                    InstanceMethod("toString", &TokenAdapter::toString),
                    InstanceMethod("toJSON", &TokenAdapter::toJSON)
                }
            );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("Token", func);

        return exports;
    }

    node::Object TokenAdapter::to(node::Env env, const Token& self) {
        node::Object obj = constructor.New(
            {node::Number::New(env, static_cast<int>(self.type)), PosAdapter::to(env, self.pos),
             node::String::New(env, self.lexeme),
             node::Number::New(env, static_cast<int>(self.category))}
        );

        TokenAdapter* wrapper = Unwrap(obj);

        wrapper->token_ = self;

        return obj;
    }

    Token TokenAdapter::from(const node::Object& obj) {
        TokenAdapter* wrapper = Unwrap(obj);

        return wrapper->token_;
    }

    node::Value TokenAdapter::getType(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), static_cast<int>(token_.type));
    }

    void TokenAdapter::setType(const node::CallbackInfo& info, const node::Value& value) {
        Arguments args(info);

        token_.type = static_cast<TokenType>(args.getWithCheck<node::Number>(0).Uint32Value());
    }

    node::Value TokenAdapter::getPos(const node::CallbackInfo& info) {
        return PosAdapter::to(info.Env(), token_.pos);
    }

    void TokenAdapter::setPos(const node::CallbackInfo& info, const node::Value& value) {
        token_.pos = PosAdapter::from(value.As<node::Object>());
    }

    node::Value TokenAdapter::getLexeme(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), token_.lexeme);
    }

    void TokenAdapter::setLexeme(const node::CallbackInfo& info, const node::Value& value) {
        token_.lexeme = value.As<node::String>();
    }

    node::Value TokenAdapter::getCategory(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), static_cast<int>(token_.category));
    }

    void TokenAdapter::setCategory(const node::CallbackInfo& info, const node::Value& value) {
        token_.category = static_cast<TokenCategory>(value.As<node::Number>().Uint32Value());
    }

    node::Value TokenAdapter::toString(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), token_.toString());
    }

    node::Value TokenAdapter::toJSON(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), token_.toJSON());
    }

}  // namespace stationeers::ic10
