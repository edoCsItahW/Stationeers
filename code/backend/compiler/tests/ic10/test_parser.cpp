// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_parser.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/07 23:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/lexer/lexer.hpp"


using namespace stationeers::ic10;

TEST(ParserTest, ParseAlias) {
    Loc::registerLanguage<ZhHans>("zh-hans");
    Loc::setLanguage("zh-hans");
    auto tokens = Lexer::tokenize("alias foo r0");
    Program ast = Parser::parsing(tokens);
    ASSERT_EQ(ast.statements.size(), 2);
    auto* alias = std::get_if<AliasDirective>(&ast.statements[0]);
    ASSERT_NE(alias, nullptr);
}