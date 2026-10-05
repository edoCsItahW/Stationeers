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

#include "semantic_operand.hpp"
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

    /**
     * @if zh
     * @brief 类型提示里的一条 `@default`
     * @details 设备成员用 `@default <分组> <字段> <值>`（分组为 `logic`/`logic-slot`/`slot`），
     *          寄存器用 `@default <值>`（分组与字段都为空）。
     * @else
     * @brief One `@default` entry of a type hint
     * @details A device member uses `@default <category> <field> <value>` (the category being
     *          `logic`/`logic-slot`/`slot`); a register uses `@default <value>` (no category/field).
     * @endif
     * */
    struct TypeHintDefault : AST<TypeHintDefault> {
        static constexpr auto nodeName = "TypeHintDefault"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TAG});

        /** 分组：`logic` / `logic-slot` / `slot`；寄存器默认值为空 */
        std::optional<std::string> category;

        /** 字段名（成员名）；寄存器默认值为空 */
        std::optional<std::string> name;

        /** 默认值（源码里的写法） */
        std::string value;

        Pos endPos;

        using AST<TypeHintDefault>::AST;

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    struct TypeHint : AST<TypeHint> {
        static constexpr auto nodeName = "TypeHint"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::TYPE_HINT_PREFIX});

        std::optional<std::string> type;

        std::optional<Description> desc;

        /** `@default` 条目（可多条，同一属性名只允许一次） */
        std::vector<TypeHintDefault> defaults;

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

        /**
         * @if zh
         * @brief 读写权限（可选，写在取值之后、默认值之前）
         * @details 形如 `@logic Setting 12 rw`。只有逻辑属性行（tag 为 `logic`）接受它：槽位逻辑属性
         *          的可选信息是「适用槽位序号」，槽位行的第二个字段本身就是槽位序号，都与读写权限无关。
         *          不写即 `std::nullopt`，表示**没有声明**（而非"不可读写"）。
         *
         * @elseif en
         * @brief Read/write access (optional, written after the value and before the default)
         * @details Written as `@logic Setting 12 rw`. Only logic-property lines (tag `logic`) accept
         *          it: a slot-logic line's optional information is the set of applicable slot indices,
         *          and a slot line's second field is the slot index itself — neither is a read/write
         *          access. Omitting it means **not declared** (not "neither readable nor writable").
         *
         * @endif
         * */
        std::optional<Access> access;

        /**
         * @if zh
         * @brief 适用槽位序号列表（可选，写在权限之后、默认值之前）
         * @details 形如 `@logic-slot Quantity 3 (0 1 2 3)`：该槽位逻辑属性适用于哪些槽位序号。
         *          只有槽位逻辑属性行（tag 为 `logic-slot`）接受它。**不写**（`std::nullopt`）表示
         *          「未声明」，与写 `()`（空集合，表示哪个槽位都不适用）是两回事。
         *
         * @elseif en
         * @brief List of applicable slot indices (optional, after the access and before the default)
         * @details Written as `@logic-slot Quantity 3 (0 1 2 3)`: which slot indices this slot-logic
         *          property applies to. Only slot-logic lines (tag `logic-slot`) accept it.
         *          **Omitting** it (`std::nullopt`) means "not declared" and differs from `()` (the
         *          empty set, meaning the property applies to no slot at all).
         *
         * @endif
         * */
        std::optional<std::vector<int>> slotIndices;

        /**
         * @if zh
         * @brief 默认值（可选，追加在取值之后）
         * @details 形如 `@logic Setting 12 1` 的第四个 token；不写表示**没有默认值**（而非 0），
         *          由求值方决定是报错还是以 0 顶替。
         * @else
         * @brief Default value (optional, appended after the value)
         * @details The fourth token of e.g. `@logic Setting 12 1`; omitting it means **no default**
         *          (not 0) and leaves the strict/lenient decision to the evaluator.
         * @endif
         * */
        std::optional<std::string> defaultValue;

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
