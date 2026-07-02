// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file build.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @if zh
 * @brief IC10 Python绑定模块入口
 * @details 定义pybind11模块ic10_python的初始化函数,注册所有适配器。
 *          注册顺序: common(Pos/Error) -> IC10Local -> Token -> Lexer ->
 *          Program -> Parser -> SymbolTable -> Analyser
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 Python binding module entry
 * @details Defines pybind11 module ic10_python initialization function, registers all adapters.
 *          Registration order: common(Pos/Error) -> IC10Local -> Token -> Lexer ->
 *          Program -> Parser -> SymbolTable -> Analyser
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#include "common_python/build.hpp"
#include "analyser_adapter.hpp"
#include "ast_adapter.hpp"
#include "lexer_adapter.hpp"
#include "local_adapter.hpp"
#include "parser_adapter.hpp"
#include "symbol_table_adapter.hpp"
#include "token_adapter.hpp"

namespace ic = stationeers::ic10;

/**
 * @if zh
 * @brief pybind11模块入口宏
 * @details 定义名为ic10_python的Python扩展模块
 * @param m pybind11模块对象
 * @elseif en
 * @brief pybind11 module entry macro
 * @details Defines Python extension module named ic10_python
 * @param m pybind11 module object
 * @endif
 */
PYBIND11_MODULE(ic10_python, m) {
    // 模块文档字符串
    // Module docstring
    m.doc() = "IC10 compiler Python bindings (pybind11)";

    // 注册顺序: common -> IC10Local -> Token -> Lexer ->
    // Program -> Parser -> SymbolTable -> Analyser
    // Registration order: common -> IC10Local -> Token -> Lexer ->
    // Program -> Parser -> SymbolTable -> Analyser
    commonInit(m);
    ic::initLocal(m);
    ic::initToken(m);
    ic::initLexer(m);
    ic::initProgram(m);
    ic::initParser(m);
    ic::initSymbolTable(m);
    ic::initAnalyser(m);
}
