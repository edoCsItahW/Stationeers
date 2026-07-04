// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental_parser.hpp
 * @author edocsitahw
 * @brief IC10增量语法分析器
 * @details 基于语句级的增量语法分析，通过行号与语句的对应关系，
 *          仅重新解析受影响的语句，复用未变化的语句节点。
 *
 * @if zh
 * @par 核心思路:
 * 1. 每条语句对应一行（IC10是行式语言）
 * 2. 缓存每个语句的AST节点及其起始行号
 * 3. 词法增量给出受影响的行范围后，仅重新解析这些行
 * 4. 将新解析的语句替换到原AST的对应位置
 * 5. 后面语句的位置信息自动随token位置更新
 *
 * @elseif en
 * @par Core Idea:
 * 1. Each statement corresponds to one line (IC10 is line-based)
 * 2. Cache each statement's AST node and its starting line number
 * 3. After lexer incremental gives affected line range, only reparse those lines
 * 4. Replace newly parsed statements into corresponding positions in original AST
 * 5. Positions of later statements are auto-updated with token positions
 *
 * @endif
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 */
#ifndef COMPILER_INCREMENTAL_PARSER_HPP
#define COMPILER_INCREMENTAL_PARSER_HPP
#pragma once

#include "ic10/lexer/token.hpp"
#include "ic10/parser/ast/ast.hpp"
#include "ic10/parser/parser.hpp"
#include <memory>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @struct ParserIncrementalResult
     * @brief 增量语法分析结果
     *
     * @elseif en
     * @struct ParserIncrementalResult
     * @brief Incremental parser result
     *
     * @endif
     */
    struct ParserIncrementalResult {
        /** @brief 解析后的Program AST */
        Program ast;

        /** @brief 是否执行了增量更新（false表示全量重解析） */
        bool incremental = false;

        /** @brief 重新解析的语句数 */
        std::size_t reparsedStatements = 0;

        /** @brief 受影响的语句起始索引 */
        std::size_t affectedStatementStart = 0;
    };

    /**
     * @if zh
     * @class IncrementalParser
     * @brief 增量语法分析器
     * @details 基于语句级缓存的增量语法分析器，利用IC10行式语言的特性，
     *          仅重新解析修改行对应的语句，大幅减少语法分析开销。
     *
     * @par 使用方式:
     * @code
     * IncrementalParser parser;
     * // 首次全量
     * auto result = parser.parseFull(tokens);
     * // 后续增量
     * result = parser.parseIncremental(newTokens, affectedTokenStart);
     * @endcode
     *
     * @elseif en
     * @class IncrementalParser
     * @brief Incremental syntax analyzer
     * @details Statement-cache based incremental parser, leveraging IC10's line-based nature,
     *          only re-parses statements corresponding to modified lines.
     *
     * @par Usage:
     * @code
     * IncrementalParser parser;
     * // First time full parse
     * auto result = parser.parseFull(tokens);
     * // Subsequent incremental
     * result = parser.parseIncremental(newTokens, affectedTokenStart);
     * @endcode
     *
     * @endif
     */
    class IncrementalParser {
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
        IncrementalParser() = default;

        /**
         * @if zh
         * @brief 全量语法分析
         * @details 对完整Token序列进行语法分析，建立语句缓存。
         *
         * @param tokens 完整Token序列
         * @return 语法分析结果，包含AST和缓存状态
         *
         * @elseif en
         * @brief Full syntax analysis
         * @details Perform full parsing on complete token sequence, building statement cache.
         *
         * @param tokens Complete token sequence
         * @return Parser result with AST and cache state
         *
         * @endif
         */
        ParserIncrementalResult parseFull(const std::vector<std::shared_ptr<Token>>& tokens);

        /**
         * @if zh
         * @brief 增量语法分析
         * @details 基于上一次的缓存进行增量语法分析。
         *          根据受影响的行范围，仅重新解析受影响的语句。
         *
         * @param tokens 新的完整Token序列（来自增量词法分析）
         * @param changedStartLine 内容变化的起始行号（从1开始，含）
         * @param oldChangedEndLine 内容变化的旧结束行号（从1开始，不含）
         * @param newChangedEndLine 内容变化的新结束行号（从1开始，不含）
         * @return 语法分析结果，包含AST和增量统计
         *
         * @elseif en
         * @brief Incremental syntax analysis
         * @details Perform incremental parsing based on previous cache.
         *          Only re-parse affected statements based on changed line range.
         *
         * @param tokens New complete token sequence (from incremental lexer)
         * @param changedStartLine Start line of changed content (1-based, inclusive)
         * @param oldChangedEndLine Old end line of changed content (1-based, exclusive)
         * @param newChangedEndLine New end line of changed content (1-based, exclusive)
         * @return Parser result with AST and incremental stats
         *
         * @endif
         */
        ParserIncrementalResult parseIncremental(
            const std::vector<std::shared_ptr<Token>>& tokens,
            int changedStartLine,
            int oldChangedEndLine,
            int newChangedEndLine
        );

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
        [[nodiscard]] bool hasCache() const { return cachedProgram_.has_value(); }

        /**
         * @brief 清除缓存
         *
         * @elseif en
         * @brief Clear cache
         *
         * @endif
         */
        void clear() { cachedProgram_.reset(); }

    private:
        /** @brief 缓存的Program AST */
        std::optional<Program> cachedProgram_;

        /**
         * @if zh
         * @brief 在Token序列中找到指定语句起始的Token索引
         * @details 从给定起始索引开始，找到下一个语句的起始Token位置。
         *          语句起始定义为：跳过NEWLINE和注释后遇到的第一个有意义Token。
         *
         * @param tokens Token序列
         * @param startIndex 起始搜索位置
         * @return 语句起始的Token索引，若到末尾则返回tokens.size()
         *
         * @elseif en
         * @brief Find token index where a statement starts
         * @details From given start index, find the next statement's starting token position.
         *          Statement start is defined as: first meaningful token after skipping NEWLINEs and comments.
         *
         * @param tokens Token sequence
         * @param startIndex Start search position
         * @return Token index of statement start, or tokens.size() if at end
         *
         * @endif
         */
        static std::size_t findStatementStart(
            const std::vector<std::shared_ptr<Token>>& tokens,
            std::size_t startIndex
        );

        /**
         * @if zh
         * @brief 找到给定Token索引对应的语句在缓存中的索引
         *
         * @param program 缓存的Program
         * @param tokenLine 目标Token所在行号
         * @return 语句在program.statements中的索引
         *
         * @elseif en
         * @brief Find statement index in cache corresponding to given token index
         *
         * @param program Cached Program
         * @param tokenLine Line number of target token
         * @return Statement index in program.statements
         *
         * @endif
         */
        static std::size_t findStatementIndexForLine(
            const Program& program,
            int lineNumber
        );

        /**
         * @if zh
         * @brief 提取Token序列中指定行范围的子序列
         * @details 从startLine开始（含），到endLine结束（含），
         *          提取所有属于这些行的Token。
         *
         * @param tokens 完整Token序列
         * @param startLine 起始行号（从1开始，含）
         * @param endLine 结束行号（从1开始，含）
         * @return 子序列的Token索引范围 [first, last)
         *
         * @elseif en
         * @brief Extract sub-sequence of tokens in given line range
         * @details From startLine (inclusive) to endLine (inclusive),
         *          extract all tokens belonging to these lines.
         *
         * @param tokens Complete token sequence
         * @param startLine Start line number (1-based, inclusive)
         * @param endLine End line number (1-based, inclusive)
         * @return Token index range [first, last) of sub-sequence
         *
         * @endif
         */
        static std::pair<std::size_t, std::size_t> getTokenRangeForLines(
            const std::vector<std::shared_ptr<Token>>& tokens,
            int startLine,
            int endLine
        );
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_INCREMENTAL_PARSER_HPP
