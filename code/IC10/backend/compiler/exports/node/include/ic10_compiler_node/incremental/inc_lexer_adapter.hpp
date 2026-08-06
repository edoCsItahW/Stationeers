// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_lexer_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/05
 * @if zh
 * @brief 增量词法分析器Node.js适配器
 * @details 将IncLexer包装为Node.js可用的类
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Incremental lexer Node.js adapter
 * @details Wraps IncLexer as a Node.js class
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_IC10_NODE_INC_LEXER_ADAPTER_HPP
#define COMPILER_IC10_NODE_INC_LEXER_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10_compiler/incremental/inc_lexer.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class IncLexerAdapter
     * @brief IncLexer的Node.js适配器
     * @elseif en
     * @class IncLexerAdapter
     * @brief Node.js adapter for IncLexer
     * @endif
     */
    class IncLexerAdapter final : public node::ObjectWrap<IncLexerAdapter> {
    public:
        static node::Object init(node::Env env, node::Object exports);

        IncLexerAdapter(const node::CallbackInfo& info);

    private:
        node::Value tokenizeFull(const node::CallbackInfo& info);
        node::Value tokenizeInc(const node::CallbackInfo& info);
        node::Value hasCache(const node::CallbackInfo& info);
        void clear(const node::CallbackInfo& info);

        static node::Object resultToJS(node::Env env, const IncLexerResult& result);

        IncLexer lexer_;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_IC10_NODE_INC_LEXER_ADAPTER_HPP
