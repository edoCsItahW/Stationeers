// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_program.hpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @if zh
 * @brief IC10抽象语法树Program节点Java绑定
 * @details 提供IC10 AST根节点(Program)的JNI绑定，支持Java层访问AST。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 AST Program node Java binding
 * @details Provides JNI bindings for IC10 AST root node (Program), supporting Java layer access to AST.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_J_PROGRAM_HPP
#define COMPILER_J_PROGRAM_HPP
#pragma once

#include "common_java/base.hpp"
#include "common_java/j_pos.hpp"
#include "ic10/parser/ast/ast.hpp"

namespace stationeers::ic10 {

    class JProgram : public java::HybridClass<JProgram> {
    public:
        static constexpr auto kJavaDescriptor = "Lio/github/stationeers/ic10/Program;";

        Program program_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        java::local_ref<JPos::javaobject> getStart();

        java::local_ref<JPos::javaobject> getEnd();

        std::string toString();

        std::string toJSON();

        static std::string getNodeName(java::alias_ref<java::JClass>);

        static auto to(const Program& program);

        static Program from(java::alias_ref<javaobject> program);

        static Program from(java::alias_ref<java::JObject> obj);

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#include "j_program.inl"

#endif  // COMPILER_J_PROGRAM_HPP
