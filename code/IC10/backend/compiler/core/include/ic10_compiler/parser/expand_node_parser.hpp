// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file expand_node_parser.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/01 18:21
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_COMPILER_CORE_EXPAND_NODE_PARSER_HPP
#define IC10_COMPILER_CORE_EXPAND_NODE_PARSER_HPP
#pragma once

#include "node_parser.hpp"
#include "ast/expand_node.hpp"

namespace stationeers::ic10 {

    // Link

    template<>
    struct NodeParser<Link> {
        static ShallowErrorable<Link> parse(Parser& p) noexcept;
    };

    // TypeHint

    template<>
    struct NodeParser<TypeHint> {
        static TypeHint parse(Parser& p) noexcept;
    };

    // EnumAnnotationValue

    template<>
    struct NodeParser<EnumAnnotationValue> {
        static ShallowErrorable<EnumAnnotationValue> parse(Parser& p) noexcept;

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // EnumAnnotation

    template<>
    struct NodeParser<EnumAnnotation> {
        static ShallowErrorable<EnumAnnotation> parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // TypeAnnotationLineBase<Name, Tag>

    template<FString Name, FString Tag>
    struct NodeParser<TypeAnnotationLineBase<Name, Tag>> {
        static TypeAnnotationLineBase<Name, Tag> parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // TypeAnnotationValueBase<Name, Tag>

    template<FString Name, FString Tag>
    struct NodeParser<TypeAnnotationValueBase<Name, Tag>> {
        static TypeAnnotationValueBase<Name, Tag> parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // DeviceAnnotation

    template<>
    struct NodeParser<DeviceAnnotation> {
        static ShallowErrorable<DeviceAnnotation> parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

}  // namespace stationeers::ic10

#include "expand_node_parser.inl"

#endif  // IC10_COMPILER_CORE_EXPAND_NODE_PARSER_HPP
