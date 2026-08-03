// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_analyser.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10语义分析器Java绑定
 * @details 提供IC10语义分析器(Analyser)的JNI绑定，支持Java层调用语义分析功能。
 *          analyse和visit方法通过阻塞协程实现同步调用。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 semantic analyser Java binding
 * @details Provides JNI bindings for IC10 semantic analyser, supporting Java layer calls to semantic analysis.
 *          analyse and visit methods implement synchronous calls via blocking coroutine.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_ANALYSER_HPP
#define COMPILER_J_ANALYSER_HPP
#pragma once

#include "common_java/base.hpp"
#include "common_java/j_diagnostic.hpp"
#include "ic10_java/parser/j_program.hpp"
#include "ic10_java/semantic/j_symbol_table.hpp"
#include "ic10_java/semantic/j_type_table.hpp"
#include "ic10/semantic/analyser.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JAnalyser
     * @brief IC10语义分析器Java适配器类
     * @details 提供IC10语义分析器(Analyser)的Java绑定封装，
     *          支持Java层调用语义分析功能，对抽象语法树进行
     *          类型检查、作用域分析、错误检测和符号表构建。
     *          analyse和visit方法通过阻塞协程实现同步调用。
     * @elseif en
     * @class JAnalyser
     * @brief IC10 semantic analyser Java adapter class
     * @details Provides Java binding encapsulation for IC10 semantic analyser,
     *          supporting Java layer calls to semantic analysis functionality,
     *          performing type checking, scope analysis, error detection,
     *          and symbol table construction on abstract syntax trees.
     *          analyse and visit methods implement synchronous calls via blocking coroutine.
     * @endif
     */
    class JAnalyser : public java::HybridClass<JAnalyser> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/Analyser;";

        Analyser analyser_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        void visit(java::alias_ref<JProgram::javaobject> program);

        java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> getDiagnostics();

        java::local_ref<JSymbolTable::javaobject> getSymbolTable();

        java::local_ref<JTypeTable::javaobject> getTypeTable();

        static void analyse(java::alias_ref<java::JClass>, java::alias_ref<JProgram::javaobject> program);

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_ANALYSER_HPP
