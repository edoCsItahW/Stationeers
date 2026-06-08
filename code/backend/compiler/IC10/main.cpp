// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file main.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/06/05 17:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

#include "ic10/lexer/lexer.hpp"
#include "ic10/locals/languages/zh_hans.hpp"
#include "ic10/parser/parser.hpp"
#include "ic10/semantic/analyser.hpp"
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>


void ouputFile(const std::filesystem::path& path) {
    std::ifstream file(path);

    // 存在性检查
    if (!std::filesystem::exists(path)) {
        std::cout << "文件不存在" << std::endl;
        return;
    }

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) std::cout << line << std::endl;
        file.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

auto readFile(const std::string& filename) -> std::expected<std::string, std::string> {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file) return std::unexpected("无法打开文件");

    const auto size = file.tellg();
    file.seekg(0);

    std::string content(size, '\0');

    if (!file.read(content.data(), size)) return std::unexpected("读取文件失败");

    return content;
}


int main() {
    using namespace stationeers::ic10;

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    system("chcp 65001>nul");  // 解决中文乱码问题

    Loc::registerLanguage<ZhHans>("zh-hans");
    Loc::setLanguage("zh-hans");

    const auto src = readFile(R"(E:\code\IC10\compiler\test\grammarTest.ic)");
    // const auto src = readFile(R"(E:\code\IC10\compiler\test\specialTest.ic)");

    const auto tokens = Lexer::tokenize(src.value());

    const auto ast = Parser::parsing(tokens);

    Analyser analyser;
    auto task = analyser.visit(ast);

    (void)task.getFuture().get();

    std::cout << analyser.getSymbolTable().toJSON() << std::endl;

    for (auto& error : analyser.getErrors())
        std::cout << error << std::endl;
}