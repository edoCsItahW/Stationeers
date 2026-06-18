/**
 * @file parser.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 13:36
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_PARSER_HPP
#define COMPILER_PARSER_HPP
#pragma once

#include <memory>

#include "common/exception/error.hpp"
#include "ic10/parser/ast/ast.hpp"
#include "../lexer/token.hpp"


namespace stationeers::ic10 {

    class Parser {
    public:
        bool debug_ = false;

        Parser(const std::vector<std::shared_ptr<Token>> &tokens, bool debug = false);

        Program parse();

        static Program parsing(
            const std::vector<std::shared_ptr<Token>> &tokens, bool debug = false
        );

    private:
        mutable std::size_t idx_ = 0;

        std::vector<std::shared_ptr<Token>> tokens_;

        Statement parseStatement();

        ShallowErrorable<LabelDef> parseLabelDef(int layer);

        PreprocessorDirective parsePreprocessorDirective(int layer);

        ShallowErrorable<AliasDirective> parseAliasDirective(int layer);

        ShallowErrorable<DefineDirective> parseDefineDirective(int layer);

        ExecutableInstruction parseExecutableInstruction(int layer);

        NullaryInstruction parseNullaryInstruction(int layer);

        UnaryInstruction parseUnaryInstruction(int layer);

        BinaryInstruction parseBinaryInstruction(int layer);

        TernaryInstruction parseTernaryInstruction(int layer);

        QuaternaryInstruction parseQuaternaryInstruction(int layer);

        QuinaryInstruction parseQuinaryInstruction(int layer);

        SenaryInstruction parseSenaryInstruction(int layer);

        Operand parseOperand(int layer);

        RegisterOrDevice parseRegisterOrDevice(int layer);

        RegisterOrIdentifier parseRegisterOrIdentifier(int layer);

        DeviceReference parseDeviceReference(int layer);

        MacroCall parseMacroCall(int layer);

        ShallowErrorable<HashCall> parseHashCall(int layer);

        ShallowErrorable<StrCall> parseStrCall(int layer);

        ShallowErrorable<Constant> parseConstant(int layer);

        LogicType parseLogicType(int layer);

        SlotIndex parseSlotIndex(int layer);

        LogicSlotType parseLogicSlotType(int layer);

        BatchMode parseBatchMode(int layer);

        ReagentMode parseReagentMode(int layer);

        Errorable<Identifier, Number> parseIdentifierOrNumber(int layer);

        ShallowErrorable<Device> parseDevice(int layer);

        ShallowErrorable<Register> parseRegister(int layer);

        ShallowErrorable<String> parseString(int layer);

        ShallowErrorable<Identifier> parseIdentifier(int layer);

        Number parseNumber(int layer);

        ShallowErrorable<BinaryNumber> parseBinaryNumber(int layer);

        ShallowErrorable<HexNumber> parseHexNumber(int layer);

        ShallowErrorable<Float> parseFloat(int layer);

        ShallowErrorable<Integer> parseInteger(int layer);

        bool inScope() const;

        void skip();

        void consume() const;

        void gotoNextLine() const;

        std::shared_ptr<Token> current(bool consume = false) const;

        std::shared_ptr<Token> expect(TokenType type, bool skipWs = true, bool consume = true);

    };

}  // namespace stationeers::ic10

#endif  // COMPILER_PARSER_HPP
