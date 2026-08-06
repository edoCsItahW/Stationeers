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
#include "ic10_compiler_node/incremental/inc_compiler_adapter.hpp"
#include "ic10_compiler_node/incremental/inc_parser_adapter.hpp"
#include "ic10_compiler_node/semantic/symbol_table_adapter.hpp"
#include "ic10_compiler_node/semantic/type_table_adapter.hpp"
#include "ic10_compiler_node/incremental/inc_lexer_adapter.hpp"
#include "ic10_compiler_node/semantic/analyser_adapter.hpp"
#include "ic10_compiler_node/parser/parser_adapter.hpp"
#include "ic10_compiler_node/locals/local_adapter.hpp"
#include "ic10_compiler_node/lexer/lexer_adapter.hpp"
#include "ic10_compiler_node/lexer/token_adapter.hpp"
#include "ic10_compiler_node/parser/ast_adapter.hpp"
#include "ic10_compiler_node/link/linker_adapter.hpp"
#include "ic10_compiler_node/semantic/type_of_node_adapter.hpp"
#include "common_node/build.hpp"


namespace ic = stationeers::ic10;

node::Object Init(node::Env env, node::Object exports) {
    commonInit(env, exports);

    ic::IC10LocalAdapter::init(env, exports);

    ic::TokenAdapter::init(env, exports);

    ic::LexerAdapter::init(env, exports);

    ic::ProgramAdapter::init(env, exports);

    ic::ParserAdapter::init(env, exports);

    ic::SymbolTableAdapter::init(env, exports);
    ic::TypeTableAdapter::init(env, exports);
    ic::AnalyserAdapter::init(env, exports);
    ic::LinkerAdapter::init(env, exports);
    ic::IncLexerAdapter::init(env, exports);
    ic::IncParserAdapter::init(env, exports);
    ic::IncCompilerAdapter::init(env, exports);

    // 导出 BasicType, TypeCategory 枚举和 TypeOfNode 映射
    (void)exports.Set("TypeOfNode", ic::exportTypeOfNode(env));

    return exports;
}


NODE_API_MODULE(addon, Init)
