// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file task.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:44
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common/async/task.hpp"

namespace stationeers {

    // TaskPromise<void>

    Task<> TaskPromise<void>::get_return_object() {
        return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }

    void TaskPromise<void>::return_void() { promise.set<Status::READY>({}); }


}  // namespace stationeers
