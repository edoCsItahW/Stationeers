// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file node.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10抽象语法树节点定义
 * @details 定义IC10程序AST的各类节点,包括基础类型(AST基类)、字面量(Integer、Float等)、
 *        标识符、寄存器、设备、宏调用、操作数、预处理指令等。
 * @note 实现位于ast.cpp和ast_node.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 abstract syntax tree node definitions
 * @details Defines various AST nodes for IC10 programs, including base types (AST base class),
 *        literals (Integer, Float, etc.), identifiers, registers, devices, macro calls,
 *        operands, preprocessor directives, etc.
 * @note Implementation in ast.cpp and ast_node.inl
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_NODE_HPP
#define COMPILER_AST_NODE_HPP
#pragma once

#include "common/utils/common.hpp"
#include "common/utils/json.hpp"
#include "common/utils/position.hpp"
#include "ic10_compiler/lexer/token.hpp"
#include "pch/utils.hpp"
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace stationeers::ic10 {

    // 没有任何技术意义，仅用于减少代码冗余
#define AST_NODE_PRE_DEFINED_METHODS(NodeType)                                                     \
    NodeType() = default;                                                                          \
    using AST::AST;                                                                                \
    [[nodiscard]] Pos end() const override;                                                        \
    [[nodiscard]] std::string toString() const override;                                           \
    [[nodiscard]] std::string toJSON() const override;

    // AST (实现于ast_node.inl)

    template<typename T>
    concept AstJsonAble =
        JsonArgsAble<T> || IsVariant<std::decay_t<T>> || requires(T t) { t.toJSON(); };

    template<typename T>
    concept HasFirst = requires {
        { T::FIRST };
    };

    /**
     * @if zh
     *
     * @class AST
     * @brief 抽象语法树节点基类模板
     * @details 所有AST节点的基类,提供位置信息和虚析构函数
     *
     * @tparam T 派生类类型(CRTP模式)
     *
     * @elseif en
     *
     * @class AST
     * @brief Abstract syntax tree node base class template
     * @details Base class for all AST nodes, providing position information and virtual destructor
     *
     * @tparam T Derived class type (CRTP pattern)
     *
     * @endif
     */
    template<typename>
    struct AST {
        /**
         * @if zh
         * @brief 虚析构函数
         * @elseif en
         * @brief Virtual destructor
         * @endif
         */
        virtual ~AST() = default;

        /**
         * @if zh
         * @brief 节点位置信息
         * @elseif en
         * @brief Node position information
         * @endif
         */
        Pos position;

        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        AST() = default;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param pos 位置信息
         *
         * @elseif en
         *
         * @brief Constructor
         * @param pos Position information
         *
         * @endif
         */
        AST(Pos pos) noexcept;

        /**
         * @if zh
         *
         * @brief 转换为字符串表示
         * @return 节点的可读字符串表示
         *
         * @elseif en
         *
         * @brief Convert to string representation
         * @return Human-readable string representation of node
         *
         * @endif
         */
        [[nodiscard]] virtual std::string toString() const = 0;

        /**
         * @if zh
         *
         * @brief 转换为JSON格式
         * @return 节点的JSON格式字符串
         *
         * @elseif en
         *
         * @brief Convert to JSON format
         * @return JSON format string of node
         *
         * @endif
         */
        [[nodiscard]] virtual std::string toJSON() const = 0;

        /**
         * @if zh
         *
         * @brief 获取起始位置
         * @return 节点起始位置
         *
         * @elseif en
         *
         * @brief Get start position
         * @return Node start position
         *
         * @endif
         */
        [[nodiscard]] Pos start() const noexcept;

        /**
         * @if zh
         *
         * @brief 获取结束位置
         * @return 节点结束位置
         *
         * @elseif en
         *
         * @brief Get end position
         * @return Node end position
         *
         * @endif
         */
        [[nodiscard]] virtual Pos end() const = 0;

        template<typename T>
        static std::string process(T&& arg);

        template<typename T, typename F>
        static std::string seqJSON(const std::vector<T>& datas, F func);

        template<FString... Vs, AstJsonAble... Args>
            requires(sizeof...(Vs) == sizeof...(Args))
        std::string jsonBase(Args&&... args) const;
    };

    // ErrorNode（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class ErrorNode
     * @brief 错误节点
     * @details 表示解析或词法分析过程中产生的错误节点
     *
     * @elseif en
     *
     * @class ErrorNode
     * @brief Error node
     * @details Represents an error node generated during parsing or lexical analysis
     *
     * @endif
     */
    struct ErrorNode : AST<ErrorNode> {
        /**
         * @if zh
         * @brief 节点名称
         * @elseif en
         * @brief Node name
         * @endif
         */
        static constexpr auto nodeName = "Error"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::UNKNOWN});

        /**
         * @if zh
         * @brief 导致错误的Token
         * @elseif en
         * @brief Token that caused the error
         * @endif
         */
        Token token;

        /**
         * @if zh
         * @brief 错误消息
         * @elseif en
         * @brief Error message
         * @endif
         */
        std::string message;

        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        ErrorNode() = default;

        /**
         * @if zh
         * @brief 使用基类构造函数
         * @elseif en
         * @brief Use base class constructor
         * @endif
         */
        using AST::AST;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param t 导致错误的Token
         * @param msg 错误消息
         *
         * @elseif en
         *
         * @brief Constructor
         * @param t Token that caused the error
         * @param msg Error message
         *
         * @endif
         */
        ErrorNode(const Token& t, std::string msg);

        /**
         * @if zh
         *
         * @brief 带自定义起始位置的构造函数
         * @param pos 错误节点起始位置（通常为整个出错构造的起始位置）
         * @param t 导致错误的Token
         * @param msg 错误消息
         *
         * @elseif en
         *
         * @brief Constructor with custom start position
         * @param pos Start position of the error node (usually the start of the whole failed
         * construct)
         * @param t Token that caused the error
         * @param msg Error message
         *
         * @endif
         */
        ErrorNode(Pos pos, const Token& t, std::string msg);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    /**
     * @if zh
     *
     * @brief 错误可包含类型别名
     * @tparam ...Ts 可包含的类型包
     * @details 将ErrorNode与给定类型组合,并进行扁平化和去重
     *
     * @elseif en
     *
     * @brief Errorable type alias
     * @tparam ...Ts Type pack that can be included
     * @details Combines ErrorNode with given types, flattening and deduplicating
     *
     * @endif
     */
    template<typename... Ts>
    using Errorable = container_helper<std::variant<Ts..., ErrorNode>>::flatten_with_unique_t;

    /**
     * @if zh
     *
     * @brief 浅层错误可包含类型别名
     * @tparam ...Ts 可包含的类型包
     * @details 将ErrorNode与给定类型直接组合,不做扁平化
     *
     * @elseif en
     *
     * @brief ShallowErrorable type alias
     * @tparam ...Ts Type pack that can be included
     * @details Combines ErrorNode with given types directly, without flattening
     *
     * @endif
     */
    template<typename... Ts>
    using ShallowErrorable = std::variant<Ts..., ErrorNode>;

    template<typename Derived, typename Value = std::string>
    struct LeafNode : AST<Derived> {
        Value value;

        LeafNode() = default;

        using AST<Derived>::AST;

        LeafNode(Pos pos, Value val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // Integer（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Integer
     * @brief 整数字面量节点
     * @details 表示IC10中的整数值
     *
     * @elseif en
     *
     * @class Integer
     * @brief Integer literal node
     * @details Represents an integer value in IC10
     *
     * @endif
     */
    struct Integer : LeafNode<Integer> {
        static constexpr auto nodeName = "Integer"_fs;

        static constexpr auto FIRST = std::make_tuple(
            std::array{TokenType::SUB, TokenType::INTEGER}, std::array{TokenType::INTEGER}
        );

        using LeafNode::LeafNode;
    };

    // Float（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Float
     * @brief 浮点数字面量节点
     * @details 表示IC10中的浮点数值
     *
     * @elseif en
     *
     * @class Float
     * @brief Float literal node
     * @details Represents a floating-point value in IC10
     *
     * @endif
     */
    struct Float : LeafNode<Float> {
        static constexpr auto nodeName = "Float"_fs;

        static constexpr auto FIRST = std::make_tuple(
            std::array{TokenType::SUB, TokenType::FLOAT}, std::array{TokenType::FLOAT}
        );

        using LeafNode::LeafNode;
    };

    // HexNumber（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class HexNumber
     * @brief 十六进制数字面量节点
     * @details 表示IC10中的十六进制数值
     *
     * @elseif en
     *
     * @class HexNumber
     * @brief Hexadecimal number literal node
     * @details Represents a hexadecimal value in IC10
     *
     * @endif
     */
    struct HexNumber : LeafNode<HexNumber> {
        static constexpr auto nodeName = "HexNumber"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::HEX_NUMBER});

        using LeafNode::LeafNode;
    };

    // BinaryNumber（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class BinaryNumber
     * @brief 二进制数字面量节点
     * @details 表示IC10中的二进制数值
     *
     * @elseif en
     *
     * @class BinaryNumber
     * @brief Binary number literal node
     * @details Represents a binary value in IC10
     *
     * @endif
     */
    struct BinaryNumber : LeafNode<BinaryNumber> {
        static constexpr auto nodeName = "BinaryNumber"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::BINARY_NUMBER});

        using LeafNode::LeafNode;
    };

    // Number

    /**
     * @if zh
     *
     * @brief Number类型别名
     * @details 表示数值类型,可以与ErrorNode组合
     *
     * @elseif en
     *
     * @brief Number type alias
     * @details Represents a numeric type, can combine with ErrorNode
     *
     * @endif
     */
    using Number = std::variant<Integer, Float, HexNumber, BinaryNumber, ErrorNode>;

    // Identifier（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Identifier
     * @brief 标识符节点
     * @details 表示IC10中的标识符(变量名、标签名等)
     *
     * @elseif en
     *
     * @class Identifier
     * @brief Identifier node
     * @details Represents an identifier in IC10 (variable name, label name, etc.)
     *
     * @endif
     */
    struct Identifier : LeafNode<Identifier> {
        static constexpr auto nodeName = "Identifier"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::IDENTIFIER});

        using LeafNode::LeafNode;
    };

    // String（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class String
     * @brief 字符串字面量节点
     * @details 表示IC10中的字符串值
     *
     * @elseif en
     *
     * @class String
     * @brief String literal node
     * @details Represents a string value in IC10
     *
     * @endif
     */
    struct String : LeafNode<String> {
        static constexpr auto nodeName = "String"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::STRING});

        using LeafNode::LeafNode;
    };

    // Register（实现于ast.cpp）

    template<FString Name>
    struct StaticRegisterBase : LeafNode<StaticRegisterBase<Name>> {
        static constexpr auto nodeName = Name;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::REGISTER});

        using LeafNode<StaticRegisterBase>::LeafNode;
    };

    using GeneralPurposeRegister = StaticRegisterBase<"GeneralPurposeRegister">;

    using AddressRegister = StaticRegisterBase<"AddressRegister">;

    using StackPointerRegister = StaticRegisterBase<"StackPointerRegister">;

    using SpecialRegister = ShallowErrorable<AddressRegister, StackPointerRegister>;

    using StaticRegister = Errorable<SpecialRegister, GeneralPurposeRegister>;

    struct DynamicRegister : AST<DynamicRegister> {
        static constexpr auto nodeName = "DynamicRegister"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::REGISTER});

        using InnerRegister =
            ShallowErrorable<DynamicRegister, GeneralPurposeRegister, AddressRegister>;

        std::shared_ptr<InnerRegister> reg;

        DynamicRegister(Pos pos, std::shared_ptr<InnerRegister> reg);

        AST_NODE_PRE_DEFINED_METHODS(DynamicRegister)
    };

    using Register = Errorable<DynamicRegister, StaticRegister>;

    // Device（实现于ast.cpp）

    template<FString Name>
    struct StaticDeviceBase : LeafNode<StaticDeviceBase<Name>> {
        static constexpr auto nodeName = Name;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::DEVICE});

        using LeafNode<StaticDeviceBase>::LeafNode;
    };

    using SelfReferenceDevice = StaticDeviceBase<"SelfReferenceDevice">;

    using OrdinaryDevice = StaticDeviceBase<"OrdinaryDevice">;

    struct StaticDevice : AST<StaticDevice> {
        static constexpr auto nodeName = "StaticDevice"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::DEVICE});

        ShallowErrorable<SelfReferenceDevice, OrdinaryDevice> device;

        std::optional<Integer> pin;

        StaticDevice(
            Pos pos, ShallowErrorable<SelfReferenceDevice, OrdinaryDevice> device,
            std::optional<Integer> pin = std::nullopt
        );

        AST_NODE_PRE_DEFINED_METHODS(StaticDevice)
    };

    struct DynamicDevice : AST<DynamicDevice> {
        static constexpr auto nodeName = "DynamicDevice"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::DEVICE});

        using InnerRegister =
            ShallowErrorable<DynamicRegister, GeneralPurposeRegister, AddressRegister>;

        InnerRegister reg;

        DynamicDevice(Pos pos, InnerRegister reg);

        AST_NODE_PRE_DEFINED_METHODS(DynamicDevice)
    };

    using Device = ShallowErrorable<DynamicDevice, StaticDevice>;

    struct Enum : AST<Enum> {
        static constexpr auto nodeName = "Enum"_fs;

        static constexpr auto FIRST =
            std::make_tuple(std::array{TokenType::IDENTIFIER, TokenType::DOT, TokenType::IDENTIFIER});

        ShallowErrorable<Identifier> name;

        ShallowErrorable<Identifier> value;

        Enum(Pos pos, Identifier name, Identifier value);

        AST_NODE_PRE_DEFINED_METHODS(Enum)
    };

    // HashCall（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class HashMacro
     * @brief HASH宏调用节点
     * @details 表示IC10中的HASH宏调用
     *
     * @elseif en
     *
     * @class HashMacro
     * @brief HashCall macro node
     * @details Represents a HASH macro call in IC10
     *
     * @endif
     */
    struct HashMacro : AST<HashMacro> {
        static constexpr auto nodeName = "HashCall"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::KEYWORD_HASH});

        ShallowErrorable<String> value;

        Pos endPos;

        HashMacro(Pos pos, String val);

        HashMacro() = default;

        using AST::AST;

        [[nodiscard]] Pos end() const noexcept override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // StrCall（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class StrMacro
     * @brief STR宏调用节点
     * @details 表示IC10中的STR宏调用
     *
     * @elseif en
     *
     * @class StrMacro
     * @brief StrCall macro node
     * @details Represents a STR macro call in IC10
     *
     * @endif
     */
    struct StrMacro : AST<StrMacro> {
        static constexpr auto nodeName = "StrCall"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::KEYWORD_STR});

        ShallowErrorable<String> value;

        Pos endPos;

        StrMacro(Pos pos, String val);

        StrMacro() = default;

        using AST::AST;

        [[nodiscard]] Pos end() const noexcept override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // MacroCall

    /**
     * @if zh
     * @brief 宏调用类型别名
     * @elseif en
     * @brief MacroCall type alias
     * @endif
     */
    using Macro = ShallowErrorable<HashMacro, StrMacro>;

    using RegTarget = Errorable<Register, Identifier>;

    using RegOrDev = Errorable<Register, Device>;

    using NumValue = Errorable<Number, Register, Identifier, Enum>;

    using JumpLine = Errorable<Number, Register, Identifier>;

    using Address = Errorable<Number, Register, Identifier, Enum>;

    using SlotIdx = Errorable<Number, Register, Identifier, Enum>;

    using HardwareId = Errorable<Number, Register, Identifier, Enum>;

    using ReagentHash = Errorable<Number, Register, Identifier, Enum>;

    using DeviceRef = Errorable<Device, Identifier>;

    using DeviceRefStrict = Errorable<Device>;

    // 旧语法LogicProp可以为Number，放开兼容旧代码，但进行警告
    using LogicProp = Errorable<Identifier, Number>;

    // 旧语法LogicSlotProp可以为Number，放开兼容旧代码，但进行警告
    using LogicSlotProp = Errorable<Identifier, Number>;

    using AggMode = Errorable<Number, Identifier, Enum>;

    using ReagentMode = Errorable<Number, Identifier, Enum>;

    using DeviceHash = Errorable<Number, Identifier, HashMacro>;

    using NameHash = Errorable<Number, Identifier, StrMacro>;

    using AliasDef = ShallowErrorable<Identifier>;

    using ConstDef = ShallowErrorable<Identifier>;

    using ConstNum = Errorable<Number, Macro>;

    // Operand

    /**
     * @if zh
     * @brief 操作数类型别名
     * @elseif en
     * @brief Operand type alias
     * @endif
     */
    using Operand = Errorable<Register, Device, Number, Identifier, Enum, Macro>;

    // LabelDef（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class LabelDef
     * @brief 标签定义节点
     * @details 表示IC10中的标签定义
     *
     * @elseif en
     *
     * @class LabelDef
     * @brief Label definition node
     * @details Represents a label definition in IC10
     *
     * @endif
     */
    struct LabelDef : AST<LabelDef> {
        static constexpr auto nodeName = "LabelDef"_fs;

        static constexpr auto FIRST = std::make_tuple(std::array{TokenType::IDENTIFIER, TokenType::COLON});

        ShallowErrorable<Identifier> identifier;

        LabelDef(Pos pos, Identifier id);

        AST_NODE_PRE_DEFINED_METHODS(LabelDef)
    };

}  // namespace stationeers::ic10

#include "node.inl"

#endif  // COMPILER_AST_NODE_HPP
