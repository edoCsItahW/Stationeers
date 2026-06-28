// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file debug.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

#include "common/exception/debug.hpp"
#include <iostream>

namespace stationeers {

    // Console

    Console::Level Console::level = INFO;

    template<Console::Level L>
    void Console::debug(const std::string &message, bool newline) {
        if (L >= level) {
            if constexpr (L == INFO)
                std::cout << "[INFO] " << message;
            else if constexpr (L == WARNING)
                std::cout << "[WARNING] " << message;
            else
                std::cerr << "[ERROR] " << message;

            if (newline) std::cout << std::endl;
        }
    }

    template<Console::Level L>
    void Console::debug(std::string_view message, bool newline) {
        if (L >= level) {
            if constexpr (L == INFO)
                std::cout << "[INFO] " << message;
            else if constexpr (L == WARNING)
                std::cout << "[WARNING] " << message;
            else
                std::cerr << "[ERROR] " << message;

            if (newline) std::cout << std::endl;
        }
    }

    void Console::log(std::string_view message, bool newline) { debug<INFO>(message, newline); }

    void Console::warning(const std::string &message, bool newline) {
        debug<WARNING>(message, newline);
    }

    void Console::warning(std::string_view message, bool newline) {
        debug<WARNING>(message, newline);
    }

    void Console::error(const std::string &message, bool newline) {
        debug<ERROR>(message, newline);
    }

    void Console::error(std::string_view message, bool newline) { debug<ERROR>(message, newline); }

}  // namespace stationeers
