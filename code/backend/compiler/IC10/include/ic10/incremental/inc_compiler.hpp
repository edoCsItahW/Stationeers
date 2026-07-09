// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_compiler.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/04
 * @if zh
 * @brief IC10增量编译器
 * @details 整合增量词法分析器和增量语法分析器，提供一站式增量编译接口。
 *          适用于编辑器场景下的频繁小修改，仅重新分析受影响的部分。
 *
 * @note 增量编译器是编译流水线的最上层封装
 *
 * @par 编译流程:
 * 1. 调用 IncLexer 进行增量词法分析
 * 2. 若词法分析回退到全量，则语法分析也全量重解析
 * 3. 若词法分析为增量，则调用 IncParser 进行增量语法分析
 * 4. 返回合并的编译结果
 *
 * @par 使用方式:
 * @code
 * IncCompiler compiler;
 * // 首次全量编译
 * auto result = compiler.compileFull(source);
 * // 后续增量编译
 * result = compiler.compileInc(newSource);
 * @endcode
 *
 * @see IncLexer 增量词法分析器
 * @see IncParser 增量语法分析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 incremental compiler
 * @details Integrates incremental lexer and incremental parser, providing
 *          a one-stop incremental compilation interface. Suitable for frequent
 *          small edits in editor scenarios, only re-analyzes affected parts.
 *
 * @note The incremental compiler is the top-level wrapper of the compilation pipeline
 *
 * @par Compilation Flow:
 * 1. Call IncLexer for incremental lexical analysis
 * 2. If lexer falls back to full, parser also does full re-parse
 * 3. If lexer is incremental, call IncParser for incremental parsing
 * 4. Return combined compilation result
 *
 * @par Usage:
 * @code
 * IncCompiler compiler;
 * // First full compile
 * auto result = compiler.compileFull(source);
 * // Subsequent incremental compile
 * result = compiler.compileInc(newSource);
 * @endcode
 *
 * @see IncLexer Incremental lexer
 * @see IncParser Incremental parser
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_INC_COMPILER_HPP
#define COMPILER_INC_COMPILER_HPP
#pragma once
#include "ic10/parser/ast/ast.hpp"
#include "inc_lexer.hpp"
#include "inc_parser.hpp"
#include <memory>
#include <vector>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @struct IncCompileResult
     * @brief 增量编译结果
     * @details 包含Token序列、AST、增量标志及相关统计信息。
     *
     * @elseif en
     * @struct IncCompileResult
     * @brief Incremental compilation result
     * @details Contains token sequence, AST, incremental flag, and related statistics.
     *
     * @endif
     */
    struct IncCompileResult {
        /** @if zh @brief 词法分析得到的Token序列 @else @brief Token sequence from lexing @endif */
        std::vector<std::shared_ptr<Token>> tokens;

        /** @if zh @brief 语法分析得到的Program AST @else @brief Program AST from parsing @endif */
        Program ast;

        /** @if zh @brief 是否为增量编译结果 @else @brief Whether this is an incremental result @endif */
        bool incremental = false;

        /** @if zh @brief 重新词法分析的行数 @else @brief Number of re-lexed lines @endif */
        std::size_t relexedLines = 0;

        /** @if zh @brief 重新解析的语句数 @else @brief Number of re-parsed statements @endif */
        std::size_t reparsedStmts = 0;
    };

    /**
     * @if zh
     * @class IncCompiler
     * @brief 增量编译器
     * @details 整合增量词法分析器和增量语法分析器的顶层封装。
     *          适用于编辑器场景下的频繁小修改，提供一站式增量编译接口。
     *
     * @elseif en
     * @class IncCompiler
     * @brief Incremental compiler
     * @details Top-level wrapper integrating incremental lexer and incremental parser.
     *          Suitable for frequent small edits in editor scenarios, providing
     *          a one-stop incremental compilation interface.
     *
     * @endif
     */
    class IncCompiler {

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
        IncCompiler() = default;

        /**
         * @if zh
         * @brief 检查是否有有效缓存
         * @return 如果词法和语法分析都有缓存返回true
         *
         * @elseif en
         * @brief Check if there is valid cache
         * @return true if both lexer and parser have cache
         *
         * @endif
         */
        [[nodiscard]] bool hasCache() const;

        /**
         * @if zh
         * @brief 清除所有缓存
         *
         * @elseif en
         * @brief Clear all caches
         *
         * @endif
         */
        void clear();

        /**
         * @if zh
         * @brief 全量编译
         * @details 对完整源码进行全量词法分析和语法分析，建立缓存。
         *
         * @param source 源码字符串
         * @return 编译结果，包含Token序列和AST
         *
         * @elseif en
         * @brief Full compilation
         * @details Perform full lexical and syntactic analysis on complete source, building caches.
         *
         * @param source Source code string
         * @return Compilation result with token sequence and AST
         *
         * @endif
         */
        IncCompileResult compileFull(std::string_view source);

        /**
         * @if zh
         * @brief 增量编译
         * @details 基于上一次的缓存进行增量编译。
         *          若缓存为空，则回退到全量编译。
         *
         * @param source 新的源码字符串
         * @return 编译结果，包含Token序列、AST和增量统计
         *
         * @elseif en
         * @brief Incremental compilation
         * @details Perform incremental compilation based on previous cache.
         *          Falls back to full compilation if cache is empty.
         *
         * @param source New source code string
         * @return Compilation result with tokens, AST and incremental stats
         *
         * @endif
         */
        IncCompileResult compileInc(std::string_view source);

    private:
        /** @if zh @brief 增量词法分析器 @else @brief Incremental lexer @endif */
        IncLexer lexer_;

        /** @if zh @brief 增量语法分析器 @else @brief Incremental parser @endif */
        IncParser parser_;
    };

}

#endif  // COMPILER_INC_COMPILER_HPP
