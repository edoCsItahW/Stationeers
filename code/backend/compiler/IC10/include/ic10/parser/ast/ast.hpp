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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_HPP
#define COMPILER_AST_HPP
#pragma once

#include "ast_senary_ins.hpp"

#include <vector>

namespace stationeers::ic10 {

    // Statement

    using Statement = Errorable<ExecutableInstruction, LabelDef, PreprocessorDirective>;

    // Program（实现于ast.cpp）

    struct Program : AST<Program> {
        static constexpr auto nodeName = FString{"Program"};

        std::vector<Statement> statements;

        using AST::AST;

        [[nodiscard]] Pos end() const override;

        [[nodiscard]] std::string toString() const override;

        [[nodiscard]] std::string toJSON() const override;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_HPP
