// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file node_parser.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/29 21:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/parser/node_parser.hpp"
#include "ic10_compiler/parser/parser.hpp"

namespace stationeers::ic10 {

    // Integer

    Integer NodeParser<Integer>::parse(Parser& parser) noexcept {
        // 已通过前瞻确定TokenType::SUB + TokenType::INTEGER，无需try-catch
        Integer integer{parser.current()->pos};

        bool isNegative = false;
        if (parser.current()->type == TokenType::SUB) {
            isNegative = true;

            parser.consume();  // SUB
        }

        integer.value = parser.expect(TokenType::INTEGER)->lexeme;

        if (isNegative) integer.value = '-' + integer.value;

        return integer;
    }

    // Float

    Float NodeParser<Float>::parse(Parser& parser) noexcept {
        // 已通过前瞻确定TokenType::SUB + TokenType::FLOAT，无需try-catch
        Float floatNum{parser.current()->pos};

        bool isNegative = false;
        if (parser.current()->type == TokenType::SUB) {
            isNegative = true;

            parser.consume();  // SUB
        }

        floatNum.value = parser.expect(TokenType::FLOAT)->lexeme;

        if (isNegative) floatNum.value = '-' + floatNum.value;

        return floatNum;
    }

    // BinaryNumber

    ShallowErrorable<BinaryNumber> NodeParser<BinaryNumber>::parse(Parser& parser) noexcept {
        BinaryNumber binNum{parser.current()->pos};

        // 已通过前瞻确定TokenType::BINARY_NUMBER，无需try-catch
        binNum.value = parser.expect(TokenType::BINARY_NUMBER)->lexeme;

        return binNum;
    }

    // HexNumber

    HexNumber NodeParser<HexNumber>::parse(Parser& parser) noexcept {
        HexNumber hexNum{parser.current()->pos};

        // 已通过前瞻确定TokenType::HEX_NUMBER，无需try-catch
        hexNum.value = parser.expect(TokenType::HEX_NUMBER)->lexeme;

        return hexNum;
    }

    // Identifier

    Identifier NodeParser<Identifier>::parse(Parser& parser) noexcept {
        Identifier identifier{parser.current()->pos};

        // 已通过前瞻确定TokenType::IDENTIFIER，无需try-catch
        identifier.value = parser.expect(TokenType::IDENTIFIER)->lexeme;

        return identifier;
    }

    // String

    String NodeParser<String>::parse(Parser& parser) noexcept {
        String string{parser.current()->pos};

        // 已通过前瞻确定TokenType::STRING，无需try-catch
        string.value = parser.expect(TokenType::STRING)->lexeme;

        return string;
    }

    // GeneralPurposeRegister

    GeneralPurposeRegister NodeParser<GeneralPurposeRegister>::parse(Parser& p) noexcept {
        auto tokenPtr = p.current();

        // 做范围检查，lexer已确保lexeme形式正确
        if (auto idx = std::stoi(tokenPtr->lexeme.substr(1)); idx < 0 || idx > 17)
            p.reporter_.warn<ICMsgId::IEP35>(tokenPtr->pos, endPos(*tokenPtr));

        p.consume();  // REGISTER

        return {tokenPtr->pos, tokenPtr->lexeme};
    }

    bool NodeParser<GeneralPurposeRegister>::is(const Parser& parser) noexcept {
        // 已被peek，则current()理论上不为nullptr
        auto& lexeme = parser.current()->lexeme;

        // r [0-9]+
        return lexeme.size() > 1 && lexeme[0] == 'r' && isAsciiDigit(lexeme[1]);
    }

    // AddressRegister

    AddressRegister NodeParser<AddressRegister>::parse(Parser& p) noexcept {
        // 已前瞻，current()不为nullptr
        auto tokenPtr = p.current();

        p.consume();  // REGISTER

        return {tokenPtr->pos, tokenPtr->lexeme};
    }

    bool NodeParser<AddressRegister>::is(const Parser& parser) noexcept {
        // 已被peek，则current()理论上不为nullptr
        return parser.current()->lexeme == "ra";
    }

    // StackPointerRegister

    StackPointerRegister NodeParser<StackPointerRegister>::parse(Parser& p) noexcept {
        // 已前瞻，current()不为nullptr
        auto tokenPtr = p.current();

        p.consume();  // REGISTER

        return {tokenPtr->pos, tokenPtr->lexeme};
    }

    // StackPointerRegister

    bool NodeParser<StackPointerRegister>::is(const Parser& parser) noexcept {
        // 已被peek，则current()理论上不为nullptr
        return parser.current()->lexeme == "sp";
    }

    // DynamicRegister

    DynamicRegister NodeParser<DynamicRegister>::parse(Parser& p) {
        using Variant = decltype(DynamicRegister::reg)::element_type;

        DynamicRegister dynamicRegister{p.current()->pos};

        auto tokenPtr = p.current();

        // 使用Y组合子匿名函子递归，逐r解析'r'+ ( 'ra' | 'r' [0-9]+ )
        auto dp = [&]<typename Self>(
                      this Self&& self, std::string_view lexeme, std::size_t offset = 1
                  ) -> Variant {
            if (lexeme == "ra")
                return AddressRegister{
                    {tokenPtr->pos.line(), static_cast<int>(tokenPtr->pos.column() + offset),
                     tokenPtr->pos.offset() + offset},
                    "ra"
                };

            if (lexeme.size() > 1 && lexeme[0] == 'r' && isAsciiDigit(lexeme[1]))
                return GeneralPurposeRegister{
                    {tokenPtr->pos.line(), static_cast<int>(tokenPtr->pos.column() + offset),
                     tokenPtr->pos.offset() + offset},
                    std::string(lexeme)
                };

            // else
            DynamicRegister dr{tokenPtr->pos};

            dr.reg = std::make_shared<Variant>(self(lexeme.substr(1), offset + 1));

            return dr;
        };

        dynamicRegister.reg =
            std::make_shared<Variant>(dp(std::string_view(tokenPtr->lexeme.substr(1))));

        p.consume();  // REGISTER

        return dynamicRegister;
    }

    bool NodeParser<DynamicRegister>::is(const Parser& parser) noexcept {
        // 已被peek，则current()理论上不为nullptr
        auto& lexeme = parser.current()->lexeme;

        // r+ ( 'ra' | 'r' [0-9]+ )
        return lexeme.size() > 2 && lexeme[0] == 'r' && lexeme[1] == 'r';
    }

    // SelfReferenceDevice

    SelfReferenceDevice NodeParser<SelfReferenceDevice>::parse(Parser& p) noexcept {
        // 已前瞻，current()不为nullptr
        auto tokenPtr = p.current();

        p.consume();  // DEVICE

        return {tokenPtr->pos, tokenPtr->lexeme};
    }

    bool NodeParser<SelfReferenceDevice>::is(const Parser& parser) noexcept {
        // 已被peek，则current()理论上不为nullptr
        return parser.current()->lexeme == "db";
    }

    // OrdinaryDevice

    OrdinaryDevice NodeParser<OrdinaryDevice>::parse(Parser& p) noexcept {
        // 已前瞻，current()不为nullptr
        auto tokenPtr = p.current();

        // 做范围检查，lexer已确保lexeme形式正确
        if (auto idx = std::stoi(tokenPtr->lexeme.substr(1)); idx < 0 || idx > 5)
            p.reporter_.warn<ICMsgId::IEP36>(tokenPtr->pos, endPos(*tokenPtr));

        p.consume();  // DEVICE

        return {tokenPtr->pos, tokenPtr->lexeme};
    }

    bool NodeParser<OrdinaryDevice>::is(const Parser& parser) noexcept {
        // 已被peek，则current()理论上不为nullptr
        auto& lexeme = parser.current()->lexeme;

        // d [0-9]+
        return lexeme.size() > 1 && lexeme[0] == 'd' && isAsciiDigit(lexeme[1]);
    }

    // StaticDevice

    StaticDevice NodeParser<StaticDevice>::parse(Parser& p) noexcept {
        StaticDevice staticDevice{p.current()->pos};

        if (NodeParser<SelfReferenceDevice>::is(p))
            staticDevice.device = NodeParser<SelfReferenceDevice>::parse(p);

        else if (NodeParser<OrdinaryDevice>::is(p))
            staticDevice.device = NodeParser<OrdinaryDevice>::parse(p);

        // 解析pin
        if (p.current()->type == TokenType::COLON) {
            p.expect(TokenType::COLON);

            if (p.current()->type == TokenType::INTEGER)
                staticDevice.pin = NodeParser<Integer>::parse(p);
        }

        return staticDevice;
    }

    bool NodeParser<StaticDevice>::is(const Parser& parser) noexcept {
        return NodeParser<SelfReferenceDevice>::is(parser)
            || NodeParser<OrdinaryDevice>::is(parser);
    }

    // DynamicDevice

    DynamicDevice NodeParser<DynamicDevice>::parse(Parser& p) {
        using Variant = decltype(DynamicRegister::reg)::element_type;

        auto tokenPtr = p.current();

        DynamicDevice dynamicDevice{tokenPtr->pos};

        // 使用Y组合子匿名函子递归，逐r解析'r'+ ( 'ra' | 'r' [0-9]+ )
        auto dp = [&]<typename Self>(
                      this Self&& self, std::string_view lexeme, std::size_t offset = 1
                  ) -> decltype(DynamicRegister::reg)::element_type {
            if (lexeme == "ra")
                return AddressRegister{
                    {tokenPtr->pos.line(), static_cast<int>(tokenPtr->pos.column() + offset),
                     tokenPtr->pos.offset() + offset},
                    "ra"
                };

            if (lexeme.size() > 1 && lexeme[0] == 'r' && isAsciiDigit(lexeme[1]))
                return GeneralPurposeRegister{
                    {tokenPtr->pos.line(), static_cast<int>(tokenPtr->pos.column() + offset),
                     tokenPtr->pos.offset() + offset},
                    std::string(lexeme)
                };

            // else
            DynamicRegister dd{tokenPtr->pos};

            dd.reg = std::make_shared<Variant>(self(lexeme.substr(1), offset + 1));

            return dd;
        };

        dynamicDevice.reg = dp(std::string_view(tokenPtr->lexeme.substr(1)));  // 排除第一个d

        p.consume();  // DEVICE

        return dynamicDevice;
    }

    bool NodeParser<DynamicDevice>::is(const Parser& parser) noexcept {
        // 已被peek，则current()理论上不为nullptr
        auto& lexeme = parser.current()->lexeme;

        // 'd' 'r'+ ( 'ra' | 'r' [0-9]+ )
        return lexeme.size() > 1 && lexeme[0] == 'd' && lexeme[1] == 'r';
    }

    // Enum

    Enum NodeParser<Enum>::parse(Parser& parser) noexcept {
        Enum enumNode{parser.current()->pos};

        // 已通过前瞻确定TokenType::IDENTIFIER + TokenType::DOT +
        // TokenType::IDENTIFIER，无需try-catch
        enumNode.name = NodeParser<Identifier>::parse(parser);

        parser.expect(TokenType::DOT);

        enumNode.value = NodeParser<Identifier>::parse(parser);

        return enumNode;
    }

    // HashMacro

    ShallowErrorable<HashMacro> NodeParser<HashMacro>::parse(Parser& parser) noexcept {
        HashMacro hashCall{parser.current()->pos};

        // 已通过前瞻确定TokenType::KEYWORD_HASH，无需try-catch
        auto tokenBeforeError = parser.expect(TokenType::KEYWORD_HASH);

        try {
            tokenBeforeError = parser.expect(TokenType::LPAREN);

            if (tokenBeforeError = parser.current();
                tokenBeforeError && tokenBeforeError->type == TokenType::IDENTIFIER)
                hashCall.value = NodeParser<String>::parse(parser);
            else
                parser.expect(TokenType::IDENTIFIER);  // 引发错误

            tokenBeforeError = parser.expect(TokenType::RPAREN);
            hashCall.endPos  = std::move(tokenBeforeError->pos);
        } catch (const Error& e) { return ErrorNode{*tokenBeforeError, std::string(e.message())}; }

        return hashCall;
    }

    // StrMacro

    ShallowErrorable<StrMacro> NodeParser<StrMacro>::parse(Parser& parser) noexcept {
        StrMacro strCall{parser.current()->pos};

        // 已通过前瞻确定TokenType::KEYWORD_STR，无需try-catch
        auto tokenBeforeError = parser.expect(TokenType::KEYWORD_STR);

        try {
            tokenBeforeError = parser.expect(TokenType::LPAREN);

            if (tokenBeforeError = parser.current();
                tokenBeforeError && tokenBeforeError->type == TokenType::IDENTIFIER)
                strCall.value = NodeParser<String>::parse(parser);
            else
                parser.expect(TokenType::IDENTIFIER);  // 引发错误

            tokenBeforeError = parser.expect(TokenType::RPAREN);
            strCall.endPos   = std::move(tokenBeforeError->pos);
        } catch (const Error& e) { return ErrorNode{*tokenBeforeError, std::string(e.message())}; }

        return strCall;
    }

    // ErrorNode

    ErrorNode NodeParser<ErrorNode>::parse(Parser& parser) noexcept {
        // 一般不会走到这里，仅用于满足前瞻调度器的类型要求
        if (auto c = parser.current(); c) return {c->pos, *c, ""};

        return {{}, {}, ""};
    }

}  // namespace stationeers::ic10
