// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file symbol_table_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/22 21:52
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_SYMBOL_TABLE_ADAPTER_HPP
#define EXPORT_SYMBOL_TABLE_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/semantic/semantic.hpp"

namespace stationeers::ic10 {

    class SymbolTableAdapter : public node::ObjectWrap<SymbolTableAdapter> {
    public:
        SymbolTableAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

        static node::Value to(node::Env env, const SymbolTable& self);

        static SymbolTable from(const node::Object& obj);

    private:
        static node::FunctionReference constructor;

        SymbolTable symbolTable_;

        node::Value toJSON(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_SYMBOL_TABLE_ADAPTER_HPP
