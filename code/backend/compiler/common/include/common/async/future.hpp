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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_FUTURE_HPP
#define COMPILER_FUTURE_HPP
#pragma once

#include "shared_state.hpp"


namespace stationeers {

    template<typename T>
    struct Promise;

    template<typename T>
    struct Future {
    private:
        std::shared_ptr<SharedState<T>> state_;

    public:
        struct Awaiter {
            std::shared_ptr<SharedState<T>> state;

            bool await_ready() const noexcept;

            void await_suspend(std::coroutine_handle<> handle) const noexcept;

            auto await_resume() const noexcept;
        };

        Future() = default;

        Future(std::shared_ptr<SharedState<T>> state);

        template<Status S>
        bool is() const;

        bool isReady() const noexcept;

        auto get() const;

        auto operator co_await() const noexcept;

        template<typename F>
        auto then(F&& f) -> Future<std::invoke_result_t<F, T>>;

        void addCallback(std::function<void()> callback) const;
    };

}  // namespace stationeers

#include "future.inl"

#endif  // COMPILER_FUTURE_HPP
