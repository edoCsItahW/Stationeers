// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file future.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:39
 * @if zh
 * @brief Future异步结果封装
 * @details 提供协程的Future类型,用于获取异步操作结果。
 *        支持协程等待(CO_AWAIT)、链式回调(THEN)、状态查询等操作。
 * @note 该文件包含Future类声明,实现在future.inl中
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Future asynchronous result encapsulation
 * @details Provides Future type for coroutines to retrieve asynchronous operation results.
 *        Supports coroutine awaiting (co_await), chained callbacks (then), state querying and other operations.
 * @note This file contains Future class declaration, implementation is in future.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_FUTURE_HPP
#define COMPILER_FUTURE_HPP
#pragma once

#include "shared_state.hpp"


namespace stationeers {

    // 前向声明

    /**
     * @if zh
     *
     * @brief Promise类型前向声明
     * @tparam T 结果值类型
     *
     * @elseif en
     *
     * @brief Promise type forward declaration
     * @tparam T Result value type
     *
     * @endif
     */
    template<typename T>
    struct Promise;

    /**
     * @if zh
     *
     * @class Future
     * @brief 异步结果封装类
     * @details 表示异步操作的最终结果,提供值获取、协程等待和回调链式调用功能。
     *
     * @tparam T 异步操作的返回值类型
     *
     * @par 使用示例:
     * @code
     * Future<int> fut = someAsyncOperation();
     * int result = co_await fut;  // 协程等待
     * fut.then([](int val) {  // 链式回调
     *     printf("Result: %d", val);
     * });
     * @endcode
     *
     * @elseif en
     *
     * @class Future
     * @brief Asynchronous result wrapper class
     * @details Represents the final result of an asynchronous operation, providing value retrieval,
     *        coroutine awaiting and callback chaining functionality.
     *
     * @tparam T Return value type of asynchronous operation
     *
     * @par Usage example:
     * @code
     * Future<int> fut = someAsyncOperation();
     * int result = co_await fut;  // Coroutine await
     * fut.then([](int val) {  // Chained callback
     *     printf("Result: %d", val);
     * });
     * @endcode
     *
     * @endif
     */
    template<typename T>
    struct Future {
    private:
        std::shared_ptr<SharedState<T>> state_;

    public:
        /**
         * @if zh
         *
         * @class Awaiter
         * @brief Future等待器
         * @details 实现协程awaitable接口,用于co_await表达式
         *
         * @elseif en
         *
         * @class Awaiter
         * @brief Future awaiter
         * @details Implements coroutine awaitable interface for co_await expressions
         *
         * @endif
         */
        struct Awaiter {
            /**
             * @if zh
             * @brief 关联的共享状态
             * @elseif en
             * @brief Associated shared state
             * @endif
             */
            std::shared_ptr<SharedState<T>> state;

            /**
             * @if zh
             *
             * @brief await_ready实现
             * @return 如果状态已就绪返回true
             *
             * @elseif en
             *
             * @brief await_ready implementation
             * @return true if state is ready
             *
             * @endif
             */
            bool await_ready() const noexcept;

            /**
             * @if zh
             *
             * @brief await_suspend实现
             * @param handle 当前协程句柄
             *
             * @elseif en
             *
             * @brief await_suspend implementation
             * @param handle Current coroutine handle
             *
             * @endif
             */
            void await_suspend(std::coroutine_handle<> handle) const noexcept;

            /**
             * @if zh
             *
             * @brief await_resume实现
             * @return 异步操作结果值
             *
             * @elseif en
             *
             * @brief await_resume implementation
             * @return Asynchronous operation result value
             *
             * @endif
             */
            auto await_resume() const noexcept;
        };

        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        Future() = default;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param state 共享状态指针
         *
         * @elseif en
         *
         * @brief Constructor
         * @param state Shared state pointer
         *
         * @endif
         */
        Future(std::shared_ptr<SharedState<T>> state);

        /**
         * @if zh
         *
         * @brief 检查是否处于指定状态
         * @tparam S 状态枚举值
         * @return 如果处于该状态返回true
         *
         * @elseif en
         *
         * @brief Check if in specified state
         * @tparam S Status enum value
         * @return true if in that state
         *
         * @endif
         */
        template<Status S>
        bool is() const;

        /**
         * @if zh
         *
         * @brief 检查是否就绪
         * @return 如果状态为READY或FAILED返回true
         *
         * @elseif en
         *
         * @brief Check if ready
         * @return true if status is READY or FAILED
         *
         * @endif
         */
        bool isReady() const noexcept;

        /**
         * @if zh
         *
         * @brief 获取异步结果
         * @return 包含结果值的expected对象
         *
         * @elseif en
         *
         * @brief Get asynchronous result
         * @return expected object containing result value
         *
         * @endif
         */
        auto get() const;

        /**
         * @if zh
         *
         * @brief 协程await运算符重载
         * @return Awaiter对象用于协程等待
         *
         * @elseif en
         *
         * @brief Coroutine await operator overload
         * @return Awaiter object for coroutine awaiting
         *
         * @endif
         */
        auto operator co_await() const noexcept;

        /**
         * @if zh
         *
         * @brief 链式回调
         * @tparam F 回调函数类型
         * @param f 回调函数,接受T类型参数并返回新Future
         * @return 新的Future对象
         *
         * @elseif en
         *
         * @brief Chained callback
         * @tparam F Callback function type
         * @param f Callback function, takes T type parameter and returns new Future
         * @return New Future object
         *
         * @endif
         */
        template<typename F>
        auto then(F&& f) -> Future<std::invoke_result_t<F, T>>;

        /**
         * @if zh
         *
         * @brief 添加回调函数
         * @param callback 无参数回调函数
         *
         * @elseif en
         *
         * @brief Add callback function
         * @param callback No-parameter callback function
         *
         * @endif
         */
        void addCallback(std::function<void()> callback) const;
    };

}  // namespace stationeers

#include "future.inl"

#endif  // COMPILER_FUTURE_HPP
