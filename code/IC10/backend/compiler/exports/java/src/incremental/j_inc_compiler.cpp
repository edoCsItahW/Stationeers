// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_inc_compiler.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_java/incremental/j_inc_compiler.hpp"

#include <sstream>

namespace stationeers::ic10 {

    namespace {

        /**
         * @if zh
         * @brief 将IncCompileResult转换为JSON字符串
         * @details JSON格式:
         *          {"tokens":[...],"ast":...,"incremental":bool,
         *           "relexedLines":int,"reparsedStmts":int}
         *          tokens数组中每个元素为Token的JSON表示,ast字段为Program AST的JSON表示。
         * @elseif en
         * @brief Convert IncCompileResult to JSON string
         * @details JSON format:
         *          {"tokens":[...],"ast":...,"incremental":bool,
         *           "relexedLines":int,"reparsedStmts":int}
         *          Each element in tokens array is a JSON representation of Token,
         *          ast field is the JSON representation of Program AST.
         * @endif
         */
        std::string resultToJSON(const IncCompileResult& result) {
            std::ostringstream oss;

            oss << "{";

            // tokens 数组
            oss << "\"tokens\":[";
            for (std::size_t i = 0; i < result.tokens.size(); ++i) {
                if (i > 0) oss << ",";
                oss << result.tokens[i]->toJSON();
            }
            oss << "],";

            oss << "\"ast\":" << result.ast.toJSON() << ",";
            oss << "\"incremental\":" << (result.incremental ? "true" : "false") << ",";
            oss << "\"relexedLines\":" << result.relexedLines << ",";
            oss << "\"reparsedStmts\":" << result.reparsedStmts;

            oss << "}";

            return oss.str();
        }

    }  // namespace

    JIncCompiler::jhybriddata JIncCompiler::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    std::string JIncCompiler::compileFull(std::string source) {
        auto result = compiler_.compileFull(source);
        return resultToJSON(result);
    }

    std::string JIncCompiler::compileInc(std::string newSource) {
        auto result = compiler_.compileInc(newSource);
        return resultToJSON(result);
    }

    jboolean JIncCompiler::hasCache() {
        return static_cast<jboolean>(compiler_.hasCache());
    }

    void JIncCompiler::clear() {
        compiler_.clear();
    }

    void JIncCompiler::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JIncCompiler::initHybrid),
            makeNativeMethod("compileFull", JIncCompiler::compileFull),
            makeNativeMethod("compileInc", JIncCompiler::compileInc),
            makeNativeMethod("hasCache", JIncCompiler::hasCache),
            makeNativeMethod("clear", JIncCompiler::clear),
        });
    }

}  // namespace stationeers::ic10
