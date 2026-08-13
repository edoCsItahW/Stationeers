// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file memory.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/08 13:43
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_MEMORY_INL
#define IC10_RUNTIME_MEMORY_INL
#pragma once

#include "../value/value.hpp"

namespace stationeers::ic10 {

    template<typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    void Memory::setReg(const std::string& name, T&& val) {
        registers_[name] = arithmeticTrans<double>(std::forward<T>(val));
    }

    template<typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    T Memory::getReg(const std::string& name) {
        return registers_[name];
    }

    template<typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    T Memory::getSP() {
        return arithmeticTrans<T>(sp_);
    }

    template<typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    void Memory::setSP(T&& val) {
        sp_ = arithmeticTrans<double>(std::forward<T>(val));
    }

    template<typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    double Memory::getStack(T index) {
        // LIFO: index 0 = 栈顶（最近 push 的值）
        return arithmeticTrans<double>(stack_[static_cast<std::size_t>(sp_ - 1 - index)]);
    }

    template<typename T>
        requires std::is_arithmetic_v<std::decay_t<T>>
    void Memory::setStack(std::size_t index, T&& val) {
        // LIFO: index 0 = 栈顶
        stack_[static_cast<std::size_t>(sp_ - 1 - index)] = arithmeticTrans<double>(std::forward<T>(val));
    }

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_MEMORY_INL
