// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file build.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/16 13:09
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common_node/build.hpp"
#include "ast_adapter.hpp"
#include "lexer_adapter.hpp"
#include "local_adapter.hpp"
#include "parser_adapter.hpp"
#include "token_adapter.hpp"
#include "symbol_table_adapter.hpp"
#include "analyser_adapter.hpp"

namespace ic = stationeers::ic10;

node::Object Init(node::Env env, node::Object exports) {
    commonInit(env, exports);

    ic::IC10LocalAdapter::init(env, exports);

    ic::TokenAdapter::init(env, exports);

    ic::LexerAdapter::init(env, exports);

    ic::ProgramAdapter::init(env, exports);

    ic::ParserAdapter::init(env, exports);

    ic::SymbolTableAdapter::init(env, exports);

    ic::AnalyserAdapter::init(env, exports);

    return exports;
}


NODE_API_MODULE(addon, Init)
