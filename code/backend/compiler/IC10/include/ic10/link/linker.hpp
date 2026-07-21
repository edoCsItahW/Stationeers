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
 * @if zh
 * @brief IC10链接器
 * @details 将多个编译单元的符号表合并为统一的全局符号表，执行跨单元的符号解析和类型检查。
 *          链接器是编译流程的最后阶段，负责整合多个源文件的编译结果。
 *
 * @note 链接器是编译流程的最后阶段，在语义分析之后执行。
 *
 * @par 链接流程:
 * 1. 逐个处理每个编译单元，提取符号表和类型信息
 * 2. 合并各单元的类型表到全局类型表
 * 3. 合并各单元的符号表到全局符号表，处理重复定义冲突
 * 4. 对所有单元进行跨单元符号解析和类型验证
 * 5. 收集并合并所有诊断信息
 *
 * @par 使用方式:
 * @code
 * Linker linker;
 * // 添加已编译的Program
 * linker.addUnit(program1, "file1.ic");
 * linker.addUnit(program2, "file2.ic");
 * // 或者添加源代码字符串
 * linker.addUnit(sourceCode, "file3.ic");
 * // 执行链接
 * SymbolTable& globalSymbols = linker.link();
 * // 获取所有诊断信息
 * auto diagnostics = linker.getDiagnostics();
 * @endcode
 *
 * @see Program 编译单元的AST表示
 * @see SymbolTable 符号表
 * @see DiagnosticReporter 诊断报告器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 Linker
 * @details Merges symbol tables from multiple compilation units into a unified global symbol table,
 *          performing cross-unit symbol resolution and type checking. The linker is the final
 *          stage of the compilation pipeline, responsible for integrating compilation results
 *          from multiple source files.
 *
 * @note The linker is the final stage of compilation, executed after semantic analysis.
 *
 * @par Linking Flow:
 * 1. Process each compilation unit, extract symbol table and type information
 * 2. Merge type tables from all units into global type table
 * 3. Merge symbol tables from all units into global symbol table, handle duplicate definition conflicts
 * 4. Perform cross-unit symbol resolution and type validation for all units
 * 5. Collect and merge all diagnostic information
 *
 * @par Usage:
 * @code
 * Linker linker;
 * // Add compiled Programs
 * linker.addUnit(program1, "file1.ic");
 * linker.addUnit(program2, "file2.ic");
 * // Or add source code strings
 * linker.addUnit(sourceCode, "file3.ic");
 * // Perform linking
 * SymbolTable& globalSymbols = linker.link();
 * // Get all diagnostics
 * auto diagnostics = linker.getDiagnostics();
 * @endcode
 *
 * @see Program AST representation of compilation unit
 * @see SymbolTable Symbol table
 * @see DiagnosticReporter Diagnostic reporter
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_LINKER_HPP
#define COMPILER_LINKER_HPP
#pragma once

#include "ic10/pch/ast.hpp"
#include "ic10/semantic/semantic.hpp"
#include "ic10/semantic/types.hpp"
#include "ic10/locals/local.hpp"
#include "common/exception/diagnostic.hpp"
#include <memory>

namespace stationeers::ic10 {

    /**
     * @if zh
     * @class Linker
     * @brief 多编译单元链接器
     * @details 将多个IC10源文件的编译结果进行链接，合并符号表，执行跨单元的符号解析和类型检查。
     *          支持两种添加单元的方式：直接添加已编译的Program对象，或添加源代码字符串自动编译。
     *
     * @note 链接器内部维护独立的诊断报告器，用于区分不同单元的诊断来源。
     *
     * @elseif en
     * @class Linker
     * @brief Multi-unit linker
     * @details Links compilation results from multiple IC10 source files, merges symbol tables,
     *          and performs cross-unit symbol resolution and type checking. Supports two ways
     *          of adding units: directly adding compiled Program objects, or adding source code
     *          strings which will be compiled automatically.
     *
     * @note The linker maintains independent diagnostic reporters internally to distinguish
     *       diagnostic sources from different units.
     *
     * @endif
     */
    class Linker {
    public:
        /**
         * @if zh
         * @brief 默认构造函数
         *
         * @elseif en
         * @brief Default constructor
         *
         * @endif
         */
        Linker() = default;

        /**
         * @if zh
         * @brief 添加编译单元（已编译的Program）
         * @details 将已完成词法和语法分析的Program对象添加到链接器。
         *          链接时会对该单元进行语义分析和符号提取。
         *
         * @param program 已解析的Program AST对象
         *
         * @elseif en
         * @brief Add compilation unit (compiled Program)
         * @details Adds a Program object that has completed lexical and syntactic analysis
         *          to the linker. Semantic analysis and symbol extraction will be performed
         *          during linking.
         *
         * @param program Parsed Program AST object
         *
         * @endif
         */
        void addUnit(Program program);

        /**
         * @if zh
         * @brief 添加编译单元（源代码字符串）
         * @details 将源代码字符串添加到链接器，链接时自动进行完整的编译流程（词法分析→语法分析→语义分析）。
         *
         * @param source IC10源代码字符串
         *
         * @elseif en
         * @brief Add compilation unit (source code string)
         * @details Adds source code string to the linker, which will automatically perform
         *          the complete compilation pipeline (lexing → parsing → semantic analysis)
         *          during linking.
         *
         * @param source IC10 source code string
         *
         * @endif
         */
        void addUnit(std::string_view source);

        /**
         * @if zh
         * @brief 添加编译单元（已编译的Program，带路径）
         * @details 将已完成词法和语法分析的Program对象添加到链接器，并指定源文件路径。
         *          路径信息用于诊断消息的定位。
         *
         * @param program 已解析的Program AST对象
         * @param path 源文件路径，用于诊断消息定位
         *
         * @elseif en
         * @brief Add compilation unit (compiled Program with path)
         * @details Adds a Program object that has completed lexical and syntactic analysis
         *          to the linker, with the source file path specified. Path information is
         *          used for diagnostic message location.
         *
         * @param program Parsed Program AST object
         * @param path Source file path for diagnostic location
         *
         * @endif
         */
        void addUnit(Program program, const std::string& path);

        /**
         * @if zh
         * @brief 添加编译单元（源代码字符串，带路径）
         * @details 将源代码字符串添加到链接器，并指定源文件路径。链接时自动进行完整的编译流程。
         *          路径信息用于诊断消息的定位。
         *
         * @param source IC10源代码字符串
         * @param path 源文件路径，用于诊断消息定位
         *
         * @elseif en
         * @brief Add compilation unit (source code string with path)
         * @details Adds source code string to the linker with the source file path specified.
         *          The complete compilation pipeline will be performed automatically during linking.
         *          Path information is used for diagnostic message location.
         *
         * @param source IC10 source code string
         * @param path Source file path for diagnostic location
         *
         * @endif
         */
        void addUnit(std::string_view source, const std::string& path);

        /**
         * @if zh
         * @brief 执行链接操作
         * @details 对所有已添加的编译单元执行完整的链接流程：
         *          1. 对每个单元执行语义分析（如果是源代码形式）
         *          2. 合并所有单元的类型表和符号表
         *          3. 执行跨单元符号解析和类型验证
         *          4. 收集所有诊断信息
         *
         * @note 链接操作是幂等的，多次调用返回相同的全局符号表。
         *       但在调用后继续添加新单元会导致下次调用重新执行完整链接。
         *
         * @return 合并后的全局符号表引用
         *
         * @elseif en
         * @brief Perform linking
         * @details Executes the complete linking flow for all added compilation units:
         *          1. Perform semantic analysis for each unit (if in source code form)
         *          2. Merge type tables and symbol tables from all units
         *          3. Perform cross-unit symbol resolution and type validation
         *          4. Collect all diagnostic information
         *
         * @note The linking operation is idempotent, multiple calls return the same
         *       global symbol table. However, adding new units after linking will cause
         *       a full re-link on the next call.
         *
         * @return Reference to the merged global symbol table
         *
         * @endif
         */
        SymbolTable& link();

        /**
         * @if zh
         * @brief 获取所有诊断信息
         * @details 返回所有编译单元在链接过程中产生的诊断信息的合并列表。
         *          诊断信息包括词法分析、语法分析和语义分析阶段的错误和警告。
         *
         * @warning 此方法返回的是所有单元诊断的合并视图，不区分来源单元。
         *          如果需要按单元区分，请使用 @ref getUnits() 方法。
         *
         * @return 所有诊断信息的常量引用
         *
         * @elseif en
         * @brief Get all diagnostics
         * @details Returns a merged list of diagnostic information generated during
         *          linking from all compilation units. Diagnostics include errors and
         *          warnings from lexing, parsing, and semantic analysis phases.
         *
         * @warning This method returns a merged view of all unit diagnostics without
         *          distinguishing source units. Use @ref getUnits() if you need unit-specific
         *          diagnostics.
         *
         * @return Const reference to all diagnostic information
         *
         * @endif
         */
        const std::vector<Diagnostic>& getDiagnostics() const;

        /**
         * @if zh
         * @struct UnitInfo
         * @brief 编译单元信息
         * @details 包含单个编译单元的路径和诊断信息，用于区分不同单元的诊断来源。
         *
         * @elseif en
         * @struct UnitInfo
         * @brief Compilation unit information
         * @details Contains path and diagnostic information for a single compilation unit,
         *          used to distinguish diagnostic sources from different units.
         *
         * @endif
         */
        struct UnitInfo {
            /** @if zh @brief 源文件路径 @else @brief Source file path @endif */
            std::string path;

            /** @if zh @brief 该单元的诊断列表 @else @brief Diagnostics of this unit @endif */
            const std::vector<Diagnostic>* diagnostics = nullptr;
        };

        /**
         * @if zh
         * @brief 获取所有编译单元信息
         * @details 返回所有已添加的编译单元的信息列表，包括每个单元的路径和独立的诊断信息。
         *
         * @return 所有编译单元信息的常量引用
         *
         * @elseif en
         * @brief Get all compilation units information
         * @details Returns a list of information for all added compilation units, including
         *          path and independent diagnostic information for each unit.
         *
         * @return Const reference to all compilation unit information
         *
         * @endif
         */
        const std::vector<UnitInfo>& getUnits() const;

    private:
        /** @if zh @brief 全局类型表，存储所有单元的类型定义 @else @brief Global type table storing type definitions from all units @endif */
        TypeTable typeTable_;

        /** @if zh @brief 全局符号表，存储所有单元的符号定义 @else @brief Global symbol table storing symbol definitions from all units @endif */
        SymbolTable symbolTable_;

        /** @if zh @brief 主诊断报告器，收集所有单元的合并诊断信息 @else @brief Main diagnostic reporter collecting merged diagnostics from all units @endif */
        DiagnosticReporter<IC10MsgPack> reporter_;

        /** @if zh @brief 已添加的编译单元列表 @else @brief List of added compilation units @endif */
        std::vector<Program> units_;

        /** @if zh @brief 各单元的信息（路径、诊断） @else @brief Information for each unit (path, diagnostics) @endif */
        std::vector<UnitInfo> unitInfos_;

        /**
         * @if zh
         * @brief 每个单元的独立诊断报告器
         * @details 用于区分不同单元的诊断来源，确保错误消息能够精确定位到源文件。
         *
         * @elseif en
         * @brief Per-unit independent diagnostic reporters
         * @details Used to distinguish diagnostic sources from different units, ensuring
         *          error messages can be accurately located to source files.
         *
         * @endif
         */
        std::vector<std::unique_ptr<DiagnosticReporter<IC10MsgPack>>> unitReporters_;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_LINKER_HPP
