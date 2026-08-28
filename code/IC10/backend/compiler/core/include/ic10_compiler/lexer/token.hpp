// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file token.hpp
 * @author edocsitahw
 * @version 1.2
 * @date 2026/07/12
 * @if zh
 * @brief IC10词法标记定义
 * @details 定义IC10编程语言的词法标记类型、标记类别、关键字映射表和Token结构。
 *        包含数字、字符串、标识符、寄存器、设备、各类指令关键字
 *        （算术、逻辑、移位与循环移位、分支与相对跳转、批量加载/存储、设备别名清除等）
 *        以及关键字常量（如nan、pi、rgas等）等标记类型。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 lexical token definitions
 * @details Defines lexical token types, token categories, keyword mapping tables and Token
 * structure for IC10 programming language. Includes token types such as numbers, strings,
 * identifiers, registers, devices, and various instruction keywords (arithmetic, logical, shift and
 * rotate, branches and relative jumps, batch load/store, device-alias clear, etc.) as well as
 * keyword constants (such as nan, pi, rgas, etc.).
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 */
#ifndef COMPILER_TOKEN_HPP
#define COMPILER_TOKEN_HPP
#pragma once

#include "common/utils/bidirectional_map.hpp"
#include "common/utils/position.hpp"
#include <string>
#include <unordered_map>

namespace stationeers::ic10 {

    enum class TokenType {
        /* 数字 */
        INTEGER,        ///< @if zh 整型 @else Integer @endif
        FLOAT,          ///< @if zh 浮点型 @else Float @endif
        HEX_NUMBER,     ///< @if zh 十六进制数 @else Hexadecimal number @endif
        BINARY_NUMBER,  ///< @if zh 二进制数 @else Binary number @endif
        STRING,         ///< @if zh 字符串 @else String @endif

        /* 变量名 */
        IDENTIFIER,  ///< @if zh 标识符 @else Identifier @endif

        /* 寄存器设备 */
        REGISTER,  ///< @if zh 寄存器 @else Register @endif
        DEVICE,    ///< @if zh 设备 @else Device @endif

        /* 符号 */
        LPAREN,  ///< @if zh 左圆括号 @else Left parenthesis @endif
        RPAREN,  ///< @if zh 右圆括号 @else Right parenthesis @endif
        COLON,   ///< @if zh 冒号 @else Colon @endif
        DOT,     ///< @if zh 点 @else Dot @endif
        SUB,     ///< @if zh 减号 @else Subtraction @endif
        DIV,     ///< @if zh 除号 @else Division @endif

        /* 注释 */
        HEX_COMMENT,    ///< @if zh #号注释 @else Hash comment @endif
        SLASH_COMMENT,  ///< @if zh //号注释 @else Slash comment @endif

        /* 换行 */
        NEWLINE,  ///< @if zh 换行符 @else Newline @endif

        /* 关键字 */
        KEYWORD,         ///< @if zh 关键字 @else Keyword @endif
        KEYWORD_HASH,    ///< @if zh 关键字@c hash @else Keyword @c hash @endif
        KEYWORD_STR,     ///< @if zh 关键字@c str @else Keyword @c str @endif
        KEYWORD_ALIAS,   ///< @if zh 关键字@c alias @else Keyword @c alias @endif
        KEYWORD_DEFINE,  ///< @if zh 关键字@c define @else Keyword @c define @endif

        // 文件结束标记
        END,  ///< @if zh 文件结束标记 @else End of file @endif

        // 未知标记（用于错误处理）
        UNKNOWN,  ///< @if zh 未知标记 @else Unknown token @endif

        TYPE_HINT_PREFIX,
        TYPE_ANNOTATION_PREFIX,
        TAG
    };

    enum class TokenCategory {
        LITERAL,     ///< @if zh 字面值 @else Literal @endif
        SYMBOL,      ///< @if zh 符号 @else Symbol @endif
        COMMENT,     ///< @if zh 注释 @else Comment @endif
        ANNOTATION,  ///< @if zh 注解(文档注释/类型提示) @else Annotation (doc comment/type hint)
                     ///< @endif
        WHITESPACE,  ///< @if zh 空白符 @else Whitespace @endif
        END,         ///< @if zh 文件结束 @else End of file @endif
        INVALID      ///< @if zh 无效标记 @else Invalid token @endif
    };

    enum class InstructionKeyword {
        // 空指令
        HCF,  ///< @if zh 关键字@c hcf @else Keyword @c hcf @endif

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        YIELD,  ///< @if zh 关键字@c yield @else Keyword @c yield @endif
#endif

        // 一元指令
        PEEK,  ///< @if zh 关键字@c peek @else Keyword @c peek @endif

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        POP,    ///< @if zh 关键字@c pop @else Keyword @c pop @endif
        PUSH,   ///< @if zh 关键字@c push @else Keyword @c push @endif
        CLR,    ///< @if zh 关键字@c clr @else Keyword @c clr @endif
        J,      ///< @if zh 关键字@c j @else Keyword @c j @endif
        JAL,    ///< @if zh 关键字@c jal @else Keyword @c jal @endif
        JR,     ///< @if zh 关键字@c jr @else Keyword @c jr @endif
        RAND,   ///< @if zh 关键字@c rand @else Keyword @c rand @endif
        SLEEP,  ///< @if zh 关键字@c sleep @else Keyword @c sleep @endif
        CLRD,   ///< @if zh 关键字@c clrd @else Keyword @c clrd @endif
#endif

        // 二元指令
        ABS,  ///< @if zh 关键字@c abs @else Keyword @c abs @endif

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        ACOS,    ///< @if zh 关键字@c acos @else Keyword @c acos @endif
        ASIN,    ///< @if zh 关键字@c asin @else Keyword @c asin @endif
        ATAN,    ///< @if zh 关键字@c atan @else Keyword @c atan @endif
        ATAN2,   ///< @if zh 关键字@c atan2 @else Keyword @c atan2 @endif
        CEIL,    ///< @if zh 关键字@c ceil @else Keyword @c ceil @endif
        COS,     ///< @if zh 关键字@c cos @else Keyword @c cos @endif
        DIV,     ///< @if zh 关键字@c div @else Keyword @c div @endif
        EXP,     ///< @if zh 关键字@c exp @else Keyword @c exp @endif
        FLOOR,   ///< @if zh 关键字@c floor @else Keyword @c floor @endif
        LOG,     ///< @if zh 关键字@c log @else Keyword @c log @endif
        MAX,     ///< @if zh 关键字@c max @else Keyword @c max @endif
        MIN,     ///< @if zh 关键字@c min @else Keyword @c min @endif
        MOD,     ///< @if zh 关键字@c mod @else Keyword @c mod @endif
        MUL,     ///< @if zh 关键字@c mul @else Keyword @c mul @endif
        POW,     ///< @if zh 关键字@c pow @else Keyword @c pow @endif
        ROUND,   ///< @if zh 关键字@c round @else Keyword @c round @endif
        SIN,     ///< @if zh 关键字@c sin @else Keyword @c sin @endif
        SQRT,    ///< @if zh 关键字@c sqrt @else Keyword @c sqrt @endif
        SGN,     ///< @if zh 关键字@c sgn @else Keyword @c sgn @endif
        SUB,     ///< @if zh 关键字@c sub @else Keyword @c sub @endif
        TAN,     ///< @if zh 关键字@c tan @else Keyword @c tan @endif
        TRUNC,   ///< @if zh 关键字@c trunc @else Keyword @c trunc @endif
        NOT,     ///< @if zh 关键字@c not @else Keyword @c not @endif
        MOVE,    ///< @if zh 关键字@c move @else Keyword @c move @endif
        POKE,    ///< @if zh 关键字@c poke @else Keyword @c poke @endif
        BEQZ,    ///< @if zh 关键字@c beqz @else Keyword @c beqz @endif
        BEQZAL,  ///< @if zh 关键字@c beqzal @else Keyword @c beqzal @endif
        BNEZ,    ///< @if zh 关键字@c bnez @else Keyword @c bnez @endif
        BNEZAL,  ///< @if zh 关键字@c bnezal @else Keyword @c bnezal @endif
        BGEZ,    ///< @if zh 关键字@c bgez @else Keyword @c bgez @endif
        BGEZAL,  ///< @if zh 关键字@c bgezal @else Keyword @c bgezal @endif
        BGTZ,    ///< @if zh 关键字@c bgtz @else Keyword @c bgtz @endif
        BGTZAL,  ///< @if zh 关键字@c bgtzal @else Keyword @c bgtzal @endif
        BLEZ,    ///< @if zh 关键字@c blez @else Keyword @c blez @endif
        BLEZAL,  ///< @if zh 关键字@c blezal @else Keyword @c blezal @endif
        BLTZ,    ///< @if zh 关键字@c bltz @else Keyword @c bltz @endif
        BLTZAL,  ///< @if zh 关键字@c bltzal @else Keyword @c bltzal @endif
        BNAN,    ///< @if zh 关键字@c bnan @else Keyword @c bnan @endif
        BDNS,    ///< @if zh 关键字@c bdns @else Keyword @c bdns @endif
        BDNSAL,  ///< @if zh 关键字@c bdnsal @else Keyword @c bdnsal @endif
        BDSE,    ///< @if zh 关键字@c bdse @else Keyword @c bdse @endif
        BDSEAL,  ///< @if zh 关键字@c bdseal @else Keyword @c bdseal @endif
        BREQZ,   ///< @if zh 关键字@c breqz @else Keyword @c breqz @endif
        BRGEZ,   ///< @if zh 关键字@c brgez @else Keyword @c brgez @endif
        BRGTZ,   ///< @if zh 关键字@c brgtz @else Keyword @c brgtz @endif
        BRLEZ,   ///< @if zh 关键字@c brlez @else Keyword @c brlez @endif
        BRLTZ,   ///< @if zh 关键字@c brltz @else Keyword @c brltz @endif
        BRNAN,   ///< @if zh 关键字@c brnan @else Keyword @c brnan @endif
        BRNEZ,   ///< @if zh 关键字@c brnez @else Keyword @c brnez @endif
        BRDNS,   ///< @if zh 关键字@c brdns @else Keyword @c brdns @endif
        BRDSE,   ///< @if zh 关键字@c brdse @else Keyword @c brdse @endif
        SEQZ,    ///< @if zh 关键字@c seqz @else Keyword @c seqz @endif
        SNEZ,    ///< @if zh 关键字@c snez @else Keyword @c snez @endif
        SGEZ,    ///< @if zh 关键字@c sgez @else Keyword @c sgez @endif
        SGTZ,    ///< @if zh 关键字@c sgtz @else Keyword @c sgtz @endif
        SLEZ,    ///< @if zh 关键字@c slez @else Keyword @c slez @endif
        SLTZ,    ///< @if zh 关键字@c sltz @else Keyword @c sltz @endif
        SNAN,    ///< @if zh 关键字@c snan @else Keyword @c snan @endif
        SNANZ,   ///< @if zh 关键字@c snanz @else Keyword @c snanz @endif
        SDNS,    ///< @if zh 关键字@c sdns @else Keyword @c sdns @endif
        SDSE,    ///< @if zh 关键字@c sdse @else Keyword @c sdse @endif
#endif

        // 三元指令
        ADD,  ///< @if zh 关键字@c add @else Keyword @c add @endif

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        AND,     ///< @if zh 关键字@c and @else Keyword @c and @endif
        NOR,     ///< @if zh 关键字@c nor @else Keyword @c nor @endif
        OR,      ///< @if zh 关键字@c or @else Keyword @c or @endif
        SLA,     ///< @if zh 关键字@c sla @else Keyword @c sla @endif
        SLL,     ///< @if zh 关键字@c sll @else Keyword @c sll @endif
        SRA,     ///< @if zh 关键字@c sra @else Keyword @c sra @endif
        SRL,     ///< @if zh 关键字@c srl @else Keyword @c srl @endif
        XOR,     ///< @if zh 关键字@c xor @else Keyword @c xor @endif
        GET,     ///< @if zh 关键字@c get @else Keyword @c get @endif
        PUT,     ///< @if zh 关键字@c put @else Keyword @c put @endif
        L,       ///< @if zh 关键字@c l @else Keyword @c l @endif
        LS,      ///< @if zh 关键字@c ls @else Keyword @c ls @endif
        LR,      ///< @if zh 关键字@c lr @else Keyword @c lr @endif
        S,       ///< @if zh 关键字@c s @else Keyword @c s @endif
        SB,      ///< @if zh 关键字@c sb @else Keyword @c sb @endif
        ROL,     ///< @if zh 关键字@c rol @else Keyword @c rol @endif
        ROR,     ///< @if zh 关键字@c ror @else Keyword @c ror @endif
        RMAP,    ///< @if zh 关键字@c rmap @else Keyword @c rmap @endif
        BEQ,     ///< @if zh 关键字@c beq @else Keyword @c beq @endif
        BEQAL,   ///< @if zh 关键字@c beqal @else Keyword @c beqal @endif
        BNE,     ///< @if zh 关键字@c bne @else Keyword @c bne @endif
        BNEAL,   ///< @if zh 关键字@c bneal @else Keyword @c bneal @endif
        BGE,     ///< @if zh 关键字@c bge @else Keyword @c bge @endif
        BGEAL,   ///< @if zh 关键字@c bgeal @else Keyword @c bgeal @endif
        BGT,     ///< @if zh 关键字@c bgt @else Keyword @c bgt @endif
        BGTAL,   ///< @if zh 关键字@c bgtal @else Keyword @c bgtal @endif
        BLE,     ///< @if zh 关键字@c ble @else Keyword @c ble @endif
        BLEAL,   ///< @if zh 关键字@c bleal @else Keyword @c bleal @endif
        BLT,     ///< @if zh 关键字@c blt @else Keyword @c blt @endif
        BLTAL,   ///< @if zh 关键字@c bltal @else Keyword @c bltal @endif
        BAPZ,    ///< @if zh 关键字@c bapz @else Keyword @c bapz @endif
        BAPZAL,  ///< @if zh 关键字@c bapzal @else Keyword @c bapzal @endif
        BNAZ,    ///< @if zh 关键字@c bnaz @else Keyword @c bnaz @endif
        BNAZAL,  ///< @if zh 关键字@c bnazal @else Keyword @c bnazal @endif
        BDNVL,   ///< @if zh 关键字@c bdnvl @else Keyword @c bdnvl @endif
        BDNVS,   ///< @if zh 关键字@c bdnvs @else Keyword @c bdnvs @endif
        BREQ,    ///< @if zh 关键字@c breq @else Keyword @c breq @endif
        BRNE,    ///< @if zh 关键字@c brne @else Keyword @c brne @endif
        BRGE,    ///< @if zh 关键字@c brge @else Keyword @c brge @endif
        BRGT,    ///< @if zh 关键字@c brgt @else Keyword @c brgt @endif
        BRLE,    ///< @if zh 关键字@c brle @else Keyword @c brle @endif
        BRLT,    ///< @if zh 关键字@c brlt @else Keyword @c brlt @endif
        BRAPZ,   ///< @if zh 关键字@c brapz @else Keyword @c brapz @endif
        BRNAZ,   ///< @if zh 关键字@c brnaz @else Keyword @c brnaz @endif
        SAPZ,    ///< @if zh 关键字@c sapz @else Keyword @c sapz @endif
        SNAZ,    ///< @if zh 关键字@c snaz @else Keyword @c snaz @endif
        SEQ,     ///< @if zh 关键字@c seq @else Keyword @c seq @endif
        SNE,     ///< @if zh 关键字@c sne @else Keyword @c sne @endif
        SGE,     ///< @if zh 关键字@c sge @else Keyword @c sge @endif
        SGT,     ///< @if zh 关键字@c sgt @else Keyword @c sgt @endif
        SLE,     ///< @if zh 关键字@c sle @else Keyword @c sle @endif
        SLT,     ///< @if zh 关键字@c slt @else Keyword @c slt @endif
#endif

        // 四元指令
        CLAMP,  ///< @if zh 关键字@c clamp @else Keyword @c clamp @endif

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        LERP,    ///< @if zh 关键字@c lerp @else Keyword @c lerp @endif
        EXT,     ///< @if zh 关键字@c ext @else Keyword @c ext @endif
        INS,     ///< @if zh 关键字@c ins @else Keyword @c ins @endif
        SS,      ///< @if zh 关键字@c ss @else Keyword @c ss @endif
        LB,      ///< @if zh 关键字@c lb @else Keyword @c lb @endif
        SBN,     ///< @if zh 关键字@c sbn @else Keyword @c sbn @endif
        SBS,     ///< @if zh 关键字@c sbs @else Keyword @c sbs @endif
        BAP,     ///< @if zh 关键字@c bap @else Keyword @c bap @endif
        BAPAL,   ///< @if zh 关键字@c bapal @else Keyword @c bapal @endif
        BNA,     ///< @if zh 关键字@c bna @else Keyword @c bna @endif
        BNAAL,   ///< @if zh 关键字@c bnaal @else Keyword @c bnaal @endif
        BRAP,    ///< @if zh 关键字@c brap @else Keyword @c brap @endif
        BRNA,    ///< @if zh 关键字@c brna @else Keyword @c brna @endif
        SAP,     ///< @if zh 关键字@c sap @else Keyword @c sap @endif
        SNA,     ///< @if zh 关键字@c sna @else Keyword @c sna @endif
        SELECT,  ///< @if zh 关键字@c select @else Keyword @c select @endif
#endif

        // 五元指令
        LBN,  ///< @if zh 关键字@c lbn @else Keyword @c lbn @endif

#ifndef STATIONEERS_SIMPLE_DEBUG_MODE
        LBS,  ///< @if zh 关键字@c lbs @else Keyword @c lbs @endif
#endif

        // 六元指令
        LBNS  ///< @if zh 关键字@c lbns @else Keyword @c lbns @endif
    };

    extern const std::unordered_map<std::string, TokenType> OTHER_KEYWORD_MAP;

    extern const std::unordered_map<std::string, InstructionKeyword> INS_KEYWORD_MAP;

    extern const BiMap<TokenType, std::string> SYMBOL_MAP;

    extern const std::unordered_map<char, char> WHITESPACE_MAP;

    extern const std::string SYMBOLS;

    /**
     * @if zh
     *
     * @struct Token
     * @brief 词法标记结构
     * @details 表示IC10源代码中的一个词法单元,包含标记类型、位置、词素和类别信息
     *
     * @elseif en
     *
     * @struct Token
     * @brief Lexical token structure
     * @details Represents a lexical unit in IC10 source code, containing token type, position,
     *        lexeme and category information
     *
     * @endif
     */
    struct Token {
        /**
         * @if zh
         * @brief 标记类型
         * @elseif en
         * @brief Token type
         * @endif
         */
        TokenType type = TokenType::UNKNOWN;

        /**
         * @if zh
         * @brief 标记起始位置
         * @elseif en
         * @brief Token start position
         * @endif
         */
        Pos pos;

        /**
         * @if zh
         * @brief 标记词素(原始文本)
         * @elseif en
         * @brief Token lexeme (raw text)
         * @endif
         */
        std::string lexeme = "";

        /**
         * @if zh
         * @brief 标记类别
         * @elseif en
         * @brief Token category
         * @endif
         */
        TokenCategory category = TokenCategory::INVALID;

        std::optional<InstructionKeyword> keyword = std::nullopt;

        /**
         * @if zh
         *
         * @brief 转换为字符串表示
         * @return 人类可读的字符串描述
         *
         * @elseif en
         *
         * @brief Convert to string representation
         * @return Human-readable string description
         *
         * @endif
         */
        [[nodiscard]] std::string toString() const;

        /**
         * @if zh
         *
         * @brief 转换为JSON格式
         * @return JSON格式的字符串
         *
         * @elseif en
         *
         * @brief Convert to JSON format
         * @return String in JSON format
         *
         * @endif
         */
        [[nodiscard]] std::string toJSON() const;

        /**
         * @if zh
         *
         * @brief 流输出运算符
         * @param os 输出流
         * @param token Token引用
         * @return 输出流引用
         *
         * @elseif en
         *
         * @brief Stream output operator
         * @param os Output stream
         * @param token Token reference
         * @return Output stream reference
         *
         * @endif
         */
        friend std::ostream &operator<<(std::ostream &os, const Token &token);
    };

    /**
     * @if zh
     *
     * @brief 流输出运算符声明
     * @param os 输出流
     * @param token Token引用
     * @return 输出流引用
     *
     * @elseif en
     *
     * @brief Stream output operator declaration
     * @param os Output stream
     * @param token Token reference
     * @return Output stream reference
     *
     * @endif
     */
    std::ostream &operator<<(std::ostream &os, const Token &token);

    /**
     * @if zh
     *
     * @brief 获取Token的结束位置
     * @param token 输入的Token
     * @return Token结束后的位置
     *
     * @elseif en
     *
     * @brief Get end position of Token
     * @param token Input token
     * @return Position after token ends
     *
     * @endif
     */
    Pos endPos(const Token &token);

}  // namespace stationeers::ic10

#endif  // COMPILER_TOKEN_HPP
