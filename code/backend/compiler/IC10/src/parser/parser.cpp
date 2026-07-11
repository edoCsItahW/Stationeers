// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

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
#include "common/exception/error.hpp"
#include "common/utils/common.hpp"
#include "ic10/locals/local.hpp"
#include <format>

namespace stationeers::ic10 {

    Parser::Parser(const std::vector<std::shared_ptr<Token>>& tokens, bool debug)
        : tokens_(tokens)
        , debug_(debug) {}

    Program Parser::parse() {
        auto program = Program{current()->pos};

        skip();

        while (inScope()) {
            if (current()->type == TokenType::END) return program;

            program.statements.push_back(parseStatement());

            // 跳过行内注释（注释与语句在同一行，不需要换行分隔）
            while (inScope() && current()->category == TokenCategory::COMMENT) {
                consume();
            }

            // 除最后一个语句可以直接以END结尾
            if (current()->type == TokenType::END) break;

            // 语句之间必须以换行分隔
            // 参考IC10.g4: program : statement (NEWLINE+ statement)* NEWLINE* EOF;
            if (current()->type == TokenType::NEWLINE) {
                expect(TokenType::NEWLINE, false);
            } else {
                reporter_.error<MsgId::IEP2>(current()->pos, endPos(*current()));
            }

            // 其余换行和注释跳过
            skip();
        }

        return program;
    }

    Program Parser::parsing(const std::vector<std::shared_ptr<Token>>& tokens, bool debug) {
        auto parser = Parser{tokens, debug};

        return parser.parse();
    }

    Statement Parser::parseStatement() {
        if (debug_) Console::log(std::format("Statement: {}", current()->toString()));

        if (!current()) {
            reporter_.error<MsgId::IMP1>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};
        }

        int layer = 1;

        switch (current()->type) {
            using enum TokenType;
            case IDENTIFIER: return wide_cast<Statement>(parseLabelDef(layer));
            case KEYWORD_ALIAS:
            case KEYWORD_DEFINE: return wide_cast<Statement>(parsePreprocessorDirective(layer));
            default: return wide_cast<Statement>(parseExecutableInstruction(layer));
        }
    }

    ShallowErrorable<LabelDef> Parser::parseLabelDef(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Label");

        LabelDef labelDef{current()->pos};

        labelDef.identifier = parseIdentifier(++layer);

        try {
            expect(TokenType::COLON);

        } catch (const Error&) {
            reporter_.error<MsgId::IMP23>(current()->pos, endPos(*current()));

            auto errToken = *current();
            consume();
            return ErrorNode{errToken, Loc::msgStr<MsgId::IMP23>()};
        }

        return labelDef;
    }

    PreprocessorDirective Parser::parsePreprocessorDirective(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "PreprocessorDirective");

        ++layer;

        if (current()->type == TokenType::KEYWORD_ALIAS) return wide_cast<PreprocessorDirective>(parseAliasDirective(layer));

        if (current()->type == TokenType::KEYWORD_DEFINE) return wide_cast<PreprocessorDirective>(parseDefineDirective(layer));

        reporter_.errorWith<MsgId::IMP2_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        auto errToken = *current();
        consume();
        return ErrorNode{errToken, Loc::msgFormat<MsgId::IMP2_1>(enumToStr(errToken.type))};
    }

    ShallowErrorable<AliasDirective> Parser::parseAliasDirective(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "AliasDirective");

        ++layer;

        AliasDirective aliasDirective{current()->pos};

        try {
            expect(TokenType::KEYWORD_ALIAS);

        } catch (const Error&) {
            reporter_.error<MsgId::IMP24>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP24>()};
        }

        aliasDirective.identifier = parseIdentifier(layer);

        aliasDirective.registerOrDevice = parseRegisterOrDevice(layer);

        return aliasDirective;
    }

    ExecutableInstruction Parser::parseExecutableInstruction(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "ExecutableInstruction");

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

        reporter_.errorWith<MsgId::IMP3_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        auto errToken = *current();
        consume();
        return ErrorNode{errToken, Loc::msgFormat<MsgId::IMP3_1>(enumToStr(errToken.type))};
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

        const auto c = current();

        consume();

        return wide_cast<NullaryInstruction>(NullaryInstructionMap::make(c->type, c->pos));
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

        reporter_.errorWith<MsgId::IMP4_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP4_1>(enumToStr(c->type))};
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

        reporter_.errorWith<MsgId::IMP5_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP5_1>(enumToStr(c->type))};
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

        reporter_.errorWith<MsgId::IMP6_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP6_1>(enumToStr(c->type))};
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

        reporter_.errorWith<MsgId::IMP7_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP7_1>(enumToStr(c->type))};
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

        reporter_.errorWith<MsgId::IMP8_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP8_1>(enumToStr(c->type))};
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

        reporter_.errorWith<MsgId::IMP9_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        return ErrorNode{*current(), Loc::msgFormat<MsgId::IMP9_1>(enumToStr(c->type))};
    }

    ShallowErrorable<DefineDirective> Parser::parseDefineDirective(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "DefineDirective");

        ++layer;

        DefineDirective defineDirective{current()->pos};

        try {
            expect(TokenType::KEYWORD_DEFINE);

        } catch (const Error&) {
            reporter_.error<MsgId::IMP25>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP25>()};
        }

        defineDirective.identifier = parseIdentifier(layer);

        defineDirective.operand = parseOperand(layer);

        return defineDirective;
    }

    Operand Parser::parseOperand(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Operand");

        ++layer;

        if (!current()) {
            reporter_.error<MsgId::IMP1>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};
        }

        // 情况1：是操作数起始 → 正常解析
        if (isStartToken<Operand>(current()->type)) {
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
                case KEYWORD_TAU:
                case KEYWORD_DEG2RAD:
                case KEYWORD_RAD2DEG:
                case KEYWORD_EPSILON:
                case KEYWORD_GAS_CONSTANT: return wide_cast<Operand>(parseConstant(layer));
                default: {
                    // 理论上不会到这里，isStartToken 已过滤
                    reporter_.errorWith<MsgId::IMP10_1>(
                        current()->pos, endPos(*current()), enumToStr(current()->type)
                    );

                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP10_1>(enumToStr(current()->type))
                    };
                }
            }
        }

        // 情况2：是停止点（语句开始/行结束）→ 不消耗，返回 ErrorNode
        if (current()->type == TokenType::NEWLINE ||
            current()->type == TokenType::END ||
            isStatementStart(current()->type)) {
            reporter_.error<MsgId::IMP17>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP17>()};
        }

        // 情况3：坏 token → 消耗，继续尝试
        reporter_.errorWith<MsgId::IMP10_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        consume();

        return parseOperand(layer);
    }

    ShallowErrorable<Register> Parser::parseRegister(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Register");

        Register reg{current()->pos};

        try {
            reg.value = expect(TokenType::REGISTER)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return reg;
    }

    RegisterOrDevice Parser::parseRegisterOrDevice(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "RegisterOrDevice");

        ++layer;

        // 情况1：是合法起始 → 正常解析
        if (isStartToken<RegisterOrDevice>(current()->type)) {
            if (current()->type == TokenType::REGISTER)
                return wide_cast<RegisterOrDevice>(parseRegister(layer));

            if (current()->type == TokenType::DEVICE)
                return wide_cast<RegisterOrDevice>(parseDevice(layer));
        }

        // 情况2：停止点 → 不消耗
        if (current()->type == TokenType::NEWLINE ||
            current()->type == TokenType::END ||
            isStatementStart(current()->type)) {
            reporter_.error<MsgId::IMP18>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP18>()};
        }

        // 情况3：坏 token → 消耗，继续尝试
        reporter_.errorWith<MsgId::IMP11_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        consume();

        return parseRegisterOrDevice(layer);
    }

    RegisterOrIdentifier Parser::parseRegisterOrIdentifier(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "RegisterOrIdentifier");

        ++layer;

        // 情况1：是合法起始 → 正常解析
        if (isStartToken<RegisterOrIdentifier>(current()->type)) {
            if (current()->type == TokenType::REGISTER)
                return wide_cast<RegisterOrIdentifier>(parseRegister(layer));

            if (current()->type == TokenType::IDENTIFIER)
                return wide_cast<RegisterOrIdentifier>(parseIdentifier(layer));
        }

        // 情况2：停止点 → 不消耗
        if (current()->type == TokenType::NEWLINE ||
            current()->type == TokenType::END ||
            isStatementStart(current()->type)) {
            reporter_.error<MsgId::IMP19>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP19>()};
        }

        // 情况3：坏 token → 消耗，继续尝试
        reporter_.errorWith<MsgId::IMP12_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        consume();

        return parseRegisterOrIdentifier(layer);
    }

    DeviceReference Parser::parseDeviceReference(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "DeviceReference");

        ++layer;

        if (!current()) {
            reporter_.error<MsgId::IMP1>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};
        }

        // 情况1：是合法起始 → 正常解析
        if (isStartToken<DeviceReference>(current()->type)) {
            switch (current()->type) {
                using enum TokenType;
                case DEVICE: return wide_cast<DeviceReference>(parseDevice(layer));
                case IDENTIFIER:
                case REGISTER: return wide_cast<DeviceReference>(parseRegisterOrIdentifier(layer));
                case INTEGER:
                case FLOAT:
                case HEX_NUMBER:
                case BINARY_NUMBER: return wide_cast<DeviceReference>(parseNumber(layer));
                default: {
                    reporter_.errorWith<MsgId::IMP13_1>(
                        current()->pos, endPos(*current()), enumToStr(current()->type)
                    );

                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP13_1>(enumToStr(current()->type))
                    };
                }
            }
        }

        // 情况2：停止点 → 不消耗
        if (current()->type == TokenType::NEWLINE ||
            current()->type == TokenType::END ||
            isStatementStart(current()->type)) {
            reporter_.error<MsgId::IMP20>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP20>()};
        }

        // 情况3：坏 token → 消耗，继续尝试
        reporter_.errorWith<MsgId::IMP13_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        consume();

        return parseDeviceReference(layer);
    }

    MacroCall Parser::parseMacroCall(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "MacroCall");

        ++layer;

        // 情况1：是合法起始 → 正常解析
        if (isStartToken<MacroCall>(current()->type)) {
            if (current()->type == TokenType::KEYWORD_HASH)
                return wide_cast<MacroCall>(parseHashCall(layer));

            if (current()->type == TokenType::KEYWORD_STR)
                return wide_cast<MacroCall>(parseStrCall(layer));
        }

        // 情况2：停止点 → 不消耗
        if (current()->type == TokenType::NEWLINE ||
            current()->type == TokenType::END ||
            isStatementStart(current()->type)) {
            reporter_.error<MsgId::IMP21>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP21>()};
        }

        // 情况3：坏 token → 消耗，继续尝试
        reporter_.errorWith<MsgId::IMP14_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        consume();

        return parseMacroCall(layer);
    }

    ShallowErrorable<HashCall> Parser::parseHashCall(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "HashCall");

        HashCall hashCall{current()->pos};

        expect(TokenType::KEYWORD_HASH);

        expect(TokenType::LPAREN);

        try {
            hashCall.value = parseString(++layer);

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        hashCall.endPosition = expect(TokenType::RPAREN)->pos;

        return hashCall;
    }

    ShallowErrorable<StrCall> Parser::parseStrCall(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "StrCall");

        StrCall strCall{current()->pos};

        expect(TokenType::KEYWORD_STR);

        expect(TokenType::LPAREN);

        strCall.value = parseString(++layer);

        try {
            strCall.endPosition = expect(TokenType::RPAREN)->pos;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }


        return strCall;
    }

    ShallowErrorable<Constant> Parser::parseConstant(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Constant");

        Constant constant{current()->pos};

        try {
            constant.keyword = current()->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        consume();

        return constant;
    }

    LogicType Parser::parseLogicType(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "LogicType");

        return parseIdentifierOrNumber(++layer);
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
        ++layer;

        if (!current()) {
            reporter_.error<MsgId::IMP1>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};
        }

        // 情况1：是合法起始 → 正常解析
        if (isStartToken<Errorable<Identifier, Number>>(current()->type)) {
            switch (current()->type) {
                using enum TokenType;
                case IDENTIFIER:
                    return wide_cast<Errorable<Identifier, Number>>(parseIdentifier(layer));
                case INTEGER:
                case FLOAT:
                case HEX_NUMBER:
                case BINARY_NUMBER:
                    return wide_cast<Errorable<Identifier, Number>>(parseNumber(layer));
                default: {
                    reporter_.errorWith<MsgId::IMP15_1>(
                        current()->pos, endPos(*current()), enumToStr(current()->type)
                    );

                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP15_1>(enumToStr(current()->type))
                    };
                }
            }
        }

        // 情况2：停止点 → 不消耗
        if (current()->type == TokenType::NEWLINE ||
            current()->type == TokenType::END ||
            isStatementStart(current()->type)) {
            reporter_.error<MsgId::IMP22>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), Loc::msgStr<MsgId::IMP22>()};
        }

        // 情况3：坏 token → 消耗，继续尝试
        reporter_.errorWith<MsgId::IMP15_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        consume();

        return parseIdentifierOrNumber(layer);
    }

    ShallowErrorable<Device> Parser::parseDevice(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Device");

        Device device{current()->pos};

        try {
            device.value = expect(TokenType::DEVICE)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return device;
    }

    ShallowErrorable<String> Parser::parseString(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "String");

        String string{current()->pos};

        try {
            string.value = expect(TokenType::STRING)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return string;
    }

    ShallowErrorable<Identifier> Parser::parseIdentifier(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Identifier");

        Identifier identifier{current()->pos};

        try {
            identifier.value = expect(TokenType::IDENTIFIER)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return identifier;
    }

    Number Parser::parseNumber(int layer) {
        if (debug_) Console::log(std::string(layer * 4, ' ') + "Number");

        try {
            ++layer;

            if (!current()) {
                reporter_.error<MsgId::IMP1>(current()->pos, endPos(*current()));

                return ErrorNode{*current(), Loc::msgStr<MsgId::IMP1>()};
            }

            switch (current()->type) {
                using enum TokenType;
                case INTEGER: return wide_cast<Number>(parseInteger(layer));
                case FLOAT: return wide_cast<Number>(parseFloat(layer));
                case HEX_NUMBER: return wide_cast<Number>(parseHexNumber(layer));
                case BINARY_NUMBER: return wide_cast<Number>(parseBinaryNumber(layer));
                default: {
                    reporter_.errorWith<MsgId::IMP16_1>(
                        current()->pos, endPos(*current()), enumToStr(current()->type)
                    );

                    return ErrorNode{
                        *current(), Loc::msgFormat<MsgId::IMP16_1>(enumToStr(current()->type))
                    };
                }
            }
        } catch (const std::exception& e) { return ErrorNode{*current(), e.what()}; }
    }

    ShallowErrorable<Integer> Parser::parseInteger(int) {
        Integer integer{current()->pos};

        try {
            integer.value = expect(TokenType::INTEGER)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return integer;
    }

    ShallowErrorable<Float> Parser::parseFloat(int) {
        Float floatNum{current()->pos};

        try {
            floatNum.value = expect(TokenType::FLOAT)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return floatNum;
    }

    ShallowErrorable<HexNumber> Parser::parseHexNumber(int) {
        HexNumber hexNum{current()->pos};

        try {
            hexNum.value = expect(TokenType::HEX_NUMBER)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return hexNum;
    }

    ShallowErrorable<BinaryNumber> Parser::parseBinaryNumber(int) {
        BinaryNumber binNum{current()->pos};

        try {
            binNum.value = expect(TokenType::BINARY_NUMBER)->lexeme;

        } catch (const Error& e) {
            return ErrorNode{*current(), {e.message().data()}};
        }

        return binNum;
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

        reporter_.errorWith<MsgId::IEP1_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        throw Error{
            RuntimeError{
                         Loc::msgFormat<MsgId::IEP1_1>(enumToStr(current()->type)), current()->pos,
                         endPos(*current())
            }
        };
    }

    std::shared_ptr<Token> Parser::current(const bool consume) const {
        if (idx_ < tokens_.size()) return tokens_[consume ? idx_++ : idx_];

        return nullptr;
    }

}  // namespace stationeers::ic10
