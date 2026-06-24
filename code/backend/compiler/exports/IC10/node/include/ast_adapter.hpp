// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/18 12:53
 * @if zh
 * @brief IC10抽象语法树Node.js适配器
 * @details 提供IC10抽象语法树(AST)的Node.js绑定。
 *        支持Program节点与JavaScript对象之间的相互转换。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 AST Node.js adapter
 * @details Provides Node.js bindings for IC10 abstract syntax tree (AST).
 *        Supports bidirectional conversion between Program nodes and JavaScript objects.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef EXPORT_AST_ADAPTER_HPP
#define EXPORT_AST_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/parser/ast/ast.hpp"

namespace stationeers::ic10 {

    /**
     * @class ProgramAdapter
     * @if zh
     *
     * @brief IC10抽象语法树Program节点Node.js适配器类
     * @details 提供IC10抽象语法树中Program节点的Node.js绑定封装，
     *          支持C++ Program对象与JavaScript对象之间的相互转换，
     *          以及节点属性的访问和操作。
     *
     *
     * @elseif en
     *
     * @brief IC10 AST Program node Node.js adapter class
     * @details Provides Node.js binding encapsulation for Program nodes in IC10 abstract syntax tree,
     *          supporting bidirectional conversion between C++ Program objects and JavaScript objects,
     *          as well as access and manipulation of node properties.
     *
     *
     * @endif
     */
    class ProgramAdapter : public node::ObjectWrap<ProgramAdapter> {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @details 根据Node.js回调信息创建ProgramAdapter实例
         *
         * @param info Node.js回调信息对象
         *
         *
         * @elseif en
         *
         * @brief Constructor
         * @details Creates a ProgramAdapter instance based on Node.js callback information
         *
         * @param info Node.js callback information object
         *
         *
         * @endif
         *
         * @public @memberof ProgramAdapter
         */
        ProgramAdapter(const node::CallbackInfo& info);

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
         * @public @memberof ProgramAdapter
         */
        static node::Object init(node::Env env, node::Object exports);

        /**
         * @if zh
         *
         * @brief 将C++ Program对象转换为JavaScript对象
         * @details 创建一个新的JavaScript对象并包装C++ Program对象
         *
         * @param env Node.js环境对象
         * @param self C++ Program对象引用
         * @return 包装后的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Convert C++ Program object to JavaScript object
         * @details Creates a new JavaScript object and wraps the C++ Program object
         *
         * @param env Node.js environment object
         * @param self C++ Program object reference
         * @return Wrapped JavaScript object
         *
         *
         * @endif
         *
         * @public @memberof ProgramAdapter
         */
        static node::Object to(node::Env env, const Program& self);

        /**
         * @if zh
         *
         * @brief 将JavaScript对象转换为C++ Program对象
         * @details 从JavaScript对象中提取并返回C++ Program对象
         *
         * @param obj JavaScript对象引用
         * @return C++ Program对象
         *
         *
         * @elseif en
         *
         * @brief Convert JavaScript object to C++ Program object
         * @details Extracts and returns the C++ Program object from a JavaScript object
         *
         * @param obj JavaScript object reference
         * @return C++ Program object
         *
         *
         * @endif
         *
         * @public @memberof ProgramAdapter
         */
        static Program from(const node::Object& obj);

    private:
        /**
         * @if zh
         *
         * @brief Program节点实例
         *
         *
         * @elseif en
         *
         * @brief Program node instance
         *
         *
         * @endif
         *
         * @private @memberof ProgramAdapter
         */
        Program program_;

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
         * @private @memberof ProgramAdapter
         */
        static node::FunctionReference constructor;

        /**
         * @if zh
         *
         * @brief 获取节点名称
         * @details 返回当前AST节点的类型名称
         *
         * @param info Node.js回调信息对象
         * @return 节点名称的JavaScript字符串
         *
         *
         * @elseif en
         *
         * @brief Get node name
         * @details Returns the type name of the current AST node
         *
         * @param info Node.js callback information object
         * @return JavaScript string of the node name
         *
         *
         * @endif
         *
         * @private @memberof ProgramAdapter
         */
        static node::Value getNodeName(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 获取语句列表
         * @details 返回Program节点包含的语句数组
         *
         * @param info Node.js回调信息对象
         * @return 语句列表的JavaScript数组
         *
         *
         * @elseif en
         *
         * @brief Get statements
         * @details Returns the statement array contained in the Program node
         *
         * @param info Node.js callback information object
         * @return JavaScript array of statement list
         *
         *
         * @endif
         *
         * @private @memberof ProgramAdapter
         */
        node::Value getStatements(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 获取结束位置
         * @details 返回Program节点在源代码中的结束位置
         *
         * @param info Node.js回调信息对象
         * @return 结束位置的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Get end position
         * @details Returns the end position of the Program node in the source code
         *
         * @param info Node.js callback information object
         * @return JavaScript object of the end position
         *
         *
         * @endif
         *
         * @private @memberof ProgramAdapter
         */
        node::Value end(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 转换为字符串
         * @details 将Program节点转换为可读的字符串表示
         *
         * @param info Node.js回调信息对象
         * @return 字符串表示的JavaScript字符串
         *
         *
         * @elseif en
         *
         * @brief Convert to string
         * @details Converts the Program node to a readable string representation
         *
         * @param info Node.js callback information object
         * @return JavaScript string of the string representation
         *
         *
         * @endif
         *
         * @private @memberof ProgramAdapter
         */
        node::Value toString(const node::CallbackInfo& info);

        /**
         * @if zh
         *
         * @brief 转换为JSON
         * @details 将Program节点转换为JSON格式的JavaScript对象
         *
         * @param info Node.js回调信息对象
         * @return JSON格式的JavaScript对象
         *
         *
         * @elseif en
         *
         * @brief Convert to JSON
         * @details Converts the Program node to a JSON format JavaScript object
         *
         * @param info Node.js callback information object
         * @return JSON format JavaScript object
         *
         *
         * @endif
         *
         * @private @memberof ProgramAdapter
         */
        node::Value toJSON(const node::CallbackInfo& info);

    };

}  // namespace stationeers::ic10

#endif  // EXPORT_AST_ADAPTER_HPP
