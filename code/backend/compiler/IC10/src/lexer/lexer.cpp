// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file lexer.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:25
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/lexer/lexer.hpp"
#include "common/exception/debug.hpp"
#include "common/utils/common.hpp"
#include "ic10/locals/local.hpp"

namespace stationeers::ic10 {

    Lexer::Lexer(const std::string_view src, const bool debug)
        : src_(src)
        , debug_(debug) {}

    std::vector<std::shared_ptr<Token>> Lexer::scan() {
        auto tokens = std::vector<std::shared_ptr<Token>>{};

        do {
            tokens.emplace_back(std::make_shared<Token>(next()));
        } while (tokens.back()->type != TokenType::END);

        return tokens;
    }

    std::vector<std::shared_ptr<Token>> Lexer::tokenize(
        const std::string_view src, const bool debug
    ) {
        return Lexer(src, debug).scan();
    }

    std::optional<char> Lexer::current() const noexcept {
        if (inScope()) return src_[pos_.offset()];

        return std::nullopt;
    }

    Token Lexer::next() {
        skip();

        const auto start = pos_;

        if (!inScope()) return {TokenType::END, start};

        const auto c = current();

        if (*c == '\n') {
            pos_.newline();
            return {TokenType::NEWLINE, start, "\\n", TokenCategory::WHITESPACE};
        }

        if (*c == '$') return extractHexNumber();

        if (*c == '%') return extractBinaryNumber();

        if (isAsciiDigit(*c)) return extractNumber();

        if (*c == '"') return extractString();

        if (*c == '#') return extractHexComment();

        if (*c == '/' && peek().value_or(' ') == '/') return extractSlashComment();

        if (SYMBOL_MAP.contains(std::string(1, *c))) return extractSymbol();

        if (*c == 'd' && isAsciiDigit(peek().value_or(' ')) && isAsciiSpace(peek(2).value_or(' ')))
            return extractDevice();

        if (auto p = isAsciiDigit(peek(2).value_or(' '));
            *c == 'r' && isAsciiDigit(peek().value_or(' '))
            && isAsciiSpace(peek(p ? 3 : 2).value_or('z')))
            return extractRegister();

        // 为适配不同语言变量名，但难以判断，因此所有分支至此都认为是标识符
        return extractIdentifier();
    }

    bool Lexer::inScope() const noexcept { return pos_.offset() < src_.size(); }

    std::optional<char> Lexer::peek(const int offset) const noexcept {
        if (pos_.offset() + offset >= src_.size()) return std::nullopt;

        return src_[pos_.offset() + offset];
    }

    void Lexer::skip() const noexcept {
        while (inScope() && isAsciiSpace(*current()) && *current() != '\n') pos_.next();
    }

    Token Lexer::extractIdentifier() const {
        std::string value;
        const auto start = pos_;

        while (inScope() && !isAsciiSpace(*current()) && !SYMBOLS.contains(*current())) {
            value += *current();
            pos_.next();
        }

        if (KEYWORD_MAP.contains(value))
            return {
                KEYWORD_MAP.at(value), std::move(start), std::move(value), TokenCategory::LITERAL
            };

        return {TokenType::IDENTIFIER, start, std::move(value), TokenCategory::LITERAL};
    }

    Token Lexer::extractNumber() const {
        std::string value;
        const auto start = pos_;

        while (inScope() && (isAsciiDigit(*current()) || *current() == '.')
               && !value.contains('.')) {
            value += *current();
            pos_.next();
        }

        // 处理科学计数法
        if (inScope() && (*current() == 'e' || *current() == 'E')) {
            const auto currentPos   = pos_;
            const auto currentValue = value;

            // e | E
            value += *current();
            pos_.next();

            // 处理符号
            if (inScope() && (*current() == '+' || *current() == '-')) {
                value += *current();
                pos_.next();
            }

            // 处理指数部分
            if (inScope() && isAsciiDigit(*current())) {
                while (inScope() && isAsciiDigit(*current())) {
                    value += *current();
                    pos_.next();
                }

                return {TokenType::FLOAT, start, currentValue, TokenCategory::LITERAL};
            }

            // 指数部分没有数字，这不是一个有效的科学计数法
            // 可能是标识符的一部分，回退到'e'/'E'之前
            value = currentValue;
            pos_  = currentPos;

            // goto finally:
        }

        // finally:
        return {
            value.contains('.') ? TokenType::FLOAT : TokenType::INTEGER, start, value,
            TokenCategory::LITERAL
        };
    }

    Token Lexer::extractHexNumber() const {
        std::string value = "$";
        pos_.next();

        const auto start = pos_;

        while (inScope() && isAsciiHexDigit(*current())) {
            value += *current();
            pos_.next();
        }

        return {TokenType::HEX_NUMBER, start, value, TokenCategory::LITERAL};
    }

    Token Lexer::extractBinaryNumber() const {
        std::string value = "%";
        pos_.next();

        const auto start = pos_;

        while (inScope() && isAsciiBinDigit(*current())) {
            value += *current();
            pos_.next();
        }

        return {TokenType::BINARY_NUMBER, start, value, TokenCategory::LITERAL};
    }

    Token Lexer::extractString() {
        std::string value = "\"";
        pos_.next();

        const auto start = pos_;

        while (inScope() && *current() != '"') {
            // 处理转义字符
            if (const auto it = WHITESPACE_MAP.find(*current()); it != WHITESPACE_MAP.end()) {
                value += '\\';
                value += it->second;
                // 循环末尾的 pos_.next() 会推进到下一个字符，无需额外处理
            }

            else
                value += *current();

            pos_.next();
        }

        if (!inScope()) {
            reporter_.errorWith<MsgId::IEL1_2>(start, stationeers::endPos(start, value.size()), std::string{ 1, '\"' });

            return { TokenType::UNKNOWN, start, std::move(value), TokenCategory::INVALID };
        }

        value += *current();
        pos_.next();

        return {TokenType::STRING, start, std::move(value), TokenCategory::LITERAL};
    }

    Token Lexer::extractHexComment() const {
        std::string value = "#";
        pos_.next();

        const auto start = pos_;

        while (inScope() && *current() != '\n') {
            value += *current();
            pos_.next();
        }

        return {TokenType::HEX_COMMENT, start, std::move(value), TokenCategory::COMMENT};
    }

    Token Lexer::extractSlashComment() const {
        std::string value = "//";
        pos_.move(2);

        const auto start = pos_;

        while (inScope() && *current() != '\n') {
            value += *current();
            pos_.next();
        }

        return {TokenType::SLASH_COMMENT, start, std::move(value), TokenCategory::COMMENT};
    }

    Token Lexer::extractSymbol() {
        const auto start = pos_;

        Token token;

        if (const auto it = SYMBOL_MAP.find(std::string(1, *current())); it != SYMBOL_MAP.end())
            token = {it->second, start, std::string(1, *current()), TokenCategory::SYMBOL};

        else {
            reporter_.errorWith<MsgId::IEL1_1>(start, endPos(token), std::string{1, *current()});

            token = {TokenType::UNKNOWN, start, std::string(1, *current()), TokenCategory::INVALID};
        }

        pos_.next();

        return token;
    }

    Token Lexer::extractDevice() const {
        const auto start = pos_;

        std::string value = "d";
        pos_.next();

        value += *current();
        pos_.next();

        return {TokenType::DEVICE, start, std::move(value), TokenCategory::LITERAL};
    }

    Token Lexer::extractRegister() const {
        const auto start = pos_;

        std::string value = "r";
        pos_.next();

        value += *current();
        pos_.next();

        if (inScope() && isAsciiDigit(*current())) {
            value += *current();
            pos_.next();
        }

        return {TokenType::REGISTER, start, std::move(value), TokenCategory::LITERAL};
    }

}  // namespace stationeers::ic10
