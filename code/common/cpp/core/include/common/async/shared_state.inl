// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file shared_state.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:37
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_SHARED_STATE_INL
#define COMPILER_SHARED_STATE_INL
#pragma once

namespace stationeers {

    // SharedState<t>

    template<typename T>
    template<Status S>
    void SharedState<T>::set(state_traits<S, T> state) {
        std::vector<std::shared_ptr<CoroutineState>> waiters;
        std::vector<std::move_only_function<void()>> callbacks;

        {
            std::unique_lock lock(mutex_);

            // 检查是否已设置
            if (std::holds_alternative<Ready>(state_) || std::holds_alternative<Failed>(state_))
                throw std::runtime_error("State already set");

            // 从旧状态中提取等待者（如果当前是 Pending）
            if (auto* pending = std::get_if<Pending>(&state_)) {
                waiters   = std::move(pending->waiters);
                callbacks = std::move(pending->callbacks);
            }

            // 替换为新状态
            state_ = std::move(state);
        }

        for (auto& w : waiters)
            if (w && !w->destroyed.load()) w->handle.resume();

        for (auto& cb : callbacks) cb();

        cv_.notify_all();
    }

    template<typename T>
    std::expected<T, std::exception_ptr> SharedState<T>::get() {
        std::unique_lock lock(mutex_);

        cv_.wait(lock, [this] { return !std::holds_alternative<Pending>(state_); });

        if (auto* ready = std::get_if<Ready>(&state_)) {
            if constexpr (std::is_same_v<T, void>)
                return std::expected<T, std::exception_ptr>{};
            else
                return ready->value;
        }

        return std::unexpected{std::get<Failed>(state_).exception};
    }

    template<typename T>
    template<Status S>
    bool SharedState<T>::is() const {
        std::unique_lock lock(mutex_);

        return std::holds_alternative<state_traits<S, T>>(state_);
    }

    template<typename T>
    bool SharedState<T>::addWaiter(std::shared_ptr<CoroutineState> waiter) {
        std::unique_lock lock(mutex_);

        // 仍待决：登记等待者，调用方需要挂起，由 set() 恢复
        if (auto* pending = std::get_if<Pending>(&state_)) {
            pending->waiters.push_back(std::move(waiter));

            return true;
        }

        // 状态已定（FAILED）：不登记也不就地恢复。
        // 就地 resume 会重入调用者自身（await_suspend 正在该协程内执行），
        // 因此改为返回 false，让调用方不挂起并直接读取结果。
        return false;
    }

    template<typename T>
    void SharedState<T>::addCallback(std::move_only_function<void()> callback) {
        bool run_now = false;

        {
            std::unique_lock lock(mutex_);
            if (auto* pending = std::get_if<Pending>(&state_))
                pending->callbacks.push_back(std::move(callback));
            else
                run_now = true;
        }

        if (run_now) callback();
    }

}  // namespace stationeers

#endif  // COMPILER_SHARED_STATE_INL
