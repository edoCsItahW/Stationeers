// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file engine_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10
 * @if zh
 * @brief IC10运行时引擎Node.js适配器实现
 * @details 实现IC10运行时引擎Node.js适配器类。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 runtime engine Node.js adapter implementation
 * @details Implements the IC10 runtime engine Node.js adapter class.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#include "ic10_compiler_node/semantic/symbol_table_adapter.hpp"
#include "ic10_compiler_node/parser/ast_adapter.hpp"
#include "ic10_runtime_node/context_adapter.hpp"
#include "ic10_runtime_node/config_adapter.hpp"
#include "ic10_runtime_node/engine_adapter.hpp"

namespace stationeers::ic10 {

    node::FunctionReference EngineAdapter::constructor;

    EngineAdapter::EngineAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info)
        , engine_([&]() -> Engine {
            Arguments args(info);
            auto program = ProgramAdapter::from(args.getWithCheck<node::Object>(0));
            auto symbols = SymbolTableAdapter::from(args.getWithCheck<node::Object>(1));
            auto cfg =
                info.Length() >= 3 ? ConfigAdapter::from(args.getWithCheck<node::Object>(2))
                                   : Config{};
            return Engine{std::move(program), std::move(symbols), cfg};
        }()) {}

    node::Object EngineAdapter::init(node::Env env, node::Object exports) {
        node::Function func = DefineClass(
            env,
            "Engine",
            {
#ifdef _MSC_VER
                InstanceAccessor("context", &EngineAdapter::getContext, nullptr),
#else
                InstanceAccessor<&EngineAdapter::getContext>("context"),
#endif
                InstanceMethod<&EngineAdapter::runTick>("runTick"),
                InstanceMethod<&EngineAdapter::runFull>("runFull"),
            }
        );

        constructor = node::Persistent(func);
        constructor.SuppressDestruct();

        (void)exports.Set("Engine", func);

        return exports;
    }

    void EngineAdapter::runTick(const node::CallbackInfo&) {
        engine_.runTick();
    }

    void EngineAdapter::runFull(const node::CallbackInfo&) {
        engine_.runFull();
    }

    node::Value EngineAdapter::getContext(const node::CallbackInfo& info) {
        if (!contextRef_.IsEmpty()) {
            return contextRef_.Value();
        }
        // 直接引用 Engine 内部 Context，不拷贝，也不每次新对象
        node::Object obj = ContextAdapter::toExisting(info.Env(), &engine_.getContext());
        contextRef_ = node::Persistent(obj);
        contextRef_.SuppressDestruct();
        return obj;
    }

}  // namespace stationeers::ic10
