// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_lexer.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_java/lexer/j_lexer.hpp"
#include "common_java/j_diagnostic.hpp"

namespace stationeers::ic10 {

    JLexer::jhybriddata JLexer::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    void JLexer::init(std::string source, bool debug) {
        source_ = std::move(source);
        lexer_ = Lexer(source_, debug);
    }

    java::local_ref<java::JArrayClass<JToken::javaobject>> JLexer::scan() {
        auto tokens = lexer_.scan();
        auto size = tokens.size();
        auto result = java::JArrayClass<JToken::javaobject>::newArray(size);

        for (std::size_t i = 0; i < size; ++i) {
            result->setElement(i, JToken::to(*tokens[i]).get());
        }

        return result;
    }

    java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> JLexer::getDiagnostics() {
        auto diagnostics = lexer_.getDiagnostics();
        auto size = diagnostics.size();
        auto result = java::JArrayClass<JDiagnostic::javaobject>::newArray(size);

        for (std::size_t i = 0; i < size; ++i) {
            result->setElement(i, JDiagnostic::to(diagnostics[i]).get());
        }

        return result;
    }

    java::local_ref<java::JArrayClass<JToken::javaobject>> JLexer::tokenize(
        java::alias_ref<java::JClass>, std::string source, bool debug
    ) {
        auto tokens = Lexer::tokenize(source, debug);
        auto size = tokens.size();
        auto result = java::JArrayClass<JToken::javaobject>::newArray(size);

        for (std::size_t i = 0; i < size; ++i) {
            result->setElement(i, JToken::to(*tokens[i]).get());
        }

        return result;
    }

    void JLexer::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JLexer::initHybrid),
            makeNativeMethod("init", JLexer::init),
            makeNativeMethod("scan", JLexer::scan),
            makeNativeMethod("getDiagnostics", JLexer::getDiagnostics),
            makeNativeMethod("tokenize", JLexer::tokenize),
        });
    }

}  // namespace stationeers::ic10
