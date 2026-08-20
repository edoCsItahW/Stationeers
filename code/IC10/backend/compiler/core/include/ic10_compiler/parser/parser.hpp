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
 *        into Abstract Syntax Tree (AST). Supports parsing of various instructions (nullary to senary),
 *        label definitions, preprocessor directives and other syntactic structures.
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

#include "../lexer/token.hpp"
#include "ic10_compiler/locals/local.hpp"
#include "ic10_compiler/parser/ast/ast.hpp"
#include "common/exception/diagnostic.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace stationeers::ic10 {

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
        Parser(const std::vector<std::shared_ptr<Token>> &tokens, bool debug = false);

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
         * @details Returns all diagnostics collected during parsing (including errors, warnings, info)
         * @return Const reference to the diagnostic list
         *
         * @endif
         */
        const std::vector<Diagnostic>& getDiagnostics() const { return reporter_.getDiagnostics(); }

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
            const std::vector<std::shared_ptr<Token>> &tokens, bool debug = false
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
         * @struct TypeHintResult
         * @brief 类型提示解析结果
         * @details 存储 @type 和 @desc 标签的解析结果
         *
         * @elseif en
         *
         * @struct TypeHintResult
         * @brief Type hint parse result
         * @details Stores parsed @type and @desc tag values
         *
         * @endif
         */
        struct TypeHintResult {
            std::optional<std::string> type;  ///< @type 指定的类型名
            std::optional<std::string> desc;  ///< @desc 指定的描述（与 type 值互斥）
        };

        /**
         * @if zh
         *
         * @brief 解析类型提示标签
         * @param lexeme token 的 lexeme（包含 "#:" 前缀）
         * @return 解析结果，包含 type 和 desc 字段
         *
         * @elseif en
         *
         * @brief Parse type hint tags
         * @param lexeme Token lexeme (includes "#:" prefix)
         * @return Parse result with type and desc fields
         *
         * @endif
         */
        TypeHintResult parseTypeHintTags(const std::string& lexeme);

        /**
         * @if zh
         * @brief 解析标签定义
         * @param layer 递归层级,用于调试输出
         * @return 解析后的LabelDef
         *
         * @elseif en
         *
         * @brief Parse label definition
         * @param layer Recursion level, for debug output
         * @return Parsed LabelDef
         *
         * @endif
         */
        ShallowErrorable<LabelDef> parseLabelDef(int layer);

        /**
         * @if zh
         *
         * @brief 解析预处理指令
         * @param layer 递归层级
         * @return 解析后的PreprocessorDirective
         *
         * @elseif en
         *
         * @brief Parse preprocessor directive
         * @param layer Recursion level
         * @return Parsed PreprocessorDirective
         *
         * @endif
         */
        PreprocessorDirective parsePreprocessorDirective(int layer);

        /**
         * @if zh
         *
         * @brief 解析alias指令
         * @param layer 递归层级
         * @return 解析后的AliasDirective
         *
         * @elseif en
         *
         * @brief Parse alias directive
         * @param layer Recursion level
         * @return Parsed AliasDirective
         *
         * @endif
         */
        ShallowErrorable<AliasDirective> parseAliasDirective(int layer);

        /**
         * @if zh
         *
         * @brief 解析define指令
         * @param layer 递归层级
         * @return 解析后的DefineDirective
         *
         * @elseif en
         *
         * @brief Parse define directive
         * @param layer Recursion level
         * @return Parsed DefineDirective
         *
         * @endif
         */
        ShallowErrorable<DefineDirective> parseDefineDirective(int layer);

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
        ExecutableInstruction parseExecutableInstruction(int layer);

        /**
         * @if zh
         *
         * @brief 解析零元指令
         * @param layer 递归层级
         * @return 解析后的NullaryInstruction
         *
         * @elseif en
         *
         * @brief Parse nullary instruction
         * @param layer Recursion level
         * @return Parsed NullaryInstruction
         *
         * @endif
         */
        NullaryInstruction parseNullaryInstruction(int layer);

        /**
         * @if zh
         *
         * @brief 解析一元指令
         * @param layer 递归层级
         * @return 解析后的UnaryInstruction
         *
         * @elseif en
         *
         * @brief Parse unary instruction
         * @param layer Recursion level
         * @return Parsed UnaryInstruction
         *
         * @endif
         */
        UnaryInstruction parseUnaryInstruction(int layer);

        /**
         * @if zh
         *
         * @brief 解析二元指令
         * @param layer 递归层级
         * @return 解析后的BinaryInstruction
         *
         * @elseif en
         *
         * @brief Parse binary instruction
         * @param layer Recursion level
         * @return Parsed BinaryInstruction
         *
         * @endif
         */
        BinaryInstruction parseBinaryInstruction(int layer);

        /**
         * @if zh
         *
         * @brief 解析三元指令
         * @param layer 递归层级
         * @return 解析后的TernaryInstruction
         *
         * @elseif en
         *
         * @brief Parse ternary instruction
         * @param layer Recursion level
         * @return Parsed TernaryInstruction
         *
         * @endif
         */
        TernaryInstruction parseTernaryInstruction(int layer);

        /**
         * @if zh
         *
         * @brief 解析四元指令
         * @param layer 递归层级
         * @return 解析后的QuaternaryInstruction
         *
         * @elseif en
         *
         * @brief Parse quaternary instruction
         * @param layer Recursion level
         * @return Parsed QuaternaryInstruction
         *
         * @endif
         */
        QuaternaryInstruction parseQuaternaryInstruction(int layer);

        /**
         * @if zh
         *
         * @brief 解析五元指令
         * @param layer 递归层级
         * @return 解析后的QuinaryInstruction
         *
         * @elseif en
         *
         * @brief Parse quinary instruction
         * @param layer Recursion level
         * @return Parsed QuinaryInstruction
         *
         * @endif
         */
        QuinaryInstruction parseQuinaryInstruction(int layer);

        /**
         * @if zh
         *
         * @brief 解析六元指令
         * @param layer 递归层级
         * @return 解析后的SenaryInstruction
         *
         * @elseif en
         *
         * @brief Parse senary instruction
         * @param layer Recursion level
         * @return Parsed SenaryInstruction
         *
         * @endif
         */
        SenaryInstruction parseSenaryInstruction(int layer);

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
        Operand parseOperand(int layer);

        /**
         * @if zh
         *
         * @brief 解析寄存器或设备
         * @param layer 递归层级
         * @return 解析后的RegisterOrDevice
         *
         * @elseif en
         *
         * @brief Parse register or device
         * @param layer Recursion level
         * @return Parsed RegisterOrDevice
         *
         * @endif
         */
        RegisterOrDevice parseRegisterOrDevice(int layer);

        /**
         * @if zh
         *
         * @brief 解析寄存器或标识符
         * @param layer 递归层级
         * @return 解析后的RegisterOrIdentifier
         *
         * @elseif en
         *
         * @brief Parse register or identifier
         * @param layer Recursion level
         * @return Parsed RegisterOrIdentifier
         *
         * @endif
         */
        RegisterOrIdentifier parseRegisterOrIdentifier(int layer);

        /**
         * @if zh
         *
         * @brief 解析设备引用
         * @details 解析设备引用,支持Device和RegisterOrIdentifier两种形式。
         *        不再支持Number(INTEGER/FLOAT/HEX_NUMBER/BINARY_NUMBER)。
         * @param layer 递归层级,用于调试输出
         * @return 解析后的DeviceReference
         * @note 已移除对Number类型的支持,数字引用应使用parseNumberValue
         *
         * @elseif en
         *
         * @brief Parse device reference
         * @details Parses device reference, supporting Device and RegisterOrIdentifier forms.
         *        Number (INTEGER/FLOAT/HEX_NUMBER/BINARY_NUMBER) is no longer supported.
         * @param layer Recursion level, for debug output
         * @return Parsed DeviceReference
         * @note Support for Number type has been removed; use parseNumberValue for number references
         *
         * @endif
         */
        DeviceReference parseDeviceReference(int layer);

        /**
         * @if zh
         *
         * @brief 解析寄存器或数字
         * @details 解析寄存器(r?)或数字字面量。支持的Token类型包括:
         *        REGISTER、IDENTIFIER、INTEGER、FLOAT、HEX_NUMBER、BINARY_NUMBER,
         *        以及KEYWORD_HASH、KEYWORD_STR、KEYWORD_NAN、KEYWORD_PINF、KEYWORD_NINF、
         *        KEYWORD_PI、KEYWORD_TAU、KEYWORD_DEG2RAD、KEYWORD_RAD2DEG、KEYWORD_EPSILON、
         *        KEYWORD_RGAS等常量关键字。
         * @param layer 递归层级,用于调试输出
         * @return 解析后的RegisterOrNumber
         * @note 该方法取代了五元/六元指令中原本使用的通用Operand类型
         *
         * @elseif en
         *
         * @brief Parse register or number
         * @details Parses a register (r?) or number literal. Supported Token types include:
         *        REGISTER, IDENTIFIER, INTEGER, FLOAT, HEX_NUMBER, BINARY_NUMBER,
         *        and constant keywords such as KEYWORD_HASH, KEYWORD_STR, KEYWORD_NAN, KEYWORD_PINF,
         *        KEYWORD_NINF, KEYWORD_PI, KEYWORD_TAU, KEYWORD_DEG2RAD, KEYWORD_RAD2DEG,
         *        KEYWORD_EPSILON, KEYWORD_RGAS.
         * @param layer Recursion level, for debug output
         * @return Parsed RegisterOrNumber
         * @note This method replaces the generic Operand type previously used in quinary/senary instructions
         *
         * @endif
         */
        RegisterOrNumber parseRegisterOrNumber(int layer);

        /**
         * @if zh
         *
         * @brief 解析数字值
         * @details 解析数字字面量或常量。支持的Token类型包括:
         *        INTEGER、FLOAT、HEX_NUMBER、BINARY_NUMBER、IDENTIFIER,
         *        以及KEYWORD_HASH、KEYWORD_STR和常量关键字(如PI、TAU等)。
         * @param layer 递归层级,用于调试输出
         * @return 解析后的NumberValue
         * @note 被parseJumpTarget复用,作为跳转目标解析的基础
         *
         * @elseif en
         *
         * @brief Parse number value
         * @details Parses a number literal or constant. Supported Token types include:
         *        INTEGER, FLOAT, HEX_NUMBER, BINARY_NUMBER, IDENTIFIER,
         *        and KEYWORD_HASH, KEYWORD_STR, and constant keywords (e.g., PI, TAU, etc.).
         * @param layer Recursion level, for debug output
         * @return Parsed NumberValue
         * @note Reused by parseJumpTarget as the basis for jump target parsing
         *
         * @endif
         */
        NumberValue parseNumberValue(int layer);

        /**
         * @if zh
         *
         * @brief 解析跳转目标
         * @details 解析跳转指令的目标地址。委托给parseNumberValue进行实际解析,
         *        支持所有NumberValue可接受的Token类型。
         * @param layer 递归层级,用于调试输出
         * @return 解析后的JumpTarget
         * @note 解析逻辑完全委托给parseNumberValue
         *
         * @elseif en
         *
         * @brief Parse jump target
         * @details Parses the target address of a jump instruction. Delegates to parseNumberValue
         *        for actual parsing, supporting all Token types accepted by NumberValue.
         * @param layer Recursion level, for debug output
         * @return Parsed JumpTarget
         * @note Parsing logic is fully delegated to parseNumberValue
         *
         * @endif
         */
        JumpTarget parseJumpTarget(int layer);

        /**
         * @if zh
         *
         * @brief 解析设备别名引用
         * @details 解析设备别名引用,支持DEVICE和IDENTIFIER两种Token类型。
         *        用于alias指令等场景中对已定义设备别名的引用。
         * @param layer 递归层级,用于调试输出
         * @return 解析后的DeviceAliasRef
         * @note 仅接受DEVICE和IDENTIFIER,不解析其他形式
         *
         * @elseif en
         *
         * @brief Parse device alias reference
         * @details Parses a device alias reference, supporting DEVICE and IDENTIFIER Token types.
         *        Used in scenarios such as alias directives for referencing defined device aliases.
         * @param layer Recursion level, for debug output
         * @return Parsed DeviceAliasRef
         * @note Only accepts DEVICE and IDENTIFIER; other forms are not parsed
         *
         * @endif
         */
        DeviceAliasRef parseDeviceAliasRef(int layer);

        /**
         * @if zh
         *
         * @brief 解析宏调用
         * @param layer 递归层级
         * @return 解析后的MacroCall
         *
         * @elseif en
         *
         * @brief Parse macro call
         * @param layer Recursion level
         * @return Parsed MacroCall
         *
         * @endif
         */
        MacroCall parseMacroCall(int layer);

        /**
         * @if zh
         *
         * @brief 解析hash宏调用
         * @param layer 递归层级
         * @return 解析后的HashCall
         *
         * @elseif en
         *
         * @brief Parse hash macro call
         * @param layer Recursion level
         * @return Parsed HashCall
         *
         * @endif
         */
        ShallowErrorable<HashCall> parseHashCall(int layer);

        /**
         * @if zh
         *
         * @brief 解析str宏调用
         * @param layer 递归层级
         * @return 解析后的StrCall
         *
         * @elseif en
         *
         * @brief Parse str macro call
         * @param layer Recursion level
         * @return Parsed StrCall
         *
         * @endif
         */
        ShallowErrorable<StrCall> parseStrCall(int layer);

        /**
         * @if zh
         *
         * @brief 解析常量
         * @param layer 递归层级
         * @return 解析后的Constant
         *
         * @elseif en
         *
         * @brief Parse constant
         * @param layer Recursion level
         * @return Parsed Constant
         *
         * @endif
         */
        ShallowErrorable<Constant> parseConstant(int layer);

        /**
         * @if zh
         *
         * @brief 解析逻辑类型
         * @param layer 递归层级
         * @return 解析后的LogicType
         *
         * @elseif en
         *
         * @brief Parse logic type
         * @param layer Recursion level
         * @return Parsed LogicType
         *
         * @endif
         */
        LogicType parseLogicType(int layer);

        /**
         * @if zh
         *
         * @brief 解析槽索引
         * @param layer 递归层级
         * @return 解析后的SlotIndex
         *
         * @elseif en
         *
         * @brief Parse slot index
         * @param layer Recursion level
         * @return Parsed SlotIndex
         *
         * @endif
         */
        SlotIndex parseSlotIndex(int layer);

        /**
         * @if zh
         *
         * @brief 解析逻辑槽类型
         * @param layer 递归层级
         * @return 解析后的LogicSlotType
         *
         * @elseif en
         *
         * @brief Parse logic slot type
         * @param layer Recursion level
         * @return Parsed LogicSlotType
         *
         * @endif
         */
        LogicSlotType parseLogicSlotType(int layer);

        /**
         * @if zh
         *
         * @brief 解析批处理模式
         * @param layer 递归层级
         * @return 解析后的BatchMode
         *
         * @elseif en
         *
         * @brief Parse batch mode
         * @param layer Recursion level
         * @return Parsed BatchMode
         *
         * @endif
         */
        BatchMode parseBatchMode(int layer);

        /**
         * @if zh
         *
         * @brief 解析试剂模式
         * @param layer 递归层级
         * @return 解析后的ReagentMode
         *
         * @elseif en
         *
         * @brief Parse reagent mode
         * @param layer Recursion level
         * @return Parsed ReagentMode
         *
         * @endif
         */
        ReagentMode parseReagentMode(int layer);

        /**
         * @if zh
         *
         * @brief 解析标识符或数字
         * @param layer 递归层级
         * @return 解析后的Identifier或Number
         *
         * @elseif en
         *
         * @brief Parse identifier or number
         * @param layer Recursion level
         * @return Parsed Identifier or Number
         *
         * @endif
         */
        Errorable<Identifier, Number> parseIdentifierOrNumber(int layer);

        /**
         * @if zh
         *
         * @brief 解析设备
         * @param layer 递归层级
         * @return 解析后的Device
         *
         * @elseif en
         *
         * @brief Parse device
         * @param layer Recursion level
         * @return Parsed Device
         *
         * @endif
         */
        ShallowErrorable<Device> parseDevice(int layer);

        /**
         * @if zh
         *
         * @brief 解析寄存器
         * @param layer 递归层级
         * @return 解析后的Register
         *
         * @elseif en
         *
         * @brief Parse register
         * @param layer Recursion level
         * @return Parsed Register
         *
         * @endif
         */
        ShallowErrorable<Register> parseRegister(int layer);

        /**
         * @if zh
         *
         * @brief 解析字符串
         * @param layer 递归层级
         * @return 解析后的String
         *
         * @elseif en
         *
         * @brief Parse string
         * @param layer Recursion level
         * @return Parsed String
         *
         * @endif
         */
        ShallowErrorable<String> parseString(int layer);

        /**
         * @if zh
         *
         * @brief 解析标识符
         * @param layer 递归层级
         * @return 解析后的Identifier
         *
         * @elseif en
         *
         * @brief Parse identifier
         * @param layer Recursion level
         * @return Parsed Identifier
         *
         * @endif
         */
        ShallowErrorable<Identifier> parseIdentifier(int layer);

        /**
         * @if zh
         *
         * @brief 解析数字
         * @param layer 递归层级
         * @return 解析后的Number
         *
         * @elseif en
         *
         * @brief Parse number
         * @param layer Recursion level
         * @return Parsed Number
         *
         * @endif
         */
        Number parseNumber(int layer);

        /**
         * @if zh
         *
         * @brief 解析二进制数
         * @param layer 递归层级
         * @return 解析后的BinaryNumber
         *
         * @elseif en
         *
         * @brief Parse binary number
         * @param layer Recursion level
         * @return Parsed BinaryNumber
         *
         * @endif
         */
        ShallowErrorable<BinaryNumber> parseBinaryNumber(int layer);

        /**
         * @if zh
         *
         * @brief 解析十六进制数
         * @param layer 递归层级
         * @return 解析后的HexNumber
         *
         * @elseif en
         *
         * @brief Parse hexadecimal number
         * @param layer Recursion level
         * @return Parsed HexNumber
         *
         * @endif
         */
        ShallowErrorable<HexNumber> parseHexNumber(int layer);

        /**
         * @if zh
         *
         * @brief 解析浮点数
         * @param layer 递归层级
         * @return 解析后的Float
         *
         * @elseif en
         *
         * @brief Parse float number
         * @param layer Recursion level
         * @return Parsed Float
         *
         * @endif
         */
        ShallowErrorable<Float> parseFloat(int layer);

        /**
         * @if zh
         *
         * @brief 解析整数
         * @param layer 递归层级
         * @return 解析后的Integer
         *
         * @elseif en
         *
         * @brief Parse integer
         * @param layer Recursion level
         * @return Parsed Integer
         *
         * @endif
         */
        ShallowErrorable<Integer> parseInteger(int layer);

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
        bool inScope() const;

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
        void skip();

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
        void consume() const;

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
        void gotoNextLine() const;

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
        std::shared_ptr<Token> current(bool consume = false) const;

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
         * @brief 操作数起始 Token 集合（编译期推导）
         * @details 从 Operand variant 的所有成员类型的 startTokens 编译期聚合而来，
         *          用于错误恢复时快速判断当前 token 是否为合法操作数起始
         * @elseif en
         * @brief Operand start token set (compile-time derived)
         * @details Compile-time aggregated from startTokens of all member types in the Operand variant,
         *          used for quickly checking whether the current token is a valid operand start during error recovery
         * @endif
         */
        static constexpr auto OPERAND_STARTS = getStartTokens<Operand>();

    };

}  // namespace stationeers::ic10

#endif  // COMPILER_PARSER_HPP
