// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_type_of_node.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief AST节点类型映射 Java绑定
 * @details 提供IC10编译器AST节点类型到BasicType/TypeCategory的映射，
 *          通过编译期元编程自动生成，避免手动重复代码。
 *          导出为JSON字符串供Java层解析使用。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief AST node type mapping Java binding
 * @details Provides mapping from IC10 compiler AST node types to BasicType/TypeCategory,
 *          auto-generated via compile-time metaprogramming to eliminate manual duplication.
 *          Exported as JSON string for Java layer parsing.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_TYPE_OF_NODE_HPP
#define COMPILER_J_TYPE_OF_NODE_HPP
#pragma once

#include "common_java/base.hpp"
#include "ic10_compiler/semantic/types.hpp"
#include <string>
#include <tuple>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JTypeOfNode
     * @brief AST节点类型映射Java适配器类
     * @details 提供AST节点类型到BasicType/TypeCategory的静态映射，
     *          通过编译期模板遍历类型列表自动生成JSON表示。
     *          所有方法均为静态方法，无需实例化。
     * @elseif en
     * @class JTypeOfNode
     * @brief AST node type mapping Java adapter class
     * @details Provides static mapping from AST node types to BasicType/TypeCategory,
     *          auto-generated as JSON via compile-time template traversal over type list.
     *          All methods are static, no instantiation required.
     * @endif
     */
    class JTypeOfNode : public java::JavaClass<JTypeOfNode> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/TypeOfNode;";

        /**
         * @if zh
         * @brief 获取所有节点类型映射的JSON字符串
         * @details 返回JSON对象，键为节点类型名(如"Integer"、"Float")，
         *          值为包含kind(BasicType)和category(TypeCategory)的对象。
         * @return JSON格式的类型映射字符串
         * @elseif en
         * @brief Get JSON string of all node type mappings
         * @details Returns a JSON object where keys are node type names (e.g. "Integer", "Float")
         *          and values are objects containing kind (BasicType) and category (TypeCategory).
         * @return JSON format type mapping string
         * @endif
         */
        static std::string toJSON(java::alias_ref<java::JClass>);

        /**
         * @if zh
         * @brief 注册native方法
         * @elseif en
         * @brief Register native methods
         * @endif
         */
        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_TYPE_OF_NODE_HPP
