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
 * @if zh
 * @brief IC10类型系统定义
 * @details 定义IC10编程语言的类型系统，包括基础类型、类型类别、自定义类型（设备类型和枚举类型）
 *          以及类型表。支持类型推导和类型检查。
 *
 * @note 类型系统是语义分析的核心组件，用于操作数类型验证和符号类型推导。
 *
 * @par 类型层次结构:
 * ```
 * Type
 * ├── BasicType (基础类型)
 * │   ├── STRING   - 字符串类型
 * │   ├── INTEGER  - 整数类型
 * │   ├── FLOAT    - 浮点类型
 * │   ├── REGISTER - 寄存器类型
 * │   ├── DEVICE   - 设备类型
 * │   ├── ENUM     - 枚举类型
 * │   └── UNKNOWN  - 未知类型
 * │
 * └── TypeCategory (类型类别)
 *     ├── LABEL    - 标签
 *     ├── STR_CALL - STR宏调用
 *     ├── HASH_CALL - HASH宏调用
 *     ├── CONSTANT - 常量
 *     ├── NUMBER   - 数字
 *     └── BASIC    - 基础类型
 * ```
 *
 * @see Analyser 语义分析器
 * @see Symbol 符号结构
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 type system definitions
 * @details Defines the type system for IC10 programming language, including basic types, type categories,
 *          custom types (device types and enum types), and type table. Supports type deduction and type checking.
 *
 * @note The type system is a core component of semantic analysis, used for operand type validation
 *       and symbol type deduction.
 *
 * @par Type Hierarchy:
 * ```
 * Type
 * ├── BasicType (Basic Types)
 * │   ├── STRING   - String type
 * │   ├── INTEGER  - Integer type
 * │   ├── FLOAT    - Float type
 * │   ├── REGISTER - Register type
 * │   ├── DEVICE   - Device type
 * │   ├── ENUM     - Enum type
 * │   └── UNKNOWN  - Unknown type
 * │
 * └── TypeCategory (Type Categories)
 *     ├── LABEL    - Label
 *     ├── STR_CALL - STR macro call
 *     ├── HASH_CALL - HASH macro call
 *     ├── CONSTANT - Constant
 *     ├── NUMBER   - Number
 *     └── BASIC    - Basic type
 * ```
 *
 * @see Analyser Semantic analyser
 * @see Symbol Symbol structure
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_TYPES_HPP
#define COMPILER_TYPES_HPP
#pragma once

#include "ic10_compiler/pch/ast.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @enum BasicType
     * @brief IC10基础类型枚举
     * @details 定义IC10编程语言支持的基础数据类型。
     *
     * @elseif en
     * @enum BasicType
     * @brief IC10 basic type enum
     * @details Defines basic data types supported by IC10 programming language.
     *
     * @endif
     */
    enum class BasicType {
        /** @if zh @brief 字符串类型 @else @brief String type @endif */
        STRING,

        /** @if zh @brief 整数类型 @else @brief Integer type @endif */
        INTEGER,

        /** @if zh @brief 浮点类型 @else @brief Float type @endif */
        FLOAT,

        /** @if zh @brief 寄存器类型(r0-r15) @else @brief Register type (r0-r15) @endif */
        REGISTER,

        /** @if zh @brief 设备类型(@开头) @else @brief Device type (starts with @) @endif */
        DEVICE,

        /** @if zh @brief 未知类型(解析错误) @else @brief Unknown type (parse error) @endif */
        UNKNOWN,

        /** @if zh @brief 枚举类型 @else @brief Enum type @endif */
        ENUM
    };

    /**
     * @if zh
     * @struct DeviceType
     * @brief 设备类型结构
     * @details 定义IC10中设备的完整类型信息，包括设备名、描述、槽位、逻辑、模式、逻辑槽位和连接。
     *          由文档注释(@device)解析生成，用于设备别名的类型检查。
     *
     * @elseif en
     * @struct DeviceType
     * @brief Device type structure
     * @details Defines complete type information for devices in IC10, including device name, description,
     *          slots, logics, modes, logic slots, and connections. Generated from doc comment (@device)
     *          parsing, used for type checking of device aliases.
     *
     * @endif
     */
    struct DeviceType {
        /** @if zh @brief 设备名称 @else @brief Device name @endif */
        std::string name;

        /** @if zh @brief 设备描述 @else @brief Device description @endif */
        std::optional<DescValue> desc;

        /** @if zh @brief 设备槽位列表 @else @brief Device slots list @endif */
        std::vector<DeviceSlot> slots;

        /** @if zh @brief 设备逻辑列表 @else @brief Device logics list @endif */
        std::vector<DeviceLogic> logics;

        /** @if zh @brief 设备模式列表 @else @brief Device modes list @endif */
        std::vector<DeviceMode> modes;

        /** @if zh @brief 设备逻辑槽位列表 @else @brief Device logic slots list @endif */
        std::vector<DeviceLogicSlot> logicSlots;

        /** @if zh @brief 设备连接列表 @else @brief Device connections list @endif */
        std::vector<DeviceConnect> connects;
    };

    /**
     * @if zh
     * @struct EnumType
     * @brief 枚举类型结构
     * @details 定义IC10中枚举的完整类型信息，包括枚举名、描述和值列表。
     *          由文档注释(@enum)解析生成，用于枚举值的类型检查。
     *
     * @elseif en
     * @struct EnumType
     * @brief Enum type structure
     * @details Defines complete type information for enums in IC10, including enum name, description,
     *          and values list. Generated from doc comment (@enum) parsing, used for type checking
     *          of enum values.
     *
     * @endif
     */
    struct EnumType {
        /** @if zh @brief 枚举名称 @else @brief Enum name @endif */
        std::string name;

        /** @if zh @brief 枚举描述 @else @brief Enum description @endif */
        std::optional<DescValue> desc;

        /** @if zh @brief 枚举值列表 @else @brief Enum values list @endif */
        std::vector<EnumValueEntry> values;
    };

    /**
     * @if zh
     * @typedef CustomType
     * @brief 自定义类型
     * @details 设备类型和枚举类型的联合类型，用于类型表中存储自定义类型定义。
     *
     * @elseif en
     * @typedef CustomType
     * @brief Custom type
     * @details Union type of device type and enum type, used for storing custom type definitions
     *          in the type table.
     *
     * @endif
     */
    using CustomType = std::variant<DeviceType, EnumType>;

    /**
     * @if zh
     * @enum TypeCategory
     * @brief 类型类别枚举
     * @details 定义类型的语义类别，用于区分不同语义用途的类型。
     *
     * @elseif en
     * @enum TypeCategory
     * @brief Type category enum
     * @details Defines semantic category of types, used to distinguish types with different semantic purposes.
     *
     * @endif
     */
    enum class TypeCategory {
        /** @if zh @brief 标签类别 @else @brief Label category @endif */
        LABEL,

        /** @if zh @brief STR宏调用类别 @else @brief STR macro call category @endif */
        STR_CALL,

        /** @if zh @brief HASH宏调用类别 @else @brief HASH macro call category @endif */
        HASH_CALL,

        /** @if zh @brief 常量类别 @else @brief Constant category @endif */
        CONSTANT,

        /** @if zh @brief 数字类别 @else @brief Number category @endif */
        NUMBER,

        /** @if zh @brief 基础类别 @else @brief Basic category @endif */
        BASIC
    };

    /**
     * @if zh
     * @struct Type
     * @brief 类型结构
     * @details 表示IC10程序的完整类型信息，包括基础类型、类别和可选的类型名。
     *          用于符号表中存储符号的类型信息。
     *
     * @elseif en
     * @struct Type
     * @brief Type structure
     * @details Represents complete type information for IC10 programs, including basic type,
     *          category, and optional type name. Used in symbol table to store type information
     *          of symbols.
     *
     * @endif
     */
    struct Type {
        /** @if zh @brief 基础类型 @else @brief Basic type @endif */
        BasicType kind = BasicType::UNKNOWN;

        /** @if zh @brief 类型类别 @else @brief Type category @endif */
        TypeCategory category = TypeCategory::BASIC;

        /** @if zh @brief 类型名(用于自定义类型) @else @brief Type name (for custom types) @endif */
        std::optional<std::string> typeName = std::nullopt;
    };

    /**
     * @if zh
     *
     * @struct type_of_node
     * @brief 节点类型萃取模板
     * @details 从AST节点类型萃取对应的Type枚举值，用于编译期类型推导。
     *
     * @tparam T AST节点类型
     *
     * @elseif en
     *
     * @struct type_of_node
     * @brief Node type extraction template
     * @details Extracts corresponding Type enum value from AST node type, used for compile-time type deduction.
     *
     * @tparam T AST node type
     *
     * @endif
     */
    template<typename>
    struct type_of_node {
        /**
         * @if zh @brief 默认类型值(UNKNOWN) @else @brief Default type value (UNKNOWN) @endif
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

    /**
     * @if zh
     * @brief 节点类型萃取变量模板
     * @details 便捷变量模板，用于获取AST节点对应的类型。
     *
     * @tparam T AST节点类型
     *
     * @elseif en
     * @brief Node type extraction variable template
     * @details Convenience variable template for getting the type corresponding to an AST node.
     *
     * @tparam T AST node type
     *
     * @endif
     */
    template<typename T>
    inline constexpr auto type_of = type_of_node<T>::value;

    /**
     * @if zh
     * @class TypeTable
     * @brief 类型表类
     * @details 管理IC10程序中的自定义类型定义，包括设备类型和枚举类型。
     *          支持类型注册和查找操作。
     *
     * @elseif en
     * @class TypeTable
     * @brief Type table class
     * @details Manages custom type definitions in IC10 programs, including device types and enum types.
     *          Supports type registration and lookup operations.
     *
     * @endif
     */
    class TypeTable {
    public:
        /**
         * @if zh
         *
         * @brief 注册自定义类型
         * @details 将自定义类型（设备类型或枚举类型）注册到类型表中。
         *
         * @param type 自定义类型
         *
         * @elseif en
         *
         * @brief Register custom type
         * @details Registers a custom type (device type or enum type) to the type table.
         *
         * @param type Custom type
         *
         * @endif
         */
        void registerType(CustomType type);

        /**
         * @if zh
         *
         * @brief 查找自定义类型
         * @details 根据类型名查找类型表中是否存在对应的自定义类型。
         *
         * @param name 类型名
         * @return 如果找到则返回指向自定义类型的指针，否则返回空指针
         *
         * @elseif en
         *
         * @brief Find custom type
         * @details Looks up whether a custom type with the given name exists in the type table.
         *
         * @param name Type name
         * @return Pointer to custom type if found, nullptr otherwise
         *
         * @endif
         */
        [[nodiscard]] const CustomType* find(const std::string& name) const;

        /**
         * @if zh
         *
         * @brief 转换为JSON格式
         * @return JSON格式的字符串
         *
         * @elseif en
         *
         * @brief Convert to JSON format
         * @return JSON format string
         *
         * @endif
         */
        [[nodiscard]] std::string toJSON() const;

    private:
        /** @if zh @brief 类型存储映射表 @else @brief Type storage map @endif */
        std::unordered_map<std::string, CustomType> types_;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_TYPES_HPP