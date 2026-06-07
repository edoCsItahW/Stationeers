// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file task.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:42
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_TASK_HPP
#define COMPILER_TASK_HPP
#pragma once

#include "promise.hpp"

namespace stationeers {

    template<typename T>
    struct Task;

    template<typename T>
    struct TaskPromiseBase {
        Promise<T> promise;

        std::weak_ptr<CoroutineState<T>> coro_state_weak_;

        auto initial_suspend() noexcept;

        auto final_suspend() noexcept;

        void unhandled_exception() noexcept;
    };

    template<typename T>
    struct TaskPromise : TaskPromiseBase<T> {
        Task<T> get_return_object();

        void return_value(T value);
    };

    template<typename T = void>
    struct TaskBase {
        using promise_type = TaskPromise<T>;

    private:
        std::coroutine_handle<promise_type> coro_;

    public:
        TaskBase(std::coroutine_handle<promise_type> handle);

        TaskBase(TaskBase&& other) noexcept;

        [[nodiscard]] Future<T> getFuture() const;

        TaskBase& operator=(TaskBase&& other) noexcept;

        ~TaskBase();

        auto operator co_await() const noexcept;
    };

    template<typename T = void>
    struct Task : public TaskBase<T> {
        using CoroState = CoroutineState<T>;

        std::shared_ptr<CoroState> coro_state_;

        Task(std::coroutine_handle<typename TaskBase<T>::promise_type> h);

        Task(Task&& other) noexcept;

        Task& operator=(Task&& other) noexcept;

        ~Task() = default;

        Task(const Task&)            = delete;
        Task& operator=(const Task&) = delete;

        void detach();
    };

    template<>
    struct Task<void> : public TaskBase<> {
        using TaskBase::TaskBase;

        Task(Task&&)            = default;
        Task& operator=(Task&&) = default;

        Task(const Task&)            = delete;
        Task& operator=(const Task&) = delete;
    };

    // 放在Task<void>后，此时Task定义完整
    template<>
    struct TaskPromise<void> : TaskPromiseBase<void> {
        Task<> get_return_object();

        void return_void();
    };

}  // namespace stationeers

#include "task.inl"

#endif  // COMPILER_TASK_HPP
