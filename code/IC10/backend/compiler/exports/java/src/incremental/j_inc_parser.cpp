// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_inc_parser.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_java/incremental/j_inc_parser.hpp"

#include <sstream>

namespace stationeers::ic10 {

    namespace {

        /**
         * @if zh
         * @brief 将IncParserResult转换为JSON字符串
         * @details JSON格式:
         *          {"ast":...,"incremental":bool,"reparsedStmts":int,"affectedStmtStart":int}
         *          ast字段为Program AST的JSON表示。
         * @elseif en
         * @brief Convert IncParserResult to JSON string
         * @details JSON format:
         *          {"ast":...,"incremental":bool,"reparsedStmts":int,"affectedStmtStart":int}
         *          The ast field is the JSON representation of Program AST.
         * @endif
         */
        std::string resultToJSON(const IncParserResult& result) {
            std::ostringstream oss;

            oss << "{";

            oss << "\"ast\":" << result.ast.toJSON() << ",";
            oss << "\"incremental\":" << (result.incremental ? "true" : "false") << ",";
            oss << "\"reparsedStmts\":" << result.reparsedStmts << ",";
            oss << "\"affectedStmtStart\":" << result.affectedStmtStart;

            oss << "}";

            return oss.str();
        }

        /**
         * @if zh
         * @brief 从Java Token数组转换为C++ Token向量
         * @elseif en
         * @brief Convert Java Token array to C++ Token vector
         * @endif
         */
        std::vector<std::shared_ptr<Token>> toTokenVector(java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens) {
            std::vector<std::shared_ptr<Token>> tokenVec;

            if (tokens) {
                auto size = tokens->size();
                tokenVec.reserve(size);
                for (std::size_t i = 0; i < size; ++i) {
                    auto jtoken = tokens->getElement(i);
                    tokenVec.push_back(std::make_shared<Token>(jtoken->cthis()->token_));
                }
            }

            return tokenVec;
        }

    }  // namespace

    JIncParser::jhybriddata JIncParser::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    std::string JIncParser::parseFull(java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens) {
        auto tokenVec = toTokenVector(tokens);
        auto result = parser_.parseFull(tokenVec);
        return resultToJSON(result);
    }

    std::string JIncParser::parseInc(java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens, jint changedStartLine) {
        auto tokenVec = toTokenVector(tokens);
        auto result = parser_.parseInc(tokenVec, static_cast<int>(changedStartLine));
        return resultToJSON(result);
    }

    jboolean JIncParser::hasCache() {
        return static_cast<jboolean>(parser_.hasCache());
    }

    void JIncParser::clear() {
        parser_.clear();
    }

    void JIncParser::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JIncParser::initHybrid),
            makeNativeMethod("parseFull", JIncParser::parseFull),
            makeNativeMethod("parseInc", JIncParser::parseInc),
            makeNativeMethod("hasCache", JIncParser::hasCache),
            makeNativeMethod("clear", JIncParser::clear),
        });
    }

}  // namespace stationeers::ic10
