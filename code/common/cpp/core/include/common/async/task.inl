// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file task.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:42
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_TASK_INL
#define COMPILER_TASK_INL
#pragma once

#include <utility>

namespace stationeers {

    // TaskPromiseBase<T>

    template<typename T>
    auto TaskPromiseBase<T>::initial_suspend() noexcept {
        return std::suspend_never{};
    }

    template<typename T>
    auto TaskPromiseBase<T>::final_suspend() noexcept {
        return std::suspend_always{};
    }

    template<typename T>
    void TaskPromiseBase<T>::unhandled_exception() noexcept {
        promise.template set<Status::FAILED>({std::current_exception()});
    }

    // TaskPromise<T>

    template<typename T>
    Task<T> TaskPromise<T>::get_return_object() {
        return {std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }

    template<typename T>
    void TaskPromise<T>::return_value(T value) {
        this->promise.template set<Status::READY>({std::move(value)});
    }

    // TaskBase<T>

    template<typename T>
    TaskBase<T>::TaskBase(std::coroutine_handle<promise_type> handle) noexcept
        : coro_(handle)
        , coro_state_(std::make_shared<CoroutineState>(handle)) {
        // 把状态弱引用写入promise：协程co_await未就绪的Future时，
        // Future::Awaiter::await_suspend 借此把该协程注册为等待者。
        // Task<void>与Task<T>共用同一状态类型，故两者都可被注册。
        handle.promise().coro_state_weak_ = coro_state_;
    }

    template<typename T>
    TaskBase<T>::TaskBase(TaskBase&& other) noexcept
        : coro_(std::exchange(other.coro_, {}))
        , coro_state_(std::move(other.coro_state_)) {}

    template<typename T>
    Future<T> TaskBase<T>::getFuture() const {
        return coro_.promise().promise.getFuture();
    }

    template<typename T>
    TaskBase<T>& TaskBase<T>::operator=(TaskBase&& other) noexcept {
        if (this != &other) {
            // 协程帧的生命周期由coro_state_负责：覆盖旧状态即销毁旧协程，
            // 不再单独 destroy coro_，避免与CoroutineState析构重复销毁
            coro_       = std::exchange(other.coro_, {});
            coro_state_ = std::move(other.coro_state_);
        }
        return *this;
    }

    template<typename T>
    TaskBase<T>::~TaskBase() {
        coro_ = {};
    }

    template<typename T>
    auto TaskBase<T>::operator co_await() const noexcept {
        return getFuture().operator co_await();
    }

    // Task<T>

    template<typename T>
    Task<T>::Task(std::coroutine_handle<typename TaskBase<T>::promise_type> h)
        : TaskBase<T>(h) {}

    template<typename T>
    void Task<T>::detach() noexcept {
        this->coro_ = {};
        this->coro_state_.reset();
    }

}  // namespace stationeers


#endif  // COMPILER_TASK_INL
