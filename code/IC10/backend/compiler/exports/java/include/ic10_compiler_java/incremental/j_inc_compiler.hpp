// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_inc_compiler.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10增量编译器Java绑定
 * @details 提供IC10增量编译器(IncCompiler)的JNI绑定,支持Java层调用增量编译功能。
 *          增量编译器整合增量词法分析器和增量语法分析器,提供一站式增量编译接口。
 *          编译结果以JSON字符串形式返回,包含Token序列、AST、增量标志和统计信息。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 incremental compiler Java binding
 * @details Provides JNI bindings for IC10 incremental compiler, supporting Java layer calls to
 *          incremental compilation. The incremental compiler integrates incremental lexer and
 *          incremental parser, providing a one-stop incremental compilation interface.
 *          Compilation results are returned as JSON string containing token sequence, AST,
 *          incremental flag, and statistics.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_INC_COMPILER_HPP
#define COMPILER_J_INC_COMPILER_HPP
#pragma once

#include "common_java/base.hpp"
#include "ic10_compiler/incremental/inc_compiler.hpp"

#include <string>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JIncCompiler
     * @brief IC10增量编译器Java适配器类
     * @elseif en
     * @class JIncCompiler
     * @brief IC10 incremental compiler Java adapter class
     * @endif
     */
    class JIncCompiler : public java::HybridClass<JIncCompiler> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/IncCompiler;";

        IncCompiler compiler_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        /**
         * @if zh
         * @brief 全量编译
         * @details 对完整源码进行全量词法分析和语法分析,建立缓存。
         * @param source 源码字符串
         * @return JSON格式结果字符串
         * @elseif en
         * @brief Full compilation
         * @details Perform full lexical and syntactic analysis on complete source, building caches.
         * @param source Source code string
         * @return JSON format result string
         * @endif
         */
        std::string compileFull(std::string source);

        /**
         * @if zh
         * @brief 增量编译
         * @details 基于上一次的缓存进行增量编译。若缓存为空,则回退到全量编译。
         * @param newSource 新的源码字符串
         * @return JSON格式结果字符串
         * @elseif en
         * @brief Incremental compilation
         * @details Perform incremental compilation based on previous cache.
         *          Falls back to full compilation if cache is empty.
         * @param newSource New source code string
         * @return JSON format result string
         * @endif
         */
        std::string compileInc(std::string newSource);

        /**
         * @if zh
         * @brief 检查是否有有效缓存
         * @elseif en
         * @brief Check if there is valid cache
         * @endif
         */
        jboolean hasCache();

        /**
         * @if zh
         * @brief 清除所有缓存
         * @elseif en
         * @brief Clear all caches
         * @endif
         */
        void clear();

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_INC_COMPILER_HPP
