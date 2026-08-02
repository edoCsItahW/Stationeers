// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_program.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_java/parser/j_program.hpp"

namespace stationeers::ic10 {

    JProgram::jhybriddata JProgram::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    java::local_ref<JPos::javaobject> JProgram::getStart() {
        return JPos::to(program_.position);
    }

    java::local_ref<JPos::javaobject> JProgram::getEnd() {
        return JPos::to(program_.end());
    }

    std::string JProgram::toString() { return program_.toString(); }

    std::string JProgram::toJSON() { return program_.toJSON(); }

    std::string JProgram::getNodeName(java::alias_ref<java::JClass>) { return std::string(Program::nodeName); }

    Program JProgram::from(java::alias_ref<javaobject> program) {
        return program->cthis()->program_;
    }

    Program JProgram::from(java::alias_ref<java::JObject> obj) {
        auto jprogram = java::dynamic_ref_cast<javaobject>(obj);

        return jprogram->cthis()->program_;
    }

    void JProgram::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JProgram::initHybrid),
            makeNativeMethod("getStart", JProgram::getStart),
            makeNativeMethod("getEnd", JProgram::getEnd),
            makeNativeMethod("toString", JProgram::toString),
            makeNativeMethod("toJSON", JProgram::toJSON),
            makeNativeMethod("getNodeName", JProgram::getNodeName),
        });
    }

}  // namespace stationeers::ic10
