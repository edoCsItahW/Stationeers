// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file analyser_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/22 18:05
 * @if zh
 * @brief IC10语义分析器Node.js适配器
 * @details 提供IC10语义分析器(Analyser)的Node.js绑定。
 *        支持对抽象语法树进行语义分析、错误检测和符号表构建。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 semantic analyser Node.js adapter
 * @details Provides Node.js bindings for IC10 semantic analyser.
 *        Supports semantic analysis, error detection, and symbol table construction for abstract syntax trees.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_ANALYSER_ADAPTER_HPP
#define EXPORT_ANALYSER_ADAPTER_HPP
#pragma once
#include "common_node/base.hpp"
#include "ic10/semantic/analyser.hpp"

namespace stationeers::ic10 {

    /**
     * @class AnalyserAdapter
     * @if zh
     *
     * @brief IC10语义分析器Node.js适配器类
     * @details 提供IC10语义分析器(Analyser)的Node.js绑定封装，
     *          支持JavaScript层调用语义分析功能，对抽象语法树进行
     *          类型检查、作用域分析、错误检测和符号表构建。
     *
     *
     * @elseif en
     *
     * @brief IC10 semantic analyser Node.js adapter class
     * @details Provides Node.js binding encapsulation for IC10 semantic analyser,
     *          supporting JavaScript layer calls to semantic analysis functionality,
     *          performing type checking, scope analysis, error detection,
     *          and symbol table construction on abstract syntax trees.
     *
     *
     * @endif
     */
    class AnalyserAdapter : public node::ObjectWrap<AnalyserAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 根据Node.js回调信息创建AnalyserAdapter实例
         *
         * @param info Node.js回调信息对象
         *
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates an AnalyserAdapter instance based on Node.js callback information
         *
         * @param info Node.js callback information object
         *
         *
         * @endif
         *
         * @public @memberof AnalyserAdapter
         */
        AnalyserAdapter(const node::CallbackInfo& info);

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
         * @public @memberof AnalyserAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

    private:
        /**
         * @if zh
         *
         * @brief IC10语义分析器实例
         *
         *
         * @elseif en
         *
         * @brief IC10 semantic analyser instance
         *
         *
         * @endif
         *
         * @private @memberof AnalyserAdapter
         */
        Analyser analyser_;

        /**
         * @if zh
         *
         * @brief 访问AST节点
         * @details 实例方法，访问并分析抽象语法树节点
         *
         * @param info Node.js回调信息对象，包含AST节点参数
         * @return 分析结果的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Visit AST node
         * @details Instance method, visits and analyzes abstract syntax tree nodes
         *
         * @param info Node.js callback information object, containing AST node parameter
         * @return JavaScript object of the analysis result
         *
         *
         * @endif
         *
         * @private @memberof AnalyserAdapter
         */
        node::Value visit(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 静态语义分析方法
         * @details JavaScript层可直接调用的静态方法，用于对源代码进行完整的语义分析
         *
         * @param info Node.js回调信息对象，包含源代码或AST参数
         * @return 语义分析结果的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Static semantic analysis method
         * @details Static method directly callable from JavaScript layer, used to perform complete semantic analysis on source code
         *
         * @param info Node.js callback information object, containing source code or AST parameter
         * @return JavaScript object of the semantic analysis result
         *
         *
         * @endif
         *
         * @private @memberof AnalyserAdapter
         */
        static node::Value analyse(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 获取错误列表
         * @details 返回语义分析过程中收集到的所有错误信息
         *
         * @param info Node.js回调信息对象
         * @return 错误列表的JavaScript数组
         *
         *
         * @elseif en
         *
         * @brief Get error list
         * @details Returns all error information collected during the semantic analysis process
         *
         * @param info Node.js callback information object
         * @return JavaScript array of error list
         *
         *
         * @endif
         *
         * @private @memberof AnalyserAdapter
         */
        node::Value getErrors(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 获取符号表
         * @details 返回语义分析过程中构建的符号表
         *
         * @param info Node.js回调信息对象
         * @return 符号表的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Get symbol table
         * @details Returns the symbol table constructed during the semantic analysis process
         *
         * @param info Node.js callback information object
         * @return JavaScript object of the symbol table
         *
         *
         * @endif
         *
         * @private @memberof AnalyserAdapter
         */
        node::Value getSymbolTable(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_ANALYSER_ADAPTER_HPP
