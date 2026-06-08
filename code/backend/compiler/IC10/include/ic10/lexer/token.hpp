// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file token.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 22:00
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
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

        /* 注释 */
        HEX_COMMENT,    ///< @if zh #号注释 @else Hexadecimal @endif
        SLASH_COMMENT,  ///< @if zh //号注释 @else Slash @endif

        /* 换行 */
        NEWLINE,  ///< @if zh 换行符 @else Newline @endif

        /* 关键字 */
        // 宏与函数
        KEYWORD_HASH,  ///< @if zh 关键字@c HASH @else Keyword @c HASH @endif
        KEYWORD_STR,   ///< @if zh 关键字@c STR @else Keyword @c STR @endif

        // 常量
        KEYWORD_NAN,           ///< @if zh 关键字@c nan @else Keyword @c nan @endif
        KEYWORD_PINF,          ///< @if zh 关键字@c pinf @else Keyword @c pinf @endif
        KEYWORD_NINF,          ///< @if zh 关键字@c ninf @else Keyword @c ninf @endif
        KEYWORD_PI,            ///< @if zh 关键字@c pi @else Keyword @c pi @endif
        KEYWORD_TAU,           ///< @if zh 关键字@c tau @else Keyword @c tau @endif
        KEYWORD_DEG2RAD,       ///< @if zh 关键字@c deg2rad @else Keyword @c deg2rad @endif
        KEYWORD_RAD2DEG,       ///< @if zh 关键字@c rad2deg @else Keyword @c rad2deg @endif
        KEYWORD_EPSILON,       ///< @if zh 关键字@c epsilon @else Keyword @c epsilon @endif
        KEYWORD_GAS_CONSTANT,  ///< @if zh 关键字@c 19as @else Keyword @c 19as @endif

        // 文件结束标记
        END,  ///< @if zh 文件结束标记 @else End of file @endif

        // 未知标记（用于错误处理）
        UNKNOWN,  ///< @if zh 未知标记 @else Unknown token @endif

        // 空指令
        KEYWORD_HCF,    ///< @if zh 关键字@c hcf @else Keyword @c hcf @endif
        KEYWORD_YIELD,  ///< @if zh 关键字@c yield @else Keyword @c yield @endif

        // 预处理指令
        KEYWORD_ALIAS,   ///< @if zh 关键字@c alias @else Keyword @c alias @endif
        KEYWORD_DEFINE,  ///< @if zh 关键字@c define @else Keyword @c define @endif

        // 一元指令
        KEYWORD_PEEK,   ///< @if zh 关键字@c peek @else Keyword @c peek @endif
        KEYWORD_POP,    ///< @if zh 关键字@c pop @else Keyword @c pop @endif
        KEYWORD_PUSH,   ///< @if zh 关键字@c push @else Keyword @c push @endif
        KEYWORD_CLR,    ///< @if zh 关键字@c clr @else Keyword @c clr @endif
        KEYWORD_J,      ///< @if zh 关键字@c j @else Keyword @c j @endif
        KEYWORD_JAL,    ///< @if zh 关键字@c jal @else Keyword @c jal @endif
        KEYWORD_JR,     ///< @if zh 关键字@c jr @else Keyword @c jr @endif
        KEYWORD_RAND,   ///< @if zh 关键字@c rand @else Keyword @c rand @endif
        KEYWORD_SLEEP,  ///< @if zh 关键字@c sleep @else Keyword @c sleep @endif

        // 二元指令
        KEYWORD_ABS,     ///< @if zh 关键字@c abs @else Keyword @c abs @endif
        KEYWORD_ACOS,    ///< @if zh 关键字@c acos @else Keyword @c acos @endif
        KEYWORD_ADD,     ///< @if zh 关键字@c add @else Keyword @c add @endif
        KEYWORD_ASIN,    ///< @if zh 关键字@c asin @else Keyword @c asin @endif
        KEYWORD_ATAN,    ///< @if zh 关键字@c atan @else Keyword @c atan @endif
        KEYWORD_ATAN2,   ///< @if zh 关键字@c atan2 @else Keyword @c atan2 @endif
        KEYWORD_CEIL,    ///< @if zh 关键字@c ceil @else Keyword @c ceil @endif
        KEYWORD_COS,     ///< @if zh 关键字@c cos @else Keyword @c cos @endif
        KEYWORD_DIV,     ///< @if zh 关键字@c div @else Keyword @c div @endif
        KEYWORD_EXP,     ///< @if zh 关键字@c exp @else Keyword @c exp @endif
        KEYWORD_FLOOR,   ///< @if zh 关键字@c floor @else Keyword @c floor @endif
        KEYWORD_LOG,     ///< @if zh 关键字@c log @else Keyword @c log @endif
        KEYWORD_MAX,     ///< @if zh 关键字@c max @else Keyword @c max @endif
        KEYWORD_MIN,     ///< @if zh 关键字@c min @else Keyword @c min @endif
        KEYWORD_MOD,     ///< @if zh 关键字@c mod @else Keyword @c mod @endif
        KEYWORD_MUL,     ///< @if zh 关键字@c mul @else Keyword @c mul @endif
        KEYWORD_POW,     ///< @if zh 关键字@c pow @else Keyword @c pow @endif
        KEYWORD_ROUND,   ///< @if zh 关键字@c round @else Keyword @c round @endif
        KEYWORD_SIN,     ///< @if zh 关键字@c sin @else Keyword @c sin @endif
        KEYWORD_SQRT,    ///< @if zh 关键字@c sqrt @else Keyword @c sqrt @endif
        KEYWORD_SUB,     ///< @if zh 关键字@c sub @else Keyword @c sub @endif
        KEYWORD_TAN,     ///< @if zh 关键字@c tan @else Keyword @c tan @endif
        KEYWORD_TRUNC,   ///< @if zh 关键字@c trunc @else Keyword @c trunc @endif
        KEYWORD_NOT,     ///< @if zh 关键字@c not @else Keyword @c not @endif
        KEYWORD_MOVE,    ///< @if zh 关键字@c move @else Keyword @c move @endif
        KEYWORD_POKE,    ///< @if zh 关键字@c poke @else Keyword @c poke @endif
        KEYWORD_BEQZ,    ///< @if zh 关键字@c beqz @else Keyword @c beqz @endif
        KEYWORD_BEQZAL,  ///< @if zh 关键字@c beqzal @else Keyword @c beqzal @endif
        KEYWORD_BNEZ,    ///< @if zh 关键字@c bnez @else Keyword @c bnez @endif
        KEYWORD_BNEZAL,  ///< @if zh 关键字@c bnezal @else Keyword @c bnezal @endif
        KEYWORD_BGEZ,    ///< @if zh 关键字@c bgez @else Keyword @c bgez @endif
        KEYWORD_BGEZAL,  ///< @if zh 关键字@c bgezal @else Keyword @c bgezal @endif
        KEYWORD_BGTZ,    ///< @if zh 关键字@c bgtz @else Keyword @c bgtz @endif
        KEYWORD_BGTZAL,  ///< @if zh 关键字@c bgtzal @else Keyword @c bgtzal @endif
        KEYWORD_BLEZ,    ///< @if zh 关键字@c blez @else Keyword @c blez @endif
        KEYWORD_BLEZAL,  ///< @if zh 关键字@c blezal @else Keyword @c blezal @endif
        KEYWORD_BLTZ,    ///< @if zh 关键字@c bltz @else Keyword @c bltz @endif
        KEYWORD_BLTZAL,  ///< @if zh 关键字@c bltzal @else Keyword @c bltzal @endif
        KEYWORD_BNAN,    ///< @if zh 关键字@c bnan @else Keyword @c bnan @endif
        KEYWORD_BNANZ,   ///< @if zh 关键字@c bnanz @else Keyword @c bnanz @endif
        KEYWORD_BDNS,    ///< @if zh 关键字@c bdns @else Keyword @c bdns @endif
        KEYWORD_BDNSAL,  ///< @if zh 关键字@c bdnsal @else Keyword @c bdnsal @endif
        KEYWORD_BDSE,    ///< @if zh 关键字@c bdse @else Keyword @c bdse @endif
        KEYWORD_BDSEAL,  ///< @if zh 关键字@c bdseal @else Keyword @c bdseal @endif
        KEYWORD_BREQZ,   ///< @if zh 关键字@c breqz @else Keyword @c breqz @endif
        KEYWORD_BRNZ,    ///< @if zh 关键字@c brnz @else Keyword @c brnz @endif
        KEYWORD_BRGEZ,   ///< @if zh 关键字@c brgez @else Keyword @c brgez @endif
        KEYWORD_BRGTZ,   ///< @if zh 关键字@c brgtz @else Keyword @c brgtz @endif
        KEYWORD_BRLEZ,   ///< @if zh 关键字@c brlez @else Keyword @c brlez @endif
        KEYWORD_BRLTZ,   ///< @if zh 关键字@c brltz @else Keyword @c brltz @endif
        KEYWORD_BRNAN,   ///< @if zh 关键字@c brnan @else Keyword @c brnan @endif
        KEYWORD_BRDNS,   ///< @if zh 关键字@c brdns @else Keyword @c brdns @endif
        KEYWORD_BRDSE,   ///< @if zh 关键字@c brdse @else Keyword @c brdse @endif
        KEYWORD_SEQZ,    ///< @if zh 关键字@c seqz @else Keyword @c seqz @endif
        KEYWORD_SNEZ,    ///< @if zh 关键字@c snez @else Keyword @c snez @endif
        KEYWORD_SGEZ,    ///< @if zh 关键字@c sgez @else Keyword @c sgez @endif
        KEYWORD_SGTZ,    ///< @if zh 关键字@c sgtz @else Keyword @c sgtz @endif
        KEYWORD_SLEZ,    ///< @if zh 关键字@c slez @else Keyword @c slez @endif
        KEYWORD_SLTZ,    ///< @if zh 关键字@c sltz @else Keyword @c sltz @endif
        KEYWORD_SNAN,    ///< @if zh 关键字@c snan @else Keyword @c snan @endif
        KEYWORD_SNANZ,   ///< @if zh 关键字@c snanz @else Keyword @c snanz @endif
        KEYWORD_SDNS,    ///< @if zh 关键字@c sdns @else Keyword @c sdns @endif
        KEYWORD_SDSE,    ///< @if zh 关键字@c sdse @else Keyword @c sdse @endif

        // 三元指令
        KEYWORD_AND,     ///< @if zh 关键字@c and @else Keyword @c and @endif
        KEYWORD_NOR,     ///< @if zh 关键字@c nor @else Keyword @c nor @endif
        KEYWORD_OR,      ///< @if zh 关键字@c or @else Keyword @c or @endif
        KEYWORD_SLA,     ///< @if zh 关键字@c sla @else Keyword @c sla @endif
        KEYWORD_SLL,     ///< @if zh 关键字@c sll @else Keyword @c sll @endif
        KEYWORD_SRA,     ///< @if zh 关键字@c sra @else Keyword @c sra @endif
        KEYWORD_SRL,     ///< @if zh 关键字@c srl @else Keyword @c srl @endif
        KEYWORD_XOR,     ///< @if zh 关键字@c xor @else Keyword @c xor @endif
        KEYWORD_GET,     ///< @if zh 关键字@c get @else Keyword @c get @endif
        KEYWORD_PUT,     ///< @if zh 关键字@c put @else Keyword @c put @endif
        KEYWORD_L,       ///< @if zh 关键字@c l @else Keyword @c l @endif
        KEYWORD_LS,      ///< @if zh 关键字@c ls @else Keyword @c ls @endif
        KEYWORD_LR,      ///< @if zh 关键字@c lr @else Keyword @c lr @endif
        KEYWORD_S,       ///< @if zh 关键字@c s @else Keyword @c s @endif
        KEYWORD_SB,      ///< @if zh 关键字@c sb @else Keyword @c sb @endif
        KEYWORD_RMAP,    ///< @if zh 关键字@c rmap @else Keyword @c rmap @endif
        KEYWORD_BEQ,     ///< @if zh 关键字@c beq @else Keyword @c beq @endif
        KEYWORD_BEQAL,   ///< @if zh 关键字@c beqal @else Keyword @c beqal @endif
        KEYWORD_BNE,     ///< @if zh 关键字@c bne @else Keyword @c bne @endif
        KEYWORD_BNEAL,   ///< @if zh 关键字@c bneal @else Keyword @c bneal @endif
        KEYWORD_BGE,     ///< @if zh 关键字@c bge @else Keyword @c bge @endif
        KEYWORD_BGEAL,   ///< @if zh 关键字@c bgeal @else Keyword @c bgeal @endif
        KEYWORD_BGT,     ///< @if zh 关键字@c bgt @else Keyword @c bgt @endif
        KEYWORD_BGTAL,   ///< @if zh 关键字@c bgtal @else Keyword @c bgtal @endif
        KEYWORD_BLE,     ///< @if zh 关键字@c ble @else Keyword @c ble @endif
        KEYWORD_BLEAL,   ///< @if zh 关键字@c bleal @else Keyword @c bleal @endif
        KEYWORD_BLT,     ///< @if zh 关键字@c blt @else Keyword @c blt @endif
        KEYWORD_BLTAL,   ///< @if zh 关键字@c bltal @else Keyword @c bltal @endif
        KEYWORD_BAPZ,    ///< @if zh 关键字@c bapz @else Keyword @c bapz @endif
        KEYWORD_BAPZAL,  ///< @if zh 关键字@c bapzal @else Keyword @c bapzal @endif
        KEYWORD_BNAZ,    ///< @if zh 关键字@c bnaz @else Keyword @c bnaz @endif
        KEYWORD_BNAZAL,  ///< @if zh 关键字@c bnazal @else Keyword @c bnazal @endif
        KEYWORD_BDNVL,   ///< @if zh 关键字@c bdnvl @else Keyword @c bdnvl @endif
        KEYWORD_BDNVS,   ///< @if zh 关键字@c bdnvs @else Keyword @c bdnvs @endif
        KEYWORD_BREQ,    ///< @if zh 关键字@c breq @else Keyword @c breq @endif
        KEYWORD_BRNE,    ///< @if zh 关键字@c brne @else Keyword @c brne @endif
        KEYWORD_BRGE,    ///< @if zh 关键字@c brge @else Keyword @c brge @endif
        KEYWORD_BRGT,    ///< @if zh 关键字@c brgt @else Keyword @c brgt @endif
        KEYWORD_BRLE,    ///< @if zh 关键字@c brle @else Keyword @c brle @endif
        KEYWORD_BRLT,    ///< @if zh 关键字@c brlt @else Keyword @c brlt @endif
        KEYWORD_BRAPZ,   ///< @if zh 关键字@c brapz @else Keyword @c brapz @endif
        KEYWORD_BRNAZ,   ///< @if zh 关键字@c brnaz @else Keyword @c brnaz @endif
        KEYWORD_SAPZ,    ///< @if zh 关键字@c sapz @else Keyword @c sapz @endif
        KEYWORD_SNAZ,    ///< @if zh 关键字@c snaz @else Keyword @c snaz @endif
        KEYWORD_SEQ,     ///< @if zh 关键字@c seq @else Keyword @c seq @endif
        KEYWORD_SNE,     ///< @if zh 关键字@c sne @else Keyword @c sne @endif
        KEYWORD_SGE,     ///< @if zh 关键字@c sge @else Keyword @c sge @endif
        KEYWORD_SGT,     ///< @if zh 关键字@c sgt @else Keyword @c sgt @endif
        KEYWORD_SLE,     ///< @if zh 关键字@c sle @else Keyword @c sle @endif
        KEYWORD_SLT,     ///< @if zh 关键字@c slt @else Keyword @c slt @endif

        // 四元指令
        KEYWORD_LERP,    ///< @if zh 关键字@c lerp @else Keyword @c lerp @endif
        KEYWORD_EXT,     ///< @if zh 关键字@c ext @else Keyword @c ext @endif
        KEYWORD_INS,     ///< @if zh 关键字@c ins @else Keyword @c ins @endif
        KEYWORD_SS,      ///< @if zh 关键字@c ss @else Keyword @c ss @endif
        KEYWORD_LB,      ///< @if zh 关键字@c lb @else Keyword @c lb @endif
        KEYWORD_SBN,     ///< @if zh 关键字@c sbn @else Keyword @c sbn @endif
        KEYWORD_SBS,     ///< @if zh 关键字@c sbs @else Keyword @c sbs @endif
        KEYWORD_BAP,     ///< @if zh 关键字@c bap @else Keyword @c bap @endif
        KEYWORD_BAPAL,   ///< @if zh 关键字@c bapal @else Keyword @c bapal @endif
        KEYWORD_BNA,     ///< @if zh 关键字@c bna @else Keyword @c bna @endif
        KEYWORD_BNAAL,   ///< @if zh 关键字@c bnaal @else Keyword @c bnaal @endif
        KEYWORD_BRAP,    ///< @if zh 关键字@c brap @else Keyword @c brap @endif
        KEYWORD_BRNA,    ///< @if zh 关键字@c brna @else Keyword @c brna @endif
        KEYWORD_SAP,     ///< @if zh 关键字@c sap @else Keyword @c sap @endif
        KEYWORD_SNA,     ///< @if zh 关键字@c sna @else Keyword @c sna @endif
        KEYWORD_SELECT,  ///< @if zh 关键字@c select @else Keyword @c select @endif

        // 五元指令
        KEYWORD_LBN,  ///< @if zh 关键字@c lbn @else Keyword @c lbn @endif
        KEYWORD_LBS,  ///< @if zh 关键字@c lbs @else Keyword @c lbs @endif

        // 六元指令
        KEYWORD_LBNS  ///< @if zh 关键字@c lbns @else Keyword @c lbns @endif
    };

    enum class TokenCategory {
        LITERAL,     ///< @if zh 字面值 @else Literal @endif
        SYMBOL,      ///< @if zh 符号 @else Symbol @endif
        COMMENT,     ///< @if zh 注释 @else Comment @endif
        WHITESPACE,  ///< @if zh 空白符 @else Whitespace @endif
        END,         ///< @if zh 文件结束 @else End of file @endif
        INVALID      ///< @if zh 无效标记 @else Invalid token @endif
    };

    extern const BiMap<TokenType, std::string> KEYWORD_MAP;

    extern const BiMap<TokenType, std::string> SYMBOL_MAP;

    extern const std::unordered_map<char, char> WHITESPACE_MAP;

    extern const std::string SYMBOLS;

    struct Token {
        TokenType type = TokenType::UNKNOWN;

        Pos pos;

        std::string lexeme = "";

        TokenCategory category = TokenCategory::INVALID;

        [[nodiscard]] std::string toString() const;

        [[nodiscard]] std::string toJSON() const;

        friend std::ostream &operator<<(std::ostream &os, const Token &token);
    };

    std::ostream &operator<<(std::ostream &os, const Token &token);

    Pos endPos(const Token &token);

}  // namespace ic10

#endif  // COMPILER_TOKEN_HPP
