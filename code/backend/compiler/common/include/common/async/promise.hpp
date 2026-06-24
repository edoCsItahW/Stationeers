// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file promise.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:40
 * @if zh
 * @brief Promise异步提供者封装
 * @details 提供协程的Promise类型,用于设置异步操作结果。
 *        Promise与Future配对使用,Promise设置值,Future获取值。
 * @note 该文件包含Promise类声明,实现在promise.inl中
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Promise asynchronous provider encapsulation
 * @details Provides Promise type for coroutines to set asynchronous operation results.
 *        Promise is used in pair with Future, Promise sets value, Future retrieves value.
 * @note This file contains Promise class declaration, implementation is in promise.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_PROMISE_HPP
#define COMPILER_PROMISE_HPP
#pragma once

#include "future.hpp"

namespace stationeers {

    /**
     * @if zh
     *
     * @class Promise
     * @brief Promise异步结果设置者
     * @details 协程通过Promise设置异步操作的结果,与Future配对使用。
     *        协程结束时自动设置结果值,或通过setException设置异常。
     *
     * @tparam T 异步操作的返回值类型
     *
     * @par 使用示例:
     * @code
     * Promise<int> prom;
     * Future<int> fut = prom.getFuture();
     *
     * // 在协程中
     * prom.setValue(42);  // 设置结果
     * // 或
     * prom.setException(std::make_exception_ptr(std::runtime_error("error")));
     * @endcode
     *
     * @elseif en
     *
     * @class Promise
     * @brief Promise asynchronous result setter
     * @details Coroutines set asynchronous operation results through Promise, used in pair with Future.
     *        When coroutine ends, result value is automatically set, or set exception via setException.
     *
     * @tparam T Return value type of asynchronous operation
     *
     * @par Usage example:
     * @code
     * Promise<int> prom;
     * Future<int> fut = prom.getFuture();
     *
     * // In coroutine
     * prom.setValue(42);  // Set result
     * // or
     * prom.setException(std::make_exception_ptr(std::runtime_error("error")));
     * @endcode
     *
     * @endif
     */
    template<typename T>
    struct Promise {
    private:
        /**
         * @if zh
         * @brief 关联的共享状态
         * @elseif en
         * @brief Associated shared state
         * @endif
         */
        std::shared_ptr<SharedState<T>> state_;

    public:
        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        Promise();

        /**
         * @if zh
         *
         * @brief 获取关联的Future
         * @return 与此Promise关联的Future对象
         *
         * @elseif en
         *
         * @brief Get associated Future
         * @return Future object associated with this Promise
         *
         * @endif
         */
        Future<T> getFuture() const;

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
         * @brief 设置就绪状态和值
         * @param state 包含值的就绪状态
         *
         * @elseif en
         *
         * @brief Set ready state and value
         * @param state Ready state containing value
         *
         * @endif
         */
        void setValue(state_traits<Status::READY, T> state);

        /**
         * @if zh
         *
         * @brief 设置异常
         * @param exception 异常指针
         *
         * @elseif en
         *
         * @brief Set exception
         * @param exception Exception pointer
         *
         * @endif
         */
        void setException(std::exception_ptr exception);
    };

}  // namespace stationeers

#include "promise.inl"

#endif  // COMPILER_PROMISE_HPP
