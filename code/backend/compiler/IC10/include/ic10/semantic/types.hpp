// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/16 10:15
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_TYPES_HPP
#define COMPILER_TYPES_HPP
#pragma once

#include "ic10/parser/ast/ast_node.hpp"

namespace stationeers::ic10 {

    enum class BasicType { STRING, INTEGER, FLOAT, REGISTER, DEVICE, UNKNOWN, ENUM };

    struct DeviceType {
        std::string name;

        std::optional<DescValue> desc;

        std::vector<DeviceSlot> slots;

        std::vector<DeviceLogic> logics;

        std::vector<DeviceMode> modes;

        std::vector<DeviceLogicSlot> logicSlots;

        std::vector<DeviceConnect> connects;
    };

    struct EnumType {
        std::string name;

        std::optional<DescValue> desc;

        std::vector<EnumValueEntry> values;
    };

    using CustomType = std::variant<DeviceType, EnumType>;

    enum class TypeCategory { LABEL, STR_CALL, HASH_CALL, CONSTANT, NUMBER, BASIC };

    struct Type {
        BasicType kind = BasicType::UNKNOWN;

        TypeCategory category = TypeCategory::BASIC;

        std::optional<std::string> typeName = std::nullopt;
    };

    /**
     * @if zh
     *
     * @struct type_of_node
     * @brief 节点类型萃取模板
     * @details 从AST节点类型萃取对应的Type枚举值
     *
     * @tparam T AST节点类型
     *
     * @elseif en
     *
     * @struct type_of_node
     * @brief Node type extraction template
     * @details Extracts corresponding Type enum value from AST node type
     *
     * @tparam T AST node type
     *
     * @endif
     */
    template<typename>
    struct type_of_node {
        /**
         * @if zh
         * @brief 默认类型值
         * @elseif en
         * @brief Default type value
         * @endif
         */
        static constexpr Type value{BasicType::UNKNOWN, TypeCategory::BASIC};
    };

    template<>
    struct type_of_node<Integer> {
        static constexpr Type value{BasicType::INTEGER, TypeCategory::NUMBER};
    };

    template<>
    struct type_of_node<Float> {
        static constexpr Type value{BasicType::FLOAT, TypeCategory::NUMBER};
    };

    template<>
    struct type_of_node<String> {
        static constexpr Type value{BasicType::STRING, TypeCategory::BASIC};
    };

    template<>
    struct type_of_node<StrCall> {
        static constexpr Type value{BasicType::INTEGER, TypeCategory::STR_CALL};
    };

    template<>
    struct type_of_node<HashCall> {
        static constexpr Type value{BasicType::INTEGER, TypeCategory::HASH_CALL};
    };

    template<>
    struct type_of_node<Register> {
        static constexpr Type value{BasicType::REGISTER, TypeCategory::BASIC};
    };

    template<>
    struct type_of_node<Device> {
        static constexpr Type value{BasicType::DEVICE, TypeCategory::BASIC};
    };

    template<>
    struct type_of_node<LabelDef> {
        static constexpr Type value{BasicType::INTEGER, TypeCategory::LABEL};
    };

    template<>
    struct type_of_node<ErrorNode> {
        static constexpr Type value{BasicType::UNKNOWN, TypeCategory::BASIC};
    };

    template<>
    struct type_of_node<Constant> {
        static constexpr Type value{BasicType::UNKNOWN, TypeCategory::CONSTANT};
    };

    template<>
    struct type_of_node<BinaryNumber> {
        static constexpr Type value{BasicType::INTEGER, TypeCategory::NUMBER};
    };

    template<>
    struct type_of_node<HexNumber> {
        static constexpr Type value{BasicType::INTEGER, TypeCategory::NUMBER};
    };

    template<typename T>
    inline constexpr auto type_of = type_of_node<T>::value;

    class TypeTable {
    public:
        void registerType(CustomType type);

        [[nodiscard]] const CustomType* find(const std::string& name) const;

    private:
        std::unordered_map<std::string, CustomType> types_;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_TYPES_HPP
