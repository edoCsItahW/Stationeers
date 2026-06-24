// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

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

    // 前向声明

    /**
     * @if zh
     *
     * @brief Task类型前向声明
     * @tparam T 协程返回值类型
     *
     * @elseif en
     *
     * @brief Task type forward declaration
     * @tparam T Coroutine return value type
     *
     * @endif
     */
    template<typename T>
    struct Task;

    /**
     * @if zh
     *
     * @class CoroutineState
     * @brief 协程状态封装类
     * @details 管理协程句柄的生命周期,提供线程安全的销毁标志追踪。
     *        每个协程对应一个CoroutineState实例,用于Future/Promise模型中协程状态共享。
     *
     * @tparam T 协程返回值类型
     *
     * @note 该类不可复制,只能通过shared_ptr管理
     *
     * @elseif en
     *
     * @class CoroutineState
     * @brief Coroutine state wrapper class
     * @details Manages coroutine handle lifecycle, provides thread-safe destruction flag tracking.
     *        Each coroutine corresponds to one CoroutineState instance, used for coroutine state sharing
     *        in Future/Promise model.
     *
     * @tparam T Coroutine return value type
     *
     * @note This class is not copyable, can only be managed via shared_ptr
     *
     * @endif
     */
    template<typename T>
    struct CoroutineState {
        /**
         * @if zh
         *
         * @brief 协程句柄类型别名
         *
         * @elseif en
         *
         * @brief Coroutine handle type alias
         *
         * @endif
         */
        using Handle = std::coroutine_handle<typename Task<T>::promise_type>;

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
        CoroutineState(Handle h);

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
