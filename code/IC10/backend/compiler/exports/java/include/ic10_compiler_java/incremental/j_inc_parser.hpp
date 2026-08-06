// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_inc_parser.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10增量语法分析器Java绑定
 * @details 提供IC10增量语法分析器(IncParser)的JNI绑定,支持Java层调用增量语法分析功能。
 *          增量语法分析器基于语句级缓存,从变化行到文件末尾重新解析,前缀语句直接复用。
 *          分析结果以JSON字符串形式返回,包含AST、增量标志和统计信息。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 incremental parser Java binding
 * @details Provides JNI bindings for IC10 incremental parser, supporting Java layer calls to
 *          incremental parsing. The incremental parser is based on statement-level cache,
 *          re-parsing from changed line to end of file, prefix statements are reused directly.
 *          Analysis results are returned as JSON string containing AST, incremental flag, and statistics.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_INC_PARSER_HPP
#define COMPILER_J_INC_PARSER_HPP
#pragma once

#include "common_java/base.hpp"
#include "ic10_compiler_java/lexer/j_token.hpp"
#include "ic10_compiler/incremental/inc_parser.hpp"

#include <string>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class JIncParser
     * @brief IC10增量语法分析器Java适配器类
     * @elseif en
     * @class JIncParser
     * @brief IC10 incremental parser Java adapter class
     * @endif
     */
    class JIncParser : public java::HybridClass<JIncParser> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/IncParser;";

        IncParser parser_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        /**
         * @if zh
         * @brief 全量语法分析
         * @param tokens Token数组
         * @return JSON格式结果字符串
         * @elseif en
         * @brief Full parsing
         * @param tokens Token array
         * @return JSON format result string
         * @endif
         */
        std::string parseFull(java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens);

        /**
         * @if zh
         * @brief 增量语法分析
         * @param tokens 新的完整Token序列
         * @param changedStartLine 变化起始行号(从1开始,含)
         * @return JSON格式结果字符串
         * @elseif en
         * @brief Incremental parsing
         * @param tokens New complete token sequence
         * @param changedStartLine Changed start line (1-based, inclusive)
         * @return JSON format result string
         * @endif
         */
        std::string parseInc(java::alias_ref<java::JArrayClass<JToken::javaobject>> tokens, jint changedStartLine);

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

#endif  // COMPILER_J_INC_PARSER_HPP
