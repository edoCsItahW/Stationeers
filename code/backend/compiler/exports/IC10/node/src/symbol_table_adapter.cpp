// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file symbol_table_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/22 21:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "symbol_table_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference SymbolTableAdapter::constructor;

    SymbolTableAdapter::SymbolTableAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object SymbolTableAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "SymbolTable", {InstanceMethod<&SymbolTableAdapter::toJSON>("toJSON")}
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("SymbolTable", func);

        return exports;
    }

    node::Value SymbolTableAdapter::to(node::Env env, const SymbolTable& self) {
        auto obj = constructor.New({});

        SymbolTableAdapter* wrapper = Unwrap(obj);

        wrapper->symbolTable_ = self;

        return obj;
    }

    SymbolTable SymbolTableAdapter::from(const node::Object& obj) {
        SymbolTableAdapter* wrapper = Unwrap(obj);

        return wrapper->symbolTable_;
    }

    node::Value SymbolTableAdapter::toJSON(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), symbolTable_.toJSON());
    }

}  // namespace stationeers::ic10
