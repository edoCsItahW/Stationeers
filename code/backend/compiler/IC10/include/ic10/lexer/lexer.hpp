// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file lexer.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:23
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_LEXER_HPP
#define COMPILER_LEXER_HPP
#pragma once

#include <string_view>
#include <optional>
#include <memory>
#include <vector>

#include "token.hpp"


namespace stationeers::ic10 {

    class Lexer {
    public:
        explicit Lexer(std::string_view src, bool debug = false);

        Lexer() = default;

        [[nodiscard]] std::vector<std::shared_ptr<Token>> scan() const;

        static std::vector<std::shared_ptr<Token>> tokenize(
            std::string_view src, bool debug = false
        );

    private:
        mutable Pos pos_;

        std::string_view src_;

        bool debug_;

        [[nodiscard]] std::optional<char> current() const noexcept;

        Token next() const;

        [[nodiscard]] bool inScope() const noexcept;

        [[nodiscard]] std::optional<char> peek(int offset = 1) const noexcept;

        void skip() const noexcept;

        Token extractIdentifier() const;

        Token extractNumber() const;

        Token extractString() const;

        Token extractSymbol() const;

        Token extractHexComment() const;

        Token extractSlashComment() const;

        Token extractHexNumber() const;

        Token extractBinaryNumber() const;

        Token extractDevice() const;

        Token extractRegister() const;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_LEXER_HPP
