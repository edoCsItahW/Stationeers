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
 * @if zh
 * @brief IC10增量语法分析器的 Token 范围解析模板实现
 * @details 实现 @ref IncParser 的模板成员 @c parseTokenRange：把任意 @c input_range 的 Token 视图物化为容器，补齐结尾 END 标记后委托全量 @ref Parser 解析。
 *          增量路径只需拼出受影响行的 Token 序列即可调用本函数，无需关心解析器状态与终止标记的维护。
 *
 * @note @ref Parser 依赖结尾的 END 标记收尾，该标记由本函数统一补齐，调用方不应再自行追加。
 * @see IncParser 增量语法分析器
 * @see Parser 全量语法分析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Template implementation for parsing a token range in the incremental parser
 * @details Implements @ref IncParser's template member @c parseTokenRange: it materializes an arbitrary @c input_range of tokens, appends the trailing END marker and delegates to the full @ref Parser.
 *          The incremental path only has to assemble the tokens of the affected lines; parser state and terminator maintenance are not its concern.
 *
 * @note @ref Parser relies on the trailing END marker, which this function appends, so callers must not add another one.
 * @see IncParser incremental parser
 * @see Parser full parser
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_INC_PARSER_INL
#define IC10_COMPILER_CORE_INC_PARSER_INL
#pragma once

#include "ic10_compiler/parser/parser.hpp"

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

#endif  // IC10_COMPILER_CORE_INC_PARSER_INL
