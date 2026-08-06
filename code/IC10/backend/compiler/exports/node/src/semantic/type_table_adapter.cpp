// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file type_table_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/26
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_node/semantic/type_table_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference TypeTableAdapter::constructor;

    TypeTableAdapter::TypeTableAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object TypeTableAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "TypeTable", {InstanceMethod<&TypeTableAdapter::toJSON>("toJSON")}
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("TypeTable", func);

        return exports;
    }

    node::Value TypeTableAdapter::to(node::Env env, const TypeTable& self) {
        auto obj = constructor.New({});

        TypeTableAdapter* wrapper = Unwrap(obj);

        wrapper->typeTable_ = self;

        return obj;
    }

    TypeTable TypeTableAdapter::from(const node::Object& obj) {
        TypeTableAdapter* wrapper = Unwrap(obj);

        return wrapper->typeTable_;
    }

    node::Value TypeTableAdapter::toJSON(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), typeTable_.toJSON());
    }

}  // namespace stationeers::ic10
