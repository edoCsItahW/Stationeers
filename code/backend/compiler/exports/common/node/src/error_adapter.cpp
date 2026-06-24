// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file error_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/23 11:36
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_node/error_adapter.hpp"
#include "common_node/pos_adapter.hpp"

namespace stationeers {

    std::unordered_map<std::string, node::FunctionReference> ErrorAdapter::constructors;

    node::Object ErrorAdapter::init(node::Env env, node::Object exports) {
        registerErrorClass<SyntaxError>(env, exports);
        registerErrorClass<TypeError>(env, exports);
        registerErrorClass<RedefinitionError>(env, exports);
        registerErrorClass<UndefinedSymbolError>(env, exports);
        registerErrorClass<RuntimeError>(env, exports);

        return exports;
    }

    node::Value ErrorAdapter::to(node::Env env, const Error& error) {
        if (auto it = constructors.find(error.getName().data()); it != constructors.end()) {
            node::Function constructor = it->second.Value();

            return constructor.New({
                node::String::New(env, error.message()),
                PosAdapter::to(env, error.getStart()),
                PosAdapter::to(env, error.getEnd()),
            });
        }

        return node::Error::New(env, error.message().data()).Value();
    }

    template<typename Derived>
    void ErrorAdapter::registerErrorClass(node::Env env, node::Object exports) {
        std::string_view name = Derived::name;

        auto errorProto =
            env.Global().Get("Error").As<node::Object>().Get("prototype").As<node::Object>();
        auto setProtoOf = env.Global()
                              .Get("Object")
                              .As<node::Object>()
                              .Get("setPrototypeOf")
                              .As<node::Function>();

        auto customProto = node::Object::New(env);

        (void)setProtoOf.Call({customProto, errorProto});

        node::Function constructor = node::Function::New(
            env,
            [customProto, setProtoOf](const node::CallbackInfo& info) -> node::Value {
                Arguments args(info);

                auto errorCtor = info.Env().Global().Get("Error").As<node::Function>();
                auto errorObj =
                    errorCtor.New({args.getWithCheck<node::String>(0)}).As<node::Object>();

                (void)setProtoOf.Call({errorObj, customProto});

                (void)errorObj.Set("start", args.getWithCheck<node::Object>(1));
                (void)errorObj.Set("end", args.getWithCheck<node::Object>(2));

                return errorObj;
            },
           name.data()
        );

        (void)constructor.Set("prototype", customProto);

        (void)exports.Set(name.data(), constructor);

        constructors.emplace(name.data(), node::Persistent(constructor));
    }

    template void ErrorAdapter::registerErrorClass<SyntaxError>(node::Env, node::Object);
    template void ErrorAdapter::registerErrorClass<TypeError>(node::Env, node::Object);
    template void ErrorAdapter::registerErrorClass<RedefinitionError>(node::Env, node::Object);
    template void ErrorAdapter::registerErrorClass<UndefinedSymbolError>(node::Env, node::Object);
    template void ErrorAdapter::registerErrorClass<RuntimeError>(node::Env, node::Object);

}  // namespace stationeers
