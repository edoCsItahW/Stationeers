// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file symbol_table_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/22 21:52
 * @if zh
 * @brief IC10符号表Node.js适配器
 * @details 提供IC10符号表(SymbolTable)的Node.js绑定。
 *        支持符号表与JavaScript对象之间的相互转换及JSON序列化。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 symbol table Node.js adapter
 * @details Provides Node.js bindings for IC10 symbol table.
 *        Supports bidirectional conversion between symbol tables and JavaScript objects, and JSON serialization.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_SYMBOL_TABLE_ADAPTER_HPP
#define EXPORT_SYMBOL_TABLE_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/semantic/semantic.hpp"

namespace stationeers::ic10 {

    /**
     * @class SymbolTableAdapter
     * @if zh
     *
     * @brief IC10符号表Node.js适配器类
     * @details 提供IC10符号表(SymbolTable)的Node.js绑定封装，
     *          支持C++ SymbolTable对象与JavaScript对象之间的相互转换，
     *          以及符号表的JSON序列化功能。
     *
     *
     * @elseif en
     *
     * @brief IC10 symbol table Node.js adapter class
     * @details Provides Node.js binding encapsulation for IC10 symbol table,
     *          supporting bidirectional conversion between C++ SymbolTable objects and JavaScript objects,
     *          as well as JSON serialization functionality for symbol tables.
     *
     *
     * @endif
     */
    class SymbolTableAdapter : public node::ObjectWrap<SymbolTableAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 根据Node.js回调信息创建SymbolTableAdapter实例
         *
         * @param info Node.js回调信息对象
         *
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates a SymbolTableAdapter instance based on Node.js callback information
         *
         * @param info Node.js callback information object
         *
         *
         * @endif
         *
         * @public @memberof SymbolTableAdapter
         */
        SymbolTableAdapter(const node::CallbackInfo& info);

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
         * @public @memberof SymbolTableAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

        /**
         * @if zh
         *
         * @brief 将C++ SymbolTable对象转换为JavaScript值
         * @details 创建一个新的JavaScript对象并包装C++ SymbolTable对象
         *
         * @param env Node.js环境对象
         * @param self C++ SymbolTable对象引用
         * @return 包装后的JavaScript值
         *
         *
         * @elseif en
         *
         * @brief Convert C++ SymbolTable object to JavaScript value
         * @details Creates a new JavaScript object and wraps the C++ SymbolTable object
         *
         * @param env Node.js environment object
         * @param self C++ SymbolTable object reference
         * @return Wrapped JavaScript value
         *
         *
         * @endif
         *
         * @public @memberof SymbolTableAdapter
         */
        static node::Value to(node::Env env, const SymbolTable& self);

        /**
         * @if zh
         *
         * @brief 将JavaScript对象转换为C++ SymbolTable对象
         * @details 从JavaScript对象中提取并返回C++ SymbolTable对象
         *
         * @param obj JavaScript对象引用
         * @return C++ SymbolTable对象
         *
         *
         * @elseif en
         *
         * @brief Convert JavaScript object to C++ SymbolTable object
         * @details Extracts and returns the C++ SymbolTable object from a JavaScript object
         *
         * @param obj JavaScript object reference
         * @return C++ SymbolTable object
         *
         *
         * @endif
         *
         * @public @memberof SymbolTableAdapter
         */
        static SymbolTable from(const node::Object& obj);

    private:
        /**
         * @if zh
         *
         * @brief 构造函数引用
         * @details 用于保存JavaScript构造函数的引用，便于创建新实例
         *
         *
         * @elseif en
         *
         * @brief Constructor reference
         * @details Used to store a reference to the JavaScript constructor for creating new instances
         *
         *
         * @endif
         *
         * @private @memberof SymbolTableAdapter
         */
        static node::FunctionReference constructor;

        /**
         * @if zh
         *
         * @brief 符号表实例
         *
         *
         * @elseif en
         *
         * @brief Symbol table instance
         *
         *
         * @endif
         *
         * @private @memberof SymbolTableAdapter
         */
        SymbolTable symbolTable_;

        /**
         * @if zh
         *
         * @brief 转换为JSON
         * @details 将符号表转换为JSON格式的JavaScript对象
         *
         * @param info Node.js回调信息对象
         * @return JSON格式的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Convert to JSON
         * @details Converts the symbol table to a JSON format JavaScript object
         *
         * @param info Node.js callback information object
         * @return JSON format JavaScript object
         *
         *
         * @endif
         *
         * @private @memberof SymbolTableAdapter
         */
        node::Value toJSON(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_SYMBOL_TABLE_ADAPTER_HPP
