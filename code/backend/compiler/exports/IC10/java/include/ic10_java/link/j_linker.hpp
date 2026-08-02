// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_linker.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10链接器Java绑定
 * @details 提供IC10链接器(Linker)的JNI绑定,支持Java层调用链接功能,
 *          将多个编译单元的符号表合并为统一的全局符号表。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 linker Java binding
 * @details Provides JNI bindings for IC10 linker, supporting Java layer calls to linking,
 *          merging symbol tables from multiple compilation units into a unified global symbol table.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_LINKER_HPP
#define COMPILER_J_LINKER_HPP
#pragma once

#include "common_java/base.hpp"
#include "common_java/j_diagnostic.hpp"
#include "ic10_java/parser/j_program.hpp"
#include "ic10_java/semantic/j_symbol_table.hpp"
#include "ic10_java/semantic/j_type_table.hpp"
#include "ic10/link/linker.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JLinker
     * @brief IC10链接器Java适配器类
     * @details 提供IC10链接器的Java绑定封装,支持添加编译单元(已编译的Program或源代码字符串),
     *          执行链接,获取诊断信息和编译单元信息。
     * @elseif en
     * @class JLinker
     * @brief IC10 linker Java adapter class
     * @details Provides Java binding encapsulation for IC10 linker, supporting adding compilation units
     *          (compiled Program or source code strings), performing linking, and retrieving
     *          diagnostic and unit information.
     * @endif
     */
    class JLinker : public java::HybridClass<JLinker> {
    public:
        static constexpr auto kJavaDescriptor = "Lcom/stationeers/ic10/Linker;";

        Linker linker_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        /**
         * @if zh
         * @brief 添加编译单元(已编译的Program,带路径)
         * @elseif en
         * @brief Add compilation unit (compiled Program with path)
         * @endif
         */
        void addUnitProgram(java::alias_ref<JProgram::javaobject> program, std::string path);

        /**
         * @if zh
         * @brief 添加编译单元(已编译的Program,无路径)
         * @elseif en
         * @brief Add compilation unit (compiled Program without path)
         * @endif
         */
        void addUnitProgramNoPath(java::alias_ref<JProgram::javaobject> program);

        /**
         * @if zh
         * @brief 添加编译单元(源代码字符串,带路径)
         * @elseif en
         * @brief Add compilation unit (source code string with path)
         * @endif
         */
        void addUnitSource(std::string source, std::string path);

        /**
         * @if zh
         * @brief 添加编译单元(源代码字符串,无路径)
         * @elseif en
         * @brief Add compilation unit (source code string without path)
         * @endif
         */
        void addUnitSourceNoPath(std::string source);

        /**
         * @if zh
         * @brief 执行链接操作
         * @elseif en
         * @brief Perform linking
         * @endif
         */
        java::local_ref<JSymbolTable::javaobject> link();

        /**
         * @if zh
         * @brief 获取所有诊断信息
         * @elseif en
         * @brief Get all diagnostics
         * @endif
         */
        java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> getDiagnostics();

        /**
         * @if zh
         * @brief 获取全局类型表
         * @elseif en
         * @brief Get global type table
         * @endif
         */
        java::local_ref<JTypeTable::javaobject> getTypeTable();

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_LINKER_HPP
