// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file shared_state.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:36
 * @if zh
 * @brief 异步状态共享管理
 * @details 提供Future/Promise异步编程模型的核心状态管理实现。
 *        通过SharedState类管理协程状态,支持等待者注册、回调函数、线程同步等操作。
 * @note 该文件包含状态机和SharedState类的声明,实现在shared_state.inl中
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Asynchronous state sharing management
 * @details Provides core state management implementation for Future/Promise asynchronous programming model.
 *        Manages coroutine state through SharedState class, supporting waiter registration,
 *        callback functions, thread synchronization and other operations.
 * @note This file contains state machine and SharedState class declarations, implementation is in shared_state.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_SHARED_STATE_HPP
#define COMPILER_SHARED_STATE_HPP
#pragma once

#include "coroutine_state.hpp"
#include <condition_variable>
#include <expected>
#include <functional>
#include <memory>
#include <mutex>
#include <variant>
#include <vector>

namespace stationeers {

    // State

    /**
     * @if zh
     *
     * @enum Status
     * @brief 协程状态枚举
     * @details 表示异步操作可能处于的状态
     *
     * @elseif en
     *
     * @enum Status
     * @brief Coroutine status enumeration
     * @details Represents possible states of an asynchronous operation
     *
     * @endif
     */
    enum class Status { EMPTY, PENDING, READY, FAILED };

    /**
     * @if zh
     *
     * @brief 状态模板结构前向声明
     * @tparam S 状态枚举值
     * @tparam ... 类型参数包
     *
     * @elseif en
     *
     * @brief State template structure forward declaration
     * @tparam S Status enum value
     * @tparam ... Type parameter pack
     *
     * @endif
     */
    template<Status, typename...>
    struct State;

    /**
     * @if zh
     *
     * @brief 状态特征类型别名
     * @tparam S 状态枚举值
     * @tparam T 值类型
     * @details 根据状态和类型条件选择合适的状态模板特化
     *
     * @elseif en
     *
     * @brief State traits type alias
     * @tparam S Status enum value
     * @tparam T Value type
     * @details Selects appropriate state template specialization based on status and type
     *
     * @endif
     */
    template<Status S, typename T>
    using state_traits = std::conditional_t<
        (S == Status::READY || S == Status::PENDING) && !std::is_void_v<T>, State<S, T>, State<S>>;

    /**
     * @if zh
     *
     * @struct State<Status::EMPTY>
     * @brief 空状态
     * @details 表示异步操作尚未开始的状态
     *
     * @elseif en
     *
     * @struct State<Status::EMPTY>
     * @brief Empty state
     * @details Represents the state where asynchronous operation has not yet started
     *
     * @endif
     */
    template<>
    struct State<Status::EMPTY> {
        /**
         * @if zh
         * @brief 状态值
         * @elseif en
         * @brief Status value
         * @endif
         */
        static constexpr auto status = Status::EMPTY;
    };

    /**
     * @if zh
     *
     * @struct State<Status::PENDING, T>
     * @brief 待处理状态(非void类型)
     * @details 表示异步操作正在进行中,持有等待者和回调函数
     *
     * @tparam T 值类型
     *
     * @elseif en
     *
     * @struct State<Status::PENDING, T>
     * @brief Pending state (non-void type)
     * @details Represents asynchronous operation in progress, holding waiters and callback functions
     *
     * @tparam T Value type
     *
     * @endif
     */
    template<typename T>
    struct State<Status::PENDING, T> {
        /**
         * @if zh
         * @brief 状态值
         * @elseif en
         * @brief Status value
         * @endif
         */
        static constexpr auto status = Status::PENDING;

        /**
         * @if zh
         * @brief 等待此状态完成的协程列表
         * @elseif en
         * @brief List of coroutines waiting for this state to complete
         * @endif
         */
        std::vector<std::shared_ptr<CoroutineState<T>>> waiters;

        /**
         * @if zh
         * @brief 状态变更时执行的回调函数列表
         * @elseif en
         * @brief List of callback functions to execute when state changes
         * @endif
         */
        std::vector<std::move_only_function<void()>> callbacks;
    };

    /**
     * @if zh
     *
     * @struct State<Status::PENDING>
     * @brief 待处理状态(void类型)
     * @details 表示异步操作正在进行中(无返回值)
     *
     * @elseif en
     *
     * @struct State<Status::PENDING>
     * @brief Pending state (void type)
     * @details Represents asynchronous operation in progress (no return value)
     *
     * @endif
     */
    template<>
    struct State<Status::PENDING> {
        /**
         * @if zh
         * @brief 状态值
         * @elseif en
         * @brief Status value
         * @endif
         */
        static constexpr auto status = Status::PENDING;

        /**
         * @if zh
         * @brief 等待此状态完成的协程列表
         * @elseif en
         * @brief List of coroutines waiting for this state to complete
         * @endif
         */
        std::vector<std::shared_ptr<CoroutineState<void>>> waiters;

        /**
         * @if zh
         * @brief 状态变更时执行的回调函数列表
         * @elseif en
         * @brief List of callback functions to execute when state changes
         * @endif
         */
        std::vector<std::move_only_function<void()>> callbacks;
    };

    /**
     * @if zh
     *
     * @struct State<Status::READY, T>
     * @brief 就绪状态(非void类型)
     * @details 表示异步操作已成功完成,持有结果值
     *
     * @tparam T 值类型
     *
     * @elseif en
     *
     * @struct State<Status::READY, T>
     * @brief Ready state (non-void type)
     * @details Represents asynchronous operation completed successfully, holding result value
     *
     * @tparam T Value type
     *
     * @endif
     */
    template<typename T>
    struct State<Status::READY, T> {
        /**
         * @if zh
         * @brief 状态值
         * @elseif en
         * @brief Status value
         * @endif
         */
        static constexpr auto status = Status::READY;

        /**
         * @if zh
         * @brief 异步操作的结果值
         * @elseif en
         * @brief Result value of asynchronous operation
         * @endif
         */
        T value;
    };

    /**
     * @if zh
     *
     * @struct State<Status::READY>
     * @brief 就绪状态(void类型)
     * @details 表示异步操作已成功完成(无返回值)
     *
     * @elseif en
     *
     * @struct State<Status::READY>
     * @brief Ready state (void type)
     * @details Represents asynchronous operation completed successfully (no return value)
     *
     * @endif
     */
    template<>
    struct State<Status::READY> {
        /**
         * @if zh
         * @brief 状态值
         * @elseif en
         * @brief Status value
         * @endif
         */
        static constexpr auto status = Status::READY;
    };

    /**
     * @if zh
     *
     * @struct State<Status::FAILED>
     * @brief 失败状态
     * @details 表示异步操作失败,持有异常指针
     *
     * @elseif en
     *
     * @struct State<Status::FAILED>
     * @brief Failed state
     * @details Represents asynchronous operation failed, holding exception pointer
     *
     * @endif
     */
    template<>
    struct State<Status::FAILED> {
        /**
         * @if zh
         * @brief 状态值
         * @elseif en
         * @brief Status value
         * @endif
         */
        static constexpr auto status = Status::FAILED;

        /**
         * @if zh
         * @brief 指向捕获的异常的指针
         * @elseif en
         * @brief Pointer to captured exception
         * @endif
         */
        std::exception_ptr exception;
    };

    // SharedState<T>

    /**
     * @if zh
     *
     * @class SharedState
     * @brief 共享状态类
     * @details 管理异步操作的完整生命周期,包括状态转换、值存储、等待者管理和回调通知。
     *        通过std::enable_shared_from_this支持安全的shared_ptr创建。
     *
     * @tparam T 异步操作的返回值类型
     *
     * @elseif en
     *
     * @class SharedState
     * @brief Shared state class
     * @details Manages the complete lifecycle of asynchronous operations, including state transitions,
     *        value storage, waiter management and callback notification. Supports safe shared_ptr creation
     *        through std::enable_shared_from_this.
     *
     * @tparam T Return value type of asynchronous operation
     *
     * @endif
     */
    template<typename T>
    struct SharedState : std::enable_shared_from_this<SharedState<T>> {
    private:
        /**
         * @if zh
         * @brief 空状态类型别名
         * @elseif en
         * @brief Empty state type alias
         * @endif
         */
        using Empty = State<Status::EMPTY>;

        /**
         * @if zh
         * @brief 待处理状态类型别名
         * @elseif en
         * @brief Pending state type alias
         * @endif
         */
        using Pending = std::conditional_t<
            std::is_void_v<T>, State<Status::PENDING>, State<Status::PENDING, T>>;

        /**
         * @if zh
         * @brief 就绪状态类型别名
         * @elseif en
         * @brief Ready state type alias
         * @endif
         */
        using Ready =
            std::conditional_t<std::is_void_v<T>, State<Status::READY>, State<Status::READY, T>>;

        /**
         * @if zh
         * @brief 失败状态类型别名
         * @elseif en
         * @brief Failed state type alias
         * @endif
         */
        using Failed = State<Status::FAILED>;

        /**
         * @if zh
         * @brief 状态变体类型
         * @details 可以是Empty、Pending、Ready或Failed状态之一
         * @elseif en
         * @brief State variant type
         * @details Can be one of Empty, Pending, Ready, or Failed state
         * @endif
         */
        using StateType = std::variant<Empty, Pending, Ready, Failed>;

        /**
         * @if zh
         * @brief 当前状态
         * @elseif en
         * @brief Current state
         * @endif
         */
        StateType state_ = Empty{};

        /**
         * @if zh
         * @brief 线程同步互斥锁
         * @elseif en
         * @brief Thread synchronization mutex
         * @endif
         */
        mutable std::mutex mutex_;

        /**
         * @if zh
         * @brief 条件变量用于线程同步
         * @elseif en
         * @brief Condition variable for thread synchronization
         * @endif
         */
        std::condition_variable cv_;

    public:
        /**
         * @if zh
         *
         * @brief 设置指定状态
         * @tparam S 目标状态枚举值
         * @param state 要设置的状态
         *
         * @elseif en
         *
         * @brief Set specified state
         * @tparam S Target status enum value
         * @param state State to set
         *
         * @endif
         */
        template<Status S>
        void set(state_traits<S, T> state);

        /**
         * @if zh
         *
         * @brief 获取异步操作结果
         * @return 包含结果值或异常指针的expected对象
         *
         * @elseif en
         *
         * @brief Get asynchronous operation result
         * @return expected object containing result value or exception pointer
         *
         * @endif
         */
        std::expected<T, std::exception_ptr> get();

        /**
         * @if zh
         *
         * @brief 检查是否处于指定状态
         * @tparam S 要检查的状态枚举值
         * @return 如果处于该状态返回true
         *
         * @elseif en
         *
         * @brief Check if in specified state
         * @tparam S Status enum value to check
         * @return true if in that state
         *
         * @endif
         */
        template<Status S>
        bool is() const;

        /**
         * @if zh
         *
         * @brief 添加等待者
         * @param waiter 指向等待协程状态的shared_ptr
         *
         * @elseif en
         *
         * @brief Add waiter
         * @param waiter shared_ptr pointing to waiting coroutine state
         *
         * @endif
         */
        void addWaiter(std::shared_ptr<CoroutineState<T>> waiter);

        /**
         * @if zh
         *
         * @brief 添加回调函数
         * @param callback 状态变更时执行的回调
         *
         * @elseif en
         *
         * @brief Add callback function
         * @param callback Callback to execute when state changes
         *
         * @endif
         */
        void addCallback(std::move_only_function<void()> callback);
    };

}  // namespace stationeers

#include "shared_state.inl"

#endif  // COMPILER_SHARED_STATE_HPP
