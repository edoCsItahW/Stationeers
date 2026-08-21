// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file local_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/locals/local_adapter.hpp"
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_compiler/locals/languages/en_us.hpp"
#include "ic10_compiler/pch/pch.hpp"

namespace stationeers::ic10 {

    void initLocal(py::module_& m) {
        // 注册语言支持
        // Register language support
        ICLoc::registerLanguage<ZhHans>("zh-hans");
        ICLoc::registerLanguage<EnUs>("en-us");

        // 设置默认语言为美式英语
        // Set default language to American English
        ICLoc::setLanguage("en-us");

        py::class_<IC10CompilerLocal>(m, "IC10CompilerLocal")
            /**
             * @if zh
             * @brief 设置当前语言
             * @param code 语言代码,支持"zh-hans"和"en-us"
             * @elseif en
             * @brief Set current language
             * @param code Language code, supports "zh-hans" and "en-us"
             * @endif
             */
            .def_static("setLanguage", [](const std::string& code) {
                ICLoc::setLanguage(code);
            }, py::arg("code"));
    }

}  // namespace stationeers::ic10
