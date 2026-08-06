// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file linker.cpp
 * @author edocsitahw
 * @version 1.3
 * @date 2026/07/19 12:58
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/link/linker.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/semantic/analyser.hpp"

namespace stationeers::ic10 {

    void Linker::addUnit(Program program) {
        units_.push_back(std::move(program));
        unitInfos_.emplace_back();
        unitReporters_.emplace_back();
    }

    void Linker::addUnit(std::string_view source) {
        auto token = Lexer::tokenize(source);

        units_.push_back(Parser::parsing(token));
        unitInfos_.emplace_back();
        unitReporters_.emplace_back();
    }

    void Linker::addUnit(Program program, const std::string& path) {
        addUnit(std::move(program));
        unitInfos_.back().path = path;
    }

    void Linker::addUnit(std::string_view source, const std::string& path) {
        addUnit(source);
        unitInfos_.back().path = path;
    }

    SymbolTable& Linker::link() {
        // 并发启动所有单元的分析任务
        // 单元间可能存在前向引用（单元1引用单元2定义的符号），
        // 顺序处理会导致死锁：单元1等待符号定义，但单元2尚未执行。
        // 并发启动后，单元1挂起等待 Future，单元2定义符号后唤醒单元1。
        //
        // 为支持按文件输出 MSVC 风格诊断，每个 unit 使用独立 reporter；
        // 处理完毕后合并到主 reporter_，并将诊断列表指针填入 unitInfos_ 供 getUnits() 查询。
        //
        // Each unit uses an independent reporter to enable per-file MSVC-style diagnostic
        // output. After processing, diagnostics are merged into the main reporter_; the
        // per-unit diagnostic list pointers are filled into unitInfos_ for getUnits() queries.
        std::size_t size = units_.size();
        std::vector<Task<>> tasks;
        tasks.reserve(size);
        std::vector<std::shared_ptr<Analyser>> analysers;
        analysers.reserve(size);

        // 确保 unitReporters_ 与 units_ 同步（支持 link() 重复调用）
        // Ensure unitReporters_ is in sync with units_ (supports repeated link() calls).
        if (unitReporters_.size() < size) unitReporters_.resize(size);

        for (std::size_t i = 0; i < size; ++i) {
            if (!unitReporters_[i])
                unitReporters_[i] = std::make_unique<DiagnosticReporter<IC10MsgPack>>();

            auto analyser = std::shared_ptr<Analyser>(
                new Analyser(typeTable_, symbolTable_, *unitReporters_[i], true)
            );

            analysers.push_back(analyser);

            tasks.push_back(analyser->visit(units_[i]));
        }

        // 等待所有 visit 完成
        // Wait for all visit tasks to complete.
        for (auto& task : tasks)
            (void)task.getFuture().get();

        // 所有单元处理完后，统一清理未决的 Future
        symbolTable_.failAllPending();

        // 合并各 unit 的诊断到主 reporter_，并填充 unitInfos_[i].diagnostics 指针
        // Merge each unit's diagnostics into the main reporter_, and fill unitInfos_[i].diagnostics pointer.
        for (std::size_t i = 0; i < size; ++i)
            if (unitReporters_[i]) {
                reporter_.mergeFrom(*unitReporters_[i]);
                unitInfos_[i].diagnostics = &unitReporters_[i]->getDiagnostics();
            }

        return symbolTable_;
    }

    const std::vector<Diagnostic>& Linker::getDiagnostics() const {
        return reporter_.getDiagnostics();
    }

    const std::vector<Linker::UnitInfo>& Linker::getUnits() const { return unitInfos_; }

    TypeTable& Linker::getTypeTable() { return typeTable_; }

}  // namespace stationeers::ic10
