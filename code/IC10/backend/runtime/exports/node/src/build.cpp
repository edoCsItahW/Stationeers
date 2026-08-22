// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file build.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/10
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime_node/context_adapter.hpp"
#include "ic10_runtime_node/manager_adapter.hpp"
#include "ic10_runtime_node/device_adapter.hpp"
#include "ic10_runtime_node/engine_adapter.hpp"
#include "ic10_runtime_node/memory_adapter.hpp"
#include "ic10_runtime_node/value_adapter.hpp"
#include "ic10_runtime_node/local_adapter.hpp"
#include "common_node/build.hpp"

namespace ic = stationeers::ic10;

node::Object Init(node::Env env, node::Object exports) {
    commonInit(env, exports);

    ic::IC10RuntimeLocalAdapter::init(env, exports);
    ic::valueInit(env, exports);
    ic::DeviceAdapter::init(env, exports);
    ic::MemoryAdapter::init(env, exports);
    ic::EngineAdapter::init(env, exports);
    ic::ManagerAdapter::init(env, exports);
    ic::ContextAdapter::init(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)
