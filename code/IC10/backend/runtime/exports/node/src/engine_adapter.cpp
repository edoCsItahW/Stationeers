// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file engine_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/10
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/engine_adapter.hpp"
#include "ic10_compiler_node/parser/ast_adapter.hpp"
#include "ic10_compiler_node/semantic/symbol_table_adapter.hpp"
#include "ic10_runtime_node/config_adapter.hpp"
#include "ic10_runtime_node/context_adapter.hpp"

namespace stationeers::ic10 {

    node::Object EngineAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env, "Engine",
            {
                #ifdef _MSC_VER
                InstanceAccessor("context", &EngineAdapter::getContext, nullptr),
                #else
                InstanceAccessor<&EngineAdapter::getContext>("context"),
                #endif
                InstanceMethod<&EngineAdapter::runTick>("runTick"),
                InstanceMethod<&EngineAdapter::runFull>("runFull")
            }
        );

        auto constructor = std::make_unique<node::FunctionReference>();

        *constructor = node::Persistent(func);

        constructor->SuppressDestruct();

        (void)exports.Set("Engine", func);

        return exports;
    }

    EngineAdapter::EngineAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info)
        , engine_([&] -> Engine {
            Arguments args(info);

            auto program = ProgramAdapter::from(args.getWithCheck<node::Object>(0));

            auto symbols = SymbolTableAdapter::from(args.getWithCheck<node::Object>(1));

            auto cfg = ConfigAdapter::from(args.getWithCheck<node::Object>(2));

            return {program, symbols, cfg};
        }()) {}

    node::Value EngineAdapter::runTick(const node::CallbackInfo& info) {
        engine_.runTick();
        return info.Env().Undefined();
    }

    node::Value EngineAdapter::runFull(const node::CallbackInfo& info) {
        engine_.runFull();
        return info.Env().Undefined();
    }

    node::Value EngineAdapter::getContext(const node::CallbackInfo& info) {
        return ContextAdapter::to(info.Env(), engine_.getContext());
    }

}  // namespace stationeers::ic10
