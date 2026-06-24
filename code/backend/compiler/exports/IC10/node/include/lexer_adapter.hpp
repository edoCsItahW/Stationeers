// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file lexer_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 17:37
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_LEXER_HPP
#define EXPORT_LEXER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/lexer/lexer.hpp"

namespace stationeers::ic10 {

    class LexerAdapter : public node::ObjectWrap<LexerAdapter> {
    public:
        LexerAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

    private:
        Lexer lexer_;

        std::string source_;

        static node::Value tokenize(const node::CallbackInfo& info);

        node::Value scan(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10


#endif  // EXPORT_LEXER_HPP
