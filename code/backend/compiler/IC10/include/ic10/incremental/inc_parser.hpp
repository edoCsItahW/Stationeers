// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_parser.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/04
 * @if zh
 * @brief IC10增量语法分析器
 * @details 基于语句级缓存的增量语法分析器，仅重新解析受影响的语句。
 *          利用IC10行式语言的特性，以行为单位进行语句级增量更新。
 *
 * @note 语法分析是编译过程的第二阶段
 *
 * @par 增量原理:
 * 1. 缓存完整的Program AST
 * 2. 根据变化行号找到第一个受影响的语句
 * 3. 从变化行到文件末尾重新解析（保证位置信息正确）
 * 4. 拼接前缀未变化语句 + 新解析语句
 *
 * @par 使用方式:
 * @code
 * IncParser parser;
 * // 首次全量
 * auto result = parser.parseFull(tokens);
 * // 后续增量
 * result = parser.parseInc(newTokens, changedStartLine);
 * @endcode
 *
 * @see Parser 全量语法分析器
 * @see IncLexer 增量词法分析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 incremental parser
 * @details Statement-cache based incremental parser, only re-parses affected statements.
 *          Leverages IC10's line-based language characteristics for statement-level
 *          incremental updates.
 *
 * @note Parsing is the second phase of compilation
 *
 * @par Incremental Principle:
 * 1. Cache the complete Program AST
 * 2. Find first affected statement based on changed line number
 * 3. Re-parse from changed line to end of file (ensures correct position info)
 * 4. Concatenate unchanged prefix + newly parsed statements
 *
 * @par Usage:
 * @code
 * IncParser parser;
 * // First time full
 * auto result = parser.parseFull(tokens);
 * // Subsequent incremental
 * result = parser.parseInc(newTokens, changedStartLine);
 * @endcode
 *
 * @see Parser Full parser
 * @see IncLexer Incremental lexer
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_INC_PARSER_HPP
#define COMPILER_INC_PARSER_HPP
#pragma once

#include <vector>
#include <memory>

#include "ic10/parser/ast/ast.hpp"


namespace stationeers::ic10 {

    /**
     * @if zh
     * @struct IncParserResult
     * @brief 增量语法分析结果
     * @details 包含AST、增量标志及相关统计信息。
     *
     * @elseif en
     * @struct IncParserResult
     * @brief Incremental parser result
     * @details Contains AST, incremental flag, and related statistics.
     *
     * @endif
     */
    struct IncParserResult {
        /** @if zh @brief 解析后的Program AST @else @brief Parsed Program AST @endif */
        Program ast;

        /** @if zh @brief 是否为增量分析结果 @else @brief Whether this is an incremental result @endif */
        bool incremental = false;

        /** @if zh @brief 重新解析的语句数 @else @brief Number of re-parsed statements @endif */
        std::size_t reparsedStmts = 0;

        /** @if zh @brief 受影响的语句起始索引 @else @brief Affected statement start index @endif */
        std::size_t affectedStmtStart = 0;
    };

    /**
     * @if zh
     * @class IncParser
     * @brief 增量语法分析器
     * @details 基于语句级缓存的增量语法分析器，适用于编辑器场景下的频繁小修改。
     *          从变化行到文件末尾重新解析，前缀语句直接复用。
     *
     * @elseif en
     * @class IncParser
     * @brief Incremental parser
     * @details Statement-cache based incremental parser, suitable for frequent small
     *          edits in editor scenarios. Re-parses from changed line to end of file,
     *          prefix statements are reused directly.
     *
     * @endif
     */
    class IncParser {
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
        IncParser() = default;

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

        /**
         * @if zh
         * @brief 全量语法分析
         * @details 对完整Token序列进行语法分析，建立AST缓存。
         *          首次编译或缓存失效时调用。
         *
         * @param tokens 完整Token序列
         * @return 语法分析结果，包含AST和缓存状态
         *
         * @elseif en
         * @brief Full parsing
         * @details Perform full parsing on complete token sequence, building AST cache.
         *          Called on first compilation or when cache is invalid.
         *
         * @param tokens Complete token sequence
         * @return Parser result with AST and cache state
         *
         * @endif
         */
        IncParserResult parseFull(const std::vector<std::shared_ptr<Token>>& tokens);

        /**
         * @if zh
         * @brief 增量语法分析
         * @details 基于上一次的缓存进行增量语法分析。
         *          如果缓存失效或变化太大，则回退到全量分析。
         *
         * @param tokens 新的完整Token序列
         * @param changedStartLine 变化起始行号（从1开始，含）
         * @return 语法分析结果，包含AST和增量统计
         *
         * @elseif en
         * @brief Incremental parsing
         * @details Perform incremental parsing based on previous cache.
         *          Falls back to full parsing if cache is invalid or change is too large.
         *
         * @param tokens New complete token sequence
         * @param changedStartLine Changed start line (1-based, inclusive)
         * @return Parser result with AST and incremental stats
         *
         * @endif
         */
        IncParserResult parseInc(const std::vector<std::shared_ptr<Token>>& tokens, int changedStartLine);

    private:
        /** @if zh @brief 缓存的Program AST @else @brief Cached Program AST @endif */
        std::optional<Program> astCached_;

        /**
         * @if zh
         * @brief 在语句列表中查找第一个行号 >= 指定行号的语句索引
         * @param program Program AST
         * @param lineNumber 目标行号（从1开始）
         * @return 语句索引，若所有语句行号都小于指定行号则返回语句总数
         *
         * @elseif en
         * @brief Find index of first statement with line number >= specified line
         * @param program Program AST
         * @param lineNumber Target line number (1-based)
         * @return Statement index, or total statement count if all statements are before the line
         *
         * @endif
         */
        static std::size_t findLineStmtIndex(const Program& program, int lineNumber);

        /**
         * @if zh
         * @brief 用最后一个Token的位置构造END Token
         * @param tokens Token序列
         * @return END Token
         *
         * @elseif en
         * @brief Build END Token using position of the last token
         * @param tokens Token sequence
         * @return END Token
         *
         * @endif
         */
        static std::shared_ptr<Token> buildEndToken(
            const std::vector<std::shared_ptr<Token>>& tokens
        );

        /**
         * @if zh
         * @brief 对Token范围进行语法分析
         * @details 接受任意输入范围，先补END Token再调用Parser解析。
         *
         * @tparam R Token范围类型（元素为 shared_ptr<Token>）
         * @param tokenRange Token范围
         * @return 解析得到的Program AST
         *
         * @elseif en
         * @brief Parse a range of tokens
         * @details Accepts any input range, appends END token then calls Parser.
         *
         * @tparam R Token range type (elements are shared_ptr<Token>)
         * @param tokenRange Token range
         * @return Parsed Program AST
         *
         * @endif
         */
        template<std::ranges::input_range R>
        static Program parseTokenRange(R&& tokenRange);
    };

}  // namespace stationeers::ic10

#include "inc_parser.inl"

#endif  // COMPILER_INC_PARSER_HPP
