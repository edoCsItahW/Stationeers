// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file future.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:39
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_FUTURE_INL
#define COMPILER_FUTURE_INL
#pragma once

namespace stationeers {

    // Future<T>::Awaiter

    template<typename T>
    bool Future<T>::Awaiter::await_ready() const noexcept {
        return state->template is<Status::READY>();
    }

    template<typename T>
    void Future<T>::Awaiter::await_suspend(std::coroutine_handle<> handle) const noexcept {
        using PromiseType = Task<T>::promise_type;

        auto typedHandle = std::coroutine_handle<PromiseType>::from_address(handle.address());
        auto& promise    = const_cast<PromiseType&>(typedHandle.promise());

        if (auto sp = promise.coro_state_weak_.lock()) state->addWaiter(std::move(sp));
    }

    template<typename T>
    auto Future<T>::Awaiter::await_resume() const noexcept {
        return state->get();
    }

    // Future<T>

    template<typename T>
    Future<T>::Future(std::shared_ptr<SharedState<T>> state)
        : state_(std::move(state)) {}

    template<typename T>
    template<Status S>
    bool Future<T>::is() const {
        return state_->template is<S>();
    }

    template<typename T>
    bool Future<T>::isReady() const noexcept {
        return state_->template is<Status::READY>();
    }

    template<typename T>
    auto Future<T>::get() const {
        return state_->get();
    }

    template<typename T>
    auto Future<T>::operator co_await() const noexcept {
        return Awaiter{state_};
    }

    template<typename T>
    template<typename F>
    auto Future<T>::then(F&& f) -> Future<std::invoke_result_t<F, T>> {
        using R = std::invoke_result_t<F, T>;

        auto promise = Promise<R>();

        auto future = promise.getFuture();

        addCallback([state = state_, promise = std::move(promise),
                     f = std::forward<F>(f)]() mutable {
            try {
                if constexpr (std::is_same_v<R, void>) {
                    f(state->get());

                    promise.template set<Status::READY>({});
                } else
                    promise.template set<Status::READY>({f(state->get())});
            } catch (...) { promise.template set<Status::FAILED>({std::current_exception()}); }
        });

        return future;
    }

    template<typename T>
    void Future<T>::addCallback(std::function<void()> callback) const {
        if (state_) state_->addCallback(std::move(callback));
    }

}  // namespace stationeers

#endif  // COMPILER_FUTURE_INL
