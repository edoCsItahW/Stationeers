// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_parser.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_java/parser/j_parser.hpp"
#include "common_java/j_diagnostic.hpp"

namespace stationeers::ic10 {

    JParser::jhybriddata JParser::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    void JParser::init(java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens, bool debug) {
        std::vector<std::shared_ptr<Token>> tokenVec;

        if (tokens) {
            auto size = tokens->size();
            tokenVec.reserve(size);
            for (std::size_t i = 0; i < size; ++i) {
                auto jtoken = tokens->getElement(i);
                tokenVec.push_back(std::make_shared<Token>(jtoken->cthis()->token_));
            }
        }

        parser_ = Parser(tokenVec, debug);
    }

    java::local_ref<JProgram::javaobject> JParser::parse() {
        return JProgram::to(parser_.parse());
    }

    java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> JParser::getDiagnostics() {
        auto diagnostics = parser_.getDiagnostics();
        auto size = diagnostics.size();
        auto result = java::JArrayClass<JDiagnostic::javaobject>::newArray(size);

        for (std::size_t i = 0; i < size; ++i) {
            result->setElement(i, JDiagnostic::to(diagnostics[i]).get());
        }

        return result;
    }

    java::local_ref<JProgram::javaobject> JParser::parsing(
        java::alias_ref<java::JClass>,
        java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens, bool debug
    ) {
        std::vector<std::shared_ptr<Token>> tokenVec;

        if (tokens) {
            auto size = tokens->size();
            tokenVec.reserve(size);
            for (std::size_t i = 0; i < size; ++i) {
                auto jtoken = tokens->getElement(i);
                tokenVec.push_back(std::make_shared<Token>(jtoken->cthis()->token_));
            }
        }

        auto result = Parser::parsing(tokenVec, debug);

        return JProgram::to(result);
    }

    void JParser::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JParser::initHybrid),
            makeNativeMethod("init", JParser::init),
            makeNativeMethod("parse", JParser::parse),
            makeNativeMethod("getDiagnostics", JParser::getDiagnostics),
            makeNativeMethod("parsing", JParser::parsing),
        });
    }

}  // namespace stationeers::ic10
