// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/18 12:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_AST_ADAPTER_HPP
#define EXPORT_AST_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/parser/ast/ast.hpp"

namespace stationeers::ic10 {

    class ProgramAdapter : public node::ObjectWrap<ProgramAdapter> {
    public:
        ProgramAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

        static node::Object to(node::Env env, const Program& self);

        static Program from(const node::Object& obj);

    private:
        Program program_;

        static node::FunctionReference constructor;

        static node::Value getNodeName(const node::CallbackInfo& info);

        node::Value getStatements(const node::CallbackInfo& info);

        node::Value end(const node::CallbackInfo& info);

        node::Value toString(const node::CallbackInfo& info);

        node::Value toJSON(const node::CallbackInfo& info);

    };

}  // namespace stationeers::ic10

#endif  // EXPORT_AST_ADAPTER_HPP
