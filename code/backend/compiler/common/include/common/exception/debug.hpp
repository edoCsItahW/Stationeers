// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file debug.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:29
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_DEBUG_HPP
#define COMPILER_DEBUG_HPP
#pragma once

#include <string>


namespace stationeers {

    class Console {
    public:
        enum Level { NONE, INFO, WARNING, ERROR };

        static Level level;

        template<Level L = INFO>
        static void debug(const std::string& message, bool newline = true);

        template<Level L = INFO>
        static void debug(std::string_view message, bool newline = true);

        static void log(std::string_view message, bool newline = true);

        static void warning(const std::string& message, bool newline = true);

        static void warning(std::string_view message, bool newline = true);

        static void error(const std::string& message, bool newline = true);

        static void error(std::string_view message, bool newline = true);
    };

}  // namespace stationeers

#endif  // COMPILER_DEBUG_HPP
