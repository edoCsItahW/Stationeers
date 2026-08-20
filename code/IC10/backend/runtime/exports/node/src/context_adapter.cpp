// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context_adapter.cpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/08/14
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/context_adapter.hpp"
#include "ic10_compiler_node/parser/ast_adapter.hpp"
#include "ic10_compiler_node/semantic/symbol_table_adapter.hpp"
#include "ic10_runtime_node/config_adapter.hpp"
#include "ic10_runtime_node/manager_adapter.hpp"
#include "ic10_runtime_node/memory_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference ContextAdapter::constructor;

    ContextAdapter::ContextAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info)
        , context_([&]() -> Context {
            // 0 参数构造：用于 toExisting 引用模式，等待外部设置 ctxPtr_
            if (info.Length() == 0) return Context{Program{}, SymbolTable{}, Config{}};
            Arguments args(info);
            auto program = ProgramAdapter::from(args.getWithCheck<node::Object>(0));
            auto symbols = SymbolTableAdapter::from(args.getWithCheck<node::Object>(1));
            auto cfg = info.Length() >= 3 ? ConfigAdapter::from(args.getWithCheck<node::Object>(2))
                                          : Config{};
            return {std::move(program), std::move(symbols), cfg};
        }()) {}

    node::Object ContextAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Context",
            {
#ifdef _MSC_VER
                InstanceAccessor("pc", &ContextAdapter::getPC, &ContextAdapter::setPC),
                InstanceAccessor("config", &ContextAdapter::getConfig, &ContextAdapter::setConfig),
#else
                InstanceAccessor<&ContextAdapter::getPC, &ContextAdapter::setPC>("pc"),
                InstanceAccessor<&ContextAdapter::getConfig, &ContextAdapter::setConfig>("config"),
#endif
                InstanceAccessor<&ContextAdapter::getMemory>("memory"),
                InstanceAccessor<&ContextAdapter::getManager>("manager"),
                InstanceAccessor<&ContextAdapter::getHalted>("halted"),
                InstanceAccessor<&ContextAdapter::getIsSleeping>("isSleeping"),
                InstanceMethod<&ContextAdapter::halt>("halt"),
                InstanceMethod<&ContextAdapter::sleep>("sleep"),
            }
        );
        constructor = node::Persistent(func);
        constructor.SuppressDestruct();
        (void)exports.Set("Context", func);
        return exports;
    }

    Context ContextAdapter::from(const node::Object& obj) {
        ContextAdapter* wrapper = Unwrap(obj);
        // Context 只有 move ctor，没有 copy ctor
        return std::move(wrapper->ctx());
    }

    node::Object ContextAdapter::toExisting(node::Env env, Context* ctx) {
        node::Object obj        = constructor.New({});
        ContextAdapter* wrapper = Unwrap(obj);
        wrapper->ctxPtr_        = ctx;
        return obj;
    }

    node::Object ContextAdapter::to(node::Env env, const Context& ctx) {
        node::Object obj = constructor.New(
            {ProgramAdapter::to(env, ctx.program), SymbolTableAdapter::to(env, ctx.symbols),
             ConfigAdapter::to(env, ctx.cfg)}
        );
        ContextAdapter* wrapper  = Unwrap(obj);
        wrapper->context_.memory = ctx.memory;
        // Manager 持有 unique_ptr，不可拷贝。to() 是值包装场景，manager 保持为空即可。
        wrapper->context_.setPC(ctx.getPC());
        if (ctx.halted()) wrapper->context_.halt();
        return obj;
    }

    node::Value ContextAdapter::getPC(const node::CallbackInfo& info) {
        return node::Number::New(info.Env(), static_cast<double>(ctx().getPC()));
    }

    void ContextAdapter::setPC(const node::CallbackInfo& info, const node::Value& value) {
        Arguments args(info);
        ctx().setPC(static_cast<std::size_t>(args.getWithCheck<node::Number>(0).Int64Value()));
    }

    node::Value ContextAdapter::getMemory(const node::CallbackInfo& info) {
        node::Env env = info.Env();
        // 引用模式下缓存 memoryRef_，避免每次 getter 都新对象
        if (ctxPtr_ && !memoryRef_.IsEmpty()) return memoryRef_.Value();
        node::Object obj = ctxPtr_ ? MemoryAdapter::toExisting(env, &ctx().memory)
                                   : MemoryAdapter::to(env, ctx().memory);
        if (ctxPtr_) {
            memoryRef_ = node::Persistent(obj);
            memoryRef_.SuppressDestruct();
        }
        return obj;
    }

    node::Value ContextAdapter::getManager(const node::CallbackInfo& info) {
        node::Env env = info.Env();
        if (ctxPtr_ && !managerRef_.IsEmpty()) return managerRef_.Value();
        node::Object obj = ctxPtr_ ? ManagerAdapter::toExisting(env, &ctx().manager)
                                   : ManagerAdapter::to(env, ctx().manager);
        if (ctxPtr_) {
            managerRef_ = node::Persistent(obj);
            managerRef_.SuppressDestruct();
        }
        return obj;
    }

    node::Value ContextAdapter::getConfig(const node::CallbackInfo& info) {
        return ConfigAdapter::to(info.Env(), ctx().cfg);
    }

    void ContextAdapter::setConfig(const node::CallbackInfo& info, const node::Value& value) {
        Arguments args(info);
        ctx().cfg = ConfigAdapter::from(args.getWithCheck<node::Object>(0));
    }

    void ContextAdapter::halt(const node::CallbackInfo& info) { ctx().halt(); }

    node::Value ContextAdapter::getHalted(const node::CallbackInfo& info) {
        return node::Boolean::New(info.Env(), ctx().halted());
    }

    void ContextAdapter::sleep(const node::CallbackInfo& info) {
        Arguments args(info);
        // Context::sleep 接受 double 秒数
        ctx().sleep(static_cast<double>(args.getWithCheck<node::Number>(0).Int64Value()));
    }

    node::Value ContextAdapter::getIsSleeping(const node::CallbackInfo& info) {
        return node::Boolean::New(info.Env(), ctx().isSleeping());
    }

}  // namespace stationeers::ic10
