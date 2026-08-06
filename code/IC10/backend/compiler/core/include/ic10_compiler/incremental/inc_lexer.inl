/**
 * @file inc_lexer.inl 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/07/04 23:15
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_INC_LEXER_INL
#define COMPILER_INC_LEXER_INL
#pragma once

namespace stationeers::ic10 {

    template<std::ranges::input_range R>
    IncLexer::LexLinesResult IncLexer::scanLinesToCache(
        R&& range, std::size_t offset, int lineNumber
    ) {
        LexLinesResult result;

        if constexpr (std::ranges::sized_range<R>)
            result.caches.reserve(std::ranges::size(range));

        for (const auto& lineContent : range) {
            LineTokenCache cache{
                .hash = detail::hashLine(lineContent),
                .tokens = scanLine(lineContent, offset, lineNumber),
                .startOffset = offset,
                .lineLen = lineContent.size()
            };

            result.tokens.insert_range(result.tokens.end(), cache.tokens);

            offset += lineContent.size();

            result.caches.push_back(std::move(cache));

            ++lineNumber;
        }

        result.nextOffset = offset;

        return result;
    }

}  // namespace stationeers::ic10

#endif //COMPILER_INC_LEXER_INL
