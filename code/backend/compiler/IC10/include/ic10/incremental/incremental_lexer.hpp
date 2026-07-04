// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental_lexer.hpp
 * @author edocsitahw
 * @brief IC10增量词法分析器
 * @details 基于行的增量词法分析，通过行哈希缓存未变化行的Token，
 *          仅重新词法分析修改的行。使用换行符作为自然同步点。
 *
 * @if zh
 * @par 核心思路:
 * 1. 每行计算哈希值，缓存该行的Token列表
 * 2. 检测到行变化时，仅重新解析变化行及其可能影响的上下文行
 * 3. 以NEWLINE token为同步点，向前/向后找到安全的复用边界
 * 4. 重新计算变化行之后所有Token的位置偏移
 *
 * @elseif en
 * @par Core Idea:
 * 1. Compute hash for each line, cache Token list for that line
 * 2. When line changes detected, only re-parse changed lines and affected context lines
 * 3. Use NEWLINE token as sync point, find safe reuse boundary forward/backward
 * 4. Recompute position offsets for all Tokens after changed lines
 *
 * @endif
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 */
#ifndef COMPILER_INCREMENTAL_LEXER_HPP
#define COMPILER_INCREMENTAL_LEXER_HPP
#pragma once

#include "ic10/lexer/lexer.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @struct LineTokenCache
     * @brief 单行Token缓存条目
     *
     * @elseif en
     * @struct LineTokenCache
     * @brief Single line token cache entry
     *
     * @endif
     */
    struct LineTokenCache {
        /** @brief 行内容的哈希值 */
        std::size_t lineHash = 0;

        /** @brief 该行产生的所有Token（包含行尾的NEWLINE） */
        std::vector<std::shared_ptr<Token>> tokens;

        /** @brief 该行在原文件中的起始偏移（字节） */
        std::size_t startOffset = 0;

        /** @brief 该行的长度（字节） */
        std::size_t lineLength = 0;
    };

    /**
     * @if zh
     * @struct LexerIncrementalResult
     * @brief 增量词法分析结果
     *
     * @elseif en
     * @struct LexerIncrementalResult
     * @brief Incremental lexer result
     *
     * @endif
     */
    struct LexerIncrementalResult {
        /** @brief 完整的Token序列 */
        std::vector<std::shared_ptr<Token>> tokens;

        /** @brief 是否执行了增量更新（false表示全量重解析） */
        bool incremental = false;

        /** @brief 重新词法分析的行数 */
        std::size_t relexedLines = 0;

        /** @brief 受影响的Token起始索引（该索引及之后的Token位置可能变化） */
        std::size_t affectedTokenStart = 0;

        /** @brief 内容变化的起始行号（从1开始，含） */
        int changedStartLine = 0;

        /** @brief 内容变化的旧结束行号（从1开始，不含） */
        int oldChangedEndLine = 0;

        /** @brief 内容变化的新结束行号（从1开始，不含） */
        int newChangedEndLine = 0;
    };

    /**
     * @if zh
     * @class IncrementalLexer
     * @brief 增量词法分析器
     * @details 基于行缓存的增量词法分析器，适用于编辑器场景下的频繁小修改。
     *          通过行级哈希缓存，大幅减少小范围修改时的词法分析开销。
     *
     * @par 使用方式:
     * @code
     * IncrementalLexer lexer;
     * // 首次全量
     * auto result = lexer.tokenizeFull(source);
     * // 后续增量
     * result = lexer.tokenizeIncremental(newSource);
     * @endcode
     *
     * @elseif en
     * @class IncrementalLexer
     * @brief Incremental lexical analyzer
     * @details Line-cache based incremental lexer, suitable for frequent small edits
     *          in editor scenarios. Significantly reduces lexer overhead for small changes.
     *
     * @par Usage:
     * @code
     * IncrementalLexer lexer;
     * // First time full lex
     * auto result = lexer.tokenizeFull(source);
     * // Subsequent incremental
     * result = lexer.tokenizeIncremental(newSource);
     * @endcode
     *
     * @endif
     */
    class IncrementalLexer {
    public:
        /**
         * @if zh
         * @brief 默认构造函数
         *
         * @elseif en
         * @brief Default constructor
         *
         * @endif
         */
        IncrementalLexer() = default;

        /**
         * @if zh
         * @brief 全量词法分析
         * @details 对完整源代码进行词法分析，建立行缓存。
         *          首次编译或无法增量时调用。
         *
         * @param source 完整源代码
         * @return 词法分析结果，包含Token序列和缓存状态
         *
         * @elseif en
         * @brief Full lexical analysis
         * @details Perform full lexing on complete source code, building line cache.
         *          Called on first compilation or when incremental is not possible.
         *
         * @param source Complete source code
         * @return Lexer result with token sequence and cache state
         *
         * @endif
         */
        LexerIncrementalResult tokenizeFull(std::string_view source);

        /**
         * @if zh
         * @brief 增量词法分析
         * @details 基于上一次的缓存进行增量词法分析。
         *          如果缓存失效或差异太大，则回退到全量分析。
         *
         * @param newSource 新的完整源代码
         * @return 词法分析结果，包含Token序列和增量统计
         *
         * @elseif en
         * @brief Incremental lexical analysis
         * @details Perform incremental lexing based on previous cache.
         *          Falls back to full lexing if cache is invalid or diff is too large.
         *
         * @param newSource New complete source code
         * @return Lexer result with token sequence and incremental stats
         *
         * @endif
         */
        LexerIncrementalResult tokenizeIncremental(std::string_view newSource);

        /**
         * @if zh
         * @brief 获取行缓存（只读）
         * @return 当前行缓存的引用
         *
         * @elseif en
         * @brief Get line cache (read-only)
         * @return Reference to current line cache
         *
         * @endif
         */
        [[nodiscard]] const std::vector<LineTokenCache>& getLineCache() const { return lineCache_; }

        /**
         * @if zh
         * @brief 检查是否有有效缓存
         * @return 如果有缓存返回true
         *
         * @elseif en
         * @brief Check if there is valid cache
         * @return true if cache exists
         *
         * @endif
         */
        [[nodiscard]] bool hasCache() const { return !lineCache_.empty(); }

        /**
         * @if zh
         * @brief 清除缓存
         *
         * @elseif en
         * @brief Clear cache
         *
         * @endif
         */
        void clear() { lineCache_.clear(); cachedSource_.clear(); }

    private:
        /** @brief 行缓存：每行对应一个LineTokenCache */
        std::vector<LineTokenCache> lineCache_;

        /** @brief 上一次的完整源代码（用于计算差异） */
        std::string cachedSource_;

        /**
         * @if zh
         * @brief 计算单行的哈希值
         * @param line 行内容
         * @return 哈希值
         *
         * @elseif en
         * @brief Compute hash for a single line
         * @param line Line content
         * @return Hash value
         *
         * @endif
         */
        static std::size_t hashLine(std::string_view line);

        /**
         * @if zh
         * @brief 将源代码按行分割
         * @param source 源代码
         * @return 每行的string_view（不包含换行符）
         *
         * @elseif en
         * @brief Split source code into lines
         * @param source Source code
         * @return string_view for each line (without newline)
         *
         * @endif
         */
        static std::vector<std::string_view> splitLines(std::string_view source);

        /**
         * @if zh
         * @brief 对单行进行词法分析
         * @details 对单独一行进行词法分析，生成该行的所有Token。
         *          注意：Token的位置是基于行内偏移的，需要后续修正全局位置。
         *
         * @param lineContent 行内容（不含换行符）
         * @param lineNumber 行号（从1开始）
         * @param startOffset 该行在文件中的起始字节偏移
         * @param includeNewline 是否包含行尾的NEWLINE token
         * @return 该行的Token列表
         *
         * @elseif en
         * @brief Lex a single line
         * @details Lex a single line, generate all tokens for that line.
         *          Note: Token positions are line-relative, need global correction later.
         *
         * @param lineContent Line content (without newline)
         * @param lineNumber Line number (1-based)
         * @param startOffset Byte offset of this line in file
         * @param includeNewline Whether to include trailing NEWLINE token
         * @return Token list for this line
         *
         * @endif
         */
        static std::vector<std::shared_ptr<Token>> lexSingleLine(
            std::string_view lineContent,
            int lineNumber,
            std::size_t startOffset,
            bool includeNewline
        );

        /**
         * @if zh
         * @brief 从指定行号开始，向后重新计算所有Token的全局位置
         * @param tokens 完整的Token序列
         * @param startLineIndex 起始行索引（缓存中的index）
         *
         * @elseif en
         * @brief Recompute global positions for all tokens starting from given line
         * @param tokens Complete token sequence
         * @param startLineIndex Starting line index (in cache)
         *
         * @endif
         */
        static void adjustTokenPositions(
            std::vector<std::shared_ptr<Token>>& tokens,
            std::size_t startTokenIndex,
            int lineDelta,
            std::size_t offsetDelta
        );

        /**
         * @if zh
         * @brief 找到给定行号在Token序列中的起始索引
         * @param lineNumber 目标行号（从1开始）
         * @return 该行第一个Token在tokens中的索引
         *
         * @elseif en
         * @brief Find token start index for a given line number
         * @param lineNumber Target line number (1-based)
         * @return Index of first token of that line in tokens
         *
         * @endif
         */
        static std::size_t findTokenStartForLine(
            const std::vector<std::shared_ptr<Token>>& tokens,
            int lineNumber
        );
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_INCREMENTAL_LEXER_HPP
