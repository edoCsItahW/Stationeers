// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 21:57
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/context_adapter.hpp"
#include "ic10_compiler_node/parser/ast_adapter.hpp"
#include "ic10_compiler_node/semantic/symbol_table_adapter.hpp"
#include "ic10_runtime/context/context.hpp"
#include "ic10_runtime_node/config_adapter.hpp"
#include "ic10_runtime_node/manager_adapter.hpp"
#include "ic10_runtime_node/memory_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference ContextAdapter::constructor;

    ContextAdapter::ContextAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info)
        , context_([&] -> Context {
            Arguments args(info);

            auto program = ProgramAdapter::from(args.getWithCheck<node::Object>(0));

            auto symbols = SymbolTableAdapter::from(args.getWithCheck<node::Object>(1));

            auto cfg = ConfigAdapter::from(args.getWithCheck<node::Object>(2));

            return {program, symbols, cfg};
        }()) {}

    node::Object ContextAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Context",
            {
#ifdef _MSC_VER
                InstanceAccessor("pc", &ContextAdapter::getPC, &ContextAdapter::setPC),
                InstanceAccessor("memory", &ContextAdapter::getMemory, nullptr),
                InstanceAccessor("config", &ContextAdapter::getConfig, &ContextAdapter::setConfig),
                InstanceAccessor("manager", &ContextAdapter::getManager, nullptr)
#else
                InstanceAccessor<&ContextAdapter::getPC, &ContextAdapter::setPC>("pc"),
                InstanceAccessor<&ContextAdapter::getMemory>("memory"),
                InstanceAccessor<&ContextAdapter::getConfig, &ContextAdapter::setConfig>("config"),
                InstanceAccessor<&ContextAdapter::getManager>("manager")
#endif
            }
        );

        constructor = node::Persistent(func);

        constructor.SuppressDestruct();

        (void)exports.Set("Context", func);

        return exports;
    }

    Context ContextAdapter::from(const node::Object& obj) {
        ContextAdapter* wrapper = Unwrap(obj);

        return std::move(wrapper->context_);
    }

    node::Object ContextAdapter::to(node::Env env, const Context& ctx) {
        node::Object obj = constructor.New(
            {ProgramAdapter::to(env, *ctx.program), SymbolTableAdapter::to(env, *ctx.symbols),
             ConfigAdapter::to(env, ctx.cfg)}
        );

        ContextAdapter* wrapper = Unwrap(obj);

        wrapper->context_.memory = ctx.memory;
        wrapper->context_.setPC(ctx.getPC());

        if (ctx.halted()) wrapper->context_.halt();

        return obj;
    }

    node::Value ContextAdapter::getPC(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), context_.getPC());
    }

    void ContextAdapter::setPC(const node::CallbackInfo& info, const node::Value& value) {
        Arguments args(info);

        context_.setPC(args.getWithCheck<node::Number>(0).Uint32Value());
    }

    node::Value ContextAdapter::getMemory(const node::CallbackInfo& info) {
        return MemoryAdapter::to(info.Env(), context_.memory);
    }

    node::Value ContextAdapter::getConfig(const node::CallbackInfo& info) {
        return ConfigAdapter::to(info.Env(), context_.cfg);
    }

    void ContextAdapter::setConfig(const node::CallbackInfo& info, const node::Value&) {
        Arguments args(info);

        context_.cfg = ConfigAdapter::from(args.getWithCheck<node::Object>(0));
    }

    node::Value ContextAdapter::getManager(const node::CallbackInfo& info) {
        return ManagerAdapter::to(info.Env(), context_.manager);
    }

}  // namespace stationeers::ic10
