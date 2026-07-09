// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file inc_parser.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/09
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/incremental/inc_parser.hpp"
#include "ic10/parser/parser.hpp"

namespace stationeers::ic10 {

    // ========================================================================
    // hasCache: 检查是否存在有效的AST缓存
    // ========================================================================
    bool IncParser::hasCache() const { return astCached_.has_value(); }

    // ========================================================================
    // clear: 清空AST缓存，使增量分析器回到初始状态
    // ========================================================================
    void IncParser::clear() { astCached_.reset(); }

    // ========================================================================
    // parseFull: 全量语法分析，建立AST缓存
    //
    // 直接调用全量Parser解析所有Token，将结果存入缓存。
    // 首次编译或缓存失效时调用。
    // ========================================================================
    IncParserResult IncParser::parseFull(const std::vector<std::shared_ptr<Token>>& tokens) {
        astCached_ = Parser::parsing(tokens);

        return {
            .ast               = astCached_.value(),
            .incremental       = false,
            .reparsedStmts     = astCached_.value().statements.size(),
            .affectedStmtStart = 0
        };
    }

    // ========================================================================
    // parseInc: 增量语法分析 —— 核心算法
    //
    // 策略：从变化行到文件末尾重新解析，前缀语句直接复用。
    //
    // 步骤：
    //   1. 安全检查：无缓存或行号非法 → 回退全量
    //   2. 定位：找到第一个行号 >= changedStartLine 的语句索引
    //   3. 重解析：截取变化行及之后的所有Token，调用全量Parser解析
    //   4. 拼接：前缀语句（复用） + 新解析语句（替换）
    //   5. 更新缓存，返回结果
    //
    // 注意：后缀（变化行之后）也被重新解析，因为：
    //   - 变化可能影响后续语句的符号解析（如alias、define）
    //   - 行号和偏移量可能变化，重解析保证位置信息正确
    // ========================================================================
    IncParserResult IncParser::parseInc(
        const std::vector<std::shared_ptr<Token>>& tokens, const int changedStartLine
    ) {
        // 无缓存或行号非法 → 回退全量解析
        if (!astCached_.has_value() || changedStartLine <= 0) return parseFull(tokens);

        // 找到旧缓存中第一个受影响的语句索引
        // （行号 >= changedStartLine 的语句起始下标）
        const auto firstStmtIdx = findLineStmtIndex(*astCached_, changedStartLine);

        // 截取变化行及之后的所有Token，重新解析
        // 使用 drop_while 跳过变化行之前的Token
        auto subProgram =
            parseTokenRange(tokens | std::views::drop_while([changedStartLine](const auto& token) {
                                return token->pos.line() < changedStartLine;
                            }));

        // 构造新AST：前缀复用 + 后缀重解析
        Program newProgram = *astCached_;
        std::vector<Statement> newStmts;

        // 预分配空间，避免多次扩容
        newStmts.reserve(firstStmtIdx + subProgram.statements.size());

        // 前缀：[0, firstStmtIdx) 直接复用缓存中的语句
        for (std::size_t i = 0; i < firstStmtIdx && i < newProgram.statements.size(); ++i)
            newStmts.push_back(newProgram.statements[i]);

        // 后缀：新解析的语句（替换旧的 [firstStmtIdx, end)）
        for (auto& stmt : subProgram.statements) newStmts.push_back(std::move(stmt));

        newProgram.statements = std::move(newStmts);

        // 更新缓存
        astCached_ = newProgram;

        return {
            .ast               = std::move(newProgram),
            .incremental       = true,
            .reparsedStmts     = subProgram.statements.size(),
            .affectedStmtStart = firstStmtIdx
        };
    }

    // ========================================================================
    // findLineStmtIndex: 二分查找第一个行号 >= lineNumber 的语句索引
    //
    // 当前实现为线性扫描（语句数量通常不大，且语句按行号有序）。
    // 若未来性能敏感，可改为二分查找。
    //
    // 返回值：
    //   - 找到：第一个满足条件的语句索引
    //   - 未找到：program.statements.size()（所有语句都在目标行之前）
    // ========================================================================
    std::size_t IncParser::findLineStmtIndex(const Program& program, int lineNumber) {
        for (std::size_t i = 0; i < program.statements.size(); ++i) {
            const Pos* stmtPos = std::visit(
                [](const auto& node) -> const Pos* { return &node.position; }, program.statements[i]
            );

            if (stmtPos->line() >= lineNumber) return i;
        }

        return program.statements.size();
    }

    // ========================================================================
    // buildEndToken: 构造END Token，用于子序列解析
    //
    // 增量解析时，截取的Token子序列末尾可能没有END Token，
    // Parser需要END Token来正确终止解析，因此手动补充一个。
    // 位置取最后一个Token的位置（若序列为空则用默认位置）。
    // ========================================================================
    std::shared_ptr<Token> IncParser::buildEndToken(
        const std::vector<std::shared_ptr<Token>>& tokens
    ) {
        auto endToken  = std::make_shared<Token>();
        endToken->type = TokenType::END;

        // 若序列非空，END位置取最后一个Token的位置
        if (!tokens.empty()) endToken->pos = tokens.back()->pos;

        endToken->lexeme   = "";
        endToken->category = TokenCategory::END;

        return endToken;
    }

}  // namespace stationeers::ic10
