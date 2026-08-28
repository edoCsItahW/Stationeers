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
#ifndef STATIONEERS_EXPAND_NODE_PARSER_HPP
#define STATIONEERS_EXPAND_NODE_PARSER_HPP
#pragma once

#include "node_parser.hpp"
#include "ast/expand_node.hpp"

namespace stationeers::ic10 {

    // Link

    template<>
    struct NodeParser<Link> {
        static ShallowErrorable<Link> parse(Parser& p) noexcept;
    };

    // TypeHintType

    template<>
    struct NodeParser<TypeHintType> {
        static TypeHintType parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // TypeHintDesc

    template<>
    struct NodeParser<TypeHintDesc> {
        static TypeHintDesc parse(Parser& p) noexcept;

        [[nodiscard]] static bool is(const Parser& p) noexcept;
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

    // DeviceAnnotationLogic

    template<>
    struct NodeParser<DeviceAnnotationLogic> {
        static DeviceAnnotationLogic parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // DeviceAnnotationLogicSlot

    template<>
    struct NodeParser<DeviceAnnotationLogicSlot> {
        static DeviceAnnotationLogicSlot parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // DeviceAnnotationDeviceHash

    template<>
    struct NodeParser<DeviceAnnotationDeviceHash> {
        static DeviceAnnotationDeviceHash parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // DeviceAnnotationNameHash

    template<>
    struct NodeParser<DeviceAnnotationNameHash> {
        static DeviceAnnotationNameHash parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // DeviceAnnotationReagentHash

    template<>
    struct NodeParser<DeviceAnnotationReagentHash> {
        static DeviceAnnotationReagentHash parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p);
    };

    // DeviceAnnotationSlot

    template<>
    struct NodeParser<DeviceAnnotationSlot> {
        static DeviceAnnotationSlot parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

    // DeviceAnnotation

    template<>
    struct NodeParser<DeviceAnnotation> {
        static ShallowErrorable<DeviceAnnotation> parse(Parser& p);

        [[nodiscard]] static bool is(const Parser& p) noexcept;
    };

}  // namespace stationeers::ic10

#endif  // STATIONEERS_EXPAND_NODE_PARSER_HPP
