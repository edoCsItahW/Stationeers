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
#include "ic10_compiler/lexer/lexer.hpp"
#include "common/exception/debug.hpp"
#include "common/utils/common.hpp"
#include "ic10_compiler/locals/local.hpp"
#include <algorithm>
#include <unordered_set>

namespace stationeers::ic10 {

    Lexer::Lexer(const std::string_view src, const bool debug) noexcept
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
        if (inScope()) [[likely]]
            return src_[pos_.offset()];

        return std::nullopt;
    }

    Token Lexer::next() {
        skip();

        const auto start = pos_;

        if (!inScope()) [[unlikely]]
            return {TokenType::END, start, "", TokenCategory::END};


        Token result;

        if (const auto c = current(); *c == '\n') {
            pos_.newline();
            result = {TokenType::NEWLINE, start, "\\n", TokenCategory::WHITESPACE};
        }

        else if (*c == '$')
            result = extractHexNumber();

        else if (*c == '%')
            result = extractBinaryNumber();

        else if (isAsciiDigit(*c))
            result = extractNumber();

        else if (*c == '"')
            result = extractString();

        else if (*c == '#')
            result = extractHash();

        else if (*c == '@')
            result = extractTag();

        else if (SYMBOLS.contains(*c))
            result = extractSymbol();

        else
            result = extractLetter();

        if (auto c = current(); c && !isAsciiSpace(*c))
            reporter_.errorWith<ICMsgId::IEL3_1>(start, endPos(result), enumToStr(result.type));

        return result;
    }

    bool Lexer::inScope() const noexcept { return pos_.offset() < src_.size(); }

    std::optional<char> Lexer::peek(const int offset) const noexcept {
        if (pos_.offset() + offset >= src_.size()) return std::nullopt;

        return src_[pos_.offset() + offset];
    }

    void Lexer::skip() const noexcept {
        while (inScope() && isAsciiSpace(*current()) && *current() != '\n')
            pos_.next(static_cast<unsigned char>(*current()));
    }

    Token Lexer::extractLetter() const {
        std::string value;
        const auto start = pos_;

        std::vector<bool> bits;

        auto type = *current();

        value += type;

        pos_.next(type);

        // 目标: 对于'r'+ ( 'ra' | 'r' [0-9]+ ) 构造为类似11111100000的位标志
        while (inScope() && !isAsciiSpace(*current()) && !SYMBOLS.contains(*current())) {
            if (auto c = *current(); c == 'r')
                bits.push_back(true);
            else if (c == 'a' || isAsciiDigit(c))
                bits.push_back(false);
            // 其它字符改变type成为普通标识符
            else
                type = '*';

            value += *current();
            pos_.next(*current());
        }

        // 栈指针
        if (value == "sp")
            return {
                TokenType::REGISTER, std::move(start), std::move(value), TokenCategory::LITERAL
            };
        // 自引用设备
        if (value == "db")
            return {TokenType::DEVICE, std::move(start), std::move(value), TokenCategory::LITERAL};

        switch (type) {
            case 'r': [[fallthrough]];
            case 'd': {
                // 不全为r
                if (auto cnt = std::ranges::count(bits, false); cnt != 0)
                    // 跟ra则只允许一个a
                    if (auto res = value.find('a'); res == value.size() - 1 ? cnt == 1 : true)
                        // 中途非r
                        if (auto it = std::ranges::adjacent_find(
                                bits, [](bool a, bool b) { return !a && b; }
                            );
                            it == bits.end())
                            return {
                                type == 'r' ? TokenType::REGISTER : TokenType::DEVICE, start,
                                std::move(value), TokenCategory::LITERAL
                            };

                [[fallthrough]];
            }
            default: {
                if (const auto& it = OTHER_KEYWORD_MAP.find(value); it != OTHER_KEYWORD_MAP.end())
                    return {it->second, std::move(start), std::move(value), TokenCategory::LITERAL};

                // 指令关键字，单独提出便于语法分析器自动分派
                if (const auto& it = INS_KEYWORD_MAP.find(value); it != INS_KEYWORD_MAP.end())
                    return {
                        TokenType::KEYWORD, std::move(start), std::move(value),
                        TokenCategory::LITERAL, it->second
                    };

                return {
                    TokenType::IDENTIFIER, std::move(start), std::move(value),
                    TokenCategory::LITERAL
                };
            }
        }
    }

    Token Lexer::extractTag() const {
        std::string value = "@";
        const auto start  = pos_;

        pos_.next('@');

        while (inScope() && (isAsciiAlpha(*current()) || current().value() == '-')) {
            value += *current();
            pos_.next(*current());
        }

        return {TokenType::TAG, std::move(start), std::move(value), TokenCategory::LITERAL};
    }

    Token Lexer::extractNumber() const {
        std::string value;
        const auto start = pos_;

        int pointCount = 0;

        while (                                                 //
            inScope()                                           //
            && (isAsciiDigit(*current()) || *current() == '.')  //
            && pointCount <= 1                                  //
        ) {
            if (*current() == '.') pointCount++;

            value += *current();
            pos_.next(static_cast<unsigned char>(*current()));
        }

        // 处理科学计数法
        if (inScope() && (*current() == 'e' || *current() == 'E')) {
            const auto currentPos   = pos_;
            const auto currentValue = value;

            // e | E
            value += *current();
            pos_.next(static_cast<unsigned char>(*current()));

            // 处理符号
            if (inScope() && (*current() == '+' || *current() == '-')) {
                value += *current();
                pos_.next(static_cast<unsigned char>(*current()));
            }

            // 处理指数部分
            if (inScope() && isAsciiDigit(*current())) {
                while (inScope() && isAsciiDigit(*current())) {
                    value += *current();
                    pos_.next(static_cast<unsigned char>(*current()));
                }

                return {TokenType::FLOAT, start, value, TokenCategory::LITERAL};
            }

            // 指数部分没有数字，这不是一个有效的科学计数法
            // 可能是标识符的一部分，回退到'e'/'E'之前
            value = currentValue;
            pos_  = currentPos;
        }

        return {
            value.contains('.') ? TokenType::FLOAT : TokenType::INTEGER, start, value,
            TokenCategory::LITERAL
        };
    }

    Token Lexer::extractHexNumber() const {
        std::string value = "$";
        pos_.next('$');

        const auto start = pos_;

        while (inScope() && isAsciiAlnum(*current())) {
            value += *current();
            pos_.next(static_cast<unsigned char>(*current()));
        }

        return {TokenType::HEX_NUMBER, start, value, TokenCategory::LITERAL};
    }

    Token Lexer::extractBinaryNumber() const {
        std::string value = "%";
        pos_.next('%');

        const auto start = pos_;

        while (inScope() && (isAsciiBinDigit(*current()) || *current() == '_')) {
            value += *current();
            pos_.next(static_cast<unsigned char>(*current()));
        }

        return {TokenType::BINARY_NUMBER, start, value, TokenCategory::LITERAL};
    }

    Token Lexer::extractString() {
        std::string value = "\"";
        pos_.next('"');

        const auto start = pos_;

        // 循环直到遇到闭合引号、换行符或输入结束
        // 换行符作为同步点：IC10中字符串不应跨行，遇到换行说明字符串未闭合
        while (inScope() && *current() != '"' && *current() != '\n') {
            // 处理转义字符
            if (const auto it = WHITESPACE_MAP.find(*current()); it != WHITESPACE_MAP.end())
                [[unlikely]] {
                value += '\\';
                value += it->second;
                // 循环末尾的 pos_.next() 会推进到下一个字符，无需额外处理
            }

            else
                value += *current();

            pos_.next(static_cast<unsigned char>(*current()));
        }

        // 未闭合字符串：到达输入末尾或遇到换行符
        if (!inScope() || *current() == '\n') {
            reporter_.errorWith<ICMsgId::IEL2_1>(
                start, stationeers::endPos(start, value), std::string{1, '\"'}
            );

            // 不消耗换行符，让lexer从下一行恢复解析（同步点）
            return {TokenType::UNKNOWN, start, std::move(value), TokenCategory::INVALID};
        }

        value += *current();
        pos_.next(static_cast<unsigned char>(*current()));

        return {TokenType::STRING, start, std::move(value), TokenCategory::LITERAL};
    }

    Token Lexer::extractHash() const {
        std::string value = "#";
        pos_.next('#');

        const auto start = pos_;

        if (inScope()) {
            // 类型注解
            if (current().value() == '>') {
                value += *current();
                pos_.next('>');

                return {
                    TokenType::TYPE_ANNOTATION_PREFIX, std::move(start), std::move(value),
                    TokenCategory::LITERAL
                };
            }

            // 类型提示
            if (current().value() == ':') {
                value += *current();
                pos_.next(':');

                return {
                    TokenType::TYPE_HINT_PREFIX, std::move(start), std::move(value),
                    TokenCategory::LITERAL
                };
            }
        }

        // 返回普通注释
        while (inScope() && *current() != '\n') {
            value += *current();
            pos_.next(*current());
        }

        return {TokenType::HEX_COMMENT, std::move(start), std::move(value), TokenCategory::COMMENT};
    }

    Token Lexer::extractSymbol() {
        const auto start = pos_;
        const auto ch    = std::string(1, *current());

        if (const auto it = SYMBOL_MAP.find(ch); it != SYMBOL_MAP.end()) [[likely]] {
            pos_.next(static_cast<unsigned char>(*current()));
            return {it->second, start, ch, TokenCategory::SYMBOL};
        }

        reporter_.errorWith<ICMsgId::IEL1_1>(start, endPos(start, ch), ch);
        pos_.next(static_cast<unsigned char>(*current()));
        return {TokenType::UNKNOWN, start, ch, TokenCategory::INVALID};
    }

}  // namespace stationeers::ic10
