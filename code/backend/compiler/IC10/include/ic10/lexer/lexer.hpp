// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file lexer.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:23
 * @if zh
 * @brief IC10词法分析器实现
 * @details 提供IC10编程语言的词法分析功能,将源代码字符串分解为词法标记序列。
 *        支持数字(整数、浮点、十六进制、二进制)、字符串、标识符、寄存器、设备、符号、
 *        注释和各类指令关键字的识别。
 *
 * @note 词法分析是编译过程的**第一阶段**,位于解析之前
 * @note 本分析器采用**递归下降**方式实现
 *
 * @par 词法分析流程:
 * @code{.mermaid}
 * graph TD
 *     A[源代码] --> B{当前字符}
 *     B -->|字母/_| C[提取标识符]
 *     B -->|数字| D[提取数字]
 *     B -->|"| E[提取字符串]
 *     B -->|0x/0X| F[提取十六进制]
 *     B -->|0b/0B| G[提取二进制]
 *     B -->|符号| H[提取符号]
 *     B -->|其他| I[跳过/结束]
 *     C --> J{关键字?}
 *     J -->|是| K[关键字Token]
 *     J -->|否| L[标识符Token]
 *     D --> M{点号?}
 *     M -->|是| N[浮点Token]
 *     M -->|否| O[整数Token]
 * @endcode
 *
 * @par 支持的Token类型:
 * | 类型 | 说明 | 示例 |
 * |:-----|:-----|:-----|
 * | 关键字 | 指令名称 | `add`, `move`, `yield` |
 * | 数字 | 整数/浮点/十六进制/二进制 | `42`, `3.14`, `0xFF`, `0b1010` |
 * | 字符串 | 双引号包围的文本 | `"Hello World"` |
 * | 寄存器 | r0-r7 | `r0`, `r1`, `r7` |
 * | 设备 | @开头 | `@Display`, `@Sensor` |
 * | 标识符 | 字母/下划线开头 | `_temp`, `myVar` |
 * | 符号 | 操作符等 | `+`, `-`, `*`, `/` |
 *
 * @warning 十六进制数必须以 `0x` 或 `0X` 开头,否则将被解析为标识符
 * @warning 二进制数必须以 `0b` 或 `0B` 开头
 *
 * @see Token Token类型定义
 * @see Parser 语法分析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 lexical analyzer implementation
 * @details Provides lexical analysis functionality for IC10 programming language, decomposing source code
 *        string into a sequence of lexical tokens. Supports recognition of numbers (integer, float,
 *        hexadecimal, binary), strings, identifiers, registers, devices, symbols, comments and
 *        various instruction keywords.
 *
 * @note Lexical analysis is the **first phase** of the compilation process, before parsing
 * @note This analyzer is implemented using **recursive descent** approach
 *
 * @par Lexical Analysis Flow:
 * @code{.mermaid}
 * graph TD
 *     A[Source Code] --> B{Current Char}
 *     B -->|Letter/_| C[Extract Identifier]
 *     B -->|Digit| D[Extract Number]
 *     B -->|"| E[Extract String]
 *     B -->|0x/0X| F[Extract Hex]
 *     B -->|0b/0B| G[Extract Binary]
 *     B -->|Symbol| H[Extract Symbol]
 *     B -->|Other| I[Skip/End]
 *     C --> J{Keyword?}
 *     J -->|Yes| K[Keyword Token]
 *     J -->|No| L[Identifier Token]
 *     D --> M{Dot?}
 *     M -->|Yes| N[Float Token]
 *     M -->|No| O[Integer Token]
 * @endcode
 *
 * @par Supported Token Types:
 * | Type | Description | Example |
 * |:-----|:------------|:--------|
 * | Keyword | Instruction names | `add`, `move`, `yield` |
 * | Number | Integer/Float/Hex/Binary | `42`, `3.14`, `0xFF`, `0b1010` |
 * | String | Text enclosed in quotes | `"Hello World"` |
 * | Register | r0-r7 | `r0`, `r1`, `r7` |
 * | Device | Starts with @ | `@Display`, `@Sensor` |
 * | Identifier | Letter/underscore prefix | `_temp`, `myVar` |
 * | Symbol | Operators etc. | `+`, `-`, `*`, `/` |
 *
 * @warning Hex numbers must start with `0x` or `0X`, otherwise will be parsed as identifier
 * @warning Binary numbers must start with `0b` or `0B`
 *
 * @see Token Token type definition
 * @see Parser Syntax analyzer
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_LEXER_HPP
#define COMPILER_LEXER_HPP
#pragma once

#include <string_view>
#include <optional>
#include <memory>
#include <vector>

#include "token.hpp"


namespace stationeers::ic10 {

    /**
     * @class Lexer
     * @if zh
     *
     * @brief 词法分析器类
     * @details 将IC10源代码字符串分解为词法标记(Token)序列,识别各种词法单元。
     *
     * @par 使用示例:
     * ```cpp
     * #include "lexer.hpp"
     * using namespace stationeers::ic10;
     *
     * // 方式1: 使用静态方法
     * std::string_view src = "add r0 r1 r2";
     * auto tokens = Lexer::tokenize(src);
     *
     * // 方式2: 创建实例后扫描
     * Lexer lexer(src);
     * auto tokens = lexer.scan();
     *
     * // 遍历所有token
     * for (const auto& token : tokens) {
     *     std::cout << token->toString() << std::endl;
     * }
     * ```
     *
     * @par 支持的数字格式:
     * | 格式 | 前缀 | 示例 | 说明 |
     * |:-----|:-----|:-----|:-----|
     * | 十进制整数 | 无 | `42`, `-7` | 标准整数 |
     * | 浮点数 | 无 | `3.14`, `-2.5` | 含小数点 |
     * | 十六进制 | `0x`/`0X` | `0xFF`, `0X1A` | 大小写不敏感 |
     * | 二进制 | `0b`/`0B` | `0b1010`, `0B0011` | 大小写不敏感 |
     *
     * @attention 构造函数不会立即执行词法分析,需要调用 scan() 或使用 tokenize()
     * @see tokenize() 静态便捷方法
     *
     * @elseif en
     *
     * @brief Lexical analyzer class
     * @details Decomposes IC10 source code string into a sequence of lexical tokens (Token),
     *        recognizing various lexical units.
     *
     * @par Usage example:
     * ```cpp
     * #include "lexer.hpp"
     * using namespace stationeers::ic10;
     *
     * // Method 1: Using static method
     * std::string_view src = "add r0 r1 r2";
     * auto tokens = Lexer::tokenize(src);
     *
     * // Method 2: Create instance and scan
     * Lexer lexer(src);
     * auto tokens = lexer.scan();
     *
     * // Iterate all tokens
     * for (const auto& token : tokens) {
     *     std::cout << token->toString() << std::endl;
     * }
     * ```
     *
     * @par Supported Number Formats:
     * | Format | Prefix | Example | Description |
     * |:-------|:-------|:--------|:------------|
     * | Decimal integer | None | `42`, `-7` | Standard integer |
     * | Float | None | `3.14`, `-2.5` | Contains decimal point |
     * | Hexadecimal | `0x`/`0X` | `0xFF`, `0X1A` | Case insensitive |
     * | Binary | `0b`/`0B` | `0b1010`, `0B0011` | Case insensitive |
     *
     * @attention Constructor does not perform lexical analysis immediately, need to call scan() or use tokenize()
     * @see tokenize() Static convenience method
     *
     * @endif
     */
    class Lexer {
    public:
        /**
         * @if zh
         *
         * @brief 构造函数
         * @param src 源代码字符串
         * @param debug 是否启用调试模式
         *
         * @elseif en
         *
         * @brief Constructor
         * @param src Source code string
         * @param debug Whether to enable debug mode
         *
         * @endif
         */
        explicit Lexer(std::string_view src, bool debug = false);

        /**
         * @if zh
         * @brief 默认构造函数
         * @elseif en
         * @brief Default constructor
         * @endif
         */
        Lexer() = default;

        /**
         * @if zh
         *
         * @brief 扫描源代码生成标记序列
         * @return 标记指针的向量
         *
         * @elseif en
         *
         * @brief Scan source code to generate token sequence
         * @return Vector of token pointers
         *
         * @endif
         */
        [[nodiscard]] std::vector<std::shared_ptr<Token>> scan() const;

        /**
         * @if zh
         *
         * @brief 静态词法分析入口
         * @param src 源代码字符串
         * @param debug 是否启用调试模式
         * @return 标记指针的向量
         *
         * @elseif en
         *
         * @brief Static lexical analysis entry point
         * @param src Source code string
         * @param debug Whether to enable debug mode
         * @return Vector of token pointers
         *
         * @endif
         */
        static std::vector<std::shared_ptr<Token>> tokenize(
            std::string_view src, bool debug = false
        );

    private:
        /**
         * @if zh
         * @brief 当前扫描位置
         * @elseif en
         * @brief Current scan position
         * @endif
         */
        mutable Pos pos_;

        /**
         * @if zh
         * @brief 源代码字符串视图
         * @elseif en
         * @brief Source code string view
         * @endif
         */
        std::string_view src_;

        /**
         * @if zh
         * @brief 调试模式标志
         * @elseif en
         * @brief Debug mode flag
         * @endif
         */
        bool debug_;

        /**
         * @if zh
         *
         * @brief 获取当前字符
         * @return 当前字符,如果超出范围则返回空
         *
         * @elseif en
         *
         * @brief Get current character
         * @return Current character, or empty if out of range
         *
         * @endif
         */
        [[nodiscard]] std::optional<char> current() const noexcept;

        /**
         * @if zh
         *
         * @brief 获取下一个标记
         * @return 识别出的Token
         *
         * @elseif en
         *
         * @brief Get next token
         * @return Identified Token
         *
         * @endif
         */
        Token next() const;

        /**
         * @if zh
         *
         * @brief 检查是否在源代码范围内
         * @return 如果还有字符则返回true
         *
         * @elseif en
         *
         * @brief Check if within source code range
         * @return true if there are still characters
         *
         * @endif
         */
        [[nodiscard]] bool inScope() const noexcept;

        /**
         * @if zh
         *
         * @brief 预览指定偏移的字符
         * @param offset 向前偏移量,默认为1
         * @return 预览的字符,如果超出范围则返回空
         *
         * @elseif en
         *
         * @brief Peek character at specified offset
         * @param offset Forward offset, defaults to 1
         * @return Peeked character, or empty if out of range
         *
         * @endif
         */
        [[nodiscard]] std::optional<char> peek(int offset = 1) const noexcept;

        /**
         * @if zh
         *
         * @brief 跳过当前字符
         *
         * @elseif en
         *
         * @brief Skip current character
         *
         * @endif
         */
        void skip() const noexcept;

        /**
         * @if zh
         *
         * @brief 提取标识符
         * @return 标识符Token
         *
         * @elseif en
         *
         * @brief Extract identifier
         * @return Identifier Token
         *
         * @endif
         */
        Token extractIdentifier() const;

        /**
         * @if zh
         *
         * @brief 提取数字
         * @return 数字Token(整数或浮点)
         *
         * @elseif en
         *
         * @brief Extract number
         * @return Number Token (integer or float)
         *
         * @endif
         */
        Token extractNumber() const;

        /**
         * @if zh
         *
         * @brief 提取字符串
         * @return 字符串Token
         *
         * @elseif en
         *
         * @brief Extract string
         * @return String Token
         *
         * @endif
         */
        Token extractString() const;

        /**
         * @if zh
         *
         * @brief 提取符号
         * @return 符号Token
         *
         * @elseif en
         *
         * @brief Extract symbol
         * @return Symbol Token
         *
         * @endif
         */
        Token extractSymbol() const;

        /**
         * @if zh
         *
         * @brief 提取HEX注释
         * @return HEX注释Token
         *
         * @elseif en
         *
         * @brief Extract HEX comment
         * @return HEX comment Token
         *
         * @endif
         */
        Token extractHexComment() const;

        /**
         * @if zh
         *
         * @brief 提取斜杠注释
         * @return 斜杠注释Token
         *
         * @elseif en
         *
         * @brief Extract slash comment
         * @return Slash comment Token
         *
         * @endif
         */
        Token extractSlashComment() const;

        /**
         * @if zh
         *
         * @brief 提取十六进制数
         * @return 十六进制数Token
         *
         * @elseif en
         *
         * @brief Extract hexadecimal number
         * @return Hexadecimal number Token
         *
         * @endif
         */
        Token extractHexNumber() const;

        /**
         * @if zh
         *
         * @brief 提取二进制数
         * @return 二进制数Token
         *
         * @elseif en
         *
         * @brief Extract binary number
         * @return Binary number Token
         *
         * @endif
         */
        Token extractBinaryNumber() const;

        /**
         * @if zh
         *
         * @brief 提取设备名
         * @return 设备Token
         *
         * @elseif en
         *
         * @brief Extract device name
         * @return Device Token
         *
         * @endif
         */
        Token extractDevice() const;

        /**
         * @if zh
         *
         * @brief 提取寄存器
         * @return 寄存器Token
         *
         * @elseif en
         *
         * @brief Extract register
         * @return Register Token
         *
         * @endif
         */
        Token extractRegister() const;
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_LEXER_HPP
