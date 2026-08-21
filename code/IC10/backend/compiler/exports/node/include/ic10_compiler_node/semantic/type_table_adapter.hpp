// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file type_table_adapter.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/26
 * @if zh
 * @brief IC10类型表Node.js适配器
 * @details 提供IC10类型表(TypeTable)的Node.js绑定。
 *        支持类型表与JavaScript对象之间的相互转换及JSON序列化。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 type table Node.js adapter
 * @details Provides Node.js bindings for IC10 type table.
 *        Supports bidirectional conversion between type tables and JavaScript objects, and JSON serialization.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_TYPE_TABLE_ADAPTER_HPP
#define EXPORT_TYPE_TABLE_ADAPTER_HPP
#pragma once

#include "ic10_compiler/semantic/types.hpp"
#include "common_node/base.hpp"

namespace stationeers::ic10 {

    /**
     * @class TypeTableAdapter
     * @if zh
     *
     * @brief IC10类型表Node.js适配器类
     * @details 提供IC10类型表(TypeTable)的Node.js绑定封装，
     *          支持C++ TypeTable对象与JavaScript对象之间的相互转换，
     *          以及类型表的JSON序列化功能。
     *
     *
     * @elseif en
     *
     * @brief IC10 type table Node.js adapter class
     * @details Provides Node.js binding encapsulation for IC10 type table,
     *          supporting bidirectional conversion between C++ TypeTable objects and JavaScript objects,
     *          as well as JSON serialization functionality for type tables.
     *
     *
     * @endif
     */
    class TypeTableAdapter : public node::ObjectWrap<TypeTableAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 根据Node.js回调信息创建TypeTableAdapter实例
         *
         * @param info Node.js回调信息对象
         *
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates a TypeTableAdapter instance based on Node.js callback information
         *
         * @param info Node.js callback information object
         *
         *
         * @endif
         *
         * @public @memberof TypeTableAdapter
         */
        TypeTableAdapter(const node::CallbackInfo& info);

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
         * @public @memberof TypeTableAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

        /**
         * @if zh
         *
         * @brief 将C++ TypeTable对象转换为JavaScript值
         * @details 创建一个新的JavaScript对象并包装C++ TypeTable对象
         *
         * @param env Node.js环境对象
         * @param self C++ TypeTable对象引用
         * @return 包装后的JavaScript值
         *
         *
         * @elseif en
         *
         * @brief Convert C++ TypeTable object to JavaScript value
         * @details Creates a new JavaScript object and wraps the C++ TypeTable object
         *
         * @param env Node.js environment object
         * @param self C++ TypeTable object reference
         * @return Wrapped JavaScript value
         *
         *
         * @endif
         *
         * @public @memberof TypeTableAdapter
         */
        static node::Value to(node::Env env, const TypeTable& self);

        /**
         * @if zh
         *
         * @brief 将JavaScript对象转换为C++ TypeTable对象
         * @details 从JavaScript对象中提取并返回C++ TypeTable对象
         *
         * @param obj JavaScript对象引用
         * @return C++ TypeTable对象
         *
         *
         * @elseif en
         *
         * @brief Convert JavaScript object to C++ TypeTable object
         * @details Extracts and returns the C++ TypeTable object from a JavaScript object
         *
         * @param obj JavaScript object reference
         * @return C++ TypeTable object
         *
         *
         * @endif
         *
         * @public @memberof TypeTableAdapter
         */
        static TypeTable from(const node::Object& obj);

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
         * @private @memberof TypeTableAdapter
         */
        static node::FunctionReference constructor;

        /**
         * @if zh
         *
         * @brief 类型表实例
         *
         *
         * @elseif en
         *
         * @brief Type table instance
         *
         *
         * @endif
         *
         * @private @memberof TypeTableAdapter
         */
        TypeTable typeTable_;

        /**
         * @if zh
         *
         * @brief 转换为JSON
         * @details 将类型表转换为JSON格式的JavaScript对象
         *
         * @param info Node.js回调信息对象
         * @return JSON格式的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Convert to JSON
         * @details Converts the type table to a JSON format JavaScript object
         *
         * @param info Node.js callback information object
         * @return JSON format JavaScript object
         *
         *
         * @endif
         *
         * @private @memberof TypeTableAdapter
         */
        node::Value toJSON(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_TYPE_TABLE_ADAPTER_HPP
