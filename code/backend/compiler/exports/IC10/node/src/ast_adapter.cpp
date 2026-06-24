// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/18 13:22
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ast_adapter.hpp"
#include "common_node/pos_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference ProgramAdapter::constructor;

    ProgramAdapter::ProgramAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object ProgramAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Program",
            {
// reslove MSVC ICE C1001
#ifdef _MSC_VER
                InstanceAccessor("statements", &ProgramAdapter::getStatements, nullptr),
                InstanceAccessor("end", &ProgramAdapter::end, nullptr),
#else
                InstanceAccessor<&ProgramAdapter::getStatements>("statements"),
                InstanceAccessor<&ProgramAdapter::end>("end"),
#endif
                StaticAccessor<&getNodeName>("nodeName"),
                InstanceMethod<&ProgramAdapter::toString>("toString"),
                InstanceMethod<&ProgramAdapter::toJSON>("toJSON")
            }
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("Program", func);

        return exports;
    }

    node::Object ProgramAdapter::to(node::Env env, const Program& self) {
        node::Object obj = constructor.New({});

        ProgramAdapter* wrapper = Unwrap(obj);

        wrapper->program_ = self;

        return obj;
    }

    Program ProgramAdapter::from(const node::Object& obj) {
        ProgramAdapter* wrapper = Unwrap(obj);

        return wrapper->program_;
    }

    node::Value ProgramAdapter::getNodeName(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), std::string(Program::nodeName));
    }

    node::Value ProgramAdapter::getStatements(const node::CallbackInfo& info) {
        return stationeers::to(info.Env(), program_.statements);
    }

    node::Value ProgramAdapter::end(const node::CallbackInfo& info) {
        return PosAdapter::to(info.Env(), program_.position);
    }

    node::Value ProgramAdapter::toString(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), program_.toString());
    }

    node::Value ProgramAdapter::toJSON(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), program_.toJSON());
    }

}  // namespace stationeers::ic10
