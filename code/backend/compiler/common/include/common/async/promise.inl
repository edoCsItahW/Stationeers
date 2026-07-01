// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file promise.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:41
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_PROMISE_INL
#define COMPILER_PROMISE_INL
#pragma once

namespace stationeers {

    template<typename T>
    Promise<T>::Promise()
        : state_(std::make_shared<SharedState<T>>()) {}

    template<typename T>
    Future<T> Promise<T>::getFuture() const {
        if (state_->template is<Status::EMPTY>()) state_->template set<Status::PENDING>({});

        return Future<T>(state_);
    }

    template<typename T>
    template<Status S>
    void Promise<T>::set(state_traits<S, T> state) {
        state_->template set<S>(std::move(state));
    }

    template<typename T>
    void Promise<T>::setValue(state_traits<Status::READY, T> state) {
        state_->template set<Status::READY>(std::move(state));
    }

    template<typename T>
    void Promise<T>::setException(std::exception_ptr exception) {
        state_->template set<Status::FAILED>({std::move(exception)});
    }

}  // namespace stationeers

#endif  // COMPILER_PROMISE_INL
