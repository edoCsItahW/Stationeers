// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental_compiler.cpp
 * @author edocsitahw
 * @brief IC10增量编译器统一入口实现
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/incremental/incremental_compiler.hpp"

namespace stationeers::ic10 {

    IncrementalCompileResult IncrementalCompiler::compileFull(const std::string_view source) {
        IncrementalCompileResult result;
        result.incremental = false;

        const auto lexerResult = lexer_.tokenizeFull(source);
        result.tokens = lexerResult.tokens;
        result.relexedLines = lexerResult.relexedLines;

        const auto parserResult = parser_.parseFull(result.tokens);
        result.ast = parserResult.ast;
        result.reparsedStatements = parserResult.reparsedStatements;

        return result;
    }

    IncrementalCompileResult IncrementalCompiler::compileIncremental(const std::string_view newSource) {
        if (!hasCache()) {
            return compileFull(newSource);
        }

        IncrementalCompileResult result;

        const auto lexerResult = lexer_.tokenizeIncremental(newSource);
        result.tokens = lexerResult.tokens;
        result.relexedLines = lexerResult.relexedLines;

        if (!lexerResult.incremental) {
            const auto parserResult = parser_.parseFull(result.tokens);
            result.ast = parserResult.ast;
            result.reparsedStatements = parserResult.reparsedStatements;
            result.incremental = false;
            return result;
        }

        const auto parserResult = parser_.parseIncremental(
            result.tokens,
            lexerResult.changedStartLine,
            lexerResult.oldChangedEndLine,
            lexerResult.newChangedEndLine
        );

        result.ast = parserResult.ast;
        result.reparsedStatements = parserResult.reparsedStatements;
        result.incremental = parserResult.incremental;

        return result;
    }

}  // namespace stationeers::ic10
