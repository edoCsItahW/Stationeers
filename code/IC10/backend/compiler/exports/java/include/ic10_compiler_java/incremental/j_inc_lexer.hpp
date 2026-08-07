// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_inc_lexer.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10增量词法分析器Java绑定
 * @details 提供IC10增量词法分析器(IncLexer)的JNI绑定,支持Java层调用增量词法分析功能。
 *          增量词法分析器基于行哈希缓存,仅对变化区间重新词法分析,适用于编辑器场景。
 *          分析结果以JSON字符串形式返回,包含Token数组、增量标志和统计信息。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 incremental lexer Java binding
 * @details Provides JNI bindings for IC10 incremental lexer, supporting Java layer calls to
 *          incremental lexical analysis. The incremental lexer is based on line-hash cache,
 *          only re-lexing the changed range, suitable for editor scenarios.
 *          Analysis results are returned as JSON string containing token array, incremental flag,
 *          and statistics.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_INC_LEXER_HPP
#define COMPILER_J_INC_LEXER_HPP
#pragma once

#include "common_java/base.hpp"
#include "ic10_compiler/incremental/inc_lexer.hpp"

#include <string>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JIncLexer
     * @brief IC10增量词法分析器Java适配器类
     * @elseif en
     * @class JIncLexer
     * @brief IC10 incremental lexer Java adapter class
     * @endif
     */
    class JIncLexer : public java::HybridClass<JIncLexer> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/IncLexer;";

        IncLexer lexer_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        /**
         * @if zh
         * @brief 全量词法分析
         * @details 对完整源码进行词法分析,建立行缓存。
         * @param source 源码字符串
         * @return JSON格式结果字符串
         * @elseif en
         * @brief Full lexical analysis
         * @details Perform full lexical analysis on complete source, building line cache.
         * @param source Source code string
         * @return JSON format result string
         * @endif
         */
        std::string tokenizeFull(std::string source);

        /**
         * @if zh
         * @brief 增量词法分析
         * @details 基于上一次的缓存进行增量词法分析。若缓存为空,则回退到全量分析。
         * @param newSource 新的源码字符串
         * @return JSON格式结果字符串
         * @elseif en
         * @brief Incremental lexical analysis
         * @details Perform incremental lexical analysis based on previous cache.
         *          Falls back to full analysis if cache is empty.
         * @param newSource New source code string
         * @return JSON format result string
         * @endif
         */
        std::string tokenizeInc(std::string newSource);

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
         * @brief 清除缓存
         * @elseif en
         * @brief Clear cache
         * @endif
         */
        void clear();

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_J_INC_LEXER_HPP
