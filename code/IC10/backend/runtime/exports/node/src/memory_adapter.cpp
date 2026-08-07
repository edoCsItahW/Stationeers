// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file memory_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/11 10:23
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/memory_adapter.hpp"
#include "ic10_runtime_node/config_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference MemoryAdapter::constructor;

    MemoryAdapter::MemoryAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info)
        , memory_([&] -> Memory {
            Arguments args(info);

            return {ConfigAdapter::from(args.getWithCheck<node::Object>(0))};
        }()) {}

    node::Object MemoryAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Memory",
            {
#ifdef _MSC_VER
                InstanceAccessor("config", &MemoryAdapter::getConfig, &MemoryAdapter::setConfig),
                InstanceAccessor("sp", &MemoryAdapter::getSP, &MemoryAdapter::setSP),
#else
                InstanceAccessor<&MemoryAdapter::getConfig, &MemoryAdapter::setConfig>("config"),
                InstanceAccessor<&MemoryAdapter::getSP, &MemoryAdapter::setSP>("sp"),
#endif
                InstanceMethod<&MemoryAdapter::getReg>("getReg"),
                InstanceMethod<&MemoryAdapter::setReg>("setReg"),
                InstanceMethod<&MemoryAdapter::getStack>("getStack"),
                InstanceMethod<&MemoryAdapter::setStack>("setStack"),
                InstanceMethod<&MemoryAdapter::toJSON>("toJSON")
            }
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("Memory", func);

        return exports;
    }

    Memory MemoryAdapter::from(const node::Object& obj) {
        MemoryAdapter* wrapper = Unwrap(obj);

        return wrapper->memory_;
    }

    node::Object MemoryAdapter::to(node::Env env, const Memory& memory) {
        node::Object obj = constructor.New({ConfigAdapter::to(env, memory.cfg)});

        MemoryAdapter* wrapper = Unwrap(obj);

        wrapper->memory_ = memory;

        return obj;
    }

    node::Value MemoryAdapter::toJSON(const node::CallbackInfo& info) {
        return node::String::New(info.Env(), memory_.toJSON());
    }

    node::Value MemoryAdapter::getConfig(const node::CallbackInfo& info) {
        return ConfigAdapter::to(info.Env(), memory_.cfg);
    }

    void MemoryAdapter::setConfig(const node::CallbackInfo& info, const node::Value&) {
        Arguments args(info);

        memory_.cfg = ConfigAdapter::from(args.getWithCheck<node::Object>(0));
    }

    node::Value MemoryAdapter::getReg(const node::CallbackInfo& info) {
        Arguments args(info);

        auto name = args.getWithCheck<node::String>(0);

        return node::Number::New(info.Env(), memory_.getReg(name));
    }

    void MemoryAdapter::setReg(const node::CallbackInfo& info) {
        Arguments args(info);

        auto name = args.getWithCheck<node::String>(0);
        auto value = args.getWithCheck<node::Number>(1);

        memory_.setReg(name, value.Int64Value());
    }

    node::Value MemoryAdapter::getSP(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), memory_.getSP());
    }

    void MemoryAdapter::setSP(const node::CallbackInfo& info, const node::Value& value) {
        Arguments args(info);

        auto val = args.getWithCheck<node::Number>(0);

        memory_.setSP(val.Int64Value());
    }

    node::Value MemoryAdapter::getStack(const node::CallbackInfo& info) {
        Arguments args(info);

        auto idx = args.getWithCheck<node::Number>(0);

        return node::Number::New(info.Env(), memory_.getStack(idx.Int64Value()));
    }

    void MemoryAdapter::setStack(const node::CallbackInfo& info) {
        Arguments args(info);

        auto idx = args.getWithCheck<node::Number>(0);
        auto val = args.getWithCheck<node::Number>(1);

        memory_.setStack(idx.Int64Value(), val.Int64Value());
    }

}  // namespace stationeers::ic10
