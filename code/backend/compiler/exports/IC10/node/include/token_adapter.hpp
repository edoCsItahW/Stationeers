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
 * @if zh
 * @brief IC10词法标记Node.js适配器
 * @details 提供IC10词法标记(Token)与Node.js对象之间的双向转换适配器。
 *        支持Token的类型、位置、词素、类别等属性的JS层访问。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 token Node.js adapter
 * @details Provides bidirectional conversion adapter between IC10 tokens and Node.js objects.
 *        Supports JS layer access to token properties such as type, position, lexeme, category, etc.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_TOKEN_HPP
#define EXPORT_TOKEN_HPP
#pragma once

#include "common_node/pos_adapter.hpp"
#include "ic10/lexer/token.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @class TokenAdapter
     * @brief 词法标记适配器类
     * @details 提供C++ Token类型与Node.js对象之间的双向转换。
     *          支持Token各属性的读写访问和方法调用。
     *
     * @elseif en
     *
     * @class TokenAdapter
     * @brief Token adapter class
     * @details Provides bidirectional conversion between C++ Token type and Node.js object.
     *          Supports read/write access to token properties and method calls.
     *
     * @endif
     */
    class TokenAdapter : public node::ObjectWrap<TokenAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @param info Node.js回调信息
         *
         * @elseif en
         *
         * @brief Constructor
         * @param info Node.js callback info
         *
         * @endif
         */
        TokenAdapter(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 初始化标记适配器
         * @param env Node.js环境
         * @param exports 导出对象
         * @return 配置后的导出对象
         *
         * @elseif en
         *
         * @brief Initialize token adapter
         * @param env Node.js environment
         * @param exports Exports object
         * @return Configured exports object
         *
         * @endif
         */
        static node::Object init(node::Env env, node::Object exports);

        /**
         * @if zh
         *
         * @brief 将C++ Token转换为Node.js对象
         * @param env Node.js环境
         * @param self C++ Token对象
         * @return Node.js标记对象
         *
         * @elseif en
         *
         * @brief Convert C++ Token to Node.js object
         * @param env Node.js environment
         * @param self C++ Token object
         * @return Node.js token object
         *
         * @endif
         */
        static node::Object to(node::Env env, const Token& self);

        /**
         * @if zh
         *
         * @brief 将Node.js对象转换为C++ Token
         * @param obj Node.js对象
         * @return C++ Token对象
         *
         * @elseif en
         *
         * @brief Convert Node.js object to C++ Token
         * @param obj Node.js object
         * @return C++ Token object
         *
         * @endif
         */
        static Token from(const node::Object& obj);

    private:
        /**
         * @if zh
         * @brief 内部C++ Token对象
         * @elseif en
         * @brief Internal C++ Token object
         * @endif
         */
        Token token_{};

        /**
         * @if zh
         * @brief 构造函数引用
         * @elseif en
         * @brief Constructor reference
         * @endif
         */
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
