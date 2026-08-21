// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_lexer.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10词法分析器Java绑定
 * @details 提供IC10词法分析器(Lexer)的JNI绑定，支持Java层调用词法分析功能。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 lexer Java binding
 * @details Provides JNI bindings for IC10 lexer, supporting Java layer calls to lexical analysis.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_LEXER_HPP
#define COMPILER_J_LEXER_HPP
#pragma once

#include "ic10_compiler_java/lexer/j_token.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include "common_java/j_diagnostic.hpp"
#include "common_java/base.hpp"

namespace stationeers::ic10 {

    class JLexer : public java::HybridClass<JLexer> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/Lexer;";

        Lexer lexer_;
        std::string source_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        void init(std::string source, bool debug);

        java::local_ref<java::JArrayClass<JToken::javaobject>> scan();

        java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> getDiagnostics();

        static java::local_ref<java::JArrayClass<JToken::javaobject>> tokenize(
            java::alias_ref<java::JClass>, std::string source, bool debug
        );

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_LEXER_HPP
