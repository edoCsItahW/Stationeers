// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file file.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:31
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "common/utils/file.hpp"
#include "common/locals/common_local.hpp"

namespace stationeers {

    std::expected<std::string, std::string> readFile(const std::string& filename) {
        if (std::ifstream file(filename, std::ios::binary | std::ios::ate); file) {
            const auto size = file.tellg();
            file.seekg(0);

            std::string content(size, '\0');
            if (file.read(content.data(), size)) return content;

            return std::unexpected(CLoc::msgFormat<CMsgId::CEF2_1>(filename));
        }

        return std::unexpected(CLoc::msgFormat<CMsgId::CEF1_1>(filename));
    }

    std::expected<void, std::string> writeFile(
        const std::string& content, const std::string& filename
    ) {
        if (std::ofstream file(filename); file) {
            file << content << '\n';

            return {};
        }

        return std::unexpected(CLoc::msgFormat<CMsgId::CEF3_1>(filename));
    }

}  // namespace stationeers
