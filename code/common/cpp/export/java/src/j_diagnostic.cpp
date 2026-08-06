// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_diagnostic.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/08/02 17:42
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_java/j_diagnostic.hpp"
#include "common_java/j_pos.hpp"

namespace stationeers {

    java::local_ref<facebook::jni::JavaClass<JDiagnostic>::javaobject> JDiagnostic::to(
        const Diagnostic& d
    ) {
        static const auto cls = javaClassStatic();

        static const auto levelField = cls->getField<jint>("level");
        static const auto idField = cls->getField<jstring>("id");
        static const auto startField = cls->getField<JPos::javaobject>("start");
        static const auto endField = cls->getField<JPos::javaobject>("end");
        static const auto msgField = cls->getField<jstring>("message");

        auto obj = newInstance();

        obj->setFieldValue(levelField, static_cast<jint>(d.level));

        obj->setFieldValue(idField, java::make_jstring(d.id).get());

        // 使用 .get() 传递 raw jobject，与 idField/msgField 处理方式一致。
        // 不能将 JPos::to() 返回的临时 local_ref 赋值给 alias_ref，
        // 因为 alias_ref 不持有 JNI 引用，临时 local_ref 析构后 alias_ref 会变成悬空指针。
        obj->setFieldValue(startField, JPos::to(d.start).get());
        obj->setFieldValue(endField, JPos::to(d.end).get());

        obj->setFieldValue(msgField, facebook::jni::make_jstring(d.message).get());

        return obj;
    }

}  // namespace stationeers::ic10
