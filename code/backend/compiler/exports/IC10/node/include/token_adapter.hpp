// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file token_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 18:15
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_TOKEN_HPP
#define EXPORT_TOKEN_HPP
#pragma once

#include "common_node/pos_adapter.hpp"
#include "ic10/lexer/token.hpp"

namespace stationeers::ic10 {

    class TokenAdapter : public node::ObjectWrap<TokenAdapter> {
    public:
        TokenAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

        static node::Object to(node::Env env, const Token& self);

        static Token from(const node::Object& obj);

    private:
        Token token_{};

        static node::FunctionReference constructor;

        EXPORT_D_ATTR(Type)

        EXPORT_D_ATTR(Pos)

        EXPORT_D_ATTR(Lexeme)

        EXPORT_D_ATTR(Category)

        EXPORT_D_METHOD_VALUE(toString)

        EXPORT_D_METHOD_VALUE(toJSON)
    };

}  // namespace stationeers::ic10


#endif  // EXPORT_TOKEN_HPP
