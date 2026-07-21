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
 * @if zh
 * @brief 文件IO工具函数
 * @details 提供跨平台的文件读写功能,使用std::expected返回错误信息。
 *        支持文本文件的读取和写入操作。
 * @note 读取采用二进制模式以保证跨平台一致性,文件内容不做任何编码转换(假定调用方处理编码)
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief File IO utility functions
 * @details Provides cross-platform file read/write functionality, using std::expected to return error information.
 *        Supports text file reading and writing operations.
 * @note Uses binary mode for reading to ensure cross-platform consistency. File content is not
 *       converted (assumes the caller handles encoding)
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_FILE_HPP
#define COMPILER_FILE_HPP
#pragma once

#include <expected>
#include <fstream>

namespace stationeers {

    /**
     * @if zh
     *
     * @brief 读取文件内容
     * @param filename 文件路径
     * @return 文件内容字符串或错误信息
     *
     * @elseif en
     *
     * @brief Read file content
     * @param filename File path
     * @return File content string or error message
     *
     * @endif
     */
    std::expected<std::string, std::string> readFile(const std::string& filename);

    /**
     * @if zh
     *
     * @brief 写入文件内容
     * @param content 要写入的内容
     * @param filename 文件路径
     * @return 成功返回void,失败返回错误信息
     *
     * @elseif en
     *
     * @brief Write file content
     * @param content Content to write
     * @param filename File path
     * @return void on success, error message on failure
     *
     * @endif
     */
    std::expected<void, std::string> writeFile(const std::string& content, const std::string& filename);

}  // namespace stationeers

#endif  // COMPILER_FILE_HPP