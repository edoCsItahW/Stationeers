// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file config_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 21:25
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_CONFIG_ADAPTER_HPP
#define IC10_RUNTIME_CONFIG_ADAPTER_HPP
#pragma once

#include "ic10_runtime/config.hpp"
#include "common_node/base.hpp"

namespace stationeers::ic10 {

    class ConfigAdapter {
    public:
        static Config from(const node::Object& obj);

        static node::Value to(node::Env env, const Config& cfg);
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_CONFIG_ADAPTER_HPP
