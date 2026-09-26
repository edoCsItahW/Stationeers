/**
 * @file inc_lexer.inl 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/07/04 23:15
 * @if zh
 * @brief IC10增量词法分析器的行扫描模板实现
 * @details 提供 @ref IncLexer 的模板成员 @c scanLinesToCache 的实现：按行执行全量词法分析，并填充行缓存中的行哈希、Token 序列、起始偏移与行长度。
 *          全量路径（@c tokenizeFull）与增量路径（@c tokenizeInc）共用本函数，两条路径由此产出结构一致的行缓存。
 *
 * @note Token 的位置由传入的 @p offset 累加得出，增量路径可从中断处续算偏移，无需事后整体回填。
 * @see IncLexer 增量词法分析器
 * @see Lexer 全量词法分析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Template implementation of line scanning for the incremental lexer
 * @details Provides the implementation of @ref IncLexer's template member @c scanLinesToCache: it lexes the source line by line and fills each line-cache entry with the line hash, token sequence, start offset and line length.
 *          The full path (@c tokenizeFull) and the incremental path (@c tokenizeInc) share this function, which is what keeps their caches structurally identical.
 *
 * @note Token positions are accumulated from the supplied @p offset, so the incremental path can resume mid-file instead of re-computing positions afterwards.
 * @see IncLexer incremental lexer
 * @see Lexer full lexer
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_INC_LEXER_INL
#define IC10_COMPILER_CORE_INC_LEXER_INL
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
            std::vector<Diagnostic> diagnostics;

            LineTokenCache cache{
                .hash = detail::hashLine(lineContent),
                .tokens = scanLine(lineContent, offset, lineNumber, diagnostics),
                .diagnostics = std::move(diagnostics),
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

#endif //IC10_COMPILER_CORE_INC_LEXER_INL
