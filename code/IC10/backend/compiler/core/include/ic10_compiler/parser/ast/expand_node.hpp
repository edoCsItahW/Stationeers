// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file expand_node.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/01 15:11
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_COMPILER_CORE_EXPAND_NODE_HPP
#define IC10_COMPILER_CORE_EXPAND_NODE_HPP
#pragma once

#include "node.hpp"

namespace stationeers::ic10 {

    // Link

    struct Link : AST<Link> {
        static constexpr auto nodeName = "Link"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::DOT});

        std::vector<std::string> fields;

        std::vector<std::string> paths;

        Pos endPos;

        AST_NODE_PRE_DEFINED_METHODS(Link)
    };

    using Description = ShallowErrorable<Link, String>;

    // TypeHint

    struct TypeHint : AST<TypeHint> {
        static constexpr auto nodeName = "TypeHint"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TYPE_HINT_PREFIX});

        std::optional<std::string> type;

        std::optional<Description> desc;

        bool builtin = false;

        Pos endPos;

        AST_NODE_PRE_DEFINED_METHODS(TypeHint)
    };

    // TypeAnnotationValueBase

    template<FString Name, FString Tag>
    struct TypeAnnotationValueBase : LeafNode<TypeAnnotationValueBase<Name, Tag>> {
        static constexpr auto nodeName = Name;

        static constexpr auto tag = Tag;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TAG});

        using LeafNode<TypeAnnotationValueBase>::LeafNode;

        [[nodiscard]] std::string toString() const override;
    };

    // TypeAnnotationLineBase

    template<FString Name, FString Tag>
    struct TypeAnnotationLineBase : AST<TypeAnnotationLineBase<Name, Tag>> {
        static constexpr auto nodeName = Name;

        static constexpr auto tag = Tag;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TAG});

        using AST<TypeAnnotationLineBase>::AST;

        std::string name;

        std::string value;

        std::optional<Description> desc;

        Pos endPos;

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // EnumAnnotationValue

    struct EnumAnnotationValue : AST<EnumAnnotationValue> {
        static constexpr auto nodeName = "EnumAnnotationValue"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TAG});

        std::string name;

        std::string value;

        std::optional<Description> desc;

        Pos endPos;

        AST_NODE_PRE_DEFINED_METHODS(EnumAnnotationValue)
    };

    // EnumAnnotation

    struct EnumAnnotation : AST<EnumAnnotation> {
        static constexpr auto nodeName = "EnumAnnotation"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TYPE_ANNOTATION_PREFIX});

        bool isEnd = false;

        std::string name;

        std::optional<Description> desc;

        std::vector<EnumAnnotationValue> values;

        Pos endPos;

        AST_NODE_PRE_DEFINED_METHODS(EnumAnnotation)
    };

    // DeviceAnnotation

    using DeviceAnnotationLogic = TypeAnnotationLineBase<"DeviceAnnotationLogic", "logic">;

    using DeviceAnnotationLogicSlot = TypeAnnotationLineBase<"DeviceAnnotationLogicSlot", "logic-slot">;

    using DeviceAnnotationDeviceHash = TypeAnnotationValueBase<"DeviceAnnotationDeviceHash", "device-hash">;

    using DeviceAnnotationNameHash = TypeAnnotationValueBase<"DeviceAnnotationNameHash", "name-hash">;

    using DeviceAnnotationReagentHash = TypeAnnotationValueBase<"DeviceAnnotationReagentHash", "reagent-hash">;

    using DeviceAnnotationSlot = TypeAnnotationLineBase<"DeviceAnnotationSlot", "slot">;

    struct DeviceAnnotation : AST<DeviceAnnotation> {
        static constexpr auto nodeName = "DeviceAnnotation"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TYPE_ANNOTATION_PREFIX});

        bool isEnd = false;

        std::string name;

        std::optional<Description> desc;

        std::optional<DeviceAnnotationDeviceHash> deviceHash;

        std::optional<DeviceAnnotationNameHash> nameHash;

        std::vector<DeviceAnnotationLogic> logics;

        std::vector<DeviceAnnotationLogicSlot> logicSlots;

        std::vector<DeviceAnnotationReagentHash> reagentHashes;

        std::vector<DeviceAnnotationSlot> slots;

        Pos endPos;

        AST_NODE_PRE_DEFINED_METHODS(DeviceAnnotation)
    };

    // TypeAnnotation

    using TypeAnnotation = ShallowErrorable<EnumAnnotation, DeviceAnnotation>;

}  // namespace stationeers::ic10

#include "expand_node.inl"

#endif  // IC10_COMPILER_CORE_EXPAND_NODE_HPP
