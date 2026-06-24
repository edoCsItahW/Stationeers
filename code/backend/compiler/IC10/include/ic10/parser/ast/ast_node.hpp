// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_node.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:51
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
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

    template<typename>
    struct AST {
        virtual ~AST() = default;

        Pos position;

        AST() = default;

        AST(Pos pos);

        [[nodiscard]] virtual std::string toString() const = 0;

        [[nodiscard]] virtual std::string toJSON() const = 0;

        [[nodiscard]] Pos start() const;

        [[nodiscard]] virtual Pos end() const = 0;

        template<typename... Ts>
        [[nodiscard]] std::string jsonBase(std::pair<std::string, Ts>... fields) const;
    };

    // ErrorNode（实现于ast.cpp）

    struct ErrorNode : AST<ErrorNode> {
        static constexpr auto nodeName = FStr{"Error"};

        Token token;

        std::string message;

        ErrorNode() = default;

        using AST::AST;

        ErrorNode(const Token& t, std::string msg);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    template<typename... Ts>
    using Errorable = container_helper<std::variant<Ts..., ErrorNode>>::flatten_with_unique_t;

    template<typename... Ts>
    using ShallowErrorable = std::variant<Ts..., ErrorNode>;

    // Integer（实现于ast.cpp）

    struct Integer : AST<Integer> {
        static constexpr auto nodeName = FStr{"Integer"};

        std::string value;

        Integer() = default;

        using AST::AST;

        Integer(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // Float（实现于ast.cpp）

    struct Float : AST<Float> {
        static constexpr auto nodeName = FStr{"Float"};

        std::string value;

        Float() = default;

        using AST::AST;

        Float(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // HexNumber（实现于ast.cpp）

    struct HexNumber : AST<HexNumber> {
        static constexpr auto nodeName = FStr{"HexNumber"};

        std::string value;

        HexNumber() = default;

        using AST::AST;

        HexNumber(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // BinaryNumber（实现于ast.cpp）

    struct BinaryNumber : AST<BinaryNumber> {
        static constexpr auto nodeName = FStr{"BinaryNumber"};

        std::string value;

        BinaryNumber() = default;

        using AST::AST;

        BinaryNumber(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // Number

    using Number = std::variant<Integer, Float, HexNumber, BinaryNumber, ErrorNode>;

    // Identifier（实现于ast.cpp）

    struct Identifier : AST<Identifier> {
        static constexpr auto nodeName = FStr{"Identifier"};

        std::string value;

        Identifier() = default;

        using AST::AST;

        Identifier(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // String（实现于ast.cpp）

    struct String : AST<String> {
        static constexpr auto nodeName = FStr{"String"};
        std::string value;

        String() = default;

        using AST::AST;

        String(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // Register（实现于ast.cpp）

    struct Register : AST<Register> {
        static constexpr auto nodeName = FStr{"Register"};
        std::string value;

        Register() = default;

        using AST::AST;

        Register(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // Device（实现于ast.cpp）

    struct Device : AST<Device> {
        static constexpr auto nodeName = FStr{"Device"};
        std::string value;

        Device() = default;

        using AST::AST;

        Device(Pos pos, std::string val);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // ReagentMode

    using ReagentMode = Errorable<Identifier, Number>;

    // BatchMode

    using BatchMode = Errorable<Identifier, Number>;

    // LogicSlotType

    using LogicSlotType = Errorable<Identifier, Number>;

    // SlotIndex

    using SlotIndex = Number;

    // LogicType

    using LogicType = Errorable<Identifier, Number>;

    // Constant（实现于ast.cpp）

    struct Constant : AST<Constant> {
        static constexpr auto nodeName = FStr{"Constant"};

        std::string keyword;

        Constant() = default;

        using AST::AST;

        Constant(Pos pos, std::string kw);

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

    // RegisterOrIdentifier

    using RegisterOrIdentifier = ShallowErrorable<Register, Identifier>;

    // DeviceReference

    using DeviceReference = Errorable<Device, RegisterOrIdentifier, Number>;

    // RegisterOrDevice

    using RegisterOrDevice = Errorable<RegisterOrIdentifier, Device>;

    // HashCall（实现于ast.cpp）

    struct HashCall : AST<HashCall> {
        static constexpr auto nodeName = FStr{"HashCall"};

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

    struct StrCall : AST<StrCall> {
        static constexpr auto nodeName = FStr{"StrCall"};

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

    using MacroCall = ShallowErrorable<HashCall, StrCall>;

    // Operand

    using Operand = Errorable<Register, Device, Number, Identifier, Constant, MacroCall>;

    // AliasDirective（实现于ast.cpp）

    struct AliasDirective : AST<AliasDirective> {
        static constexpr auto nodeName = FStr{"AliasDirective"};

        static constexpr auto keyword = FStr{"alias"};

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

    struct DefineDirective : AST<DefineDirective> {
        static constexpr auto nodeName = FStr{"DefineDirective"};

        static constexpr auto keyword = FStr{"define"};

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

    using PreprocessorDirective = ShallowErrorable<AliasDirective, DefineDirective>;

    // LabelDef（实现于ast.cpp）

    struct LabelDef : AST<LabelDef> {
        static constexpr auto nodeName = FStr{"LabelDef"};

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
