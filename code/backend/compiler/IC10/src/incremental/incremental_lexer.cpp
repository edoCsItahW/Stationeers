// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental_lexer.cpp
 * @author edocsitahw
 * @brief IC10增量词法分析器实现
 * @details 基于行的增量词法分析实现，通过行哈希缓存未变化行的Token，
 *          仅重新词法分析修改的行。
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/incremental/incremental_lexer.hpp"
#include <algorithm>
#include <functional>
#include <sstream>

namespace stationeers::ic10 {

    std::size_t IncrementalLexer::hashLine(const std::string_view line) {
        return std::hash<std::string_view>{}(line);
    }

    std::vector<std::string_view> IncrementalLexer::splitLines(const std::string_view source) {
        std::vector<std::string_view> lines;
        std::size_t start = 0;

        for (std::size_t i = 0; i < source.size(); ++i) {
            if (source[i] == '\n') {
                lines.emplace_back(source.substr(start, i - start));
                start = i + 1;
            }
        }

        if (start <= source.size()) {
            lines.emplace_back(source.substr(start));
        }

        return lines;
    }

    std::vector<std::shared_ptr<Token>> IncrementalLexer::lexSingleLine(
        const std::string_view lineContent,
        const int lineNumber,
        const std::size_t startOffset,
        const bool includeNewline
    ) {
        std::string fullLine;
        if (includeNewline) {
            fullLine = std::string(lineContent) + '\n';
        } else {
            fullLine = std::string(lineContent);
        }

        Lexer lexer(fullLine);
        auto tokens = lexer.scan();

        Pos basePos;
        for (int i = 1; i < lineNumber; ++i) {
            basePos.newline();
        }
        basePos.move(startOffset);

        std::size_t offsetDelta = startOffset;
        int lineDelta = lineNumber - 1;

        for (auto& token : tokens) {
            if (token->type == TokenType::END) continue;

            Pos newPos;
            std::size_t origOffset = token->pos.offset();
            int origLine = token->pos.line();
            int origCol = token->pos.column();

            std::size_t newOffset = origOffset + offsetDelta;
            int newLine = origLine + lineDelta;
            int newCol = origCol;

            std::stringstream ss;
            ss << "newline";
            for (int i = 1; i < newLine; ++i) {
                ss << "";
            }

            Pos pos;
            for (int i = 1; i < newLine; ++i) {
                pos.newline();
            }
            for (int i = 1; i < newCol; ++i) {
                pos.next();
            }
            std::size_t lineStartOffset = newOffset - (newCol - 1);
            while (pos.offset() < lineStartOffset) {
                pos.next();
            }

            token->pos = pos;
        }

        std::vector<std::shared_ptr<Token>> result;
        for (auto& token : tokens) {
            if (token->type == TokenType::END) continue;
            result.push_back(token);
        }

        return result;
    }

    void IncrementalLexer::adjustTokenPositions(
        std::vector<std::shared_ptr<Token>>& tokens,
        const std::size_t startTokenIndex,
        const int lineDelta,
        const std::size_t offsetDelta
    ) {
        for (std::size_t i = startTokenIndex; i < tokens.size(); ++i) {
            auto& token = tokens[i];

            Pos newPos;
            const int origLine = token->pos.line();
            const int origCol = token->pos.column();
            const int newLine = origLine + lineDelta;

            for (int l = 1; l < newLine; ++l) {
                newPos.newline();
            }

            const std::size_t origLineStartOffset = token->pos.offset() - (origCol - 1);
            const std::size_t newLineStartOffset = origLineStartOffset + offsetDelta;

            while (newPos.offset() < newLineStartOffset + (origCol - 1)) {
                newPos.next();
            }

            token->pos = newPos;
        }
    }

    std::size_t IncrementalLexer::findTokenStartForLine(
        const std::vector<std::shared_ptr<Token>>& tokens,
        const int lineNumber
    ) {
        for (std::size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i]->pos.line() >= lineNumber) {
                return i;
            }
        }
        return tokens.size();
    }

    LexerIncrementalResult IncrementalLexer::tokenizeFull(const std::string_view source) {
        LexerIncrementalResult result;
        result.incremental = false;

        const auto newLines = splitLines(source);
        lineCache_.clear();
        lineCache_.reserve(newLines.size());

        std::size_t currentOffset = 0;

        for (std::size_t lineIdx = 0; lineIdx < newLines.size(); ++lineIdx) {
            const auto& lineContent = newLines[lineIdx];
            const int lineNumber = static_cast<int>(lineIdx) + 1;
            const bool isLastLine = (lineIdx == newLines.size() - 1);

            LineTokenCache cache;
            cache.lineHash = hashLine(lineContent);
            cache.startOffset = currentOffset;
            cache.lineLength = lineContent.size();

            cache.tokens = lexSingleLine(lineContent, lineNumber, currentOffset, !isLastLine);

            for (const auto& token : cache.tokens) {
                result.tokens.push_back(token);
            }

            currentOffset += lineContent.size() + 1;
            lineCache_.push_back(std::move(cache));
        }

        auto endToken = std::make_shared<Token>();
        endToken->type = TokenType::END;
        Pos endPos;
        for (int i = 1; i <= static_cast<int>(newLines.size()); ++i) {
            endPos.newline();
        }
        while (endPos.offset() < source.size()) {
            endPos.next();
        }
        endToken->pos = endPos;
        endToken->lexeme = "";
        endToken->category = TokenCategory::END;
        result.tokens.push_back(endToken);

        cachedSource_ = std::string(source);
        result.relexedLines = newLines.size();
        result.affectedTokenStart = 0;
        result.changedStartLine = 1;
        result.oldChangedEndLine = static_cast<int>(newLines.size()) + 1;
        result.newChangedEndLine = static_cast<int>(newLines.size()) + 1;

        return result;
    }

    LexerIncrementalResult IncrementalLexer::tokenizeIncremental(const std::string_view newSource) {
        if (lineCache_.empty()) {
            return tokenizeFull(newSource);
        }

        const auto newLines = splitLines(newSource);
        const auto oldLines = splitLines(cachedSource_);

        std::size_t firstChanged = 0;
        while (firstChanged < newLines.size() && firstChanged < oldLines.size()
               && hashLine(newLines[firstChanged]) == hashLine(oldLines[firstChanged])) {
            ++firstChanged;
        }

        if (firstChanged == newLines.size() && firstChanged == oldLines.size()) {
            LexerIncrementalResult result;
            result.incremental = true;
            result.relexedLines = 0;
            result.affectedTokenStart = 0;

            std::size_t offset = 0;
            for (const auto& cache : lineCache_) {
                for (const auto& token : cache.tokens) {
                    result.tokens.push_back(token);
                }
            }

            auto endToken = std::make_shared<Token>();
            endToken->type = TokenType::END;
            Pos endPos;
            for (int i = 1; i <= static_cast<int>(lineCache_.size()); ++i) {
                endPos.newline();
            }
            while (endPos.offset() < cachedSource_.size()) {
                endPos.next();
            }
            endToken->pos = endPos;
            endToken->lexeme = "";
            endToken->category = TokenCategory::END;
            result.tokens.push_back(endToken);

            return result;
        }

        std::size_t lastChangedOld = oldLines.size();
        std::size_t lastChangedNew = newLines.size();

        while (lastChangedOld > firstChanged && lastChangedNew > firstChanged) {
            if (hashLine(oldLines[lastChangedOld - 1]) == hashLine(newLines[lastChangedNew - 1])) {
                --lastChangedOld;
                --lastChangedNew;
            } else {
                break;
            }
        }

        const std::size_t syncStartLine = firstChanged;
        const std::size_t oldEndLine = lastChangedOld;
        const std::size_t newEndLine = lastChangedNew;

        LexerIncrementalResult result;
        result.incremental = true;
        result.relexedLines = 0;

        std::size_t affectedTokenStart = 0;
        for (std::size_t i = 0; i < syncStartLine && i < lineCache_.size(); ++i) {
            affectedTokenStart += lineCache_[i].tokens.size();
        }
        result.affectedTokenStart = affectedTokenStart;

        for (std::size_t i = 0; i < syncStartLine && i < lineCache_.size(); ++i) {
            for (const auto& token : lineCache_[i].tokens) {
                result.tokens.push_back(token);
            }
        }

        std::size_t currentOffset = 0;
        for (std::size_t i = 0; i < syncStartLine; ++i) {
            if (i < newLines.size()) {
                currentOffset += newLines[i].size() + 1;
            }
        }

        std::vector<LineTokenCache> newCacheLines;
        for (std::size_t lineIdx = syncStartLine; lineIdx < newEndLine; ++lineIdx) {
            const auto& lineContent = newLines[lineIdx];
            const int lineNumber = static_cast<int>(lineIdx) + 1;
            const bool isLastLine = (lineIdx == newLines.size() - 1);

            LineTokenCache cache;
            cache.lineHash = hashLine(lineContent);
            cache.startOffset = currentOffset;
            cache.lineLength = lineContent.size();

            cache.tokens = lexSingleLine(lineContent, lineNumber, currentOffset, !isLastLine);

            for (const auto& token : cache.tokens) {
                result.tokens.push_back(token);
            }

            currentOffset += lineContent.size() + 1;
            newCacheLines.push_back(std::move(cache));
            result.relexedLines++;
        }

        std::size_t oldOffsetAfter = 0;
        for (std::size_t i = 0; i < oldEndLine; ++i) {
            if (i < oldLines.size()) {
                oldOffsetAfter += oldLines[i].size() + 1;
            }
        }

        std::size_t newOffsetAfter = currentOffset;

        const int lineDelta = static_cast<int>(newEndLine) - static_cast<int>(oldEndLine);
        const std::ptrdiff_t signedOffsetDelta = static_cast<std::ptrdiff_t>(newOffsetAfter)
                                                - static_cast<std::ptrdiff_t>(oldOffsetAfter);

        std::size_t tailStartTokenIdx = result.tokens.size();

        for (std::size_t i = oldEndLine; i < lineCache_.size(); ++i) {
            for (const auto& token : lineCache_[i].tokens) {
                auto newToken = std::make_shared<Token>(*token);
                result.tokens.push_back(newToken);
            }
        }

        if (lineDelta != 0 || signedOffsetDelta != 0) {
            for (std::size_t i = tailStartTokenIdx; i < result.tokens.size(); ++i) {
                auto& token = result.tokens[i];

                const int origLine = token->pos.line();
                const int origCol = token->pos.column();
                const std::size_t origOffset = token->pos.offset();

                const int newLine = origLine + lineDelta;
                const std::size_t newOffset = origOffset + signedOffsetDelta;

                Pos newPos;
                for (int l = 1; l < newLine; ++l) {
                    newPos.newline();
                }

                const std::size_t lineStartOffset = newOffset - (origCol - 1);
                while (newPos.offset() < lineStartOffset + (origCol - 1)) {
                    newPos.next();
                }

                token->pos = newPos;
            }
        }

        std::vector<LineTokenCache> newLineCache;
        newLineCache.reserve(newLines.size());

        for (std::size_t i = 0; i < syncStartLine && i < lineCache_.size(); ++i) {
            newLineCache.push_back(lineCache_[i]);
        }

        for (auto& cache : newCacheLines) {
            newLineCache.push_back(std::move(cache));
        }

        for (std::size_t i = oldEndLine; i < lineCache_.size(); ++i) {
            LineTokenCache updatedCache = lineCache_[i];

            std::size_t newStartOffset = updatedCache.startOffset + signedOffsetDelta;
            updatedCache.startOffset = newStartOffset;

            newLineCache.push_back(std::move(updatedCache));
        }

        lineCache_ = std::move(newLineCache);

        auto endToken = std::make_shared<Token>();
        endToken->type = TokenType::END;
        Pos endPos;
        for (int i = 1; i <= static_cast<int>(newLines.size()); ++i) {
            endPos.newline();
        }
        while (endPos.offset() < newSource.size()) {
            endPos.next();
        }
        endToken->pos = endPos;
        endToken->lexeme = "";
        endToken->category = TokenCategory::END;
        result.tokens.push_back(endToken);

        cachedSource_ = std::string(newSource);

        result.changedStartLine = static_cast<int>(syncStartLine) + 1;
        result.oldChangedEndLine = static_cast<int>(oldEndLine) + 1;
        result.newChangedEndLine = static_cast<int>(newEndLine) + 1;

        return result;
    }

}  // namespace stationeers::ic10
