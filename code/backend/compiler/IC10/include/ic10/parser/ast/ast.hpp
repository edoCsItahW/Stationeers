// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:50
 * @if zh
 * @brief IC10抽象语法树顶层定义
 * @details 定义IC10程序的顶层AST结构,包括Program(程序)和Statement(语句)类型。
 *        程序由一系列语句组成,语句可以是可执行指令、标签定义或预处理指令。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 abstract syntax tree top-level definitions
 * @details Defines top-level AST structures for IC10 programs, including Program and Statement types.
 *        A program consists of a series of statements, which can be executable instructions,
 *        label definitions, or preprocessor directives.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_AST_HPP
#define COMPILER_AST_HPP
#pragma once

#include "ast_senary_ins.hpp"

#include <vector>

namespace stationeers::ic10 {

    // Statement

    /**
     * @if zh
     *
     * @brief 语句类型别名
     * @details 表示IC10程序中的语句,可以是可执行指令、标签定义或预处理指令
     *
     * @elseif en
     *
     * @brief Statement type alias
     * @details Represents a statement in IC10 program, can be executable instruction,
     *        label definition or preprocessor directive
     *
     * @endif
     */
    using Statement = Errorable<ExecutableInstruction, LabelDef, PreprocessorDirective>;

    // Program（实现于ast.cpp）

    /**
     * @if zh
     *
     * @class Program
     * @brief IC10程序节点
     * @details 表示完整的IC10程序,是AST的根节点
     *
     * @elseif en
     *
     * @class Program
     * @brief IC10 program node
     * @details Represents a complete IC10 program, the root node of AST
     *
     * @endif
     */
    struct Program : AST<Program> {
        /**
         * @if zh
         * @brief 节点名称
         * @elseif en
         * @brief Node name
         * @endif
         */
        static constexpr auto nodeName = FString{"Program"};

        /**
         * @if zh
         * @brief 程序语句列表
         * @elseif en
         * @brief Program statement list
         * @endif
         */
        std::vector<Statement> statements;

        /**
         * @if zh
         * @brief 使用基类构造函数
         * @elseif en
         * @brief Use base class constructor
         * @endif
         */
        using AST::AST;

        /**
         * @if zh
         * @brief 获取结束位置
         * @return 程序结束位置
         * @elseif en
         * @brief Get end position
         * @return Program end position
         * @endif
         */
        [[nodiscard]] Pos end() const override;

        /**
         * @if zh
         * @brief 转换为字符串表示
         * @return 程序的可读字符串表示
         * @elseif en
         * @brief Convert to string representation
         * @return Human-readable string representation of program
         * @endif
         */
        [[nodiscard]] std::string toString() const override;

        /**
         * @if zh
         * @brief 转换为JSON格式
         * @return 程序的JSON格式字符串
         * @elseif en
         * @brief Convert to JSON format
         * @return JSON format string of program
         * @endif
         */
        [[nodiscard]] std::string toJSON() const override;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_HPP
