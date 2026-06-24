// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file analyser.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:47
 * @if zh
 * @brief IC10语义分析器
 * @details 提供IC10编程语言的语义分析功能,遍历抽象语法树进行类型检查、符号解析、
 *        重定义检测等语义分析。使用访问者模式遍历AST节点。
 * @note 语义分析是编译过程的第三阶段
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 semantic analyser
 * @details Provides semantic analysis functionality for IC10 programming language, traversing AST
 *        for type checking, symbol resolution, redefinition detection and other semantic analysis.
 *        Uses visitor pattern to traverse AST nodes.
 * @note Semantic analysis is the third phase of the compilation process
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_ANALYSER_HPP
#define COMPILER_ANALYSER_HPP
#pragma once

#include "common/exception/error.hpp"
#include "semantic.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     *
     * @class Analyser
     * @brief IC10语义分析器类
     * @details 遍历抽象语法树(AST)执行语义分析,包括符号解析、类型检查、
     *          重定义检测等。使用访问者模式处理各种AST节点类型。
     *
     * @par 使用示例:
     * @code
     * Program program = Parser::parsing(tokens);
     * co_await Analyser::analyse(program);
     * @endcode
     *
     * @elseif en
     *
     * @class Analyser
     * @brief IC10 semantic analyser class
     * @details Traverses Abstract Syntax Tree (AST) to perform semantic analysis,
     *          including symbol resolution, type checking, redefinition detection, etc.
     *          Uses visitor pattern to handle various AST node types.
     *
     * @par Usage example:
     * @code
     * Program program = Parser::parsing(tokens);
     * co_await Analyser::analyse(program);
     * @endcode
     *
     * @endif
     */
    class Analyser : std::enable_shared_from_this<Analyser> {
    public:
        /**
         * @if zh
         *
         * @brief 静态分析入口
         * @param program 待分析的程序AST
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Static analysis entry point
         * @param program Program AST to analyze
         * @return Coroutine task
         *
         * @endif
         */
        static Task<> analyse(const Program& program);

        /**
         * @if zh
         *
         * @brief 获取符号表
         * @return 符号表引用
         *
         * @elseif en
         *
         * @brief Get symbol table
         * @return Symbol table reference
         *
         * @endif
         */
        SymbolTable& getSymbolTable();

        /**
         * @if zh
         *
         * @brief 获取错误列表
         * @return 错误向量引用
         *
         * @elseif en
         *
         * @brief Get error list
         * @return Error vector reference
         *
         * @endif
         */
        std::vector<Error>& getErrors();

        /**
         * @if zh
         *
         * @brief 访问程序节点
         * @param program 程序节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit program node
         * @param program Program node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> visit(const Program& program);

    private:
        /**
         * @if zh
         * @brief 符号表实例
         * @elseif en
         * @brief Symbol table instance
         * @endif
         */
        SymbolTable symbolTable_{};

        /**
         * @if zh
         * @brief 语义错误列表
         * @elseif en
         * @brief Semantic error list
         * @endif
         */
        std::vector<Error> errors_;

        /**
         * @if zh
         *
         * @brief 报告错误
         * @tparam T 错误类型
         * @tparam Args 构造参数类型包
         * @param args 构造错误的参数
         *
         * @elseif en
         *
         * @brief Report error
         * @tparam T Error type
         * @tparam Args Constructor argument type pack
         * @param args Arguments to construct error
         *
         * @endif
         */
        template<typename T, typename... Args>
        void reportError(Args&&... args);

        /**
         * @if zh
         *
         * @brief 解析符号
         * @param name 符号名
         * @param pos 引用位置
         * @return 符号指针的Future
         *
         * @elseif en
         *
         * @brief Resolve symbol
         * @param name Symbol name
         * @param pos Reference position
         * @return Future of symbol pointer
         *
         * @endif
         */
        Task<std::shared_ptr<Symbol>> resolveSymbol(const std::string& name, const Pos& pos);

        /**
         * @if zh
         *
         * @brief 定义符号
         * @param identifier 标识符节点
         * @param symbol 符号实例
         *
         * @elseif en
         *
         * @brief Define symbol
         * @param identifier Identifier node
         * @param symbol Symbol instance
         *
         * @endif
         */
        void defineSymbol(const Identifier& identifier, Symbol&& symbol);

        /**
         * @if zh
         *
         * @brief 访问标签定义节点
         * @param labelDef 标签定义节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit label definition node
         * @param labelDef Label definition node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const LabelDef& labelDef);

        /**
         * @if zh
         *
         * @brief 访问alias指令节点
         * @param aliasDirective alias指令节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit alias directive node
         * @param aliasDirective Alias directive node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const AliasDirective& aliasDirective);

        /**
         * @if zh
         *
         * @brief 访问define指令节点
         * @param defineDirective define指令节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit define directive node
         * @param defineDirective Define directive node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const DefineDirective& defineDirective);

        /**
         * @if zh
         *
         * @brief 访问通用指令节点
         * @tparam Ins 指令模板类型
         * @tparam V 指令名FString值
         * @tparam Args 操作数类型包
         * @param ins 指令节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit generic instruction node
         * @tparam Ins Instruction template type
         * @tparam V Instruction name FString value
         * @tparam Args Operand type pack
         * @param ins Instruction node
         * @return Coroutine task
         *
         * @endif
         */
        template<template<auto, typename...> class Ins, FString V, typename... Args>
        Task<> operator()(const Ins<V, Args...>& ins);

        /**
         * @if zh
         *
         * @brief 访问str宏调用节点
         * @param strCall str宏调用节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit str macro call node
         * @param strCall Str macro call node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const StrCall& strCall);

        /**
         * @if zh
         *
         * @brief 访问hash宏调用节点
         * @param hashCall hash宏调用节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit hash macro call node
         * @param hashCall Hash macro call node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const HashCall& hashCall);

        /**
         * @if zh
         *
         * @brief 访问常量节点
         * @param constant 常量节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit constant node
         * @param constant Constant node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Constant& constant);

        /**
         * @if zh
         *
         * @brief 访问设备节点
         * @param device 设备节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit device node
         * @param device Device node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Device& device);

        /**
         * @if zh
         *
         * @brief 访问寄存器节点
         * @param register_ 寄存器节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit register node
         * @param register_ Register node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Register& register_);

        /**
         * @if zh
         *
         * @brief 访问字符串节点
         * @param string 字符串节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit string node
         * @param string String node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const String& string);

        /**
         * @if zh
         *
         * @brief 访问标识符节点
         * @param identifier 标识符节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit identifier node
         * @param identifier Identifier node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Identifier& identifier);

        /**
         * @if zh
         *
         * @brief 访问二进制数节点
         * @param binaryNumber 二进制数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit binary number node
         * @param binaryNumber Binary number node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const BinaryNumber& binaryNumber);

        /**
         * @if zh
         *
         * @brief 访问十六进制数节点
         * @param hexNumber 十六进制数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit hexadecimal number node
         * @param hexNumber Hexadecimal number node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const HexNumber& hexNumber);

        /**
         * @if zh
         *
         * @brief 访问浮点数节点
         * @param float_ 浮点数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit float number node
         * @param float_ Float number node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Float& float_);

        /**
         * @if zh
         *
         * @brief 访问整数节点
         * @param integer 整数节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit integer node
         * @param integer Integer node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const Integer& integer);

        /**
         * @if zh
         *
         * @brief 访问错误节点
         * @param errorNode 错误节点
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Visit error node
         * @param errorNode Error node
         * @return Coroutine task
         *
         * @endif
         */
        Task<> operator()(const ErrorNode& errorNode);

        /**
         * @if zh
         *
         * @brief 泛型节点访问器
         * @tparam T 节点类型
         * @param node 节点引用
         * @return 协程任务
         *
         * @elseif en
         *
         * @brief Generic node visitor
         * @tparam T Node type
         * @param node Node reference
         * @return Coroutine task
         *
         * @endif
         */
        template<typename T>
        Task<> operator()(T&& node);
    };

}  // namespace stationeers::ic10

#include "analyser.inl"

#endif  // COMPILER_ANALYSER_HPP
