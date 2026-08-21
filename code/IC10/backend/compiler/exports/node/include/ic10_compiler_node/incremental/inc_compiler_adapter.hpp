// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_compiler_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @if zh
 * @brief 增量编译器Node.js适配器
 * @details 将IncCompiler包装为Node.js可用的类
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Incremental compiler Node.js adapter
 * @details Wraps IncCompiler as a Node.js class
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_IC10_NODE_INC_COMPILER_ADAPTER_HPP
#define COMPILER_IC10_NODE_INC_COMPILER_ADAPTER_HPP
#pragma once

#include "ic10_compiler/incremental/inc_compiler.hpp"
#include "common_node/base.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class IncCompilerAdapter
     * @brief IncCompiler的Node.js适配器
     * @elseif en
     * @class IncCompilerAdapter
     * @brief Node.js adapter for IncCompiler
     * @endif
     */
    class IncCompilerAdapter final : public node::ObjectWrap<IncCompilerAdapter> {
    public:
        static node::Object init(node::Env env, node::Object exports);

        IncCompilerAdapter(const node::CallbackInfo& info);

    private:
        node::Value compileFull(const node::CallbackInfo& info);
        node::Value compileInc(const node::CallbackInfo& info);
        node::Value hasCache(const node::CallbackInfo& info);
        void clear(const node::CallbackInfo& info);

        static node::Object resultToJS(node::Env env, const IncCompileResult& result);

        IncCompiler compiler_;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_IC10_NODE_INC_COMPILER_ADAPTER_HPP
