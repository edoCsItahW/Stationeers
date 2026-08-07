// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_lexer.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/04
 * @if zh
 * @brief IC10增量词法分析器
 * @details 基于行哈希缓存的增量词法分析器，仅重新分析受影响的行。
 *          利用IC10行式语言的特性，以行为粒度进行增量更新。
 *
 * @note 词法分析是编译过程的第一阶段
 *
 * @par 增量原理:
 * 1. 缓存逐行的Token序列及行哈希
 * 2. 双指针从前后两端比较行哈希，定位变化区间
 * 3. 前缀行直接复用，变化行重新词法分析，后缀行深拷贝并修正位置
 * 4. 更新行缓存，供下次增量分析使用
 *
 * @par 使用方式:
 * @code
 * IncLexer lexer;
 * // 首次全量
 * auto result = lexer.tokenizeFull(source);
 * // 后续增量
 * result = lexer.tokenizeInc(newSource);
 * @endcode
 *
 * @see Lexer 全量词法分析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 incremental lexer
 * @details Line-hash-cache based incremental lexer, only re-lexes affected lines.
 *          Leverages IC10's line-based language characteristics for line-granular
 *          incremental updates.
 *
 * @note Lexical analysis is the first phase of compilation
 *
 * @par Incremental Principle:
 * 1. Cache per-line token sequences and line hashes
 * 2. Two-pointer hash comparison from both ends to locate changed range
 * 3. Prefix lines reused directly, changed lines re-lexed, suffix lines deep-copied with position adjustment
 * 4. Update line cache for next incremental analysis
 *
 * @par Usage:
 * @code
 * IncLexer lexer;
 * // First time full
 * auto result = lexer.tokenizeFull(source);
 * // Subsequent incremental
 * result = lexer.tokenizeInc(newSource);
 * @endcode
 *
 * @see Lexer Full lexer
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_INC_LEXER_HPP
#define COMPILER_INC_LEXER_HPP
#pragma once

#include "ic10_compiler/lexer/lexer.hpp"
#include <memory>
#include <ranges>
#include <string_view>
#include <vector>

namespace stationeers::ic10 {

    namespace detail {

        /**
         * @if zh
         * @brief 计算单行内容的哈希值
         * @param line 单行内容视图
         * @return 哈希值
         *
         * @elseif en
         * @brief Compute hash value for a single line
         * @param line Single line content view
         * @return Hash value
         *
         * @endif
         */
        std::size_t hashLine(std::string_view line);

    }

    /**
     * @if zh
     * @struct IncLexerResult
     * @brief 增量词法分析结果
     * @details 包含Token序列、增量标志及相关统计信息。
     *
     * @elseif en
     * @struct IncLexerResult
     * @brief Incremental lexer result
     * @details Contains token sequence, incremental flag, and related statistics.
     *
     * @endif
     */
    struct IncLexerResult {
        /** @if zh @brief 词法分析得到的Token序列 @else @brief Token sequence from lexing @endif */
        std::vector<std::shared_ptr<Token>> tokens;

        /** @if zh @brief 是否为增量分析结果 @else @brief Whether this is an incremental result @endif */
        bool incremental = false;

        /** @if zh @brief 重新词法分析的行数 @else @brief Number of re-lexed lines @endif */
        std::size_t relexedLines = 0;

        /** @if zh @brief 变化起始行号（从1开始，含） @else @brief Changed start line (1-based, inclusive) @endif */
        int changedStartLine = 0;

        /** @if zh @brief 旧变化结束行号（从1开始，不含） @else @brief Old changed end line (1-based, exclusive) @endif */
        int oldChangedEndLine = 0;

        /** @if zh @brief 新变化结束行号（从1开始，不含） @else @brief New changed end line (1-based, exclusive) @endif */
        int newChangedEndLine = 0;
    };

    /**
     * @if zh
     * @class IncLexer
     * @brief 增量词法分析器
     * @details 基于行哈希缓存的增量词法分析器，适用于编辑器场景下的频繁小修改。
     *          通过双指针差异检测，仅对变化区间重新词法分析，大幅减少重复计算。
     *
     * @elseif en
     * @class IncLexer
     * @brief Incremental lexer
     * @details Line-hash-cache based incremental lexer, suitable for frequent small
     *          edits in editor scenarios. Uses two-pointer diff detection to only
     *          re-lex the changed range, significantly reducing redundant computation.
     *
     * @endif
     */
    class IncLexer {
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
        IncLexer() = default;

        /**
         * @if zh
         * @brief 全量词法分析
         * @details 对完整源码进行词法分析，建立行缓存。
         *          首次编译或缓存失效时调用。
         *
         * @param source 源码字符串
         * @return 词法分析结果，包含Token序列和缓存状态
         *
         * @elseif en
         * @brief Full lexical analysis
         * @details Perform full lexical analysis on complete source, building line cache.
         *          Called on first compilation or when cache is invalid.
         *
         * @param source Source code string
         * @return Lexer result with token sequence and cache state
         *
         * @endif
         */
        IncLexerResult tokenizeFull(std::string_view source);

        /**
         * @if zh
         * @brief 增量词法分析
         * @details 基于上一次的缓存进行增量词法分析。
         *          若缓存为空，则回退到全量分析。
         *
         * @param newSource 新的源码字符串
         * @return 词法分析结果，包含Token序列和增量统计
         *
         * @elseif en
         * @brief Incremental lexical analysis
         * @details Perform incremental lexical analysis based on previous cache.
         *          Falls back to full analysis if cache is empty.
         *
         * @param newSource New source code string
         * @return Lexer result with token sequence and incremental stats
         *
         * @endif
         */
        IncLexerResult tokenizeInc(std::string_view newSource);

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
        [[nodiscard]] bool hasCache() const;

        /**
         * @if zh
         * @brief 清除缓存
         *
         * @elseif en
         * @brief Clear cache
         *
         * @endif
         */
        void clear();

    private:
        /**
         * @if zh
         * @struct LineTokenCache
         * @brief 单行Token缓存条目
         * @details 记录单行的哈希、Token序列、全局偏移量和行长。
         *
         * @elseif en
         * @struct LineTokenCache
         * @brief Single line token cache entry
         * @details Records hash, token sequence, global offset and length for one line.
         *
         * @endif
         */
        struct LineTokenCache {
            /** @if zh @brief 行内容哈希值 @else @brief Line content hash value @endif */
            std::size_t hash = 0;

            /** @if zh @brief 该行的Token序列 @else @brief Token sequence of this line @endif */
            std::vector<std::shared_ptr<Token>> tokens;

            /** @if zh @brief 该行在全局源码中的起始偏移量 @else @brief Start offset of this line in global source @endif */
            std::size_t startOffset = 0;

            /** @if zh @brief 该行的长度（含换行符） @else @brief Length of this line (including newline) @endif */
            std::size_t lineLen = 0;
        };

        /** @if zh @brief 逐行Token缓存 @else @brief Per-line token cache @endif */
        std::vector<LineTokenCache> lineCache_;

        /** @if zh @brief 缓存的源码副本 @else @brief Cached source copy @endif */
        std::string sourceCache_;

        /**
         * @if zh
         * @struct LexLinesResult
         * @brief 批量词法分析结果
         * @details 包含下一行偏移量、Token序列和缓存条目。
         *
         * @elseif en
         * @struct LexLinesResult
         * @brief Batch lexing result
         * @details Contains next line offset, token sequence and cache entries.
         *
         * @endif
         */
        struct LexLinesResult {
            /** @if zh @brief 处理完所有行后的下一个全局偏移量 @else @brief Next global offset after processing all lines @endif */
            std::size_t nextOffset;

            /** @if zh @brief 所有行的Token序列 @else @brief Token sequence of all lines @endif */
            std::vector<std::shared_ptr<Token>> tokens;

            /** @if zh @brief 所有行的缓存条目 @else @brief Cache entries for all lines @endif */
            std::vector<LineTokenCache> caches;
        };

        /**
         * @if zh
         * @brief 批量对指定行范围进行词法分析
         * @details 接受任意输入范围（支持 range-based for），对每行执行词法分析，
         *          返回下一行偏移量、Token序列和缓存条目。
         *
         * @tparam R 行范围类型（元素为 string_view）
         * @param range 行范围
         * @param offset 起始行的全局偏移量
         * @param lineNumber 起始行号（从1开始）
         * @return 批量词法分析结果
         *
         * @elseif en
         * @brief Batch lexing for a range of lines
         * @details Accepts any input range (range-based for supported), lexes each line,
         *          returns next line offset, token sequence and cache entries.
         *
         * @tparam R Line range type (elements are string_view)
         * @param range Line range
         * @param offset Global offset of the start line
         * @param lineNumber Start line number (1-based)
         * @return Batch lexing result
         *
         * @endif
         */
        template<std::ranges::input_range R>
        LexLinesResult scanLinesToCache(R&& range, std::size_t offset, int lineNumber);

        /**
         * @if zh
         * @brief 对单行内容进行词法分析
         * @param lineContent 行内容
         * @param startOffset 该行在全局源码中的起始偏移量
         * @param lineNumber 行号（从1开始）
         * @return 该行的Token序列（已过滤END）
         *
         * @elseif en
         * @brief Lex a single line
         * @param lineContent Line content
         * @param startOffset Start offset of this line in global source
         * @param lineNumber Line number (1-based)
         * @return Token sequence of this line (END filtered out)
         *
         * @endif
         */
        static std::vector<std::shared_ptr<Token>> scanLine(
            std::string_view lineContent, std::size_t startOffset, int lineNumber
        );

        /**
         * @if zh
         * @brief 构造文件结束标记（END Token）
         * @param lines 行列表
         * @param totalSize 源码总字节数
         * @return END Token
         *
         * @elseif en
         * @brief Build end-of-file token (END Token)
         * @param lines Line list
         * @param totalSize Total source byte count
         * @return END Token
         *
         * @endif
         */
        static std::shared_ptr<Token> buildEndToken(
            const std::vector<std::string_view>& lines, const std::size_t totalSize
        );
    };

}  // namespace stationeers::ic10

#include "inc_lexer.inl"

#endif  // COMPILER_INC_LEXER_HPP
