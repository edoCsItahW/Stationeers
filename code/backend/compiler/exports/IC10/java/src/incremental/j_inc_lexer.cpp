// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_inc_lexer.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_java/incremental/j_inc_lexer.hpp"

#include <sstream>

namespace stationeers::ic10 {

    namespace {

        /**
         * @if zh
         * @brief 将IncLexerResult转换为JSON字符串
         * @details JSON格式:
         *          {"tokens":[...],"incremental":bool,"relexedLines":int,
         *           "changedStartLine":int,"oldChangedEndLine":int,"newChangedEndLine":int}
         *          tokens数组中每个元素为Token的JSON表示。
         * @elseif en
         * @brief Convert IncLexerResult to JSON string
         * @details JSON format:
         *          {"tokens":[...],"incremental":bool,"relexedLines":int,
         *           "changedStartLine":int,"oldChangedEndLine":int,"newChangedEndLine":int}
         *          Each element in tokens array is a JSON representation of Token.
         * @endif
         */
        std::string resultToJSON(const IncLexerResult& result) {
            std::ostringstream oss;

            oss << "{";

            // tokens 数组
            oss << "\"tokens\":[";
            for (std::size_t i = 0; i < result.tokens.size(); ++i) {
                if (i > 0) oss << ",";
                oss << result.tokens[i]->toJSON();
            }
            oss << "],";

            oss << "\"incremental\":" << (result.incremental ? "true" : "false") << ",";
            oss << "\"relexedLines\":" << result.relexedLines << ",";
            oss << "\"changedStartLine\":" << result.changedStartLine << ",";
            oss << "\"oldChangedEndLine\":" << result.oldChangedEndLine << ",";
            oss << "\"newChangedEndLine\":" << result.newChangedEndLine;

            oss << "}";

            return oss.str();
        }

    }  // namespace

    JIncLexer::jhybriddata JIncLexer::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    std::string JIncLexer::tokenizeFull(std::string source) {
        auto result = lexer_.tokenizeFull(source);
        return resultToJSON(result);
    }

    std::string JIncLexer::tokenizeInc(std::string newSource) {
        auto result = lexer_.tokenizeInc(newSource);
        return resultToJSON(result);
    }

    jboolean JIncLexer::hasCache() {
        return static_cast<jboolean>(lexer_.hasCache());
    }

    void JIncLexer::clear() {
        lexer_.clear();
    }

    void JIncLexer::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JIncLexer::initHybrid),
            makeNativeMethod("tokenizeFull", JIncLexer::tokenizeFull),
            makeNativeMethod("tokenizeInc", JIncLexer::tokenizeInc),
            makeNativeMethod("hasCache", JIncLexer::hasCache),
            makeNativeMethod("clear", JIncLexer::clear),
        });
    }

}  // namespace stationeers::ic10
