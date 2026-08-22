// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file semantic.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:45
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_SEMANTIC_INL
#define COMPILER_SEMANTIC_INL
#pragma once

namespace stationeers::ic10 {

    // SymbolTable

    template<typename Self>
    auto& SymbolTable::resolve(this Self& self, std::string name, const Pos& pos) {
        // 直接检查 symbols_ map 是否已有条目，而不使用 contains()。
        // contains() 只对已就绪（READY）的条目返回 true，pending/FAILED 条目均返回 false。
        // 若使用 contains()，对同一个未定义符号的多次 resolve() 会反复创建新 pending 条目
        // 覆盖旧条目，导致旧 Future 的等待者永远挂起。
        // We check symbols_ directly instead of using contains(), because contains() only
        // returns true for READY entries. Using contains() here would cause repeated resolve()
        // calls for the same undefined symbol to overwrite the existing pending entry, leaving
        // waiters on the old Future suspended forever.
        if (auto it = self.symbols_.find(name); it == self.symbols_.end()) {
            Promise<std::shared_ptr<Symbol>> promise;

            auto future = promise.getFuture();

            self.symbols_[name] = {std::move(promise), future, pos};
        }

        return self.symbols_[name].future;
    }

    template<typename Self>
    auto SymbolTable::find(this Self& self, std::string name) noexcept {
        return self.symbols_.find(name);
    }

    template<typename Self>
    auto SymbolTable::begin(this Self& self) noexcept {
        return self.symbols_.begin();
    }

    template<typename Self>
    auto SymbolTable::end(this Self& self) noexcept {
        return self.symbols_.end();
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_SEMANTIC_INL
