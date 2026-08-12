// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 21:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_CONTEXT_ADAPTER_HPP
#define IC10_RUNTIME_CONTEXT_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10_runtime/context/context.hpp"

namespace stationeers::ic10 {

    class ContextAdapter : public node::ObjectWrap<ContextAdapter> {
    public:
        ContextAdapter(const node::CallbackInfo& info);

        EXPORT_D_INIT()

        EXPORT_D_FROM(Context)

        static node::Object to(node::Env env, const Context& ctx);

    private:
        static node::FunctionReference constructor;

        Context context_;

        EXPORT_D_ATTR(PC)

        EXPORT_D_ATTR_GETTER(Memory)

        EXPORT_D_ATTR_GETTER(Manager)

        EXPORT_D_ATTR(Config)
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_CONTEXT_ADAPTER_HPP
