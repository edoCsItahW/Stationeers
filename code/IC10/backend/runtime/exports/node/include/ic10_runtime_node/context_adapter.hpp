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

#include "ic10_runtime/context/context.hpp"
#include "common_node/base.hpp"

namespace stationeers::ic10 {

    class ContextAdapter : public node::ObjectWrap<ContextAdapter> {
    public:
        ContextAdapter(const node::CallbackInfo& info);

        EXPORT_D_INIT()

        EXPORT_D_FROM(Context)

        /**
         * @brief 基于已存在的 Context 构造 JS 包装器（引用模式，不拷贝数据）。
         * @details 用于 EngineAdapter 缓存 Context：返回的 JS 对象内部 ctxPtr_ 直接指向
         *          Engine 内部的 Context，避免每次 getter 都 constructor.New 造成的深拷贝
         *          和 N-API 句柄爆炸。同时 memoryRef_/managerRef_ 会缓存子适配器对象。
         */
        static node::Object toExisting(node::Env env, Context* ctx);

        static node::Object to(node::Env env, const Context& ctx);

    private:
        static node::FunctionReference constructor;

        /// 值模式：JS 直接 new Context(program, symbols, cfg) 时拥有的副本
        Context context_;

        /// 引用模式：指向 Engine 内部真实 Context，不拥有
        Context* ctxPtr_ = nullptr;

        /// 缓存子适配器（引用模式下使用，避免每次 getter 新对象）
        node::ObjectReference memoryRef_;
        node::ObjectReference managerRef_;

        [[nodiscard]] Context& ctx() noexcept { return ctxPtr_ ? *ctxPtr_ : context_; }
        [[nodiscard]] const Context& ctx() const noexcept { return ctxPtr_ ? *ctxPtr_ : context_; }

        EXPORT_D_ATTR(PC)

        EXPORT_D_ATTR_GETTER(Memory)

        EXPORT_D_ATTR_GETTER(Manager)

        EXPORT_D_ATTR(Config)

        EXPORT_D_METHOD_VOID(halt)

        EXPORT_D_ATTR_GETTER(Halted)

        EXPORT_D_METHOD_VOID(sleep)

        EXPORT_D_ATTR_GETTER(IsSleeping)

        EXPORT_D_METHOD_VALUE(getAddr)

        EXPORT_D_METHOD_VALUE(getLine)
    };

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_CONTEXT_ADAPTER_HPP
