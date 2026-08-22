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

#include "ic10_runtime/memory/memory.hpp"
#include "common_node/base.hpp"

namespace stationeers::ic10 {

    class MemoryAdapter : public node::ObjectWrap<MemoryAdapter> {
    public:
        MemoryAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

        static Memory from(const node::Object& obj);

        /**
         * @brief 基于已存在的 Memory 构造 JS 包装器（引用模式），不拷贝。
         * @details 用于 ContextAdapter 的 memory getter：返回的 JS 对象直接指向
         *          Context 内部 Memory，读写都立即反映在 Engine 的运行时状态中。
         */
        static node::Object toExisting(node::Env env, Memory* memory);

        static node::Object to(node::Env env, const Memory& memory);

    private:
        static node::FunctionReference constructor;

        /// 值模式：JS 直接 new Memory(cfg) 时拥有的副本
        Memory memory_;

        /// 引用模式：指向 Context 内部真实 Memory，不拥有
        Memory* memPtr_ = nullptr;

        [[nodiscard]] Memory& mem() noexcept { return memPtr_ ? *memPtr_ : memory_; }
        [[nodiscard]] const Memory& mem() const noexcept { return memPtr_ ? *memPtr_ : memory_; }

        EXPORT_D_METHOD_VALUE(toJSON)

        EXPORT_D_ATTR(Config)

        EXPORT_D_METHOD_VALUE(getReg)

        EXPORT_D_METHOD_VOID(setReg)

        EXPORT_D_ATTR(SP)

        EXPORT_D_METHOD_VALUE(getStack)

        EXPORT_D_METHOD_VOID(setStack)

        EXPORT_D_METHOD_VOID(push)

        EXPORT_D_METHOD_VALUE(pop)

        EXPORT_D_METHOD_VOID(poke)

        EXPORT_D_METHOD_VALUE(peek)

    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_MEMORY_ADAPTER_HPP
