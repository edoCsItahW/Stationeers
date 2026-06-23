// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file analyser_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/22 18:05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_ANALYSER_ADAPTER_HPP
#define EXPORT_ANALYSER_ADAPTER_HPP
#pragma once
#include "common_node/base.hpp"
#include "ic10/semantic/analyser.hpp"

namespace stationeers::ic10 {

    class AnalyserAdapter : public node::ObjectWrap<AnalyserAdapter> {
    public:
        AnalyserAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

    private:
        Analyser analyser_;

        node::Value visit(const node::CallbackInfo& info);

        static node::Value analyse(const node::CallbackInfo& info);

        node::Value getErrors(const node::CallbackInfo& info);

        node::Value getSymbolTable(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_ANALYSER_ADAPTER_HPP
