// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_type_table.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_java/semantic/j_type_table.hpp"

namespace stationeers::ic10 {

    JTypeTable::jhybriddata JTypeTable::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    std::string JTypeTable::toJSON() { return typeTable_.toJSON(); }

    TypeTable JTypeTable::from(java::alias_ref<javaobject> typeTable) {
        return typeTable->cthis()->typeTable_;
    }

    TypeTable JTypeTable::from(java::alias_ref<java::JObject> obj) {
        auto jtt = java::dynamic_ref_cast<javaobject>(obj);

        return jtt->cthis()->typeTable_;
    }

    void JTypeTable::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JTypeTable::initHybrid),
            makeNativeMethod("toJSON", JTypeTable::toJSON),
        });
    }

}  // namespace stationeers::ic10
