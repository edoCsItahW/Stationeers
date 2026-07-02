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
#include "local_adapter.hpp"
#include "ic10/locals/languages/en_us.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/locals/local.hpp"

namespace stationeers::ic10 {

    void initLocal(py::module_& m) {
        // 注册语言支持
        // Register language support
        IC10Local::registerLanguage<ZhHans>("zh-hans");
        IC10Local::registerLanguage<EnUs>("en-us");

        // 设置默认语言为美式英语
        // Set default language to American English
        IC10Local::setLanguage("en-us");

        py::class_<IC10Local>(m, "IC10Local")
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
                IC10Local::setLanguage(code);
            }, py::arg("code"));
    }

}  // namespace stationeers::ic10
