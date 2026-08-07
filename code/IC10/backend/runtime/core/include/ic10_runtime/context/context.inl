// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file context.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/10 13:46
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_CONTEXT_INL
#define IC10_RUNTIME_CONTEXT_INL
#pragma once

namespace stationeers::ic10 {

    template<typename T, typename U>
        requires std::is_arithmetic_v<std::decay_t<U>>
    T Context::getAddr(U&& line) {
        return arithmeticTrans<T>(addrs_[arithmeticTrans<std::size_t>(std::forward<U>(line))]);
    }

    template<typename T>
        requires std::is_arithmetic_v<T>
    void Context::setPC(T pc) {
        pc_ = arithmeticTrans<decltype(pc_)>(pc);
    }

    template<typename T>
    T Context::getPC() const {
        return arithmeticTrans<T>(pc_);
    }

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_CONTEXT_INL
