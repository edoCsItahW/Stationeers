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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_PROMISE_HPP
#define COMPILER_PROMISE_HPP
#pragma once

#include "future.hpp"

namespace stationeers {

    template<typename T>
    struct Promise {
    private:
        std::shared_ptr<SharedState<T>> state_;

    public:
        Promise();

        Future<T> getFuture() const;

        template<Status S>
        void set(state_traits<S, T> state);

        void setValue(state_traits<Status::READY, T> state);

        void setException(std::exception_ptr exception);
    };

}  // namespace stationeers

#include "promise.inl"

#endif  // COMPILER_PROMISE_HPP
