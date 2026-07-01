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
        // 创建一个Future，之后所有依赖于此的等待者都会被挂起
        if (!self.contains(name)) {
            Promise<std::shared_ptr<Symbol>> promise;

            auto future = promise.getFuture();

            self.symbols_[name] = {std::move(promise), future, pos};
        }

        return self.symbols_[name].future;
    }

    template<typename Self>
    auto SymbolTable::find(this Self& self, std::string name) {
        return self.symbols_.find(name);
    }

    template<typename Self>
    auto SymbolTable::begin(this Self& self) {
        return self.symbols_.begin();
    }

    template<typename Self>
    auto SymbolTable::end(this Self& self) {
        return self.symbols_.end();
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_SEMANTIC_INL
