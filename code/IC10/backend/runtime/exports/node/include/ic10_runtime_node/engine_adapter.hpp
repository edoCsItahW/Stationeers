// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file engine_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/10
 * @if zh
 * @brief IC10运行时引擎Node.js适配器
 * @details 提供IC10运行时引擎(Engine)的Node.js绑定。
 *        支持IC10程序的全量运行、单步执行、寄存器/栈访问。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 runtime engine Node.js adapter
 * @details Provides Node.js bindings for IC10 runtime engine.
 *        Supports full execution, step-by-step execution, register/stack access.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_ENGINE_ADAPTER_HPP
#define EXPORT_ENGINE_ADAPTER_HPP
#pragma once

#include "ic10_runtime/engine.hpp"
#include "common_node/base.hpp"

namespace stationeers::ic10 {

    /**
     * @class EngineAdapter
     * @if zh
     *
     * @brief IC10运行时引擎Node.js适配器类
     * @details 提供IC10运行时引擎(Engine)的Node.js绑定封装，
     *          支持JavaScript层调用程序执行、寄存器读写和栈操作。
     *
     * @note Engine会持有Program和SymbolTable的副本，确保在JS对象被GC后C++对象仍然有效。
     *
     * @elseif en
     *
     * @brief IC10 runtime engine Node.js adapter class
     * @details Provides Node.js binding encapsulation for IC10 runtime engine,
     *          supporting JavaScript layer calls for program execution, register read/write, and stack operations.
     *
     * @note Engine holds copies of Program and SymbolTable, ensuring C++ objects remain valid after JS objects are GC'd.
     *
     * @endif
     */
    class EngineAdapter : public node::ObjectWrap<EngineAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 从JS层接收Program、SymbolTable和配置对象，创建Engine实例
         *
         * @param info Node.js回调信息对象
         *          - info[0]: ProgramAdapter 实例
         *          - info[1]: SymbolTableAdapter 实例
         *          - info[2]: (可选) 配置对象 { tickDuration?, maxInstructions?, maxStackSize? }
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Receives Program, SymbolTable and config object from JS layer, creates Engine instance
         *
         * @param info Node.js callback information object
         *          - info[0]: ProgramAdapter instance
         *          - info[1]: SymbolTableAdapter instance
         *          - info[2]: (optional) config object { tickDuration?, maxInstructions?, maxStackSize? }
         *
         * @endif
         *
         * @public @memberof EngineAdapter
         */
        EngineAdapter(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 初始化适配器并导出到Node.js模块
         *
         * @elseif en
         *
         * @brief Initialize the adapter and export to Node.js module
         *
         * @endif
         *
         * @public @memberof EngineAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

    private:
        static node::FunctionReference constructor;

        Engine engine_;

        /// 缓存 ContextAdapter 的 JS 句柄，避免每次 getContext() 都 constructor.New
        node::ObjectReference contextRef_;

        EXPORT_D_METHOD_VOID(runTick)

        EXPORT_D_METHOD_VOID(runFull)

        EXPORT_D_METHOD_VALUE(step)

        EXPORT_D_ATTR_GETTER(Context)

        EXPORT_D_ATTR_GETTER(Diagnostics)
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_ENGINE_ADAPTER_HPP
