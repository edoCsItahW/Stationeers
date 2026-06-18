// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file locals.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/16 17:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "locals.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/locals/languages/en_us.hpp"
#include "ic10/locals/local.hpp"

namespace ic = st::ic10;

node::Value setLanguage(const node::CallbackInfo& info) {
    node::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        node::TypeError::New(env, "Expected a string argument").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    std::string lang = info[0].As<node::String>().Utf8Value();

    try {
        ic::IC10Local::setLanguage(lang);
    } catch (const std::exception& e) {
        node::Error::New(env, e.what()).ThrowAsJavaScriptException();
    }

    return env.Undefined();
}

void initLocals(node::Env& env, node::Object& exports) {
    ic::IC10Local::registerLanguage<ic::ZhHans>("zh-hans");
    ic::IC10Local::registerLanguage<ic::EnUs>("en-us");

    ic::IC10Local::setLanguage("en-us");

    exports.Set("setLanguage", node::Function::New(env, setLanguage));
}
