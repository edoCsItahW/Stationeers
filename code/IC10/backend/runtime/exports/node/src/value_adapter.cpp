// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file value_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/11 12:28
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/value_adapter.hpp"
#include "ic10_runtime/value/value.hpp"


namespace stationeers::ic10 {

    node::Object valueInit(node::Env env, node::Object exports) {
        exports.Set("hashValue", to<node::Number, node::String>(env, hashValue));

        exports.Set("strValue", to<node::Number, node::String>(env, strValue));

        exports.Set("constantValue", node::Function::New(env, [](const node::CallbackInfo& info) -> node::Value {
            Arguments args(info);

            auto result = constantValue(args.getWithCheck<node::String>(0));

            return result ? node::Number::New(info.Env(), *result) : info.Env().Undefined();
        }));

        exports.Set("EPSILON_TIMES_8", node::Number::New(env, EPSILON_TIMES_8));

        return exports;
    }

}  // namespace stationeers::ic10
