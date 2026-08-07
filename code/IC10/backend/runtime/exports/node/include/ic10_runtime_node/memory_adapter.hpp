// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file memory_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/11 10:21
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_MEMORY_ADAPTER_HPP
#define IC10_RUNTIME_MEMORY_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10_runtime/memory/memory.hpp"

namespace stationeers::ic10 {

    class MemoryAdapter : public node::ObjectWrap<MemoryAdapter> {
    public:
        MemoryAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

        static Memory from(const node::Object& obj);

        static node::Object to(node::Env env, const Memory& memory);

    private:
        static node::FunctionReference constructor;

        Memory memory_;

        EXPORT_D_METHOD_VALUE(toJSON)

        EXPORT_D_ATTR(Config)

        EXPORT_D_METHOD_VALUE(getReg)

        EXPORT_D_METHOD_VOID(setReg)

        EXPORT_D_ATTR(SP)

        EXPORT_D_METHOD_VALUE(getStack)

        EXPORT_D_METHOD_VOID(setStack)

    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_MEMORY_ADAPTER_HPP
