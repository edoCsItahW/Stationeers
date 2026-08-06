// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_analyser.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_java/semantic/j_analyser.hpp"
#include "common_java/j_diagnostic.hpp"

namespace stationeers::ic10 {

    namespace {

        /**
         * @if zh
         * @brief 阻塞执行协程任务
         * @details 阻塞等待协程任务完成，若协程抛出异常则重新抛出。
         *          用于将C++20协程(Task)转换为同步Java调用。
         * @param task 协程任务
         * @elseif en
         * @brief Block execute coroutine task
         * @details Blocks until coroutine task completes,
         *          rethrows if coroutine threw exception.
         *          Used to convert C++20 coroutine(Task) into synchronous Java call.
         * @param task Coroutine task
         * @endif
         */
        void runCoroutine(Task<> task) {
            auto result = task.getFuture().get();

            if (!result.has_value() && result.error()) {
                std::rethrow_exception(result.error());
            }
        }

    }  // namespace

    JAnalyser::jhybriddata JAnalyser::initHybrid(java::alias_ref<jhybridobject>) {
        return makeCxxInstance().release();
    }

    void JAnalyser::visit(java::alias_ref<JProgram::javaobject> program) {
        auto prog = JProgram::from(program);

        runCoroutine(analyser_.visit(prog));
    }

    java::local_ref<java::JArrayClass<JDiagnostic::javaobject>> JAnalyser::getDiagnostics() {
        auto diagnostics = analyser_.getDiagnostics();
        auto size = diagnostics.size();
        auto result = java::JArrayClass<JDiagnostic::javaobject>::newArray(size);

        for (std::size_t i = 0; i < size; ++i) {
            result->setElement(i, JDiagnostic::to(diagnostics[i]).get());
        }

        return result;
    }

    java::local_ref<JSymbolTable::javaobject> JAnalyser::getSymbolTable() {
        return JSymbolTable::to(analyser_.getSymbolTable());
    }

    java::local_ref<JTypeTable::javaobject> JAnalyser::getTypeTable() {
        return JTypeTable::to(analyser_.getTypeTable());
    }

    void JAnalyser::analyse(java::alias_ref<java::JClass>, java::alias_ref<JProgram::javaobject> program) {
        auto prog = JProgram::from(program);

        runCoroutine(Analyser::analyse(prog));
    }

    void JAnalyser::registerNatives() {
        registerHybrid({
            makeNativeMethod("initHybrid", JAnalyser::initHybrid),
            makeNativeMethod("visit", JAnalyser::visit),
            makeNativeMethod("getDiagnostics", JAnalyser::getDiagnostics),
            makeNativeMethod("getSymbolTable", JAnalyser::getSymbolTable),
            makeNativeMethod("getTypeTable", JAnalyser::getTypeTable),
            makeNativeMethod("analyse", JAnalyser::analyse),
        });
    }

}  // namespace stationeers::ic10
