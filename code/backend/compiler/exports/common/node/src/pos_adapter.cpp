// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file pos_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/17 13:26
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_node/pos_adapter.hpp"

namespace stationeers {

    node::FunctionReference PosAdapter::constructor;

    node::Object PosAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Pos",
            {
#ifdef _MSC_VER
                InstanceAccessor("line", &PosAdapter::getLine, nullptr),
                InstanceAccessor("column", &PosAdapter::getColumn, nullptr),
                InstanceAccessor("offset", &PosAdapter::getOffset, nullptr),
#else
                InstanceAccessor<&PosAdapter::getLine>("line"),
                InstanceAccessor<&PosAdapter::getColumn>("column"),
                InstanceAccessor<&PosAdapter::getOffset>("offset"),
#endif
                InstanceMethod("newline", &PosAdapter::newline),
                InstanceMethod("next", &PosAdapter::next), InstanceMethod("move", &PosAdapter::move)
            }
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("Pos", func);

        return exports;
    }

    Pos PosAdapter::from(const node::Object& obj) {
        PosAdapter* wrapper = Unwrap(obj);

        return wrapper->pos_;
    }

    node::Object PosAdapter::to(node::Env env, const Pos& self) {
        node::Object obj = constructor.New({});

        PosAdapter* wrapper = Unwrap(obj);

        wrapper->pos_ = self;

        return obj;
    }

    PosAdapter::PosAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Value PosAdapter::getLine(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), pos_.line());
    }

    node::Value PosAdapter::getColumn(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), pos_.column());
    }

    node::Value PosAdapter::getOffset(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), pos_.offset());
    }

    void PosAdapter::newline(const node::CallbackInfo& info) { pos_.newline(); }

    void PosAdapter::next(const node::CallbackInfo& info) { pos_.next(); }

    void PosAdapter::move(const node::CallbackInfo& info) {
        Arguments args(info);

        pos_.move(args.getWithCheck<node::Number>(0).Uint32Value());
    }

}  // namespace stationeers
