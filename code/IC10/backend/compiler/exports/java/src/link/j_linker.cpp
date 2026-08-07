// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_linker.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_java/link/j_linker.hpp"

namespace stationeers::ic10 {

    JLinker::jhybriddata JLinker::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    void JLinker::addUnitProgram(java::alias_ref<JProgram::javaobject> program, std::string path) {
        auto prog = JProgram::from(program);
        linker_.addUnit(std::move(prog), path);
    }

    void JLinker::addUnitProgramNoPath(java::alias_ref<JProgram::javaobject> program) {
        auto prog = JProgram::from(program);
        linker_.addUnit(std::move(prog));
    }

    void JLinker::addUnitSource(std::string source, std::string path) {
        linker_.addUnit(source, path);
    }

    void JLinker::addUnitSourceNoPath(std::string source) {
        linker_.addUnit(source);
    }

    java::local_ref<JSymbolTable::javaobject> JLinker::link() {
        auto& symbolTable = linker_.link();
        return JSymbolTable::to(symbolTable);
    }

    java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> JLinker::getDiagnostics() {
        auto diagnostics = linker_.getDiagnostics();
        auto size = diagnostics.size();
        auto result = java::JArrayClass<JDiagnostic::javaobject>::newArray(size);

        for (std::size_t i = 0; i < size; ++i) {
            result->setElement(i, JDiagnostic::to(diagnostics[i]).get());
        }

        return result;
    }

    java::local_ref<JTypeTable::javaobject> JLinker::getTypeTable() {
        return JTypeTable::to(linker_.getTypeTable());
    }

    void JLinker::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JLinker::initHybrid),
            makeNativeMethod("addUnitProgram", JLinker::addUnitProgram),
            makeNativeMethod("addUnitProgramNoPath", JLinker::addUnitProgramNoPath),
            makeNativeMethod("addUnitSource", JLinker::addUnitSource),
            makeNativeMethod("addUnitSourceNoPath", JLinker::addUnitSourceNoPath),
            makeNativeMethod("link", JLinker::link),
            makeNativeMethod("getDiagnostics", JLinker::getDiagnostics),
            makeNativeMethod("getTypeTable", JLinker::getTypeTable),
        });
    }

}  // namespace stationeers::ic10
