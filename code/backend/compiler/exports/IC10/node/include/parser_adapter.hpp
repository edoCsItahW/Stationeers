// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file parser_adapter.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/18 19:05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORRT_PARSER_ADAPTER_HPP
#define EXPORRT_PARSER_ADAPTER_HPP
#pragma once

#include "common_node/base.hpp"
#include "ic10/parser/parser.hpp"

namespace stationeers::ic10 {

    class ParserAdapter : public node::ObjectWrap<ParserAdapter> {
    public:
        ParserAdapter(const node::CallbackInfo& info);

        static node::Object init(node::Env env, node::Object exports);

    private:
        Parser parser_;

        node::Value parse(const node::CallbackInfo& info);

        static node::Value parsing(const node::CallbackInfo& info);
    };

}  // namespace stationeers::ic10

#endif  // EXPORT_PARSER_ADAPTER_HPP
