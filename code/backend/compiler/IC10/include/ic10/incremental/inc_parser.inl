// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_parser.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/04 13:52
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_INC_PARSER_INL
#define COMPILER_INC_PARSER_INL
#pragma once

#include "ic10/parser/parser.hpp"

namespace stationeers::ic10 {

    template<std::ranges::input_range R>
    Program IncParser::parseTokenRange(R&& tokenRange) {
        std::vector<std::shared_ptr<Token>> tokens;
        if constexpr (std::ranges::sized_range<R>)
            tokens.reserve(std::ranges::size(tokenRange) + 1);

        for (const auto& token : tokenRange) tokens.push_back(token);

        tokens.push_back(buildEndToken(tokens));

        return Parser::parsing(tokens);
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_INC_PARSER_INL
