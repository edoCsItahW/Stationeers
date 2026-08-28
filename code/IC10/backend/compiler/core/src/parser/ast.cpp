// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 12:16
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

#include "ic10_compiler/pch/ast.hpp"
#include "common/utils/common.hpp"
#include <format>
#include <sstream>
#include <utility>

namespace stationeers::ic10 {

    // Program

    Pos Program::end() const {
        if (statements.empty()) return position;
        return call(statements.back(), [](auto&& s) { return s.end(); });
    }

    std::string Program::toString() const {
        return seqJSON(statements, [](auto&& s) {
            return call(s, [](auto&& x) { return x.toString(); });
        });
    }

    std::string Program::toJSON() const {
        std::stringstream ss;
        ss << "[";

        for (std::size_t i = 0; i < statements.size(); ++i)
            ss << call(statements[i], [](auto&& s) { return s.toJSON(); })
               << (i == statements.size() - 1 ? "" : ",");

        ss << "]";

        return jsonBase<"statements">(ss.str());
    }

    // LabelDef

    LabelDef::LabelDef(Pos pos, Identifier id)
        : AST(pos)
        , identifier(std::move(id)) {}

    Pos LabelDef::end() const {
        return call(identifier, [](auto&& id) { return id.end(); });
    }

    std::string LabelDef::toString() const {
        return std::format("{}:", call(identifier, [](auto&& id) { return id.toString(); }));
    }

    std::string LabelDef::toJSON() const { return jsonBase<"identifier">(identifier); }

    // DefineDirective

    DefineDirective::DefineDirective(Pos pos, Identifier id, ConstNum op)
        : AST(pos)
        , identifier(std::move(id))
        , operand(std::move(op)) {}

    Pos DefineDirective::end() const {
        return call(operand, [](auto&& op) { return op.end(); });
    }

    std::string DefineDirective::toString() const {
        return std::format(
            "define {} {}", call(identifier, [](auto&& id) { return id.toString(); }),
            call(operand, [](auto&& num) { return num.toString(); })
        );
    }

    std::string DefineDirective::toJSON() const {
        return jsonBase<"identifier", "number", "typeHint">(
            identifier, operand, typeHint ? std::optional(typeHint->toJSON()) : std::nullopt
        );
    }

    // AliasDirective

    AliasDirective::AliasDirective(Pos pos, Identifier id, RegOrDev regOrDev)
        : AST(pos)
        , identifier(std::move(id))
        , registerOrDevice(std::move(regOrDev)) {}

    Pos AliasDirective::end() const {
        return call(registerOrDevice, [](auto&& regOrDev) { return regOrDev.end(); });
    }

    std::string AliasDirective::toString() const {
        auto result = std::format(
            "alias {} {}", call(identifier, [](auto&& id) { return id.toString(); }),
            call(registerOrDevice, [](auto&& regOrDev) { return regOrDev.toString(); })
        );

        return result;
    }

    std::string AliasDirective::toJSON() const {
        return jsonBase<"identifier", "registerOrDevice", "typeHint">(
            identifier, registerOrDevice,
            typeHint ? std::optional(typeHint->toJSON()) : std::nullopt
        );
    }

    // StrCall

    StrMacro::StrMacro(Pos pos, String val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos StrMacro::end() const noexcept { return endPos; }

    std::string StrMacro::toString() const {
        return std::visit(
            []<typename T>(T&& t) { return std::format("STR({})", std::forward<T>(t).toString()); },
            value
        );
    }

    std::string StrMacro::toJSON() const { return jsonBase<"value">(value); }

    // HashCall

    HashMacro::HashMacro(Pos pos, String val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos HashMacro::end() const noexcept { return endPos; }

    std::string HashMacro::toString() const {
        return std::visit(
            []<typename T>(T&& t) {
                return std::format("HASH({})", std::forward<T>(t).toString());
            },
            value
        );
    }

    std::string HashMacro::toJSON() const { return jsonBase<"value">(value); }

    // DynamicRegister

    DynamicRegister::DynamicRegister(Pos pos, std::shared_ptr<InnerRegister> reg)
        : AST{pos}
        , reg(std::move(reg)) {}

    Pos DynamicRegister::end() const {
        return call(*reg, [](auto&& r) { return r.end(); });
    }

    std::string DynamicRegister::toString() const {
        return std::format("r{}", call(*reg, [](auto&& r) { return r.toString(); }));
    }

    std::string DynamicRegister::toJSON() const { return jsonBase<"register">(*reg); }

    // StaticDevice

    StaticDevice::StaticDevice(
        Pos pos, ShallowErrorable<SelfReferenceDevice, OrdinaryDevice> device,
        std::optional<Integer> pin
    )
        : AST{pos}
        , device(std::move(device))
        , pin(pin) {}

    Pos StaticDevice::end() const {
        return call(device, [](auto&& d) { return d.end(); });
    }

    std::string StaticDevice::toString() const {
        auto result = call(device, [](auto&& d) { return d.toString(); });

        if (pin) result += std::format(":{}", pin->toString());

        return result;
    }

    std::string StaticDevice::toJSON() const {
        return jsonBase<"device", "pin">(device, pin ? std::optional(pin->toJSON()) : std::nullopt);
    }

    // DynamicDevice

    DynamicDevice::DynamicDevice(Pos pos, InnerRegister reg)
        : AST{pos}
        , reg(std::move(reg)) {}

    Pos DynamicDevice::end() const {
        return call(reg, [](auto&& r) { return r.end(); });
    }

    std::string DynamicDevice::toString() const {
        return std::format("d{}", call(reg, [](auto&& r) { return r.toString(); }));
    }

    std::string DynamicDevice::toJSON() const { return jsonBase<"register">(reg); }

    // Enum

    Enum::Enum(Pos pos, Identifier name, Identifier value)
        : AST{pos}
        , name(std::move(name))
        , value(std::move(value)) {}

    Pos Enum::end() const {
        return call(value, [](auto&& v) { return v.end(); });
    }

    std::string Enum::toJSON() const { return jsonBase<"name", "value">(name, value); }

    std::string Enum::toString() const {
        return std::format(
            "{}.{}", call(name, [](auto&& n) { return n.toString(); }),
            call(value, [](auto&& v) { return v.toString(); })
        );
    }

    // ErrorNode

    ErrorNode::ErrorNode(const Token& t, std::string msg)
        : AST{t.pos}
        , token(t)
        , message(std::move(msg)) {}

    ErrorNode::ErrorNode(Pos pos, const Token& t, std::string msg)
        : AST{pos}
        , token(t)
        , message(std::move(msg)) {}

    Pos ErrorNode::end() const { return endPos(token); }

    std::string ErrorNode::toString() const { return token.lexeme; }

    std::string ErrorNode::toJSON() const { return jsonBase<"token", "message">(token, message); }

}  // namespace stationeers::ic10
