// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file incremental_parser.cpp
 * @author edocsitahw
 * @brief IC10增量语法分析器实现
 * @details 基于语句级的增量语法分析实现，仅重新解析受影响的语句。
 *          利用IC10行式语言的特性，以行为单位进行语句级增量更新。
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/incremental/incremental_parser.hpp"
#include <algorithm>

namespace stationeers::ic10 {

    std::size_t IncrementalParser::findStatementStart(
        const std::vector<std::shared_ptr<Token>>& tokens,
        std::size_t startIndex
    ) {
        std::size_t i = startIndex;

        while (i < tokens.size()) {
            const auto& token = tokens[i];

            if (token->type == TokenType::END) {
                return i;
            }

            if (token->type == TokenType::NEWLINE
                || token->type == TokenType::HEX_COMMENT
                || token->type == TokenType::SLASH_COMMENT) {
                ++i;
                continue;
            }

            return i;
        }

        return tokens.size();
    }

    std::size_t IncrementalParser::findStatementIndexForLine(
        const Program& program,
        const int lineNumber
    ) {
        for (std::size_t i = 0; i < program.statements.size(); ++i) {
            const auto& stmt = program.statements[i];
            const Pos* stmtPos = std::visit([](const auto& node) -> const Pos* {
                return &node.position;
            }, stmt);

            if (stmtPos->line() >= lineNumber) {
                return i;
            }
        }

        return program.statements.size();
    }

    std::pair<std::size_t, std::size_t> IncrementalParser::getTokenRangeForLines(
        const std::vector<std::shared_ptr<Token>>& tokens,
        const int startLine,
        const int endLine
    ) {
        std::size_t first = tokens.size();
        std::size_t last = tokens.size();

        for (std::size_t i = 0; i < tokens.size(); ++i) {
            const int line = tokens[i]->pos.line();

            if (line >= startLine && first == tokens.size()) {
                first = i;
            }

            if (line >= endLine) {
                last = i;
                break;
            }
        }

        if (first == tokens.size()) {
            first = tokens.size() > 0 ? tokens.size() - 1 : 0;
        }

        return {first, last};
    }

    ParserIncrementalResult IncrementalParser::parseFull(
        const std::vector<std::shared_ptr<Token>>& tokens
    ) {
        ParserIncrementalResult result;
        result.incremental = false;

        Parser parser(tokens);
        result.ast = parser.parse();
        result.reparsedStatements = result.ast.statements.size();
        result.affectedStatementStart = 0;

        cachedProgram_ = result.ast;

        return result;
    }

    ParserIncrementalResult IncrementalParser::parseIncremental(
        const std::vector<std::shared_ptr<Token>>& tokens,
        const int changedStartLine,
        const int oldChangedEndLine,
        const int newChangedEndLine
    ) {
        if (!cachedProgram_.has_value()) {
            return parseFull(tokens);
        }

        if (changedStartLine <= 0) {
            return parseFull(tokens);
        }

        const std::size_t firstStmtIdx = findStatementIndexForLine(*cachedProgram_, changedStartLine);

        std::size_t oldEndStmtIdx = cachedProgram_->statements.size();
        for (std::size_t i = firstStmtIdx; i < cachedProgram_->statements.size(); ++i) {
            const auto& stmt = cachedProgram_->statements[i];
            const Pos* stmtPos = std::visit([](const auto& node) -> const Pos* {
                return &node.position;
            }, stmt);
            if (stmtPos->line() >= oldChangedEndLine) {
                oldEndStmtIdx = i;
                break;
            }
        }

        const auto [tokenStartIdx, tokenEndIdx] = getTokenRangeForLines(
            tokens, changedStartLine, newChangedEndLine
        );

        std::vector<std::shared_ptr<Token>> subTokens;
        subTokens.reserve(tokenEndIdx - tokenStartIdx + 1);
        for (std::size_t i = tokenStartIdx; i < tokenEndIdx; ++i) {
            subTokens.push_back(tokens[i]);
        }

        auto endToken = std::make_shared<Token>();
        endToken->type = TokenType::END;
        if (!subTokens.empty()) {
            endToken->pos = subTokens.back()->pos;
        }
        endToken->lexeme = "";
        endToken->category = TokenCategory::END;
        subTokens.push_back(endToken);

        Parser subParser(subTokens);
        Program subProgram = subParser.parse();

        Program newProgram = *cachedProgram_;
        std::vector<Statement> newStatements;
        newStatements.reserve(
            firstStmtIdx + subProgram.statements.size()
            + (newProgram.statements.size() - oldEndStmtIdx)
        );

        for (std::size_t i = 0; i < firstStmtIdx && i < newProgram.statements.size(); ++i) {
            newStatements.push_back(newProgram.statements[i]);
        }

        for (auto& stmt : subProgram.statements) {
            newStatements.push_back(std::move(stmt));
        }

        for (std::size_t i = oldEndStmtIdx; i < newProgram.statements.size(); ++i) {
            newStatements.push_back(newProgram.statements[i]);
        }

        newProgram.statements = std::move(newStatements);

        ParserIncrementalResult result;
        result.ast = newProgram;
        result.incremental = true;
        result.reparsedStatements = subProgram.statements.size();
        result.affectedStatementStart = firstStmtIdx;

        cachedProgram_ = newProgram;

        return result;
    }

}  // namespace stationeers::ic10
