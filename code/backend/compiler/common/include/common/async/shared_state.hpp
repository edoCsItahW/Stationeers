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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_SHARED_STATE_HPP
#define COMPILER_SHARED_STATE_HPP
#pragma once

#include "coroutine_state.hpp"
#include <expected>
#include <functional>
#include <memory>
#include <mutex>
#include <variant>
#include <vector>

namespace stationeers {

    // State

    enum class Status { EMPTY, PENDING, READY, FAILED };

    template<Status, typename...>
    struct State;

    template<Status S, typename T>
    using state_traits = std::conditional_t<
        (S == Status::READY || S == Status::PENDING) && !std::is_void_v<T>, State<S, T>, State<S>>;

    template<>
    struct State<Status::EMPTY> {
        static constexpr auto status = Status::EMPTY;
    };

    template<typename T>
    struct State<Status::PENDING, T> {
        static constexpr auto status = Status::PENDING;

        std::vector<std::shared_ptr<CoroutineState<T>>> waiters;

        std::vector<std::move_only_function<void()>> callbacks;
    };

    template<>
    struct State<Status::PENDING> {
        static constexpr auto status = Status::PENDING;

        std::vector<std::shared_ptr<CoroutineState<void>>> waiters;

        std::vector<std::move_only_function<void()>> callbacks;
    };

    template<typename T>
    struct State<Status::READY, T> {
        static constexpr auto status = Status::READY;

        T value;
    };

    template<>
    struct State<Status::READY> {
        static constexpr auto status = Status::READY;
    };

    template<>
    struct State<Status::FAILED> {
        static constexpr auto status = Status::FAILED;

        std::exception_ptr exception;
    };

    // SharedState<T>

    template<typename T>
    struct SharedState : std::enable_shared_from_this<SharedState<T>> {
    private:
        using Empty = State<Status::EMPTY>;

        using Pending = std::conditional_t<
            std::is_void_v<T>, State<Status::PENDING>, State<Status::PENDING, T>>;

        using Ready =
            std::conditional_t<std::is_void_v<T>, State<Status::READY>, State<Status::READY, T>>;

        using Failed = State<Status::FAILED>;

        using StateType = std::variant<Empty, Pending, Ready, Failed>;

        StateType state_ = Empty{};

        mutable std::mutex mutex_;

        std::condition_variable cv_;

    public:
        template<Status S>
        void set(state_traits<S, T> state);

        std::expected<T, std::exception_ptr> get();

        template<Status S>
        bool is() const;

        void addWaiter(std::shared_ptr<CoroutineState<T>> waiter);

        void addCallback(std::move_only_function<void()> callback);
    };

}  // namespace stationeers

#include "shared_state.inl"

#endif  // COMPILER_SHARED_STATE_HPP
