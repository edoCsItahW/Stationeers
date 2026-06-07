// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file common.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:35
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common/utils/common.hpp"


namespace stationeers {

    bool isAsciiSpace(const unsigned char c) noexcept {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
    }

    bool isAsciiDigit(const unsigned char c) noexcept { return c >= '0' && c <= '9'; }

    bool isAsciiHexDigit(const unsigned char c) noexcept {
        return c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F';
    }

    bool isAsciiAlpha(const unsigned char c) noexcept {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
    }

    bool isAsciiBinDigit(const unsigned char c) noexcept { return c == '0' || c == '1'; }

    bool isAsciiAlnum(const unsigned char c) noexcept { return isAsciiAlpha(c) || isAsciiDigit(c); }

}  // namespace stationeers