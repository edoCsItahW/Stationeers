// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_compiler.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/04 14:02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/incremental/inc_compiler.hpp"

namespace stationeers::ic10 {

    // hasCache: 检查词法和语法分析器是否都有有效缓存
    //
    // 只有两者都有缓存时，才能进行完整的增量编译。
    bool IncCompiler::hasCache() const { return lexer_.hasCache() && parser_.hasCache(); }

    // clear: 清空所有缓存（词法+语法），使增量编译器回到初始状态
    void IncCompiler::clear() {
        lexer_.clear();
        parser_.clear();
    }

    // compileFull: 全量编译
    //
    // 先调用全量词法分析，再调用全量语法分析，
    // 建立完整的缓存，返回合并的编译结果。
    IncCompileResult IncCompiler::compileFull(std::string_view source) {
        const auto lexerResult = lexer_.tokenizeFull(source);

        const auto parserResult = parser_.parseFull(lexerResult.tokens);

        return {
            .tokens        = std::move(lexerResult.tokens),
            .ast           = std::move(parserResult.ast),
            .incremental   = false,
            .relexedLines  = lexerResult.relexedLines,
            .reparsedStmts = parserResult.reparsedStmts
        };
    }

    // compileInc: 增量编译 —— 核心调度逻辑
    //
    // 调度策略：
    //   1. 无缓存 → 回退全量编译
    //   2. 先进行增量词法分析
    //   3. 若词法分析回退到全量 → 语法分析也全量重解析
    //   4. 若词法分析为增量 → 调用增量语法分析
    //   5. 返回合并的编译结果
    IncCompileResult IncCompiler::compileInc(std::string_view source) {
        if (!hasCache()) return compileFull(source);

        const auto lexerResult = lexer_.tokenizeInc(source);

        IncCompileResult result{
            .tokens = std::move(lexerResult.tokens), .relexedLines = lexerResult.relexedLines
        };

        if (!lexerResult.incremental) {
            const auto parserResult = parser_.parseFull(result.tokens);

            result.ast           = std::move(parserResult.ast);
            result.reparsedStmts = parserResult.reparsedStmts;
            result.incremental   = false;

            return result;
        }

        const auto parserResult = parser_.parseInc(result.tokens, lexerResult.changedStartLine);

        result.ast           = std::move(parserResult.ast);
        result.reparsedStmts = parserResult.reparsedStmts;
        result.incremental   = parserResult.incremental;

        return result;
    }

}  // namespace stationeers::ic10
