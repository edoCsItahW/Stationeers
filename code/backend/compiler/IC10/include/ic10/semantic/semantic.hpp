// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file semantic.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:28
 * @if zh
 * @brief IC10语义分析基础
 * @details 提供IC10编程语言语义分析的基础类型定义,包括类型枚举、符号表、符号定义等。
 *        支持基于Future/Promise的符号异步解析机制,处理前向引用。
 *
 * @note 语义分析是编译过程的**第三阶段**,位于语法分析之后
 *
 * @par 语义分析流程:
 * @code{.mermaid}
 * graph TD
 *     A[Program AST] --> B[Analyser]
 *     B --> C{遍历AST节点}
 *     C --> D[处理AliasDirective]
 *     C --> E[处理DefineDirective]
 *     C --> F[处理LabelDef]
 *     C --> G[处理Instruction]
 *     D --> H[defineSymbol]
 *     E --> H
 *     F --> H
 *     G --> I[resolveSymbol]
 *     H --> J[SymbolTable]
 *     I --> J
 *     J --> K{所有引用已解析?}
 *     K -->|是| L[分析完成]
 *     K -->|否| M[等待Future]
 * @endcode
 *
 * @par Type枚举值:
 * | 值 | 说明 | 示例 |
 * |:-----|:-----|:-----|
 * | `UNKNOWN` | 未知类型 | 解析错误时 |
 * | `INTEGER` | 整数类型 | `42`, `0xFF` |
 * | `FLOAT` | 浮点类型 | `3.14`, `-2.5` |
 * | `STRING` | 字符串类型 | `"Hello"` |
 * | `REGISTER` | 寄存器类型 | `r0`, `r1` |
 * | `DEVICE` | 设备类型 | `@Display` |
 * | `OTHER` | 其他类型 | 宏调用等 |
 *
 * @par 前向引用处理:
 * IC10支持前向引用,例如标签可以在定义之前被引用。SymbolTable使用Future/Promise模式处理这种情况:
 * 1. 当引用一个尚未定义的符号时,返回一个未决的Future
 * 2. 当符号被定义时,Promise被填充,所有等待的Future收到通知
 *
 * @warning 确保在分析结束时调用 `failAllPending()` 来处理未决符号
 *
 * @see Analyser 语义分析器
 * @see SymbolTable 符号表
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 semantic analysis basics
 * @details Provides basic type definitions for IC10 programming language semantic analysis,
 *        including type enum, symbol table, symbol definition, etc.
 *        Supports Future/Promise-based asynchronous symbol resolution mechanism for forward references.
 *
 * @note Semantic analysis is the **third phase** of the compilation process, after syntax analysis
 *
 * @par Semantic Analysis Flow:
 * @code{.mermaid}
 * graph TD
 *     A[Program AST] --> B[Analyser]
 *     B --> C{Traverse AST Nodes}
 *     C --> D[Process AliasDirective]
 *     C --> E[Process DefineDirective]
 *     C --> F[Process LabelDef]
 *     C --> G[Process Instruction]
 *     D --> H[defineSymbol]
 *     E --> H
 *     F --> H
 *     G --> I[resolveSymbol]
 *     H --> J[SymbolTable]
 *     I --> J
 *     J --> K{All References Resolved?}
 *     K -->|Yes| L[Analysis Complete]
 *     K -->|No| M[Wait for Future]
 * @endcode
 *
 * @par Type Enum Values:
 * | Value | Description | Example |
 * |:------|:------------|:--------|
 * | `UNKNOWN` | Unknown type | On parse error |
 * | `INTEGER` | Integer type | `42`, `0xFF` |
 * | `FLOAT` | Float type | `3.14`, `-2.5` |
 * | `STRING` | String type | `"Hello"` |
 * | `REGISTER` | Register type | `r0`, `r1` |
 * | `DEVICE` | Device type | `@Display` |
 * | `OTHER` | Other type | Macro calls etc. |
 *
 * @par Forward Reference Handling:
 * IC10 supports forward references, e.g. labels can be referenced before definition. SymbolTable uses Future/Promise pattern:
 * 1. When referencing an undefined symbol, return a pending Future
 * 2. When symbol is defined, Promise is filled, all waiting Futures are notified
 *
 * @warning Ensure to call `failAllPending()` at the end of analysis to handle pending symbols
 *
 * @see Analyser Semantic analyser
 * @see SymbolTable Symbol table
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_SEMANTIC_HPP
#define COMPILER_SEMANTIC_HPP
#pragma once

#include "ic10/pch/ast.hpp"
#include "pch/async.hpp"
#include <expected>
#include <unordered_map>

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @enum Type
     * @brief IC10类型枚举
     * @details 表示IC10中各种值的类型分类
     *
     * @elseif en
     *
     * @enum Type
     * @brief IC10 type enumeration
     * @details Represents the type classification of various values in IC10
     *
     * @endif
     */
    enum class Type { UNKNOWN, INTEGER, FLOAT, STRING, REGISTER, DEVICE, OTHER };

    /**
     * @if zh
     *
     * @struct Symbol
     * @brief 符号结构
     * @details 表示IC10程序中定义的符号,包括类型和名称
     *
     * @elseif en
     *
     * @struct Symbol
     * @brief Symbol structure
     * @details Represents a symbol defined in IC10 program, including type and name
     *
     * @endif
     */
    struct Symbol {
        /**
         * @if zh
         * @brief 符号类型
         * @elseif en
         * @brief Symbol type
         * @endif
         */
        Type type;

        /**
         * @if zh
         * @brief 符号名称
         * @elseif en
         * @brief Symbol name
         * @endif
         */
        std::string name;

        /**
         * @if zh
         *
         * @brief 转换为JSON格式
         * @return JSON格式的字符串
         *
         * @elseif en
         *
         * @brief Convert to JSON format
         * @return JSON format string
         *
         * @endif
         */
        [[nodiscard]] std::string toJSON() const;
    };

    /**
     * @if zh
     *
     * @struct type_of_node
     * @brief 节点类型萃取模板
     * @details 从AST节点类型萃取对应的Type枚举值
     *
     * @tparam T AST节点类型
     *
     * @elseif en
     *
     * @struct type_of_node
     * @brief Node type extraction template
     * @details Extracts corresponding Type enum value from AST node type
     *
     * @tparam T AST node type
     *
     * @endif
     */
    template<typename>
    struct type_of_node {
        /**
         * @if zh
         * @brief 默认类型值
         * @elseif en
         * @brief Default type value
         * @endif
         */
        static constexpr auto value = Type::OTHER;
    };

    template<>
    struct type_of_node<Integer> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<Float> {
        static constexpr auto value = Type::FLOAT;
    };

    template<>
    struct type_of_node<String> {
        static constexpr auto value = Type::STRING;
    };

    template<>
    struct type_of_node<StrCall> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<HashCall> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<Register> {
        static constexpr auto value = Type::REGISTER;
    };

    template<>
    struct type_of_node<Device> {
        static constexpr auto value = Type::DEVICE;
    };

    template<>
    struct type_of_node<LabelDef> {
        static constexpr auto value = Type::INTEGER;
    };

    template<>
    struct type_of_node<ErrorNode> {
        static constexpr auto value = Type::UNKNOWN;
    };

    template<typename T>
    inline constexpr auto type_of = type_of_node<T>::value;

    /**
     * @if zh
     *
     * @class SymbolTable
     * @brief 符号表类
     * @details 管理IC10程序中的符号定义和解析,支持前向引用的异步解析机制。
     *          使用Future/Promise模式处理符号的声明与定义分离。
     *
     * @par 使用方式:
     * @code
     * SymbolTable st;
     * // 定义符号
     * st.define("x", std::make_shared<Symbol>(Type::INTEGER, "x"));
     * // 查找符号(可前向引用)
     * auto& entry = st.resolve("y", pos);
     * auto sym = co_await entry.future;
     * @endcode
     *
     * @elseif en
     *
     * @class SymbolTable
     * @brief Symbol table class
     * @details Manages symbol definitions and resolution in IC10 programs, supporting asynchronous
     *          resolution mechanism for forward references.
     *          Uses Future/Promise pattern to handle declaration-definition separation of symbols.
     *
     * @par Usage:
     * @code
     * SymbolTable st;
     * // Define symbol
     * st.define("x", std::make_shared<Symbol>(Type::INTEGER, "x"));
     * // Resolve symbol (supports forward reference)
     * auto& entry = st.resolve("y", pos);
     * auto sym = co_await entry.future;
     * @endcode
     *
     * @endif
     */
    struct SymbolTable {
        /**
         * @if zh
         *
         * @struct Entry
         * @brief 符号表条目
         * @details 符号表中的单个条目,包含Promise、Future和位置信息
         *
         * @elseif en
         *
         * @struct Entry
         * @brief Symbol table entry
         * @details Single entry in symbol table, contains Promise, Future and position info
         *
         * @endif
         */
        struct Entry {
            /**
             * @if zh
             * @brief 符号Promise,用于设置符号值
             * @elseif en
             * @brief Symbol Promise, used to set symbol value
             * @endif
             */
            Promise<std::shared_ptr<Symbol>> promise;

            /**
             * @if zh
             * @brief 符号Future,用于获取符号值
             * @elseif en
             * @brief Symbol Future, used to retrieve symbol value
             * @endif
             */
            Future<std::shared_ptr<Symbol>> future;

            /**
             * @if zh
             * @brief 符号位置
             * @elseif en
             * @brief Symbol position
             * @endif
             */
            Pos pos;

            /**
             * @if zh
             *
             * @brief 检查符号是否已就绪
             * @return 如果符号已定义返回true
             *
             * @elseif en
             *
             * @brief Check if symbol is ready
             * @return true if symbol is defined
             *
             * @endif
             */
            [[nodiscard]] bool ready() const;
        };

    private:
        /**
         * @if zh
         * @brief 符号存储映射表
         * @elseif en
         * @brief Symbol storage map
         * @endif
         */
        std::unordered_map<std::string, Entry> symbols_;

    public:
        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        SymbolTable() = default;

        /**
         * @if zh
         *
         * @brief 定义符号
         * @details 如果符号不存在,则将符号加入符号表中。如果符号已存在,有两种情况:
         *          1. 重复定义,则返回错误信息(此情况适用RedefinitionError);
         *          2. 虽然已定义,但是定义的是一个未决Future,表明之前已经有程序引用了
         *             这个符号,但是还没有得到真正的定义。这种情况是合法的,那么将符号
         *             加入符号表中,并通知所有等待者。
         *
         * @param name 符号名
         * @param symbol 符号
         * @return 如果符号确已存在,则返回错误信息,否则返回空值。
         *
         * @elseif en
         *
         * @brief Define symbol
         * @details If the symbol does not exist, add it to the symbol table. If the symbol
         *          already exists, there are two cases:
         *          1. Redefinition, return error message (RedefinitionError applies);
         *          2. Already defined but as a pending Future, indicating the symbol was
         *             referenced before its actual definition. This case is legal, add the
         *             symbol to the table and notify all waiters.
         *
         * @param name Symbol name
         * @param symbol Symbol
         * @return Error message if symbol already exists, void otherwise.
         *
         * @endif
         */
        [[nodiscard]] std::expected<void, std::string> define(
            const std::string& name, const std::shared_ptr<Symbol>& symbol
        );

        /**
         * @if zh
         *
         * @brief 查找符号
         * @details 查找符号,如果符号不存在,则将该符号初始化为未决Future,并加入符号表中,
         *          然后返回(无论存不存在)该符号的Future。
         *
         * @param name 符号名
         * @param pos 引用位置
         * @return 符号表条目引用
         *
         * @elseif en
         *
         * @brief Resolve symbol
         * @details Resolves a symbol. If the symbol does not exist, initialize it as a pending
         *          Future and add it to the symbol table, then return the Future of the symbol
         *          (whether it existed or not).
         *
         * @param name Symbol name
         * @param pos Reference position
         * @return Symbol table entry reference
         *
         * @endif
         */
        template<typename Self>
        auto& resolve(this Self& self, std::string name, const Pos& pos);

        /**
         * @if zh
         *
         * @brief 检查是否包含指定符号
         * @param name 符号名
         * @return 如果包含返回true
         *
         * @elseif en
         *
         * @brief Check if contains specified symbol
         * @param name Symbol name
         * @return true if contains
         *
         * @endif
         */
        [[nodiscard]] bool contains(const std::string& name) const;

        /**
         * @if zh
         *
         * @brief 查找符号迭代器
         * @param name 符号名
         * @return 指向符号的迭代器
         *
         * @elseif en
         *
         * @brief Find symbol iterator
         * @param name Symbol name
         * @return Iterator pointing to symbol
         *
         * @endif
         */
        template<typename Self>
        auto find(this Self& self, std::string name);

        /**
         * @if zh
         *
         * @brief 获取起始迭代器
         * @return 起始迭代器
         *
         * @elseif en
         *
         * @brief Get begin iterator
         * @return Begin iterator
         *
         * @endif
         */
        template<typename Self>
        auto begin(this Self& self);

        /**
         * @if zh
         *
         * @brief 获取结束迭代器
         * @return 结束迭代器
         *
         * @elseif en
         *
         * @brief Get end iterator
         * @return End iterator
         *
         * @endif
         */
        template<typename Self>
        auto end(this Self& self);

        /**
         * @if zh
         *
         * @brief 转换为JSON格式
         * @return JSON格式的字符串
         *
         * @elseif en
         *
         * @brief Convert to JSON format
         * @return JSON format string
         *
         * @endif
         */
        [[nodiscard]] std::string toJSON() const;

        /**
         * @if zh
         *
         * @brief 标记所有未决符号为失败
         * @details 将所有仍处于未决状态的符号标记为失败,用于分析结束时清理
         *
         * @elseif en
         *
         * @brief Mark all pending symbols as failed
         * @details Marks all symbols still in pending state as failed, used for cleanup at end of analysis
         *
         * @endif
         */
        void failAllPending();
    };

}  // namespace stationeers::ic10

#include "semantic.inl"

#endif  // COMPILER_SEMANTIC_HPP
