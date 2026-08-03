// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_symbol_table.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_java/semantic/j_symbol_table.hpp"

namespace stationeers::ic10 {

    JSymbolTable::jhybriddata JSymbolTable::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    std::string JSymbolTable::toJSON() { return symbolTable_.toJSON(); }

    SymbolTable JSymbolTable::from(java::alias_ref<javaobject> symbolTable) {
        return symbolTable->cthis()->symbolTable_;
    }

    SymbolTable JSymbolTable::from(java::alias_ref<java::JObject> obj) {
        auto jst = java::dynamic_ref_cast<javaobject>(obj);

        return jst->cthis()->symbolTable_;
    }

    void JSymbolTable::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JSymbolTable::initHybrid),
            makeNativeMethod("toJSON", JSymbolTable::toJSON),
        });
    }

}  // namespace stationeers::ic10
