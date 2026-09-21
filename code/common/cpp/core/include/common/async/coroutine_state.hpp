// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file coroutine_state.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:31
 * @if zh
 * @brief 协程状态管理
 * @details 提供协程状态的结构化封装,管理协程句柄的生命周期和销毁标志。
 *        用于Future/Promise异步编程模型中协程状态的管理。
 * @note 该文件包含协程状态类的声明,实现在coroutine_state.inl中
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Coroutine state management
 * @details Provides structured encapsulation of coroutine state, managing coroutine handle lifecycle
 *        and destruction flag. Used for managing coroutine state in Future/Promise asynchronous programming model.
 * @note This file contains declarations of coroutine state class, implementation is in coroutine_state.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_COROUTINE_STATE_HPP
#define COMPILER_COROUTINE_STATE_HPP
#pragma once

#include <atomic>
#include <coroutine>


namespace stationeers {

    /**
     * @if zh
     *
     * @class CoroutineState
     * @brief 协程状态封装类
     * @details 管理协程句柄的生命周期,提供线程安全的销毁标志追踪。
     *        每个协程对应一个CoroutineState实例,用于Future/Promise模型中协程状态共享。
     *
     * @par 类型无关性:
     * 该类不携带协程返回值类型,因此`Task<void>`与`Task<T>`的协程状态是同一类型。
     * 等待者注册时无需按被等待的Future类型重新解释状态,任意协程都可注册为任意
     * Future的等待者(参见 @ref Future::Awaiter::await_suspend)。
     *
     * @note 该类不可复制,只能通过shared_ptr管理;当最后一个shared_ptr析构时,
     *       若协程帧尚未销毁则销毁之
     *
     * @elseif en
     *
     * @class CoroutineState
     * @brief Coroutine state wrapper class
     * @details Manages coroutine handle lifecycle, provides thread-safe destruction flag tracking.
     *        Each coroutine corresponds to one CoroutineState instance, used for coroutine state sharing
     *        in Future/Promise model.
     *
     * @par Type erasure:
     * This class carries no coroutine return type, so `Task<void>` and `Task<T>` coroutines share
     * the very same state type. Waiter registration needs no reinterpretation of the state against
     * the awaited Future type: any coroutine can register as a waiter of any Future
     * (see @ref Future::Awaiter::await_suspend).
     *
     * @note This class is not copyable, can only be managed via shared_ptr; destroying the last
     *       shared_ptr destroys the coroutine frame if it has not been destroyed yet
     *
     * @endif
     */
    struct CoroutineState {
        /**
         * @if zh
         *
         * @brief 协程句柄类型别名
         * @details 类型擦除的句柄,与协程的Promise类型无关
         *
         * @elseif en
         *
         * @brief Coroutine handle type alias
         * @details Type-erased handle, independent of the coroutine's promise type
         *
         * @endif
         */
        using Handle = std::coroutine_handle<>;

        /**
         * @if zh
         *
         * @brief 协程句柄
         * @details 指向关联协程的句柄
         *
         * @elseif en
         *
         * @brief Coroutine handle
         * @details Handle pointing to the associated coroutine
         *
         * @endif
         */
        Handle handle;

        /**
         * @if zh
         *
         * @brief 销毁标志
         * @details 原子布尔值,标记协程状态是否已被销毁
         *
         * @elseif en
         *
         * @brief Destruction flag
         * @details Atomic boolean marking whether the coroutine state has been destroyed
         *
         * @endif
         */
        std::atomic<bool> destroyed{false};

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param h 协程句柄
         *
         * @elseif en
         *
         * @brief Constructor
         * @param h Coroutine handle
         *
         * @endif
         */
        CoroutineState(Handle h) noexcept;

        /**
         * @if zh
         * @brief 析构函数
         * @elseif en
         * @brief Destructor
         * @endif
         */
        ~CoroutineState();

        /**
         * @if zh
         * @brief 禁用复制构造函数
         * @elseif en
         * @brief Disabled copy constructor
         * @endif
         */
        CoroutineState(const CoroutineState&)            = delete;

        /**
         * @if zh
         * @brief 禁用复制赋值运算符
         * @elseif en
         * @brief Disabled copy assignment operator
         * @endif
         */
        CoroutineState& operator=(const CoroutineState&) = delete;
    };

}  // namespace stationeers

#include "coroutine_state.inl"

#endif  // COMPILER_COROUTINE_STATE_HPP
