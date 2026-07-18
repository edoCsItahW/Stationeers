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
        return jsonBase<decltype(identifier)>({"identifier", identifier});
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
        return jsonBase<decltype(identifier), decltype(operand), std::string, std::string>(
            {
                {"identifier", identifier}
        },
            {{"number", operand}},
            type ? std::optional{std::pair{"typeName", *type}} : std::nullopt,
            desc ? std::optional{std::pair{"desc", *desc}} : std::nullopt
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
        auto result = std::format(
            "alias {} {}", call(identifier, [](auto&& id) { return id.toString(); }),
            call(registerOrDevice, [](auto&& regOrDev) { return regOrDev.toString(); })
        );

        result += "#:";

        if (type) result += std::format(" @type {} ", *type);

        if (desc) result += std::format(" @desc {}", *desc);

        return result;
    }

    std::string AliasDirective::toJSON() const {
        return jsonBase<decltype(identifier), decltype(registerOrDevice), std::string, std::string>(
            {
                {"identifier", identifier}
        },
            {{"registerOrDevice", registerOrDevice}},
            type ? std::optional{std::pair{"typeName", *type}} : std::nullopt,
            desc ? std::optional{std::pair{"desc", *desc}} : std::nullopt
        );
    }

    // DeviceDocComment

    Pos DeviceDocComment::end() const { return position; }

    std::string DeviceDocComment::toString() const {
        auto result = std::format("#> @device\n#> @name {}", name);

        if (desc) result += std::format("\n#> @desc {}", desc->value);

        for (const auto& s : slots) {
            result += std::format(
                "\n#> @slot {} {}", s.index,
                s.direction == SlotDirection::INPUT ? "input" : "output"
            );

            if (s.desc) result += std::format(" {}", s.desc->value);
        }

        for (const auto& l : logics) {
            std::string accessStr;
            if (l.access == LogicAccess::R)
                accessStr = "r";
            else if (l.access == LogicAccess::W)
                accessStr = "w";
            else
                accessStr = "rw";

            result += std::format("\n#> @logic {} {}", l.name, accessStr);
        }

        for (const auto& m : modes) {
            result += std::format("\n#> @mode {}", m.index);

            if (m.desc) result += std::format(" {}", m.desc->value);
        }

        for (const auto& ls : logicSlots) result += std::format("\n#> @logicSlot {}", ls.name);

        for (const auto& c : connects) {
            result += std::format("\n#> @connect {}", c.index);

            if (c.desc) result += std::format(" {}", c.desc->value);
        }

        result += "\n#> @end-device";

        return result;
    }

    std::string DeviceDocComment::toJSON() const {
        return jsonBase<
            std::string, std::string, std::string, std::string, std::string, std::string,
            std::string>(
            {"name", name}, {"desc", desc ? desc->value : "null"},
            {"slots", seqJSON(
                          slots,
                          [this](const DeviceSlot& slot) -> std::string {
                              return fieldsJSON<std::string, std::string, std::string>(
                                  {"number", slot.index},
                                  {"direction",
                                   slot.direction == SlotDirection::INPUT ? "input" : "output"},
                                  {"desc", slot.desc ? slot.desc->value : "null"}
                              );
                          }
                      )},
            {"logics", seqJSON(
                           logics,
                           [this](const DeviceLogic& logic) -> std::string {
                               std::string accessStr;
                               if (logic.access == LogicAccess::R)
                                   accessStr = "r";
                               else if (logic.access == LogicAccess::W)
                                   accessStr = "w";
                               else
                                   accessStr = "rw";

                               return fieldsJSON<std::string, std::string>(
                                   {"name", logic.name}, {"access", accessStr}
                               );
                           }
                       )},
            {"modes", seqJSON(
                          modes,
                          [this](const DeviceMode& mode) -> std::string {
                              return fieldsJSON<std::string, std::string>(
                                  {"number", mode.index},
                                  {"desc", mode.desc ? mode.desc->value : "null"}
                              );
                          }
                      )},
            {"logicSlots", seqJSON(
                               logicSlots,
                               [](const DeviceLogicSlot& logicSlot) -> std::string {
                                   return std::format(R"({{ "name": "{}" }})", logicSlot.name);
                               }
                           )},
            {"connects", seqJSON(connects, [this](const DeviceConnect& connect) -> std::string {
                 return fieldsJSON<std::string, std::string>(
                     {"number", connect.index},
                     {"desc", connect.desc ? connect.desc->value : "null"}
                 );
             })}
        );
    }

    // EnumDocComment

    Pos EnumDocComment::end() const { return position; }

    std::string EnumDocComment::toString() const {
        auto result = std::format("#> @enum");

        result += std::format("\n#> @name {}", name);

        if (desc) result += std::format("\n#> @desc {}", desc->value);

        for (const auto& v : values) result += std::format("\n#> @value {} {}", v.name, v.value);

        result += "\n#> @end-enum";

        return result;
    }

    std::string EnumDocComment::toJSON() const {
        return jsonBase<std::string, std::string, std::string>(
            {"name", name}, {"desc", desc ? desc->value : "null"},
            {"values", seqJSON(values, [this](auto&& val) {
                 return fieldsJSON<std::string, std::string, std::string>(
                     {"name", val.name}, {"value", val.value},
                     {"desc", val.desc ? val.desc->value : "null"}
                 );
             })}
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

    std::string Constant::toJSON() const {
        return jsonBase<std::string>({"keyword", '"' + keyword + '"'});
    }

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

    std::string DescValue::toJSON() const {
        return std::format(
            R"({{"kind": "{}", "value": "{}"}})", kind == Kind::TEXT ? "text" : "link", value
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

    __1IMP__("peek", OperandType::REG_IDENT)
    __1IMP__("rand", OperandType::REG_IDENT)
    __1IMP__("pop", OperandType::REG_IDENT)
    __1IMP__("clr", OperandType::DEV_ALIAS)
    __1IMP__("sleep", OperandType::REG_NUM)
    __1IMP__("clrd", OperandType::REG_NUM)
    __1IMP__("push", OperandType::REG_NUM)
    __1IMP__("jal", OperandType::JUMP_TARGET)
    __1IMP__("jr", OperandType::JUMP_TARGET)
    __1IMP__("j", OperandType::JUMP_TARGET)

#undef __1IMP__

#define __2IMP__(lowerCase, ...) __IMP__(BinaryInstructionBase, lowerCase, __VA_ARGS__)

    __2IMP__("abs", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("acos", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("asin", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("atan", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("ceil", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("cos", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("exp", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("floor", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("log", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("round", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("sin", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("sqrt", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("tan", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("trunc", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("not", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("move", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("sgn", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("seqz", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("snez", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("sgez", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("sgtz", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("slez", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("sltz", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("snan", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("snanz", OperandType::REG_IDENT, OperandType::REG_NUM)
    __2IMP__("bdns", OperandType::DEV_REF, OperandType::REG_NUM)
    __2IMP__("bdnsal", OperandType::DEV_REF, OperandType::REG_NUM)
    __2IMP__("bdse", OperandType::DEV_REF, OperandType::REG_NUM)
    __2IMP__("bdseal", OperandType::DEV_REF, OperandType::REG_NUM)
    __2IMP__("brdns", OperandType::DEV_REF, OperandType::REG_NUM)
    __2IMP__("brdse", OperandType::DEV_REF, OperandType::REG_NUM)
    __2IMP__("sdns", OperandType::REG_IDENT, OperandType::DEV_REF)
    __2IMP__("sdse", OperandType::REG_IDENT, OperandType::DEV_REF)
    __2IMP__("poke", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("beqz", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("beqzal", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bnez", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bnezal", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bgez", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bgezal", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bgtz", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bgtzal", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("blez", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("blezal", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bltz", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bltzal", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("bnan", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("breqz", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("brnez", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("brgez", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("brgtz", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("brlez", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("brltz", OperandType::REG_NUM, OperandType::REG_NUM)
    __2IMP__("brnan", OperandType::REG_NUM, OperandType::REG_NUM)

#undef __2IMP__

#define __3IMP__(lowerCase, ...) __IMP__(TernaryInstructionBase, lowerCase, __VA_ARGS__)

    __3IMP__("add", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("atan2", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("div", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("max", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("min", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("mod", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("mul", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("pow", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sub", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("rol", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("ror", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("and", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("nor", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("or", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sla", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sll", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sra", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("srl", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("xor", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sapz", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("snaz", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("seq", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sne", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sge", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sgt", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("sle", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("slt", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("get", OperandType::REG_IDENT, OperandType::DEV_REF, OperandType::REG_NUM)
    __3IMP__("rmap", OperandType::REG_IDENT, OperandType::DEV_ALIAS, OperandType::REG_NUM)
    __3IMP__("put", OperandType::DEV_REF, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("l", OperandType::REG_IDENT, OperandType::DEV_REF, OperandType::LOGIC_TYPE)
    __3IMP__("s", OperandType::DEV_REF, OperandType::LOGIC_TYPE, OperandType::REG_IDENT)
    __3IMP__("sb", OperandType::REG_NUM, OperandType::LOGIC_TYPE, OperandType::REG_IDENT)
    __3IMP__("bdnvl", OperandType::DEV_REF, OperandType::LOGIC_TYPE, OperandType::REG_NUM)
    __3IMP__("bdnvs", OperandType::DEV_REF, OperandType::LOGIC_TYPE, OperandType::REG_NUM)
    __3IMP__("beq", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("beqal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bne", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bneal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bge", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bgeal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bgt", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bgtal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("ble", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bleal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("blt", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bltal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bapz", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bapzal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bnaz", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("bnazal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("breq", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("brne", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("brge", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("brgt", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("brle", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("brlt", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("brapz", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __3IMP__("brnaz", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)

#undef __3IMP__

#define __4IMP__(lowerCase, ...) __IMP__(QuaternaryInstructionBase, lowerCase, __VA_ARGS__)

    __4IMP__("clamp", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("lerp", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("ext", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("ins", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("sap", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("sna", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("select", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("ss", OperandType::DEV_REF, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT, OperandType::REG_IDENT)
    __4IMP__("lb", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::LOGIC_TYPE, OperandType::BATCH_MODE)
    __4IMP__("sbn", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::LOGIC_TYPE, OperandType::REG_IDENT)
    __4IMP__("sbs", OperandType::REG_NUM, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT, OperandType::REG_IDENT)
    __4IMP__("bap", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("bapal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("bna", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("bnaal", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("brap", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("brna", OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::REG_NUM)
    __4IMP__("ls", OperandType::REG_IDENT, OperandType::DEV_REF, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT)
    __4IMP__("lr", OperandType::REG_IDENT, OperandType::DEV_REF, OperandType::REAGENT_MODE, OperandType::JUMP_TARGET)

#undef __4IMP__

#define __5IMP__(lowerCase, ...) __IMP__(QuinaryInstructionBase, lowerCase, __VA_ARGS__)

    __5IMP__("lbn", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::LOGIC_TYPE, OperandType::BATCH_MODE)
    __5IMP__("lbs", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT, OperandType::BATCH_MODE)

#undef __5IMP__

#define __6IMP__(lowerCase, ...) __IMP__(SenaryInstructionBase, lowerCase, __VA_ARGS__)

    __6IMP__(
        "lbns", OperandType::REG_IDENT, OperandType::REG_NUM, OperandType::REG_NUM, OperandType::SLOT_IDX, OperandType::LOGIC_SLOT,
        OperandType::BATCH_MODE
    )

#undef __6IMP__

#undef __IMP__

}  // namespace stationeers::ic10
