// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file parser.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 16:55
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

#include "ic10/parser/parser.hpp"
#include "common/exception/debug.hpp"
#include "common/utils/common.hpp"
#include "ic10/locals/local.hpp"
#include <format>

namespace stationeers::ic10 {

    Parser::Parser(const std::vector<std::shared_ptr<Token>>& tokens, bool debug)
        : tokens_(tokens)
        , debug_(debug) {}

    Program Parser::parse() {
        try {
            auto program = Program{current()->pos};

            skip();

            while (inScope()) {
                if (current()->type == TokenType::END) return program;

                program.statements.push_back(parseStatement());

                // 除最后一个语句可以直接以END结尾
                if (current()->type == TokenType::END) break;

                // 其它语句需要以换行结尾
                if (current()->type == TokenType::NEWLINE) expect(TokenType::NEWLINE, false);

                // 其余换行跳过
                skip();
            }

            return program;
        } catch (const std::exception& e) {
            Console::error(std::string(e.what()));
            return {};
        }
    }

    Program Parser::parsing(const std::vector<std::shared_ptr<Token>>& tokens, bool debug) {
        auto parser = Parser{tokens, debug};

        return parser.parse();
    }

    Statement Parser::parseStatement() {
        if (debug_) Console::log(std::format("Statement: {}", current()->toString()));

        try {
            if (!current()) return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};

            int layer = 1;

            switch (current()->type) {
                using enum TokenType;
                case IDENTIFIER: return wide_cast<Statement>(parseLabelDef(layer));
                case KEYWORD_ALIAS:
                case KEYWORD_DEFINE: return wide_cast<Statement>(parsePreprocessorDirective(layer));
                default: return wide_cast<Statement>(parseExecutableInstruction(layer));
            }
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<LabelDef> Parser::parseLabelDef(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Label");

        try {
            LabelDef labelDef{current()->pos};

            labelDef.identifier = parseIdentifier(++layer);

            expect(TokenType::COLON);

            return labelDef;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    PreprocessorDirective Parser::parsePreprocessorDirective(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "PreprocessorDirective");

        try {
            ++layer;

            if (current()->type == TokenType::KEYWORD_ALIAS)
                return wide_cast<PreprocessorDirective>(parseAliasDirective(layer));

            if (current()->type == TokenType::KEYWORD_DEFINE)
                return wide_cast<PreprocessorDirective>(parseDefineDirective(layer));

            return ErrorNode{
                *current(), Loc::msgFormat<MsgId::IMP2_1>(enumToStr(current()->type))
            };
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<AliasDirective> Parser::parseAliasDirective(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "AliasDirective");

        try {
            ++layer;

            AliasDirective aliasDirective{current()->pos};

            expect(TokenType::KEYWORD_ALIAS);

            aliasDirective.identifier = parseIdentifier(layer);

            aliasDirective.registerOrDevice = parseRegisterOrDevice(layer);

            return aliasDirective;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ExecutableInstruction Parser::parseExecutableInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "ExecutableInstruction");

        try {
            ++layer;

            if (NullaryInstructionMap::contains(current()->type))
                return wide_cast<ExecutableInstruction>(parseNullaryInstruction(layer));

            if (UnaryInstructionMap::contains(current()->type))
                return wide_cast<ExecutableInstruction>(parseUnaryInstruction(layer));

            if (BinaryInstructionMap::contains(current()->type))
                return wide_cast<ExecutableInstruction>(parseBinaryInstruction(layer));

            if (TernaryInstructionMap::contains(current()->type))
                return wide_cast<ExecutableInstruction>(parseTernaryInstruction(layer));

            if (QuaternaryInstructionMap::contains(current()->type))
                return wide_cast<ExecutableInstruction>(parseQuaternaryInstruction(layer));

            if (QuinaryInstructionMap::contains(current()->type))
                return wide_cast<ExecutableInstruction>(parseQuinaryInstruction(layer));

            if (SenaryInstructionMap::contains(current()->type))
                return wide_cast<ExecutableInstruction>(parseSenaryInstruction(layer));

            auto errNode =
                ErrorNode{*current(), Loc::msgFormat<MsgId::IMP3_1>(enumToStr(current()->type))};

            gotoNextLine();

            return errNode;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

#define VARIANT_TRANS_FACTORY(narrowType, wideType, ...)                                           \
    wide_cast<wideType>(std::apply(                                                                \
        []<typename... Args>(Args&&... args) {                                                     \
            return narrowType::make(std::forward<Args>(args)...);                                  \
        },                                                                                         \
        std::tuple{__VA_ARGS__}                                                                    \
    ));

#ifdef _MSC_VER

    #define INSTRUCTION_CASE(narrowType, wideType, currentType, ...)                               \
        if (narrowType::contains(currentType))                                                     \
        return VARIANT_TRANS_FACTORY(narrowType, wideType, currentType, ##__VA_ARGS__)

#else

    #define INSTRUCTION_CASE(narrowType, wideType, currentType, ...)                               \
        if (narrowType::contains(currentType))                                                     \
        return VARIANT_TRANS_FACTORY(narrowType, wideType, currentType __VA_OPT__(, ) __VA_ARGS__)

#endif

    NullaryInstruction Parser::parseNullaryInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "NullaryInstruction");

        try {
            const auto c = current();

            consume();

            return wide_cast<NullaryInstruction>(NullaryInstructionMap::make(c->type, c->pos));
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    UnaryInstruction Parser::parseUnaryInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "UnaryInstruction");

        ++layer;

        const auto c = current();

        consume();

        if (UnaryInstructionMap_RI::contains(c->type))
            return std::visit(
                // 该lambda将宽变体转换为窄变体
                []<typename T>(T&& t) -> UnaryInstruction { return std::forward<T>(t); },
                // 使用std::apply将参数包展开，应用参数
                std::apply(
                    // 该lambda将函数模板转为可调用对象，以符合std::apply的要求
                    []<typename... Args>(Args&&... args) {
                        return UnaryInstructionMap_RI::make(std::forward<Args>(args)...);
                    },
                    // 使用列表初始化显式地从左向右进行参数求值，而非编译器的从右向左求值
                    std::tuple{c->type, c->pos, parseRegisterOrIdentifier(layer)}
                )
            );

        INSTRUCTION_CASE(
            UnaryInstructionMap_DR, UnaryInstruction, c->type, c->pos, parseDeviceReference(layer)
        )

        INSTRUCTION_CASE(
            UnaryInstructionMap_O, UnaryInstruction, c->type, c->pos, parseOperand(layer)
        )

        auto errNode = ErrorNode{*current(), Loc::msgFormat<MsgId::IMP4_1>(enumToStr(c->type))};

        gotoNextLine();

        return errNode;
    }

    BinaryInstruction Parser::parseBinaryInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "BinaryInstruction");

        ++layer;

        const auto c = current();

        consume();

        INSTRUCTION_CASE(
            BinaryInstructionMap_RI_O, BinaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseOperand(layer)
        )

        INSTRUCTION_CASE(
            BinaryInstructionMap_DR_O, BinaryInstruction, c->type, c->pos,
            parseDeviceReference(layer), parseOperand(layer)
        )

        INSTRUCTION_CASE(
            BinaryInstructionMap_RI_DR, BinaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseDeviceReference(layer)
        )

        INSTRUCTION_CASE(
            BinaryInstructionMap_O_O, BinaryInstruction, c->type, c->pos, parseOperand(layer),
            parseOperand(layer)
        )

        auto errNode = ErrorNode{*current(), Loc::msgFormat<MsgId::IMP5_1>(enumToStr(c->type))};

        gotoNextLine();

        return errNode;
    }

    TernaryInstruction Parser::parseTernaryInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "TernaryInstruction");

        ++layer;

        const auto c = current();

        consume();

        INSTRUCTION_CASE(
            TernaryInstructionMap_RI_O_O, TernaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseOperand(layer), parseOperand(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_RI_DR_O, TernaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseDeviceReference(layer), parseOperand(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_DR_O_O, TernaryInstruction, c->type, c->pos,
            parseDeviceReference(layer), parseOperand(layer), parseOperand(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_RI_DR_LT, TernaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseDeviceReference(layer), parseLogicType(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_RI_DR_RM, TernaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseDeviceReference(layer), parseReagentMode(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_DR_LT_RI, TernaryInstruction, c->type, c->pos,
            parseDeviceReference(layer), parseLogicType(layer), parseRegisterOrIdentifier(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_O_LT_RI, TernaryInstruction, c->type, c->pos, parseOperand(layer),
            parseLogicType(layer), parseRegisterOrIdentifier(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_DR_LT_O, TernaryInstruction, c->type, c->pos,
            parseDeviceReference(layer), parseLogicType(layer), parseOperand(layer)
        )

        INSTRUCTION_CASE(
            TernaryInstructionMap_O_O_O, TernaryInstruction, c->type, c->pos, parseOperand(layer),
            parseOperand(layer), parseOperand(layer)
        )

        auto errNode = ErrorNode{*current(), Loc::msgFormat<MsgId::IMP6_1>(enumToStr(c->type))};

        gotoNextLine();

        return errNode;
    }

    QuaternaryInstruction Parser::parseQuaternaryInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "QuaternaryInstruction");

        ++layer;

        const auto c = current();

        consume();

        INSTRUCTION_CASE(
            QuaternaryInstructionMap_RI_O_O_O, QuaternaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseOperand(layer), parseOperand(layer),
            parseOperand(layer)
        )

        INSTRUCTION_CASE(
            QuaternaryInstructionMap_DR_SI_LS_RI, QuaternaryInstruction, c->type, c->pos,
            parseDeviceReference(layer), parseSlotIndex(layer), parseLogicSlotType(layer),
            parseRegisterOrIdentifier(layer)
        )

        INSTRUCTION_CASE(
            QuaternaryInstructionMap_RI_O_LT_BM, QuaternaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseOperand(layer), parseLogicType(layer),
            parseBatchMode(layer)
        )

        INSTRUCTION_CASE(
            QuaternaryInstructionMap_O_O_LT_RI, QuaternaryInstruction, c->type, c->pos,
            parseOperand(layer), parseOperand(layer), parseLogicSlotType(layer),
            parseRegisterOrIdentifier(layer)
        )

        INSTRUCTION_CASE(
            QuaternaryInstructionMap_O_O_O_O, QuaternaryInstruction, c->type, c->pos,
            parseOperand(layer), parseOperand(layer), parseOperand(layer), parseOperand(layer)
        )

        INSTRUCTION_CASE(
            QuaternaryInstructionMap_RI_DR_SI_LS, QuaternaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseDeviceReference(layer), parseSlotIndex(layer),
            parseLogicSlotType(layer)
        )

        auto errNode = ErrorNode{*current(), Loc::msgFormat<MsgId::IMP7_1>(enumToStr(c->type))};

        gotoNextLine();

        return errNode;
    }

    QuinaryInstruction Parser::parseQuinaryInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "QuinaryInstruction");

        ++layer;

        const auto c = current();

        consume();

        INSTRUCTION_CASE(
            QuinaryInstructionMap_RI_O_O_LT_BM, QuinaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseOperand(layer), parseOperand(layer),
            parseLogicType(layer), parseBatchMode(layer)
        )

        INSTRUCTION_CASE(
            QuinaryInstructionMap_RI_O_SI_LS_BM, QuinaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseOperand(layer), parseSlotIndex(layer),
            parseLogicSlotType(layer), parseBatchMode(layer)
        )

        auto errNode = ErrorNode{*current(), Loc::msgFormat<MsgId::IMP8_1>(enumToStr(c->type))};

        gotoNextLine();

        return errNode;
    }

    SenaryInstruction Parser::parseSenaryInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "SenaryInstruction");

        ++layer;

        const auto c = current();

        consume();

        INSTRUCTION_CASE(
            SenaryInstructionMap_RI_O_O_SI_LS_BM, SenaryInstruction, c->type, c->pos,
            parseRegisterOrIdentifier(layer), parseOperand(layer), parseOperand(layer),
            parseSlotIndex(layer), parseLogicSlotType(layer), parseBatchMode(layer)
        )

        auto errNode = ErrorNode{*current(), Loc::msgFormat<MsgId::IMP9_1>(enumToStr(c->type))};

        gotoNextLine();

        return errNode;
    }

    ShallowErrorable<DefineDirective> Parser::parseDefineDirective(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "DefineDirective");

        try {
            ++layer;

            DefineDirective defineDirective{current()->pos};

            expect(TokenType::KEYWORD_DEFINE);

            defineDirective.identifier = parseIdentifier(layer);

            defineDirective.operand = parseOperand(layer);

            return defineDirective;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    Operand Parser::parseOperand(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Operand");

        try {
            ++layer;

            if (!current()) return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};

            switch (current()->type) {
                using enum TokenType;
                case REGISTER: return wide_cast<Operand>(parseRegister(layer));
                case DEVICE: return wide_cast<Operand>(parseDevice(layer));
                case INTEGER:
                case FLOAT:
                case HEX_NUMBER:
                case BINARY_NUMBER: return wide_cast<Operand>(parseNumber(layer));
                case IDENTIFIER: return wide_cast<Operand>(parseIdentifier(layer));
                case KEYWORD_HASH:
                case KEYWORD_STR: return wide_cast<Operand>(parseMacroCall(layer));
                case KEYWORD_NAN:
                case KEYWORD_PINF:
                case KEYWORD_NINF:
                case KEYWORD_PI:
                case KEYWORD_TAN:
                case KEYWORD_DEG2RAD:
                case KEYWORD_RAD2DEG:
                case KEYWORD_EPSILON:
                case KEYWORD_GAS_CONSTANT: return wide_cast<Operand>(parseConstant(layer));
                default:
                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP10_1>(enumToStr(current()->type))
                    };
            }
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<Register> Parser::parseRegister(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Register");

        try {
            Register reg{current()->pos};

            reg.value = expect(TokenType::REGISTER)->lexeme;

            return reg;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    RegisterOrDevice Parser::parseRegisterOrDevice(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "RegisterOrDevice");

        ++layer;

        if (current()->type == TokenType::REGISTER)
            return wide_cast<RegisterOrDevice>(parseRegister(layer));

        if (current()->type == TokenType::DEVICE)
            return wide_cast<RegisterOrDevice>(parseDevice(layer));

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP11_1>(enumToStr(current()->type))};
    }

    RegisterOrIdentifier Parser::parseRegisterOrIdentifier(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "RegisterOrIdentifier");

        ++layer;

        if (current()->type == TokenType::REGISTER)
            return wide_cast<RegisterOrIdentifier>(parseRegister(layer));

        if (current()->type == TokenType::IDENTIFIER)
            return wide_cast<RegisterOrIdentifier>(parseIdentifier(layer));

        return ErrorNode{
            *current(true), Loc::msgFormat<MsgId::IMP12_1>(enumToStr(current()->type))
        };
    }

    DeviceReference Parser::parseDeviceReference(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "DeviceReference");

        try {
            ++layer;

            if (!current()) return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};

            switch (current()->type) {
                using enum TokenType;
                case DEVICE: return wide_cast<DeviceReference>(parseDevice(layer));
                case IDENTIFIER:
                case REGISTER: return wide_cast<DeviceReference>(parseRegisterOrIdentifier(layer));
                case INTEGER:
                case FLOAT:
                case HEX_NUMBER:
                case BINARY_NUMBER: return wide_cast<DeviceReference>(parseNumber(layer));
                default:
                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP13_1>(enumToStr(current()->type))
                    };
            }
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    MacroCall Parser::parseMacroCall(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "MacroCall");

        ++layer;

        if (current()->type == TokenType::KEYWORD_HASH)
            return wide_cast<MacroCall>(parseHashCall(layer));

        if (current()->type == TokenType::KEYWORD_STR)
            return wide_cast<MacroCall>(parseStrCall(layer));

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP14_1>(enumToStr(current()->type))};
    }

    ShallowErrorable<HashCall> Parser::parseHashCall(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "HashCall");

        try {
            HashCall hashCall{current()->pos};

            expect(TokenType::KEYWORD_HASH);

            expect(TokenType::LPAREN);

            hashCall.value = parseString(++layer);

            hashCall.endPosition = expect(TokenType::RPAREN)->pos;

            return hashCall;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<StrCall> Parser::parseStrCall(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "StrCall");

        try {
            StrCall strCall{current()->pos};

            expect(TokenType::KEYWORD_STR);

            expect(TokenType::LPAREN);

            strCall.value = parseString(++layer);

            strCall.endPosition = expect(TokenType::RPAREN)->pos;

            return strCall;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<Constant> Parser::parseConstant(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Constant");

        try {
            Constant constant{current()->pos};

            constant.keyword = current()->lexeme;

            consume();

            return constant;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    LogicType Parser::parseLogicType(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "LogicType");
        try {
            return parseIdentifierOrNumber(++layer);
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    SlotIndex Parser::parseSlotIndex(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "SlotIndex");

        return parseNumber(++layer);
    }

    LogicSlotType Parser::parseLogicSlotType(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "LogicSlotType");

        return parseIdentifierOrNumber(++layer);
    }

    BatchMode Parser::parseBatchMode(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "BatchMode");

        return parseIdentifierOrNumber(++layer);
    }

    ReagentMode Parser::parseReagentMode(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "ReagentMode");

        return parseIdentifierOrNumber(++layer);
    }

    Errorable<Identifier, Number> Parser::parseIdentifierOrNumber(int layer) {
        try {
            ++layer;

            if (!current()) return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};

            switch (current()->type) {
                using enum TokenType;
                case IDENTIFIER:
                    return wide_cast<Errorable<Identifier, Number>>(parseIdentifier(layer));
                case INTEGER:
                case FLOAT:
                case HEX_NUMBER:
                case BINARY_NUMBER:
                    return wide_cast<Errorable<Identifier, Number>>(parseNumber(layer));
                default:
                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP15_1>(enumToStr(current()->type))
                    };
            }
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<Device> Parser::parseDevice(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Device");

        try {
            Device device{current()->pos};

            device.value = expect(TokenType::DEVICE)->lexeme;

            return device;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<String> Parser::parseString(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "String");
        try {
            String string{current()->pos};

            string.value = expect(TokenType::STRING)->lexeme;

            return string;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<Identifier> Parser::parseIdentifier(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Identifier");

        try {
            Identifier identifier{current()->pos};

            identifier.value = expect(TokenType::IDENTIFIER)->lexeme;

            return identifier;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    Number Parser::parseNumber(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Number");

        try {
            ++layer;

            if (!current()) return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};

            switch (current()->type) {
                using enum TokenType;
                case INTEGER: return wide_cast<Number>(parseInteger(layer));
                case FLOAT: return wide_cast<Number>(parseFloat(layer));
                case HEX_NUMBER: return wide_cast<Number>(parseHexNumber(layer));
                case BINARY_NUMBER: return wide_cast<Number>(parseBinaryNumber(layer));
                default:
                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP16_1>(enumToStr(current()->type))
                    };
            }
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<Integer> Parser::parseInteger(int layer) {
        try {
            Integer integer{current()->pos};

            integer.value = expect(TokenType::INTEGER)->lexeme;

            return integer;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<Float> Parser::parseFloat(int layer) {
        try {
            Float floatNum{current()->pos};

            floatNum.value = expect(TokenType::FLOAT)->lexeme;

            return floatNum;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<HexNumber> Parser::parseHexNumber(int layer) {
        try {
            HexNumber hexNum{current()->pos};

            hexNum.value = expect(TokenType::HEX_NUMBER)->lexeme;

            return hexNum;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<BinaryNumber> Parser::parseBinaryNumber(int layer) {
        try {
            BinaryNumber binNum{current()->pos};

            binNum.value = expect(TokenType::BINARY_NUMBER)->lexeme;

            return binNum;
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    bool Parser::inScope() const {
        return idx_ < tokens_.size() && current()->type != TokenType::END;
    }

    void Parser::skip() {
        while (inScope()
               && (current()->type == TokenType::NEWLINE
                   || current()->category == TokenCategory::COMMENT))
            idx_++;

        // idx_--;
    }

    void Parser::consume() const {
        if (inScope()) idx_++;
    }

    void Parser::gotoNextLine() const {
        while (inScope() && current()->type != TokenType::NEWLINE) idx_++;

        if (inScope()) consume();
    }

    std::shared_ptr<Token> Parser::expect(TokenType type, bool skipWs, bool errorConsume) {
        if (skipWs && type != TokenType::NEWLINE) skip();

        if (inScope() && current()->type == type) return tokens_[idx_++];

        if (inScope() && errorConsume) consume();

        throw std::runtime_error(Loc::msgFormat<MsgId::IEP1_1>(enumToStr(type)));
    }

    std::shared_ptr<Token> Parser::current(const bool consume) const {
        if (idx_ < tokens_.size()) return tokens_[consume ? idx_++ : idx_];

        return nullptr;
    }

}  // namespace stationeers::ic10
