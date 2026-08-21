// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file type_of_node_adapter.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/22
 * @if zh
 * @brief AST节点类型映射 Node.js 适配器（使用编译期元编程自动生成）
 * @details 通过模板遍历类型列表，自动生成所有节点类型到 BasicType/TypeCategory 的映射，
 *          避免手动重复代码。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief AST node type mapping Node.js adapter (auto-generated via compile-time metaprogramming)
 * @details Uses template traversal over a type list to automatically generate mappings from
 *          node types to BasicType/TypeCategory, eliminating manual duplication.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_TYPE_OF_NODE_ADAPTER_HPP
#define EXPORT_TYPE_OF_NODE_ADAPTER_HPP
#pragma once

#include "ic10_compiler/semantic/types.hpp"
#include "common_node/base.hpp"
#include <string_view>
#include <tuple>

namespace stationeers::ic10 {

    // 所有需要导出的 AST 节点类型列表（需与 types.hpp 中 type_of_node 特化一致）
    using ExportedNodeTypes = std::tuple<
        Integer,
        Float,
        String,
        StrCall,
        HashCall,
        Register,
        Device,
        LabelDef,
        ErrorNode,
        Constant,
        BinaryNumber,
        HexNumber
    >;

    // 递归遍历类型列表的辅助函数（编译期展开）
    template<typename Tuple, std::size_t I = 0>
    void setTypeEntries(node::Object& obj, node::Env env) {
        if constexpr (I < std::tuple_size_v<Tuple>) {
            using T = std::tuple_element_t<I, Tuple>;
            constexpr auto typeVal = type_of<T>;

            auto entry = node::Object::New(env);
            (void)entry.Set("kind", node::Number::New(env, static_cast<int>(typeVal.kind)));
            (void)entry.Set("category", node::Number::New(env, static_cast<int>(typeVal.category)));
            (void)obj.Set(node::String::New(env, T::nodeName.value.data(), T::nodeName.value.size()), entry);

            // 继续处理下一个类型
            setTypeEntries<Tuple, I + 1>(obj, env);
        }
    }

    /**
     * @if zh
     * @brief 导出 TypeOfNode 映射对象到 Node.js（自动生成）
     * @param env Node.js 环境
     * @return 包含所有节点类型映射的 Node.js 对象
     * @elseif en
     * @brief Export TypeOfNode mapping object to Node.js (auto-generated)
     * @param env Node.js environment
     * @return Node.js object containing all node type mappings
     * @endif
     */
    inline node::Object exportTypeOfNode(node::Env env) {
        auto obj = node::Object::New(env);
        setTypeEntries<ExportedNodeTypes>(obj, env);
        return obj;
    }

}  // namespace stationeers::ic10

#endif  // EXPORT_TYPE_OF_NODE_ADAPTER_HPP