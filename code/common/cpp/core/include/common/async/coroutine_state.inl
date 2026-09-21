// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file coroutine_state.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COROUTINE_STATE_INL
#define COMPILER_COROUTINE_STATE_INL
#pragma once

namespace stationeers {

    // 非模板类定义在头文件中被多个TU包含，必须inline避免重复定义
    // Non-template definitions live in a header included by several TUs, hence inline

    inline CoroutineState::CoroutineState(Handle h) noexcept
        : handle(h) {}

    inline CoroutineState::~CoroutineState() {
        if (!destroyed.exchange(true) && handle) handle.destroy();
    }

}  // namespace stationeers

#endif  // COMPILER_COROUTINE_STATE_INL
