// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_lexer.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/04
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/incremental/inc_lexer.hpp"
#include <algorithm>
#include <numeric>

namespace stationeers::ic10 {

    namespace detail {

        std::size_t hashLine(std::string_view line) { return std::hash<std::string_view>{}(line); }

    }  // namespace detail

    // splitLines: 将源码按行分割，每行包含换行符
    //
    // 遍历源码字符串，遇到 '\n' 时截取一行（包含换行符）。
    // 最后一行如果不以换行符结尾，则单独截取。
    // 使用 string_view 避免字符串拷贝，提高效率。
    static std::vector<std::string_view> splitLines(const std::string_view source) {
        std::vector<std::string_view> lines;
        std::size_t start = 0;

        const auto size = source.length();
        for (std::size_t i = 0; i < size; ++i)
            if (source[i] == '\n') {
                lines.emplace_back(source.substr(start, i - start + 1));
                start = i + 1;
            }

        if (start < size) lines.emplace_back(source.substr(start));

        return lines;
    }

    // hasCache: 检查是否存在有效的行缓存
    //
    // 通过判断 lineCache_ 是否为空来确定是否有缓存。
    // 有缓存时可以进行增量词法分析。
    bool IncLexer::hasCache() const { return !lineCache_.empty(); }

    // clear: 清空所有缓存，使增量分析器回到初始状态
    //
    // 清空行缓存和源码缓存，下次调用 tokenizeInc 时会回退到全量分析。
    void IncLexer::clear() {
        lineCache_.clear();
        sourceCache_.clear();
    }

    // tokenizeFull: 全量词法分析，建立行缓存
    //
    // 对完整源码进行词法分析，逐行扫描并建立行缓存。
    // 首次编译或缓存失效时调用。
    // 最后追加 END Token 标记文件结束。
    IncLexerResult IncLexer::tokenizeFull(std::string_view source) {
        const auto newlines = splitLines(source);

        lineCache_.clear();
        lineCache_.reserve(newlines.size());

        auto [nextOffset, tokens, caches] = scanLinesToCache(newlines, 0, 1);

        lineCache_ = std::move(caches);

        sourceCache_ = std::string(source);

        tokens.push_back(buildEndToken(newlines, source.size()));

        return {
            .tokens            = std::move(tokens),
            .incremental       = false,
            .relexedLines      = newlines.size(),
            .changedStartLine  = 1,
            .oldChangedEndLine = static_cast<int>(newlines.size()) + 1,
            .newChangedEndLine = static_cast<int>(newlines.size()) + 1
        };
    }

    // tokenizeInc: 增量词法分析 —— 核心算法
    //
    // 策略：基于行哈希的双指针差异检测，仅重新分析变化的行。
    //
    // 步骤：
    //   1. 无缓存 → 回退全量分析
    //   2. 前向指针：从头部找第一个变化的行
    //   3. 源码完全未变 → 直接复用缓存，返回增量结果
    //   4. 后向指针：从尾部找最后一个变化的行
    //   5. 三段拼接 Token：前缀（复用）+ 中间（重析）+ 后缀（深拷贝+位置修正）
    //   6. 三段拼接行缓存：前缀（复用）+ 中间（新生成）+ 后缀（复用+偏移修正）
    //   7. 更新源码缓存，追加 END Token，返回结果
    //
    // 注意：后缀 Token 必须深拷贝，否则会修改缓存中 Token 的位置，
    //       导致下次增量分析时位置信息错误。
    IncLexerResult IncLexer::tokenizeInc(std::string_view newSource) {
        if (lineCache_.empty()) return tokenizeFull(newSource);

        const auto newlines = splitLines(newSource), oldlines = splitLines(sourceCache_);
        const auto newlineSize = newlines.size(), oldLineSize = oldlines.size();

        // 双指针找变化区间
        std::size_t firstChanged = 0;
        while (                            //
            firstChanged < newlineSize     //
            && firstChanged < oldLineSize  //
            && detail::hashLine(newlines[firstChanged]) == detail::hashLine(oldlines[firstChanged]) //
        )
            ++firstChanged;

        // 源码完全未变，直接复用缓存
        if (firstChanged == newlineSize && firstChanged == oldLineSize) {
            std::vector<std::shared_ptr<Token>> tokens;

            for (const auto& cache : lineCache_) tokens.insert_range(tokens.end(), cache.tokens);

            tokens.push_back(buildEndToken(oldlines, sourceCache_.size()));

            return {.tokens = std::move(tokens), .incremental = true, .relexedLines = 0};
        }

        std::size_t lastChangedNew = newlineSize, lastChangedOld = oldLineSize;
        while (                               //
            lastChangedOld > firstChanged     //
            && lastChangedNew > firstChanged  //
            && detail::hashLine(oldlines[lastChangedOld - 1])
                   == detail::hashLine(newlines[lastChangedNew - 1])//
        )
            --lastChangedOld, --lastChangedNew;

        // ---- 三段拼接 Token 序列 ----
        std::vector<std::shared_ptr<Token>> tokens;

        // 第一段：前缀 [0, firstChanged) — 直接复用缓存，位置无需修正
        for (const auto& lineCache : lineCache_ | std::views::take(firstChanged))
            tokens.insert_range(tokens.end(), lineCache.tokens);

        // 计算前缀在新源码中的全局偏移量
        const auto prefixOffset = std::accumulate(
            newlines.begin(), newlines.begin() + firstChanged, std::size_t{0},
            [](std::size_t acc, const std::string_view line) { return acc + line.size(); }
        );

        // 第二段：中间 [firstChanged, lastChangedNew) — 重新词法分析
        const auto [midEndOffset, midTokens, newCacheLines] = scanLinesToCache(
            newlines | std::views::drop(firstChanged)
                | std::views::take(lastChangedNew - firstChanged),
            prefixOffset, static_cast<int>(firstChanged) + 1
        );
        tokens.insert_range(tokens.end(), midTokens);

        // 计算旧源码变化区间的偏移量，用于求差值
        const auto oldPrefixOffset = std::accumulate(
            oldlines.begin(), oldlines.begin() + lastChangedOld, std::size_t{0},
            [](std::size_t acc, const std::string_view line) { return acc + line.size(); }
        );

        const int lineDelta = static_cast<int>(lastChangedNew) - static_cast<int>(lastChangedOld);
        const std::ptrdiff_t offsetDelta = static_cast<std::ptrdiff_t>(midEndOffset)
                                         - static_cast<std::ptrdiff_t>(oldPrefixOffset);

        // 第三段：后缀 [lastChangedOld, end) — 深拷贝缓存 Token，位置按差值平移
        // 必须深拷贝，否则会修改缓存中 Token 的位置，破坏下次增量分析
        for (const auto& lineCache : lineCache_ | std::views::drop(lastChangedOld))
            for (const auto& token : lineCache.tokens) {
                auto newToken = std::make_shared<Token>(*token);
                if (lineDelta != 0 || offsetDelta != 0)
                    newToken->pos =
                        Pos(newToken->pos.line() + lineDelta, newToken->pos.column(),
                            newToken->pos.offset() + static_cast<std::size_t>(offsetDelta));
                tokens.push_back(std::move(newToken));
            }

        // ---- 更新行缓存（同样三段拼接） ----
        std::vector<LineTokenCache> newLineCache;
        newLineCache.reserve(newlineSize);

        // 前缀：直接复用
        newLineCache.insert_range(newLineCache.end(), lineCache_ | std::views::take(firstChanged));

        // 中间：本次新生成的缓存
        newLineCache.insert_range(newLineCache.end(), newCacheLines);

        // 后缀：复用旧缓存，更新 startOffset 和 Token 位置
        // 必须同步修正 Token 位置，否则后续增量分析时 Token 位置与 startOffset 不一致，
        // 导致位置偏移量累加错误
        for (auto cache : lineCache_ | std::views::drop(lastChangedOld)) {
            cache.startOffset += offsetDelta;
            if (lineDelta != 0 || offsetDelta != 0)
                for (auto& token : cache.tokens) {
                    token = std::make_shared<Token>(*token);
                    token->pos =
                        Pos(token->pos.line() + lineDelta, token->pos.column(),
                            token->pos.offset() + static_cast<std::size_t>(offsetDelta));
                }
            newLineCache.push_back(std::move(cache));
        }

        lineCache_ = std::move(newLineCache);

        // ---- 收尾 ----
        tokens.push_back(buildEndToken(newlines, newSource.size()));

        sourceCache_ = std::string(newSource);

        return {
            .tokens            = std::move(tokens),
            .incremental       = true,
            .relexedLines      = lastChangedNew - firstChanged,
            .changedStartLine  = static_cast<int>(firstChanged) + 1,
            .oldChangedEndLine = static_cast<int>(lastChangedOld) + 1,
            .newChangedEndLine = static_cast<int>(lastChangedNew) + 1
        };
    }

    // scanLine: 对单行内容进行词法分析
    //
    // 调用全量 Lexer 对单行内容进行词法分析，
    // 然后过滤掉 END Token（因为每行单独分析都会产生END），
    // 最后修正 Token 的全局位置（行号和偏移量）。
    std::vector<std::shared_ptr<Token>> IncLexer::scanLine(
        std::string_view lineContent, std::size_t startOffset, int lineNumber
    ) {
        auto tokens = Lexer::tokenize(lineContent);

        std::erase_if(tokens, [](const auto& ptr) { return ptr->type == TokenType::END; });

        std::ranges::for_each(tokens, [&](auto& ptr) {
            ptr->pos = Pos(lineNumber, ptr->pos.column(), startOffset + ptr->pos.offset());
        });

        return tokens;
    }

    // buildEndToken: 构造文件结束标记（END Token）
    //
    // 根据行列表和总字节数构造 END Token。
    // 处理两种情况：
    //   - 最后一行以 '\n' 结尾 → END 在下一行开头
    //   - 最后一行不以 '\n' 结尾 → END 紧跟最后一个字符之后
    std::shared_ptr<Token> IncLexer::buildEndToken(
        const std::vector<std::string_view>& lines, const std::size_t totalSize
    ) {
        int endCol = 1, endLine = lines.empty() ? 1 : static_cast<int>(lines.size());

        if (!lines.empty()) {
            if (const std::size_t lastLineLen = lines.back().size();
                lastLineLen > 0 && lines.back().back() == '\n')
                // 最后一行以 '\n' 结尾 → END 在新一行开头
                endLine++;
            else
                // 最后一行不以 '\n' 结尾 → END 紧跟最后一个字符之后
                endCol = static_cast<int>(lastLineLen) + 1;
        }

        return std::make_shared<Token>(
            TokenType::END, Pos(endLine, endCol, totalSize), "", TokenCategory::END
        );
    }

}  // namespace stationeers::ic10
