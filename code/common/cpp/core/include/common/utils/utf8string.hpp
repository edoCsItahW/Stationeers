// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for
// commercial purposes is prohibited without the author's permission. If you have any questions
// or require permission, please contact the author: edocsitahw@qq.com

/**
 * @file utf8string.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_UTF8STRING_INL
#define COMPILER_UTF8STRING_INL
#pragma once

#ifdef _MSC_VER

    #pragma warning(push)
    #pragma warning(disable: 4244 4389)
    #include "common/utils/utf8str.hpp"
    #pragma warning(pop)

#else

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wconversion"    // C4244
    #pragma GCC diagnostic ignored "-Wsign-compare"  // C4389
    #include "common/utils/utf8str.hpp"
    #pragma GCC diagnostic pop

#endif  // _MSC_VER

#endif  // COMPILER_TYPE_MAP_INL
