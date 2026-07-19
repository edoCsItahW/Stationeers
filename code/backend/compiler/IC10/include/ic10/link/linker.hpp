// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file linker.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 12:57
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_LINKER_HPP
#define COMPILER_LINKER_HPP
#pragma once

#include "ic10/pch/ast.hpp"
#include "ic10/semantic/semantic.hpp"
#include "ic10/semantic/types.hpp"
#include <memory>

namespace stationeers::ic10 {

    class Linker {
    public:
        Linker() = default;

        void addUnit(Program program);

        void addUnit(std::string_view source);

        void addUnit(Program program, const std::string& path);

        void addUnit(std::string_view source, const std::string& path);

        SymbolTable& link();

        const std::vector<Diagnostic>& getDiagnostics() const;

        struct UnitInfo {
            std::string path;                                ///< 源文件路径 / Source file path
            const std::vector<Diagnostic>* diagnostics = nullptr;  ///< 该单元的诊断列表 / Diagnostics of this unit
        };

        const std::vector<UnitInfo>& getUnits() const;

    private:
        TypeTable typeTable_;

        SymbolTable symbolTable_;

        DiagnosticReporter<IC10MsgPack> reporter_;

        std::vector<Program> units_;

        std::vector<UnitInfo> unitInfos_;

        // 每个 unit 的独立 reporter，用于区分诊断来源。
        // Per-unit independent reporters, used to distinguish diagnostic sources.
        std::vector<std::unique_ptr<DiagnosticReporter<IC10MsgPack>>> unitReporters_;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_LINKER_HPP
