// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_pos.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/02 11:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_java/j_pos.hpp"

namespace stationeers {

    JPos::jhybriddata JPos::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    jint JPos::getLine() noexcept { return pos_.line(); }

    jint JPos::getColumn() noexcept { return pos_.column(); }

    jint JPos::getOffset() noexcept { return pos_.offset(); }

    void JPos::newline() noexcept { pos_.newline(); }

    void JPos::next(jint byteValue) noexcept { pos_.next(byteValue); }

    void JPos::move(jint charOffset, jint byteOffset) noexcept { pos_.move(charOffset, byteOffset); }

    Pos JPos::from(java::alias_ref<javaobject> pos) { return pos->cthis()->pos_; }

    Pos JPos::from(java::alias_ref<java::JObject> obj) {
        auto jpos = java::dynamic_ref_cast<javaobject>(obj);

        return jpos->cthis()->pos_;
    }

    void JPos::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JPos::initHybrid),
            makeNativeMethod("getLine", JPos::getLine),
            makeNativeMethod("getColumn", JPos::getColumn),
            makeNativeMethod("getOffset", JPos::getOffset),
            makeNativeMethod("newline", JPos::newline),
            makeNativeMethod("next", JPos::next),
            makeNativeMethod("move", JPos::move),
        });
    }

}  // namespace stationeers
