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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COROUTINE_STATE_HPP
#define COMPILER_COROUTINE_STATE_HPP
#pragma once

#include <atomic>
#include <coroutine>


namespace stationeers {

    template<typename T>
    struct Task;

    template<typename T>
    struct CoroutineState {
        using Handle = std::coroutine_handle<typename Task<T>::promise_type>;

        Handle handle;

        std::atomic<bool> destroyed{false};

        CoroutineState(Handle h);

        ~CoroutineState();

        CoroutineState(const CoroutineState&)            = delete;
        CoroutineState& operator=(const CoroutineState&) = delete;
    };

}  // namespace stationeers

#include "coroutine_state.inl"

#endif  // COMPILER_COROUTINE_STATE_HPP
