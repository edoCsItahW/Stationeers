// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_pos.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/02 11:51
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef EXPORT_J_POS_HPP
#define EXPORT_J_POS_HPP
#pragma once

#include "base.hpp"
#include "common/utils/position.hpp"

namespace stationeers {

    class JPos : public java::HybridClass<JPos> {
    public:
        static constexpr auto kJavaDescriptor = "Lcom/stationeers/ic10/Pos;";

        Pos pos_;

        static jhybriddata initHybrid(java::alias_ref<jhybridobject>);

        jint getLine();
        jint getColumn();
        jint getOffset();

        void newline();
        void next(jint byteValue);
        void move(jint charOffset, jint byteOffset);

        static auto to(const Pos& pos);

        static Pos from(java::alias_ref<javaobject> pos);

        static Pos from(java::alias_ref<java::JObject> obj);

        static void registerNatives();
    };

}  // namespace stationeers::ic10

#include "j_pos.inl"

#endif  // EXPORT_J_POS_HPP
