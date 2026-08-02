// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_type_of_node.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_java/semantic/j_type_of_node.hpp"

#include <sstream>

namespace stationeers::ic10 {

    namespace {

        /**
         * @if zh
         * @brief 需要导出的AST节点类型列表
         * @details 与Node.js/Python导出保持一致，包含所有需要类型映射的节点类型。
         * @elseif en
         * @brief List of AST node types to export
         * @details Consistent with Node.js/Python exports, includes all node types requiring type mapping.
         * @endif
         */
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

        /**
         * @if zh
         * @brief 递归遍历类型列表构建JSON字符串
         * @details 通过编译期模板递归，为每个节点类型生成对应的JSON条目。
         * @param oss 输出字符串流
         * @param isFirst 是否为第一个条目(用于控制逗号分隔)
         * @elseif en
         * @brief Recursively traverse type list to build JSON string
         * @details Uses compile-time template recursion to generate JSON entries for each node type.
         * @param oss Output string stream
         * @param isFirst Whether this is the first entry (for comma separation control)
         * @endif
         */
        template<typename Tuple, std::size_t I = 0>
        void appendTypeEntry(std::ostringstream& oss, bool& isFirst) {
            if constexpr (I < std::tuple_size_v<Tuple>) {
                using T = std::tuple_element_t<I, Tuple>;
                constexpr auto typeVal = type_of<T>;

                if (!isFirst) {
                    oss << ",";
                }
                isFirst = false;

                oss << "\"" << std::string(T::nodeName) << "\":{"
                    << "\"kind\":" << static_cast<int>(typeVal.kind) << ","
                    << "\"category\":" << static_cast<int>(typeVal.category)
                    << "}";

                appendTypeEntry<Tuple, I + 1>(oss, isFirst);
            }
        }

    }  // namespace

    std::string JTypeOfNode::toJSON(java::alias_ref<java::JClass>) {
        std::ostringstream oss;
        bool isFirst = true;

        oss << "{";
        appendTypeEntry<ExportedNodeTypes>(oss, isFirst);
        oss << "}";

        return oss.str();
    }

    void JTypeOfNode::registerNatives() {
        javaClassStatic()->registerNatives({
            makeNativeMethod("toJSON", JTypeOfNode::toJSON),
        });
    }

}  // namespace stationeers::ic10
