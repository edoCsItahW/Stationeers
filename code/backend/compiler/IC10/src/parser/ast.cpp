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

#include "ic10/pch/ast.hpp"
#include "common/utils/common.hpp"
#include <format>
#include <sstream>
#include <utility>

namespace stationeers::ic10 {

    // Program

    Pos Program::end() const {
        return call(statements[statements.size() - 1], [](auto&& s) { return s.end(); });
    }

    std::string Program::toString() const {
        std::stringstream ss;

        for (const auto& statement : statements)
            ss << call(statement, [](auto&& s) { return s.toString(); }) << std::endl;

        return ss.str();
    }

    std::string Program::toJSON() const {
        std::stringstream ss;
        ss << "[";

        for (std::size_t i = 0; i < statements.size(); ++i)
            ss << call(statements[i], [](auto&& s) { return s.toJSON(); })
               << (i == statements.size() - 1 ? "" : ",");

        ss << "]";

        return jsonBase<std::string>({"statements", ss.str()});
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

    std::string LabelDef::toJSON() const {
        return jsonBase<std::string>({"identifier", call(identifier, [](auto&& id) {
                                          return id.toJSON();
                                      })});
    }

    // DefineDirective

    DefineDirective::DefineDirective(Pos pos, Identifier id, NumberValue op)
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
        return jsonBase<std::string, std::string>(
            {"identifier", call(identifier, [](auto&& id) { return id.toJSON(); })},
            {"number", call(operand, [](auto&& num) { return num.toJSON(); })}
        );
    }

    // AliasDirective

    AliasDirective::AliasDirective(Pos pos, Identifier id, RegisterOrDevice regOrDev)
        : AST(pos)
        , identifier(std::move(id))
        , registerOrDevice(std::move(regOrDev)) {}

    Pos AliasDirective::end() const {
        return call(registerOrDevice, [](auto&& regOrDev) { return regOrDev.end(); });
    }

    std::string AliasDirective::toString() const {
        return std::format(
            "alias {} {}", call(identifier, [](auto&& id) { return id.toString(); }),
            call(registerOrDevice, [](auto&& regOrDev) { return regOrDev.toString(); })
        );
    }

    std::string AliasDirective::toJSON() const {
        return jsonBase<std::string, std::string>(
            {"identifier", call(identifier, [](auto&& id) { return id.toJSON(); })},
            {"registerOrDevice",
             call(registerOrDevice, [](auto&& regOrDev) { return regOrDev.toJSON(); })}
        );
    }

    // StrCall

    StrCall::StrCall(Pos pos, String val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos StrCall::end() const { return endPos(endPosition, 1); }

    std::string StrCall::toString() const {
        return std::visit(
            []<typename T>(T&& t) { return std::format("STR({})", std::forward<T>(t).toString()); },
            value
        );
    }

    std::string StrCall::toJSON() const {
        return jsonBase<std::string>({"value", call(value, [](auto&& val) {
                                          return val.toJSON();
                                      })});
    }

    // HashCall

    HashCall::HashCall(Pos pos, String val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos HashCall::end() const { return endPos(endPosition, 1); }

    std::string HashCall::toString() const {
        return std::visit(
            []<typename T>(T&& t) {
                return std::format("HASH({})", std::forward<T>(t).toString());
            },
            value
        );
    }

    std::string HashCall::toJSON() const {
        return jsonBase<std::string>({"value", call(value, [](auto&& val) {
                                          return val.toJSON();
                                      })});
    }

    // Constant

    Constant::Constant(Pos pos, std::string kw)
        : AST{pos}
        , keyword(std::move(kw)) {}

    Pos Constant::end() const { return endPos(position, keyword.size()); }

    std::string Constant::toString() const { return keyword; }

    std::string Constant::toJSON() const { return jsonBase<std::string>({"keyword", '"' + keyword + '"'}); }

    // Device

    Device::Device(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos Device::end() const { return endPos(position, value.size()); }

    std::string Device::toString() const { return value; }

    std::string Device::toJSON() const {
        return jsonBase<std::string>({"value", '"' + value + '"'});
    }

    // Register

    Register::Register(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos Register::end() const { return endPos(position, value.size()); }

    std::string Register::toString() const { return value; }

    std::string Register::toJSON() const {
        return jsonBase<std::string>({"value", '"' + value + '"'});
    }

    // String

    String::String(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos String::end() const { return endPos(position, value.size()); }

    std::string String::toString() const { return value; }

    std::string String::toJSON() const { return jsonBase<std::string>({"value", value}); }

    // Identifier

    Identifier::Identifier(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos Identifier::end() const { return endPos(position, value.size()); }

    std::string Identifier::toString() const { return value; }

    std::string Identifier::toJSON() const {
        return jsonBase<std::string>({"value", '"' + value + '"'});
    }

    // BinaryNumber

    BinaryNumber::BinaryNumber(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos BinaryNumber::end() const { return endPos(position, value.size()); }

    std::string BinaryNumber::toString() const { return value; }

    std::string BinaryNumber::toJSON() const {
        return jsonBase<std::string>({"value", '"' + value + '"'});
    }

    // HexNumber

    HexNumber::HexNumber(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos HexNumber::end() const { return endPos(position, value.size()); }

    std::string HexNumber::toString() const { return value; }

    std::string HexNumber::toJSON() const {
        return jsonBase<std::string>({"value", '"' + value + '"'});
    }

    // Float

    Float::Float(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos Float::end() const { return endPos(position, value.size()); }

    std::string Float::toString() const { return value; }

    std::string Float::toJSON() const { return jsonBase<std::string>({"value", value}); }

    // Integer

    Integer::Integer(Pos pos, std::string val)
        : AST{pos}
        , value(std::move(val)) {}

    Pos Integer::end() const { return endPos(position, value.size()); }

    std::string Integer::toString() const { return value; }

    std::string Integer::toJSON() const { return jsonBase<std::string>({"value", value}); }

    // ErrorNode

    ErrorNode::ErrorNode(const Token& t, std::string msg)
        : AST{t.pos}
        , token(t)
        , message(std::move(msg)) {}

    Pos ErrorNode::end() const { return endPos(token); }

    std::string ErrorNode::toString() const { return token.lexeme; }

    std::string ErrorNode::toJSON() const {
        return jsonBase<std::string, std::string>(
            {"token", token.toJSON()}, {"message", '"' + message + '"'}
        );
    }

    // 显示模板实例化，避免实例膨胀导致编译爆内存

#ifdef _MSC_VER
    #define __IMP__(insType, lowerCase, ...) template struct insType<lowerCase, ##__VA_ARGS__>;
#else
    #define __IMP__(insType, lowerCase, ...)                                                       \
        template struct insType<lowerCase __VA_OPT__(, ) __VA_ARGS__>;
#endif

#define __0IMP__(lowerCase, ...) __IMP__(NullaryInstructionBase, lowerCase, __VA_ARGS__)

    __0IMP__("hcf")
    __0IMP__("yield")

#undef __0IMP__

#define __1IMP__(lowerCase, ...) __IMP__(UnaryInstructionBase, lowerCase, __VA_ARGS__)

    __1IMP__("peek", RegisterOrIdentifier)
    __1IMP__("rand", RegisterOrIdentifier)
    __1IMP__("pop", RegisterOrIdentifier)
    __1IMP__("clr", DeviceAliasRef)
    __1IMP__("sleep", RegisterOrNumber)
    __1IMP__("clrd", RegisterOrNumber)
    __1IMP__("push", RegisterOrNumber)
    __1IMP__("jal", JumpTarget)
    __1IMP__("jr", JumpTarget)
    __1IMP__("j", JumpTarget)

#undef __1IMP__

#define __2IMP__(lowerCase, ...) __IMP__(BinaryInstructionBase, lowerCase, __VA_ARGS__)

    __2IMP__("abs", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("acos", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("asin", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("atan", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("ceil", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("cos", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("exp", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("floor", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("log", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("round", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("sin", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("sqrt", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("tan", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("trunc", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("not", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("move", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("sgn", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("seqz", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("snez", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("sgez", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("sgtz", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("slez", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("sltz", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("snan", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("snanz", RegisterOrIdentifier, RegisterOrNumber)
    __2IMP__("bdns", DeviceReference, RegisterOrNumber)
    __2IMP__("bdnsal", DeviceReference, RegisterOrNumber)
    __2IMP__("bdse", DeviceReference, RegisterOrNumber)
    __2IMP__("bdseal", DeviceReference, RegisterOrNumber)
    __2IMP__("brdns", DeviceReference, RegisterOrNumber)
    __2IMP__("brdse", DeviceReference, RegisterOrNumber)
    __2IMP__("sdns", RegisterOrIdentifier, DeviceReference)
    __2IMP__("sdse", RegisterOrIdentifier, DeviceReference)
    __2IMP__("poke", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("beqz", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("beqzal", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bnez", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bnezal", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bgez", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bgezal", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bgtz", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bgtzal", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("blez", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("blezal", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bltz", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bltzal", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("bnan", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("breqz", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("brnez", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("brgez", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("brgtz", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("brlez", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("brltz", RegisterOrNumber, RegisterOrNumber)
    __2IMP__("brnan", RegisterOrNumber, RegisterOrNumber)

#undef __2IMP__

#define __3IMP__(lowerCase, ...) __IMP__(TernaryInstructionBase, lowerCase, __VA_ARGS__)

    __3IMP__("add", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("atan2", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("div", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("max", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("min", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("mod", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("mul", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("pow", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sub", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("rol", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("ror", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("and", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("nor", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("or", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sla", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sll", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sra", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("srl", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("xor", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sapz", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("snaz", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("seq", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sne", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sge", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sgt", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("sle", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("slt", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("get", RegisterOrIdentifier, DeviceReference, RegisterOrNumber)
    __3IMP__("rmap", RegisterOrIdentifier, DeviceAliasRef, RegisterOrNumber)
    __3IMP__("put", DeviceReference, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("l", RegisterOrIdentifier, DeviceReference, LogicType)
    __3IMP__("s", DeviceReference, LogicType, RegisterOrIdentifier)
    __3IMP__("sb", RegisterOrNumber, LogicType, RegisterOrIdentifier)
    __3IMP__("bdnvl", DeviceReference, LogicType, RegisterOrNumber)
    __3IMP__("bdnvs", DeviceReference, LogicType, RegisterOrNumber)
    __3IMP__("beq", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("beqal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bne", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bneal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bge", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bgeal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bgt", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bgtal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("ble", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bleal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("blt", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bltal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bapz", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bapzal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bnaz", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("bnazal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("breq", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("brne", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("brge", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("brgt", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("brle", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("brlt", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("brapz", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __3IMP__("brnaz", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)

#undef __3IMP__

#define __4IMP__(lowerCase, ...) __IMP__(QuaternaryInstructionBase, lowerCase, __VA_ARGS__)

    __4IMP__("clamp", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("lerp", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("ext", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("ins", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("sap", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("sna", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("select", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("ss", DeviceReference, SlotIndex, LogicSlotType, RegisterOrIdentifier)
    __4IMP__("lb", RegisterOrIdentifier, RegisterOrNumber, LogicType, BatchMode)
    __4IMP__("sbn", RegisterOrNumber, RegisterOrNumber, LogicType, RegisterOrIdentifier)
    __4IMP__("sbs", RegisterOrNumber, SlotIndex, LogicSlotType, RegisterOrIdentifier)
    __4IMP__("bap", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("bapal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("bna", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("bnaal", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("brap", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("brna", RegisterOrNumber, RegisterOrNumber, RegisterOrNumber, RegisterOrNumber)
    __4IMP__("ls", RegisterOrIdentifier, DeviceReference, SlotIndex, LogicSlotType)
    __4IMP__("lr", RegisterOrIdentifier, DeviceReference, ReagentMode, JumpTarget)

#undef __4IMP__

#define __5IMP__(lowerCase, ...) __IMP__(QuinaryInstructionBase, lowerCase, __VA_ARGS__)

    __5IMP__("lbn", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, LogicType, BatchMode)
    __5IMP__("lbs", RegisterOrIdentifier, RegisterOrNumber, SlotIndex, LogicSlotType, BatchMode)

#undef __5IMP__

#define __6IMP__(lowerCase, ...) __IMP__(SenaryInstructionBase, lowerCase, __VA_ARGS__)

    __6IMP__("lbns", RegisterOrIdentifier, RegisterOrNumber, RegisterOrNumber, SlotIndex, LogicSlotType, BatchMode)

#undef __6IMP__

#undef __IMP__

}  // namespace stationeers::ic10
