// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file build.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/16 13:09
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/locals/local.hpp"
#include "token_adapter.hpp"
#include "lexer_adapter.hpp"
#include "locals.hpp"


node::Object Init(node::Env env, node::Object exports) {
    initLocals(env, exports);

    TokenAdapter::init(env, exports);

    LexerAdapter::init(env, exports);

    PosAdapter::init(env, exports);

    return exports;
}


NODE_API_MODULE(addon, Init)
