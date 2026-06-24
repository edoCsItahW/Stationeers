// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file parser_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/18 19:05
 * @if zh
 * @brief IC10语法分析器Node.js适配器
 * @details 提供IC10语法分析器(Parser)的Node.js绑定。
 *        支持将Token序列解析为抽象语法树(AST)的JS层调用。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 parser Node.js adapter
 * @details Provides Node.js bindings for IC10 parser.
 *        Supports JS layer calls to parse token sequences into abstract syntax trees (AST).
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORRT_PARSER_ADAPTER_HPP
#define EXPORRT_PARSER_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/parser/parser.hpp"

namespace stationeers::ic10 {

    /**
     * @class ParserAdapter
     * @if zh
     *
     * @brief IC10语法分析器Node.js适配器类
     * @details 提供IC10语法分析器(Parser)的Node.js绑定封装，
     *          支持JavaScript层调用语法分析功能，将Token序列解析为抽象语法树(AST)。
     *
     *
     * @elseif en
     *
     * @brief IC10 parser Node.js adapter class
     * @details Provides Node.js binding encapsulation for IC10 parser,
     *          supporting JavaScript layer calls to syntax analysis functionality,
     *          parsing token sequences into abstract syntax trees (AST).
     *
     *
     * @endif
     */
    class ParserAdapter : public node::ObjectWrap<ParserAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 根据Node.js回调信息创建ParserAdapter实例
         *
         * @param info Node.js回调信息对象
         *
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates a ParserAdapter instance based on Node.js callback information
         *
         * @param info Node.js callback information object
         *
         *
         * @endif
         *
         * @public @memberof ParserAdapter
         */
        ParserAdapter(const node::CallbackInfo& info);

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
         * @public @memberof ParserAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

    private:
        /**
         * @if zh
         *
         * @brief IC10语法分析器实例
         *
         *
         * @elseif en
         *
         * @brief IC10 parser instance
         *
         *
         * @endif
         *
         * @private @memberof ParserAdapter
         */
        Parser parser_;

        /**
         * @if zh
         *
         * @brief 解析方法
         * @details 实例方法，解析Token序列并生成抽象语法树
         *
         * @param info Node.js回调信息对象，包含Token序列参数
         * @return 抽象语法树的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Parse method
         * @details Instance method, parses token sequence and generates abstract syntax tree
         *
         * @param info Node.js callback information object, containing token sequence parameter
         * @return JavaScript object of the abstract syntax tree
         *
         *
         * @endif
         *
         * @private @memberof ParserAdapter
         */
        node::Value parse(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 静态解析方法
         * @details JavaScript层可直接调用的静态方法，用于解析源代码
         *
         * @param info Node.js回调信息对象，包含源代码字符串参数
         * @return 抽象语法树的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Static parse method
         * @details Static method directly callable from JavaScript layer, used to parse source code
         *
         * @param info Node.js callback information object, containing source code string parameter
         * @return JavaScript object of the abstract syntax tree
         *
         *
         * @endif
         *
         * @private @memberof ParserAdapter
         */
        static node::Value parsing(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_PARSER_ADAPTER_HPP
