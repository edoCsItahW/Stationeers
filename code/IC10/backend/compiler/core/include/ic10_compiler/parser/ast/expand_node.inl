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

        // 注解里的书写顺序：`@logic Setting 12 rw 1` / `@logic-slot Quantity 3 (0 1 2 3)`
        if (access) result += " " + std::string(access_literal(*access));

        if (slotIndices) {
            std::string indices;

            for (const auto index : *slotIndices) indices += std::format("{}{}", indices.empty() ? "" : " ", index);

            result += std::format(" ({})", indices);
        }

        if (defaultValue) result += " " + *defaultValue;

        if (desc)
            result += " " + call(*desc, [](auto&& d) { return d.toString(); });

        return result;
    }

    template<FString Name, FString Tag>
    std::string TypeAnnotationLineBase<Name, Tag>::toJSON() const {
        // 约定：包进 `std::optional` 的值由 toJson 决定导出形态——有值时原样输出（以 `{` / `[`
        // 开头的 JSON 片段不加引号，普通字符串加引号），**空 optional 则整个键都不出现**
        //（与 DeviceAnnotation::toJSON 传 desc 的写法一致）。因此本函数四个可选字段一律用 optional：
        //  - access 未声明 = 该字段不出现（`access_literal` 的 `""` 只是"没有字面写法"，不是取值）
        //  - slotIndices 未声明 = 该字段不出现；`()` = 空数组 `[]`（seqJSON 产出以 `[` 开头）
        return AST<TypeAnnotationLineBase>::template jsonBase<
            "name", "value", "access", "slotIndices", "defaultValue", "desc">(
            name, value,
            access ? std::optional(std::string(access_literal(*access))) : std::nullopt,
            slotIndices ? std::optional(AST<TypeAnnotationLineBase>::template seqJSON<int>(
                              *slotIndices, [](int index) { return std::to_string(index); }
                          ))
                        : std::nullopt,
            defaultValue ? defaultValue : std::nullopt,
            desc ? std::optional(call(*desc, [](auto&& d) { return d.toJSON(); })) : std::nullopt
        );
    }

}  // namespace stationeers::ic10

#endif  // IC10_COMPILER_CORE_EXPAND_NODE_INL
