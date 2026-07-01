// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_node.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:51
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

#include "common/utils/position.hpp"
#include "ic10/lexer/token.hpp"
#include "pch/utils.hpp"

#include <string>
#include <utility>
#include <variant>


namespace stationeers::ic10 {

    // AST (实现于ast_node.inl)

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
        AST(Pos pos);

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
        [[nodiscard]] Pos start() const;

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

        /**
         * @if zh
         *
         * @brief JSON基类辅助函数
         * @tparam ...Ts 字段类型包
         * @param ... fields 字段名和值的对
         * @return JSON格式字符串
         *
         * @elseif en
         *
         * @brief JSON base helper function
         * @tparam ...Ts Field type pack
         * @param ... fields Pairs of field names and values
         * @return JSON format string
         *
         * @endif
         */
        template<typename... Ts>
        [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
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
        static constexpr auto nodeName = FString{"Error"};

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
    struct Integer : AST<Integer> {
        static constexpr auto nodeName = FString{"Integer"};
        std::string value;
        Integer() = default;
        using AST::AST;
        Integer(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
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
    struct Float : AST<Float> {
        static constexpr auto nodeName = FString{"Float"};
        std::string value;
        Float() = default;
        using AST::AST;
        Float(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
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
    struct HexNumber : AST<HexNumber> {
        static constexpr auto nodeName = FString{"HexNumber"};
        std::string value;
        HexNumber() = default;
        using AST::AST;
        HexNumber(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
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
    struct BinaryNumber : AST<BinaryNumber> {
        static constexpr auto nodeName = FString{"BinaryNumber"};
        std::string value;
        BinaryNumber() = default;
        using AST::AST;
        BinaryNumber(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
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
    struct Identifier : AST<Identifier> {
        static constexpr auto nodeName = FString{"Identifier"};
        std::string value;
        Identifier() = default;
        using AST::AST;
        Identifier(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
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
    struct String : AST<String> {
        static constexpr auto nodeName = FString{"String"};
        std::string value;
        String() = default;
        using AST::AST;
        String(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    // Register（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Register
     * @brief 寄存器节点
     * @details 表示IC10中的寄存器引用(如r0, r1等)
     *
     * @elseif en
     *
     * @class Register
     * @brief Register node
     * @details Represents a register reference in IC10 (such as r0, r1, etc.)
     *
     * @endif
     */
    struct Register : AST<Register> {
        static constexpr auto nodeName = FString{"Register"};
        std::string value;
        Register() = default;
        using AST::AST;
        Register(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    // Device（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Device
     * @brief 设备节点
     * @details 表示IC10中的设备引用
     *
     * @elseif en
     *
     * @class Device
     * @brief Device node
     * @details Represents a device reference in IC10
     *
     * @endif
     */
    struct Device : AST<Device> {
        static constexpr auto nodeName = FString{"Device"};
        std::string value;
        Device() = default;
        using AST::AST;
        Device(Pos pos, std::string val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    // ReagentMode

    /**
     * @if zh
     * @brief 试剂模式类型别名
     * @elseif en
     * @brief ReagentMode type alias
     * @endif
     */
    using ReagentMode = Errorable<Identifier, Number>;

    // BatchMode

    /**
     * @if zh
     * @brief 批处理模式类型别名
     * @elseif en
     * @brief BatchMode type alias
     * @endif
     */
    using BatchMode = Errorable<Identifier, Number>;

    // LogicSlotType

    /**
     * @if zh
     * @brief 逻辑槽类型别名
     * @elseif en
     * @brief LogicSlotType type alias
     * @endif
     */
    using LogicSlotType = Errorable<Identifier, Number>;

    // SlotIndex

    /**
     * @if zh
     * @brief 槽索引类型别名
     * @elseif en
     * @brief SlotIndex type alias
     * @endif
     */
    using SlotIndex = Number;

    // LogicType

    /**
     * @if zh
     * @brief 逻辑类型别名
     * @elseif en
     * @brief LogicType type alias
     * @endif
     */
    using LogicType = Errorable<Identifier, Number>;

    // Constant（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Constant
     * @brief 常量节点
     * @details 表示IC10中的关键字常量(如nan, pi等)
     *
     * @elseif en
     *
     * @class Constant
     * @brief Constant node
     * @details Represents a keyword constant in IC10 (such as nan, pi, etc.)
     *
     * @endif
     */
    struct Constant : AST<Constant> {
        static constexpr auto nodeName = FString{"Constant"};
        std::string keyword;
        Constant() = default;
        using AST::AST;
        Constant(Pos pos, std::string kw);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    // RegisterOrIdentifier

    /**
     * @if zh
     * @brief 寄存器或标识符类型别名
     * @elseif en
     * @brief RegisterOrIdentifier type alias
     * @endif
     */
    using RegisterOrIdentifier = ShallowErrorable<Register, Identifier>;

    // DeviceReference

    /**
     * @if zh
     * @brief 设备引用类型别名
     * @elseif en
     * @brief DeviceReference type alias
     * @endif
     */
    using DeviceReference = Errorable<Device, RegisterOrIdentifier, Number>;

    // RegisterOrDevice

    /**
     * @if zh
     * @brief 寄存器或设备类型别名
     * @elseif en
     * @brief RegisterOrDevice type alias
     * @endif
     */
    using RegisterOrDevice = Errorable<RegisterOrIdentifier, Device>;

    // HashCall（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class HashCall
     * @brief HASH宏调用节点
     * @details 表示IC10中的HASH宏调用
     *
     * @elseif en
     *
     * @class HashCall
     * @brief HashCall macro node
     * @details Represents a HASH macro call in IC10
     *
     * @endif
     */
    struct HashCall : AST<HashCall> {
        static constexpr auto nodeName = FString{"HashCall"};
        ShallowErrorable<String> value;
        Pos endPosition;
        HashCall() = default;
        using AST::AST;
        HashCall(Pos pos, String val);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    // StrCall（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class StrCall
     * @brief STR宏调用节点
     * @details 表示IC10中的STR宏调用
     *
     * @elseif en
     *
     * @class StrCall
     * @brief StrCall macro node
     * @details Represents a STR macro call in IC10
     *
     * @endif
     */
    struct StrCall : AST<StrCall> {
        static constexpr auto nodeName = FString{"StrCall"};
        ShallowErrorable<String> value;
        Pos endPosition;
        StrCall() = default;
        using AST::AST;
        StrCall(Pos pos, String val);
        [[nodiscard]] Pos end() const override;
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
    using MacroCall = ShallowErrorable<HashCall, StrCall>;

    // Operand

    /**
     * @if zh
     * @brief 操作数类型别名
     * @elseif en
     * @brief Operand type alias
     * @endif
     */
    using Operand = Errorable<Register, Device, Number, Identifier, Constant, MacroCall>;

    // AliasDirective（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class AliasDirective
     * @brief alias预处理指令节点
     * @details 表示IC10中的alias预处理指令,用于给设备或寄存器起别名
     *
     * @elseif en
     *
     * @class AliasDirective
     * @brief Alias directive node
     * @details Represents an alias preprocessor directive in IC10, used to alias devices or registers
     *
     * @endif
     */
    struct AliasDirective : AST<AliasDirective> {
        static constexpr auto nodeName = FString{"AliasDirective"};
        static constexpr auto keyword = FString{"alias"};
        ShallowErrorable<Identifier> identifier;
        RegisterOrDevice registerOrDevice;
        AliasDirective() = default;
        using AST::AST;
        AliasDirective(Pos pos, Identifier id, RegisterOrDevice regOrDev);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    // DefineDirective（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class DefineDirective
     * @brief define预处理指令节点
     * @details 表示IC10中的define预处理指令,用于定义常量
     *
     * @elseif en
     *
     * @class DefineDirective
     * @brief Define directive node
     * @details Represents a define preprocessor directive in IC10, used to define constants
     *
     * @endif
     */
    struct DefineDirective : AST<DefineDirective> {
        static constexpr auto nodeName = FString{"DefineDirective"};
        static constexpr auto keyword = FString{"define"};
        ShallowErrorable<Identifier> identifier;
        Operand operand;
        DefineDirective() = default;
        using AST::AST;
        DefineDirective(Pos pos, Identifier id, Operand op);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    // PreprocessorDirective

    /**
     * @if zh
     * @brief 预处理指令类型别名
     * @elseif en
     * @brief PreprocessorDirective type alias
     * @endif
     */
    using PreprocessorDirective = ShallowErrorable<AliasDirective, DefineDirective>;

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
        static constexpr auto nodeName = FString{"LabelDef"};
        ShallowErrorable<Identifier> identifier;
        LabelDef() = default;
        using AST::AST;
        LabelDef(Pos pos, Identifier id);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

}  // namespace stationeers::ic10

#include "ast_node.inl"

#endif  // COMPILER_AST_NODE_HPP
