// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental_compiler.hpp
 * @author edocsitahw
 * @brief IC10增量编译器统一入口
 * @details 整合增量词法分析、增量语法分析和全量语义分析，
 *          提供统一的增量编译接口。适用于编辑器场景下的频繁小修改。
 *
 * @if zh
 * @par 编译流程:
 * 1. 增量词法分析（IncrementalLexer）：仅重解析修改行
 * 2. 增量语法分析（IncrementalParser）：仅重解析受影响语句
 * 3. 全量语义分析（Analyser）：完整AST语义检查（因无依赖追踪）
 *
 * @par 使用方式:
 * @code
 * IncrementalCompiler compiler;
 * // 首次全量编译
 * auto result = compiler.compileFull(source);
 * // 后续增量编译
 * result = compiler.compileIncremental(newSource);
 * @endcode
 *
 * @elseif en
 * @par Compilation Flow:
 * 1. Incremental lexing (IncrementalLexer): only re-parse modified lines
 * 2. Incremental parsing (IncrementalParser): only re-parse affected statements
 * 3. Full semantic analysis (Analyser): full AST semantic check (no dependency tracking)
 *
 * @par Usage:
 * @code
 * IncrementalCompiler compiler;
 * // First time full compile
 * auto result = compiler.compileFull(source);
 * // Subsequent incremental compile
 * result = compiler.compileIncremental(newSource);
 * @endcode
 *
 * @endif
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 */
#ifndef COMPILER_INCREMENTAL_COMPILER_HPP
#define COMPILER_INCREMENTAL_COMPILER_HPP
#pragma once

#include "ic10/incremental/incremental_lexer.hpp"
#include "ic10/incremental/incremental_parser.hpp"
#include "ic10/lexer/token.hpp"
#include "ic10/parser/ast/ast.hpp"
#include <string>
#include <string_view>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @struct IncrementalCompileResult
     * @brief 增量编译结果
     *
     * @elseif en
     * @struct IncrementalCompileResult
     * @brief Incremental compilation result
     *
     * @endif
     */
    struct IncrementalCompileResult {
        /** @brief 完整的Token序列 */
        std::vector<std::shared_ptr<Token>> tokens;

        /** @brief 解析后的Program AST */
        Program ast;

        /** @brief 是否执行了增量更新（false表示全量重编译） */
        bool incremental = false;

        /** @brief 重新词法分析的行数 */
        std::size_t relexedLines = 0;

        /** @brief 重新解析的语句数 */
        std::size_t reparsedStatements = 0;
    };

    /**
     * @if zh
     * @class IncrementalCompiler
     * @brief IC10增量编译器
     * @details 整合增量词法分析、增量语法分析的统一入口。
     *          语义分析由于缺乏依赖追踪，仍为全量执行。
     *
     * @elseif en
     * @class IncrementalCompiler
     * @brief IC10 incremental compiler
     * @details Unified entry point integrating incremental lexing and parsing.
     *          Semantic analysis is still full due to lack of dependency tracking.
     *
     * @endif
     */
    class IncrementalCompiler {
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
        IncrementalCompiler() = default;

        /**
         * @if zh
         * @brief 全量编译
         * @details 对完整源代码执行全量编译，建立缓存。
         *          首次编译或缓存失效时调用。
         *
         * @param source 完整源代码
         * @return 编译结果，包含Token、AST和编译统计
         *
         * @elseif en
         * @brief Full compilation
         * @details Perform full compilation on complete source code, building cache.
         *          Called on first compilation or when cache is invalid.
         *
         * @param source Complete source code
         * @return Compilation result with tokens, AST, and stats
         *
         * @endif
         */
        IncrementalCompileResult compileFull(std::string_view source);

        /**
         * @if zh
         * @brief 增量编译
         * @details 基于上一次的缓存进行增量编译。
         *          如果缓存失效或差异太大，则回退到全量编译。
         *
         * @param newSource 新的完整源代码
         * @return 编译结果，包含Token、AST和增量统计
         *
         * @elseif en
         * @brief Incremental compilation
         * @details Perform incremental compilation based on previous cache.
         *          Falls back to full compilation if cache is invalid or diff is too large.
         *
         * @param newSource New complete source code
         * @return Compilation result with tokens, AST, and incremental stats
         *
         * @endif
         */
        IncrementalCompileResult compileIncremental(std::string_view newSource);

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
        [[nodiscard]] bool hasCache() const { return lexer_.hasCache() && parser_.hasCache(); }

        /**
         * @if zh
         * @brief 清除所有缓存
         *
         * @elseif en
         * @brief Clear all caches
         *
         * @endif
         */
        void clear() {
            lexer_.clear();
            parser_.clear();
        }

        /**
         * @if zh
         * @brief 获取增量词法分析器（只读）
         * @return 增量词法分析器的常量引用
         *
         * @elseif en
         * @brief Get incremental lexer (read-only)
         * @return Const reference to incremental lexer
         *
         * @endif
         */
        [[nodiscard]] const IncrementalLexer& getLexer() const { return lexer_; }

        /**
         * @if zh
         * @brief 获取增量语法分析器（只读）
         * @return 增量语法分析器的常量引用
         *
         * @elseif en
         * @brief Get incremental parser (read-only)
         * @return Const reference to incremental parser
         *
         * @endif
         */
        [[nodiscard]] const IncrementalParser& getParser() const { return parser_; }

    private:
        /** @brief 增量词法分析器 */
        IncrementalLexer lexer_;

        /** @brief 增量语法分析器 */
        IncrementalParser parser_;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_INCREMENTAL_COMPILER_HPP
