// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10语法分析器
 * @details 提供IC10编程语言的语法分析功能,将词法标记序列解析为抽象语法树(AST)。
 *        支持各类指令(零元到六元)、标签定义、预处理指令等语法结构的解析。
 *
 * @note 语法分析是编译过程的**第二阶段**,位于词法分析之后
 * @note 本分析器采用**递归下降**解析器实现
 *
 * @par 语法分析流程:
 * ```mermaid
 * graph LR
 *     A[Token序列] --> B{Parser}
 *     B --> C{解析Statement}
 *     C -->|指令| D[解析ExecutableInstruction]
 *     C -->|标签| E[解析LabelDef]
 *     C -->|预处理| F[解析PreprocessorDirective]
 *     D --> G[指令类型判断]
 *     G -->|0操作数| H[NullaryInstruction]
 *     G -->|1操作数| I[UnaryInstruction]
 *     G -->|2操作数| J[BinaryInstruction]
 *     G -->|3操作数| K[TernaryInstruction]
 *     G -->|4操作数| L[QuaternaryInstruction]
 *     G -->|5操作数| M[QuinaryInstruction]
 *     G -->|6操作数| N[SenaryInstruction]
 *     H & I & J & K & L & M & N --> O[Program.statements]
 *     E --> O
 *     F --> O
 * ```
 *
 * @par 支持的语法结构:
 * | 结构 | 说明 | 示例 |
 * |:-----|:-----|:-----|
 * | 零元指令 | 无操作数的指令 | `yield`, `hcf` |
 * | 一元指令 | 1个操作数 | `j 10`, `sleep 1` |
 * | 二元指令 | 2个操作数 | `move r0 r1`, `abs r0 1` |
 * | 三元指令 | 3个操作数 | `add r0 r1 r2`, `get r0 @Display 0` |
 * | 四元指令 | 4个操作数 | `lerp r0 r1 r2 r3` |
 * | 五元指令 | 5个操作数 | `lbn r0 r1 r2 r3 r4` |
 * | 六元指令 | 6个操作数 | `lbns r0 r1 r2 r3 r4 r5` |
 * | 标签定义 | 冒号结尾的标识符 | `main:` |
 * | alias指令 | 设备别名定义 | `alias myDisp @Display` |
 * | define指令 | 常量定义 | `define BUFFER_SIZE 1024` |
 *
 * @attention 本解析器不执行语义分析,仅进行语法解析
 * @attention 错误恢复机制:遇到错误时,解析器会跳过当前标记并继续解析
 *
 * @see Lexer 词法分析器
 * @see Program AST根节点
 * @see Analyser 语义分析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 parser
 * @details Provides parsing functionality for IC10 programming language, parsing token sequence
 *        into Abstract Syntax Tree (AST). Supports parsing of various instructions (nullary to
 * senary), label definitions, preprocessor directives and other syntactic structures.
 *
 * @note Parsing is the **second phase** of the compilation process, after lexical analysis
 * @note This analyzer is implemented as a **recursive descent** parser
 *
 * @par Parsing Flow:
 * ```mermaid
 * graph LR
 *     A[Token Sequence] --> B{Parser}
 *     B --> C{Parse Statement}
 *     C -->|Instruction| D[Parse ExecutableInstruction]
 *     C -->|Label| E[Parse LabelDef]
 *     C -->|Preprocessor| F[Parse PreprocessorDirective]
 *     D --> G{Instruction Type}
 *     G -->|0 operands| H[NullaryInstruction]
 *     G -->|1 operand| I[UnaryInstruction]
 *     G -->|2 operands| J[BinaryInstruction]
 *     G -->|3 operands| K[TernaryInstruction]
 *     G -->|4 operands| L[QuaternaryInstruction]
 *     G -->|5 operands| M[QuinaryInstruction]
 *     G -->|6 operands| N[SenaryInstruction]
 *     H & I & J & K & L & M & N --> O[Program.statements]
 *     E --> O
 *     F --> O
 * ```
 *
 * @par Supported Syntax Structures:
 * | Structure | Description | Example |
 * |:----------|:------------|:--------|
 * | Nullary Instruction | No operands | `yield`, `hcf` |
 * | Unary Instruction | 1 operand | `j 10`, `sleep 1` |
 * | Binary Instruction | 2 operands | `move r0 r1`, `abs r0 1` |
 * | Ternary Instruction | 3 operands | `add r0 r1 r2`, `get r0 @Display 0` |
 * | Quaternary Instruction | 4 operands | `lerp r0 r1 r2 r3` |
 * | Quinary Instruction | 5 operands | `lbn r0 r1 r2 r3 r4` |
 * | Senary Instruction | 6 operands | `lbns r0 r1 r2 r3 r4 r5` |
 * | Label Definition | Identifier ending with colon | `main:` |
 * | alias Directive | Device alias definition | `alias myDisp @Display` |
 * | define Directive | Constant definition | `define BUFFER_SIZE 1024` |
 *
 * @attention This parser does not perform semantic analysis, only syntax parsing
 * @attention Error recovery: on error, parser skips current token and continues
 *
 * @see Lexer Lexical analyzer
 * @see Program AST root node
 * @see Analyser Semantic analyzer
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_PARSER_HPP
#define COMPILER_PARSER_HPP
#pragma once

#include "ic10_compiler/locals/local.hpp"
#include "ic10_compiler/pch/ast.hpp"
#include "ic10_compiler/pch/pch.hpp"
#include "node_parser.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace stationeers::ic10 {

    namespace detail {
        template<HasFirst... Ts>
        struct FirstLookaheadMatcher;
    }  // namespace detail

    // LabelDef

    template<>
    struct NodeParser<LabelDef> {
        static ShallowErrorable<LabelDef> parse(Parser& parser);
    };

    // AliasDirective

    template<>
    struct NodeParser<AliasDirective> {
        static AliasDirective parse(Parser& parser);
    };

    // DefineDirective

    template<>
    struct NodeParser<DefineDirective> {
        static DefineDirective parse(Parser& p);
    };

    /**
     * @class Parser
     * @if zh
     *
     * @brief IC10语法分析器类
     * @details 递归下降解析器,将词法标记序列解析为抽象语法树(AST)。
     *        支持零元到六元指令、标签定义、预处理指令等的解析。
     *
     * @par 使用示例:
     * ```cpp
     * #include "parser.hpp"
     * using namespace stationeers::ic10;
     *
     * // 1. 词法分析
     * auto tokens = Lexer::tokenize(source);
     *
     * // 2. 语法分析
     * Parser parser(tokens);
     * Program program = parser.parse();
     *
     * // 3. 遍历AST
     * for (const auto& stmt : program.statements) {
     *     std::cout << std::visit([](const auto& node) {
     *         return node.toString();
     *     }, stmt) << std::endl;
     * }
     * ```
     *
     * @par 解析结果:
     * - 成功: 返回包含所有语句的 Program 节点
     * - 失败: 程序中会包含 ErrorNode,可在后续语义分析阶段处理
     *
     * @important 调试模式: 设置 `debug_ = true` 可输出详细解析过程
     * @see Lexer::tokenize() 词法分析
     *
     * @elseif en
     *
     * @brief IC10 parser class
     * @details Recursive descent parser, parses token sequence into Abstract Syntax Tree (AST).
     *        Supports parsing of nullary to senary instructions, label definitions,
     *        preprocessor directives, etc.
     *
     * @par Usage example:
     * ```cpp
     * #include "parser.hpp"
     * using namespace stationeers::ic10;
     *
     * // 1. Lexical analysis
     * auto tokens = Lexer::tokenize(source);
     *
     * // 2. Syntax analysis
     * Parser parser(tokens);
     * Program program = parser.parse();
     *
     * // 3. Traverse AST
     * for (const auto& stmt : program.statements) {
     *     std::cout << std::visit([](const auto& node) {
     *         return node.toString();
     *     }, stmt) << std::endl;
     * }
     * ```
     *
     * @par Parsing Result:
     * - Success: Returns Program node containing all statements
     * - Failure: Program contains ErrorNode, can be handled in semantic analysis phase
     *
     * @important Debug mode: Set `debug_ = true` to output detailed parsing process
     * @see Lexer::tokenize() Lexical analysis
     *
     * @endif
     */
    class Parser {
    public:
        /**
         * @if zh
         * @brief 调试模式标志
         * @elseif en
         * @brief Debug mode flag
         * @endif
         */
        bool debug_ = false;

        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        Parser() = default;

        /**
         * @if zh
         *
         * @brief 构造函数
         * @param tokens 词法标记向量
         * @param debug 是否启用调试模式
         *
         * @elseif en
         *
         * @brief Constructor
         * @param tokens Token vector
         * @param debug Whether to enable debug mode
         *
         * @endif
         */
        Parser(const std::vector<std::shared_ptr<Token>>& tokens, bool debug = false);

        /**
         * @if zh
         *
         * @brief 解析整个程序
         * @return 解析后的Program节点
         *
         * @elseif en
         *
         * @brief Parse the whole program
         * @return Parsed Program node
         *
         * @endif
         */
        Program parse();

        /**
         * @if zh
         *
         * @brief 获取诊断列表
         * @details 返回语法分析过程中收集到的所有诊断信息（包含错误、警告、提示）
         * @return 诊断列表的常量引用
         *
         * @elseif en
         *
         * @brief Get the list of diagnostics
         * @details Returns all diagnostics collected during parsing (including errors, warnings,
         * info)
         * @return Const reference to the diagnostic list
         *
         * @endif
         */
        const std::vector<Diagnostic>& getDiagnostics() const noexcept {
            return reporter_.getDiagnostics();
        }

        /**
         * @if zh
         *
         * @brief 静态解析入口
         * @param tokens 词法标记向量
         * @param debug 是否启用调试模式
         * @return 解析后的Program节点
         *
         * @elseif en
         *
         * @brief Static parse entry point
         * @param tokens Token vector
         * @param debug Whether to enable debug mode
         * @return Parsed Program node
         *
         * @endif
         */
        static Program parsing(
            const std::vector<std::shared_ptr<Token>>& tokens, bool debug = false
        );

    private:
        /**
         * @if zh
         * @brief 当前标记索引
         * @elseif en
         * @brief Current token index
         * @endif
         */
        mutable std::size_t idx_ = 0;

        /**
         * @if zh
         * @brief 诊断报告器
         * @details 收集词法/语法分析过程中产生的错误、警告等诊断信息
         * @elseif en
         * @brief Diagnostic reporter
         * @details Collects errors, warnings and other diagnostic information during parsing
         * @endif
         */
        DiagnosticReporter<IC10CompilerMsgPack> reporter_;

        /**
         * @if zh
         * @brief 输入标记序列
         * @elseif en
         * @brief Input token sequence
         * @endif
         */
        std::vector<std::shared_ptr<Token>> tokens_{};

        /**
         * @if zh
         *
         * @brief 解析语句
         * @return 解析后的Statement
         *
         * @elseif en
         *
         * @brief Parse statement
         * @return Parsed Statement
         *
         * @endif
         */
        Statement parseStatement();

        /**
         * @if zh
         *
         * @brief 解析可执行指令
         * @param layer 递归层级
         * @return 解析后的ExecutableInstruction
         *
         * @elseif en
         *
         * @brief Parse executable instruction
         * @param layer Recursion level
         * @return Parsed ExecutableInstruction
         *
         * @endif
         */
        ExecutableInstruction parseExecutableInstruction();

        static std::optional<double> evaluateBuiltin(const std::string& name) noexcept;

        /**
         * @if zh
         *
         * @brief 检查是否在范围内
         * @return 如果还有标记则返回true
         *
         * @elseif en
         *
         * @brief Check if in range
         * @return true if there are still tokens
         *
         * @endif
         */
        bool inScope() const noexcept;

        /**
         * @if zh
         *
         * @brief 跳过当前标记
         *
         * @elseif en
         *
         * @brief Skip current token
         *
         * @endif
         */
        void skip() noexcept;

        /**
         * @if zh
         *
         * @brief 消费当前标记
         *
         * @elseif en
         *
         * @brief Consume current token
         *
         * @endif
         */
        void consume() const noexcept;

        /**
         * @if zh
         *
         * @brief 跳转到下一行
         *
         * @elseif en
         *
         * @brief Go to next line
         *
         * @endif
         */
        void gotoNextLine() const noexcept;

        /**
         * @if zh
         *
         * @brief 获取当前标记
         * @param consume 是否消费当前标记
         * @return 当前标记指针
         *
         * @elseif en
         *
         * @brief Get current token
         * @param consume Whether to consume current token
         * @return Current token pointer
         *
         * @endif
         */
        std::shared_ptr<Token> current(bool consume = false) const noexcept;

        std::shared_ptr<Token> peek(std::size_t offset = 1) const noexcept;

        /**
         * @if zh
         *
         * @brief 期望指定类型标记
         * @param type 期望的标记类型
         * @param skipWs 是否跳过空白
         * @param consume 是否消费标记
         * @return 匹配的标记指针
         *
         * @elseif en
         *
         * @brief Expect token of specified type
         * @param type Expected token type
         * @param skipWs Whether to skip whitespace
         * @param consume Whether to consume token
         * @return Matched token pointer
         *
         * @endif
         */
        std::shared_ptr<Token> expect(TokenType type, bool skipWs = true, bool consume = true);

        /**
         * @if zh
         *
         * @brief 解析操作数
         * @param layer 递归层级
         * @return 解析后的Operand
         *
         * @elseif en
         *
         * @brief Parse operand
         * @param layer Recursion level
         * @return Parsed Operand
         *
         * @endif
         */
        template<OperandType O>
        auto matchOperand();

        template<IsVariant Variant>
        auto matchVariant();

        template<HasFirst... Ts>
        auto match();

        template<HasFirst T>
        [[nodiscard]] bool matchFirst(auto& result) noexcept;

        template<HasFirst T, std::array Array>
        [[nodiscard]] bool matchArray(auto& result) noexcept;

        template<HasFirst T>
        [[nodiscard]] bool matchPredicate() noexcept;

        template<HasFirst T>
        [[nodiscard]] bool isMatch() noexcept;

        template<HasFirst... Ts>
        [[nodiscard]] bool isAnyMatch() noexcept;

        template<typename T>
        friend struct NodeParser;

        template<typename T>
        friend struct NodeParserDispatcher;
    };

}  // namespace stationeers::ic10

#include "parser.inl"

#endif  // COMPILER_PARSER_HPP
