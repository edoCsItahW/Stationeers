// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file test_lexer.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/07 15:48
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>
#include "ic10/lexer/lexer.hpp"
#include "ic10/locals/languages/zh_hans.hpp"

using namespace stationeers::ic10;


TEST(LexerTest, EmptyInput) {
    Loc::registerLanguage<ZhHans>("zh-hans");
    Loc::setLanguage("zh-hans");
    auto tokens = Lexer::tokenize("123");
    ASSERT_EQ(tokens.size(), 2); // 123 + END
    EXPECT_EQ(tokens[0]->type, TokenType::INTEGER);
    EXPECT_EQ(tokens[0]->lexeme, "123");
}