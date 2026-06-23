// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file local_adapter.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/16 17:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "local_adapter.hpp"
#include "ic10/locals/languages/en_us.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/locals/local.hpp"

namespace ic = st::ic10;

namespace stationeers::ic10 {

    IC10LocalAdapter::IC10LocalAdapter(const node::CallbackInfo& info)
        : ObjectWrap(info) {}

    node::Object IC10LocalAdapter::init(node::Env env, node::Object exports) {
        IC10Local::registerLanguage<ZhHans>("zh-hans");
        IC10Local::registerLanguage<EnUs>("en-us");

        IC10Local::setLanguage("en-us");

        node::Function func =
            DefineClass(env, "IC10Local", {StaticMethod("setLanguage", &setLanguage)});

        node::FunctionReference* constructor = new node::FunctionReference();

        *constructor = node::Persistent(func);

        constructor->SuppressDestruct();

        (void)exports.Set("IC10Local", func);

        return exports;
    }

    void IC10LocalAdapter::setLanguage(const node::CallbackInfo& info) {
        Arguments args(info);

        IC10Local::setLanguage(args.getWithCheck<node::String>(0));
    }

}  // namespace stationeers::ic10
