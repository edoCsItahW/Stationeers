// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_parser.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10语法分析器Java绑定
 * @details 提供IC10语法分析器(Parser)的JNI绑定，支持Java层调用语法分析功能。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 parser Java binding
 * @details Provides JNI bindings for IC10 parser, supporting Java layer calls to syntax analysis.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_PARSER_HPP
#define COMPILER_J_PARSER_HPP
#pragma once

#include "common_java/base.hpp"
#include "common_java/j_diagnostic.hpp"
#include "ic10_java/lexer/j_token.hpp"
#include "ic10_java/parser/j_program.hpp"
#include "ic10/parser/parser.hpp"

namespace stationeers::ic10 {

    class JParser : public java::HybridClass<JParser> {
    public:
        static constexpr auto kJavaDescriptor = "Lcom/stationeers/ic10/Parser;";

        Parser parser_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        void init(java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens, bool debug);

        java::local_ref<JProgram::javaobject> parse();

        java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> getDiagnostics();

        static java::local_ref<JProgram::javaobject> parsing(
            java::alias_ref<java::JClass>,
            java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens, bool debug
        );

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_PARSER_HPP
