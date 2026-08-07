// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file config_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 21:25
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/config_adapter.hpp"

namespace stationeers::ic10 {

    Config ConfigAdapter::from(const node::Object& obj) {
        Config config;

        config.tickDuration = obj.Get("tickDuration").As<node::Number>();

        config.maxInstructions = obj.Get("maxInstructions").As<node::Number>();

        config.maxStackSize = obj.Get("maxStackSize").As<node::Number>();

        return config;
    }

    node::Value ConfigAdapter::to(node::Env env, const Config& cfg) {
        auto obj = node::Object::New(env);

        obj.Set("tickDuration", node::Number::New(env, cfg.tickDuration));

        obj.Set("maxInstructions", node::Number::New(env, cfg.maxInstructions));

        obj.Set("maxStackSize", node::Number::New(env, cfg.maxStackSize));

        return obj;
    }

}  // namespace stationeers::ic10
