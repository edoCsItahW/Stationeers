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
 * @if zh
 * @brief IC10词法分析器Node.js适配器
 * @details 提供IC10词法分析器(Lexer)的Node.js绑定。
 *        支持将源代码字符串分词为Token序列的JS层调用。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 lexer Node.js adapter
 * @details Provides Node.js bindings for IC10 lexer.
 *        Supports JS layer calls to tokenize source code string into token sequence.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_LEXER_HPP
#define EXPORT_LEXER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/lexer/lexer.hpp"

namespace stationeers::ic10 {

    /**
     * @class LexerAdapter
     * @if zh
     *
     * @brief IC10词法分析器Node.js适配器类
     * @details 提供IC10词法分析器(Lexer)的Node.js绑定封装，
     *          支持JavaScript层调用词法分析功能，将源代码字符串分词为Token序列。
     *
     *
     * @elseif en
     *
     * @brief IC10 lexer Node.js adapter class
     * @details Provides Node.js binding encapsulation for IC10 lexer,
     *          supporting JavaScript layer calls to lexical analysis functionality,
     *          tokenizing source code strings into token sequences.
     *
     *
     * @endif
     */
    class LexerAdapter : public node::ObjectWrap<LexerAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 根据Node.js回调信息创建LexerAdapter实例
         *
         * @param info Node.js回调信息对象
         *
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates a LexerAdapter instance based on Node.js callback information
         *
         * @param info Node.js callback information object
         *
         *
         * @endif
         *
         * @public @memberof LexerAdapter
         */
        LexerAdapter(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 初始化适配器并导出到Node.js模块
         * @details 注册类到Node.js环境并将其添加到exports对象
         *
         * @param env Node.js环境对象
         * @param exports Node.js模块导出对象
         * @return 注册后的类构造函数对象
         *
         *
         * @elseif en
         *
         * @brief Initialize the adapter and export to Node.js module
         * @details Registers the class to the Node.js environment and adds it to the exports object
         *
         * @param env Node.js environment object
         * @param exports Node.js module exports object
         * @return Registered class constructor object
         *
         *
         * @endif
         *
         * @public @memberof LexerAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

    private:
        /**
         * @if zh
         *
         * @brief IC10词法分析器实例
         *
         *
         * @elseif en
         *
         * @brief IC10 lexer instance
         *
         *
         * @endif
         *
         * @private @memberof LexerAdapter
         */
        Lexer lexer_;

        /**
         * @if zh
         *
         * @brief 源代码字符串
         *
         *
         * @elseif en
         *
         * @brief Source code string
         *
         *
         * @endif
         *
         * @private @memberof LexerAdapter
         */
        std::string source_;

        /**
         * @if zh
         *
         * @brief 静态分词方法
         * @details JavaScript层可直接调用的静态方法，用于将源代码分词为Token序列
         *
         * @param info Node.js回调信息对象，包含源代码字符串参数
         * @return Token序列的JavaScript数组
         *
         *
         * @elseif en
         *
         * @brief Static tokenize method
         * @details Static method directly callable from JavaScript layer, used to tokenize source code into token sequence
         *
         * @param info Node.js callback information object, containing source code string parameter
         * @return JavaScript array of token sequence
         *
         *
         * @endif
         *
         * @private @memberof LexerAdapter
         */
        static node::Value tokenize(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 扫描单个Token
         * @details 从源代码中扫描并返回下一个Token
         *
         * @param info Node.js回调信息对象
         * @return 下一个Token的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Scan a single token
         * @details Scans and returns the next token from the source code
         *
         * @param info Node.js callback information object
         * @return JavaScript object of the next token
         *
         *
         * @endif
         *
         * @private @memberof LexerAdapter
         */
        node::Value scan(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10


#endif  // EXPORT_LEXER_HPP
