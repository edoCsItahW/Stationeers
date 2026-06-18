// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file pos_adapter.hpp
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/17 13:26
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_POS_HPP
#define EXPORT_POS_HPP
#pragma once

#include "base.hpp"
#include "common/utils/position.hpp"


class PosAdapter : public node::ObjectWrap<PosAdapter> {
public:
    static node::Object init(node::Env env, node::Object exports);

    static node::Object to(node::Env env, const st::Pos& self);

    static st::Pos from(const node::Object& obj);

    PosAdapter(const node::CallbackInfo& info);

private:
    st::Pos pos_;

    static node::FunctionReference constructor;

    EXPORT_D_ATTR_GETTER(Line)

    EXPORT_D_ATTR_GETTER(Column)

    EXPORT_D_ATTR_GETTER(Offset)

    EXPORT_D_METHOD_VOID(newline)

    EXPORT_D_METHOD_VOID(next)

    EXPORT_D_METHOD_VOID(move)
};

#endif //EXPORT_POS_HPP
