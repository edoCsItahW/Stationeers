// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file j_local.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/08/02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_java/locals/j_local.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/locals/languages/en_us.hpp"
#include "ic10_compiler/locals/local.hpp"

namespace stationeers::ic10 {

    void JIC10Local::setLanguage(java::alias_ref<java::JClass>, std::string name) {
        IC10Local::setLanguage(name);
    }

    void JIC10Local::registerNatives() {
        IC10Local::registerLanguage<ZhHans>("zh-hans");
        IC10Local::registerLanguage<EnUs>("en-us");
        IC10Local::setLanguage("en-us");

        javaClassStatic()->registerNatives({
            makeNativeMethod("setLanguage", JIC10Local::setLanguage),
        });
    }

}  // namespace stationeers::ic10
