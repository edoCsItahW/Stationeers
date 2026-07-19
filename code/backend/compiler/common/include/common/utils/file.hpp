// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file file.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:29
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_FILE_HPP
#define COMPILER_FILE_HPP
#pragma once

#include <expected>
#include <fstream>

namespace stationeers {

    std::expected<std::string, std::string> readFile(const std::string& filename);

    std::expected<void, std::string> writeFile(const std::string& content, const std::string& filename);

}  // namespace stationeers

#endif  // COMPILER_FILE_HPP
