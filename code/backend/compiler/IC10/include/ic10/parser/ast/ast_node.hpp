// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_node.hpp
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

#include "common/utils/position.hpp"
#include "common/utils/common.hpp"
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
        static constexpr auto nodeName = "Error"_fs;

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
        static constexpr auto nodeName = "Integer"_fs;

        static constexpr auto startTokens = std::array{TokenType::INTEGER};

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
        static constexpr auto nodeName    = "Float"_fs;
        static constexpr auto startTokens = std::array{TokenType::FLOAT};
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
        static constexpr auto nodeName    = "HexNumber"_fs;
        static constexpr auto startTokens = std::array{TokenType::HEX_NUMBER};
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
        static constexpr auto nodeName    = "BinaryNumber"_fs;
        static constexpr auto startTokens = std::array{TokenType::BINARY_NUMBER};
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
        static constexpr auto nodeName    = "Identifier"_fs;
        static constexpr auto startTokens = std::array{TokenType::IDENTIFIER};
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
        static constexpr auto nodeName    = "String"_fs;
        static constexpr auto startTokens = std::array{TokenType::STRING};
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
        static constexpr auto nodeName    = "Register"_fs;
        static constexpr auto startTokens = std::array{TokenType::REGISTER};
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
        static constexpr auto nodeName    = "Device"_fs;
        static constexpr auto startTokens = std::array{TokenType::DEVICE};
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
     * @details 表示IC10中的关键字常量(如nan、pinf、ninf、pi、tau、deg2rad、rad2deg、epsilon、rgas等)
     *
     * @elseif en
     *
     * @class Constant
     * @brief Constant node
     * @details Represents a keyword constant in IC10 (such as nan, pinf, ninf, pi, tau, deg2rad,
     *          rad2deg, epsilon, rgas, etc.)
     *
     * @endif
     */
    struct Constant : AST<Constant> {
        static constexpr auto nodeName = "Constant"_fs;
        static constexpr auto startTokens =
            std::array{TokenType::KEYWORD_NAN,         TokenType::KEYWORD_PINF,
                       TokenType::KEYWORD_NINF,        TokenType::KEYWORD_PI,
                       TokenType::KEYWORD_TAU,         TokenType::KEYWORD_DEG2RAD,
                       TokenType::KEYWORD_RAD2DEG,     TokenType::KEYWORD_EPSILON,
                       TokenType::KEYWORD_RGAS};
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
     * @details 表示对设备的引用,可为设备字面量(Device)或寄存器/标识符(RegisterOrIdentifier)。
     *          设备引用用于指令中需要设备操作数的场景(如get/put/l/s等)。
     * @note 不再接受Number类型,数值字面量不再视为合法的设备引用
     * @elseif en
     * @brief DeviceReference type alias
     * @details Represents a reference to a device, which can be a device literal (Device)
     *          or a register/identifier (RegisterOrIdentifier).
     *          Device references are used in instructions that require a device operand (such as get/put/l/s, etc.).
     * @note Number is no longer accepted; numeric literals are not valid device references
     * @endif
     */
    using DeviceReference = Errorable<Device, RegisterOrIdentifier>;

    // RegisterOrDevice

    /**
     * @if zh
     * @brief 寄存器或设备类型别名
     * @elseif en
     * @brief RegisterOrDevice type alias
     * @endif
     */
    using RegisterOrDevice = Errorable<RegisterOrIdentifier, Device>;

    // RegisterOrNumber - 移到 MacroCall 定义之后

    // DeviceAliasRef - 移到 MacroCall 定义之后

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
        static constexpr auto nodeName    = "HashCall"_fs;
        static constexpr auto startTokens = std::array{TokenType::KEYWORD_HASH};
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
        static constexpr auto nodeName    = "StrCall"_fs;
        static constexpr auto startTokens = std::array{TokenType::KEYWORD_STR};
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

    // RegisterOrNumber (r?|num)

    /**
     * @if zh
     * @brief 寄存器或数字类型别名 (语法: r?|num)
     * @details 表示一个可以是寄存器或数字值的操作数,包含以下子类型:
     *          RegisterOrIdentifier(寄存器或标识符)、Number(数值字面量)、
     *          Constant(关键字常量)和MacroCall(宏调用)。
     *          用于接受寄存器或数字字面量作为操作数的指令场景(如算术、逻辑、移位指令的源操作数)。
     * @elseif en
     * @brief Register or number type alias (syntax: r?|num)
     * @details Represents an operand that can be a register or a numeric value, including the following
     *          subtypes: RegisterOrIdentifier (register or identifier), Number (numeric literal),
     *          Constant (keyword constant) and MacroCall (macro call).
     *          Used in instruction scenarios that accept a register or numeric literal as an operand
     *          (such as source operands of arithmetic, logical, and shift instructions).
     * @endif
     */
    using RegisterOrNumber = Errorable<RegisterOrIdentifier, Number, Constant, MacroCall>;

    // DeviceAliasRef (d?)

    /**
     * @if zh
     * @brief 设备别名引用类型别名 (语法: d?)
     * @details 表示对一个设备的引用,包含以下子类型: Device(设备字面量)和Identifier(标识符)。
     *          标识符通常是通过 alias 预处理指令定义的设备别名。
     *          用于需要引用具名设备的场景(如 clrd 指令的设备参数、设备别名解析等)。
     * @elseif en
     * @brief Device alias reference type alias (syntax: d?)
     * @details Represents a reference to a device, including the following subtypes:
     *          Device (device literal) and Identifier.
     *          The identifier is typically a device alias defined via the alias preprocessor directive.
     *          Used in scenarios that require referencing a named device
     *          (such as the device parameter of the clrd instruction, device alias resolution, etc.).
     * @endif
     */
    using DeviceAliasRef = Errorable<Device, Identifier>;

    // NumberValue (num)

    /**
     * @if zh
     * @brief 数字值类型别名 (语法: num)
     * @details 表示任意的数字值,包含以下子类型: Number(数值字面量)、Constant(关键字常量)、
     *          MacroCall(宏调用)和Identifier(标识符)。
     *          标识符子类型用于支持通过 define 定义的常量名。
     *          用于需要数值语义的位置(如 define 指令的操作数、跳转目标等)。
     * @elseif en
     * @brief Number value type alias (syntax: num)
     * @details Represents an arbitrary numeric value, including the following subtypes:
     *          Number (numeric literal), Constant (keyword constant), MacroCall (macro call)
     *          and Identifier.
     *          The identifier subtype supports constant names defined via define.
     *          Used in positions that require numeric semantics (such as the operand of the define
     *          directive, jump targets, etc.).
     * @endif
     */
    using NumberValue = Errorable<Number, Constant, MacroCall, Identifier>;

    // JumpTarget

    /**
     * @if zh
     * @brief 跳转目标类型别名
     * @details 表示跳转指令的目标地址,类型等同于 NumberValue。
     *          用于 j、jal、jr 以及各类相对跳转(br*|brnez 等)指令的目标操作数,
     *          目标通常以行号(整数)形式给出。
     * @elseif en
     * @brief Jump target type alias
     * @details Represents the target address of a jump instruction, type-identical to NumberValue.
     *          Used as the target operand of j, jal, jr and various relative jump instructions
     *          (br*|brnez, etc.), where the target is typically given as a line number (integer).
     * @endif
     */
    using JumpTarget = NumberValue;

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
     * @details Represents an alias preprocessor directive in IC10, used to alias devices or
     * registers
     *
     * @endif
     */
    struct AliasDirective : AST<AliasDirective> {
        static constexpr auto nodeName = "AliasDirective"_fs;
        static constexpr auto keyword  = "alias"_fs;
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
     * @details 表示IC10中的define预处理指令,用于定义常量。
     *          其操作数(operand)类型为NumberValue,接受数值字面量(Number)、关键字常量(Constant)、
     *          宏调用(MacroCall)或标识符(Identifier,用于引用其他已定义的常量),
     *          不再接受寄存器或设备等非数值类型。
     *
     * @elseif en
     *
     * @class DefineDirective
     * @brief Define directive node
     * @details Represents a define preprocessor directive in IC10, used to define constants.
     *          Its operand is of type NumberValue, accepting numeric literals (Number),
     *          keyword constants (Constant), macro calls (MacroCall), or identifiers
     *          (Identifier, to reference other defined constants),
     *          but no longer accepting non-numeric types such as registers or devices.
     *
     * @endif
     */
    struct DefineDirective : AST<DefineDirective> {
        static constexpr auto nodeName = "DefineDirective"_fs;
        static constexpr auto keyword  = "define"_fs;
        ShallowErrorable<Identifier> identifier;
        NumberValue operand;
        DefineDirective() = default;
        using AST::AST;
        DefineDirective(Pos pos, Identifier id, NumberValue op);
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
        static constexpr auto nodeName = "LabelDef"_fs;
        ShallowErrorable<Identifier> identifier;
        LabelDef() = default;
        using AST::AST;
        LabelDef(Pos pos, Identifier id);
        [[nodiscard]] Pos end() const override;
        [[nodiscard]] std::string toString() const override;
        [[nodiscard]] std::string toJSON() const override;
    };

    /**
     * @if zh
     * @brief 编译期 token 集合推导的内部实现细节
     * @details 提供从 AST 节点元数据（startTokens）编译期推导 variant 合法起始 token 集合的工具。
     *          采用非递归 fold expression 实现以兼容 MSVC 的 constexpr 限制。
     * @note 外部不应直接使用此命名空间中的符号
     * @elseif en
     * @brief Internal implementation details for compile-time token set derivation
     * @details Provides utilities for compile-time derivation of valid starting token sets
     *          for variant types from AST node metadata (startTokens).
     *          Implemented with non-recursive fold expressions to be compatible with MSVC constexpr limits.
     * @note External code should not directly use symbols in this namespace
     * @endif
     */
    namespace detail {

        /**
         * @if zh
         * @brief 类型萃取：判断是否为 std::array<TokenType, N>
         * @tparam T 待检测类型
         * @elseif en
         * @brief Type trait: check whether a type is std::array<TokenType, N>
         * @tparam T Type to check
         * @endif
         */
        template<typename>
        struct is_std_array_of_token : std::false_type {};

        template<std::size_t N>
        struct is_std_array_of_token<std::array<TokenType, N>> : std::true_type {};

        /**
         * @if zh
         * @brief 概念：类型 T 具有 static constexpr startTokens 成员且为 TokenType 数组
         * @tparam T 待检测类型
         * @elseif en
         * @brief Concept: type T has a static constexpr startTokens member that is a TokenType array
         * @tparam T Type to check
         * @endif
         */
        template<typename T>
        concept HasStartTokens = requires {
            requires is_std_array_of_token<std::remove_cvref_t<decltype(T::startTokens)>>::value;
        };

        // 前向声明（实现于 ast_node.inl）

        /**
         * @if zh
         * @brief 编译期计算多个数组的总大小（fold expression）
         * @tparam Arrays 数组类型包
         * @return 所有数组大小之和
         * @elseif en
         * @brief Compile-time total size calculation of multiple arrays (fold expression)
         * @tparam Arrays Array type pack
         * @return Sum of all array sizes
         * @endif
         */
        template<typename... Arrays>
        constexpr std::size_t totalArraySize();

        /**
         * @if zh
         * @brief 编译期合并多个数组到固定大小的结果数组（非递归，直接拷贝）
         * @tparam N 结果数组大小
         * @tparam Arrays 输入数组类型包
         * @param arrays 输入数组
         * @return 合并后的数组
         * @elseif en
         * @brief Compile-time concatenation of multiple arrays into a fixed-size result (non-recursive, direct copy)
         * @tparam N Result array size
         * @tparam Arrays Input array type pack
         * @param arrays Input arrays
         * @return Concatenated array
         * @endif
         */
        template<std::size_t N, typename... Arrays>
        constexpr std::array<TokenType, N> concatArraysDirect(Arrays... arrays);

        /**
         * @if zh
         * @brief 编译期数组合并（自动推导大小）
         * @tparam Arrays 输入数组类型包
         * @param arrays 输入数组
         * @return 合并后的数组
         * @elseif en
         * @brief Compile-time array concatenation (size auto-deduced)
         * @tparam Arrays Input array type pack
         * @param arrays Input arrays
         * @return Concatenated array
         * @endif
         */
        template<typename... Arrays>
        constexpr auto concatArrays(Arrays... arrays);

        /**
         * @if zh
         * @brief variant token 集合推导实现（通过索引序列展开每个 alternative）
         * @tparam Variant variant 类型
         * @tparam Is 索引序列
         * @elseif en
         * @brief Variant token set derivation implementation (expand each alternative via index sequence)
         * @tparam Variant Variant type
         * @tparam Is Index sequence
         * @endif
         */
        template<IsVariant Variant, std::size_t... Is>
        constexpr auto variantTokensImpl(std::index_sequence<Is...>);

        /**
         * @if zh
         * @brief 推导 variant 类型所有成员的起始 token 集合
         * @tparam Variant variant 类型
         * @return 包含所有成员起始 token 的数组（自动展开嵌套 variant）
         * @elseif en
         * @brief Derive starting token set for all members of a variant type
         * @tparam Variant Variant type
         * @return Array containing starting tokens of all members (nested variants are auto-expanded)
         * @endif
         */
        template<IsVariant Variant>
        constexpr auto variantTokens();

        /**
         * @if zh
         * @brief 提取类型 T 的起始 token 集合
         * @details 若 T 是 variant 则递归展开；若 T 有 startTokens 则直接返回；否则返回空数组
         * @tparam T 待提取的类型
         * @return 起始 token 数组
         * @elseif en
         * @brief Extract starting token set for type T
         * @details If T is a variant, recursively expand; if T has startTokens, return it directly;
         *          otherwise return an empty array
         * @tparam T Type to extract from
         * @return Starting token array
         * @endif
         */
        template<typename T>
        constexpr auto extractTokens();

    }  // namespace detail

    /**
     * @if zh
     * @brief 获取 variant 类型所有成员的起始 token 集合
     * @tparam Variant 要分析的 variant 类型
     * @return 包含所有可能起始 token 的数组（包含嵌套 variant 的展开）
     * @elseif en
     * @brief Get all starting tokens for a variant type's members
     * @tparam Variant The variant type to analyze
     * @return Array containing all possible starting tokens (including nested variant expansion)
     * @endif
     */
    template<IsVariant Variant>
    constexpr auto getStartTokens();

    /**
     * @if zh
     * @brief 判断给定 token 类型是否是某个 variant 的合法起始
     * @tparam Variant 要检查的 variant 类型
     * @param t 要判断的 token 类型
     * @return 如果 t 是 variant 任一成员的起始 token，返回 true
     * @elseif en
     * @brief Check if a given token type is a valid start for a variant type
     * @tparam Variant The variant type to check
     * @param t The token type to test
     * @return true if t is a starting token for any member of the variant
     * @endif
     */
    template<IsVariant Variant>
    constexpr bool isStartToken(TokenType t);

    /**
     * @if zh
     * @brief 判断 token 是否为合法操作数起始
     * @param t 要判断的 token 类型
     * @return 如果 t 是 Operand variant 任一成员的起始 token，返回 true
     * @elseif en
     * @brief Check if a token is a valid operand start
     * @param t Token type to test
     * @return true if t is a starting token for any member of the Operand variant
     * @endif
     */
    constexpr bool isOperandStart(TokenType t);

    /**
     * @if zh
     * @brief 判断给定 token 类型是否是语句的合法起始
     * @details 包含所有指令关键字、标签(IDENTIFIER)、预处理指令(ALIAS/DEFINE)
     * @param t 要判断的 token 类型
     * @return 如果 t 是语句起始 token，返回 true
     * @elseif en
     * @brief Check if a given token type is a valid start of a statement
     * @details Includes all instruction keywords, labels (IDENTIFIER), and preprocessor directives (ALIAS/DEFINE)
     * @param t The token type to test
     * @return true if t is a statement starting token
     * @endif
     */
    constexpr bool isStatementStart(TokenType t);

}  // namespace stationeers::ic10

#include "ast_node.inl"

#endif  // COMPILER_AST_NODE_HPP
