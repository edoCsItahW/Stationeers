// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file expand_node.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/19 13:50
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_COMPILER_CORE_EXPAND_NODE_INL
#define IC10_COMPILER_CORE_EXPAND_NODE_INL
#pragma once

#include "common/utils/json.hpp"

namespace stationeers::ic10 {

    // TypeAnnotationValueBase

    template<FString Name, FString Tag>
    std::string TypeAnnotationValueBase<Name, Tag>::toString() const {
        return std::format("@{} {}", std::string(Tag), LeafNode<TypeAnnotationValueBase>::value);
    }

    // TypeAnnotationLineBase

    template<FString Name, FString Tag>
    Pos TypeAnnotationLineBase<Name, Tag>::end() const {
        return endPos;
    }

    template<FString Name, FString Tag>
    std::string TypeAnnotationLineBase<Name, Tag>::toString() const {
        auto result = std::format("@{} {} {}", std::string(Tag), name, value);

        if (desc)
            result += " " + call(*desc, [](auto&& d) { return d.toString(); });

        return result;
    }

    template<FString Name, FString Tag>
    std::string TypeAnnotationLineBase<Name, Tag>::toJSON() const {
        return AST<TypeAnnotationLineBase>::template jsonBase<"name", "value", "desc">(name, value, desc ? std::optional(call(*desc, [](auto&& d) { return d.toJSON(); })) : std::nullopt);
    }

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_EXPAND_NODE_INL
