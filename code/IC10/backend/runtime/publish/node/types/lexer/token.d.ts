// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file token.d.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/22 15:36
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {Pos} from "../common";


/**
 * IC10 词法标记类型枚举。
 *
 * 对应 C++ `ic10::TokenType`，值从 0 开始递增。
 * 运行时由原生模块导出为不可变对象，可在 JS 层直接使用 `TokenType.FLOAT` 等访问。
 *
 * @public
 */
export enum TokenType {
    /* 数字 */
    INTEGER = 0,
    FLOAT,
    HEX_NUMBER,
    BINARY_NUMBER,
    STRING,

    /* 变量名 */
    IDENTIFIER,

    /* 寄存器设备 */
    REGISTER,
    DEVICE,

    /* 符号 */
    LPAREN,
    RPAREN,
    COLON,

    /* 注释 */
    HEX_COMMENT,
    SLASH_COMMENT,
    DOC_COMMENT,
    TYPE_HINT,

    /* 换行 */
    NEWLINE,

    /* 关键字 - 宏与函数 */
    KEYWORD_HASH,
    KEYWORD_STR,

    /* 关键字 - 常量 */
    KEYWORD_NAN,
    KEYWORD_PINF,
    KEYWORD_NINF,
    KEYWORD_PI,
    KEYWORD_TAU,
    KEYWORD_DEG2RAD,
    KEYWORD_RAD2DEG,
    KEYWORD_EPSILON,
    KEYWORD_RGAS,

    /* 文件结束标记 */
    END,

    /* 未知标记 */
    UNKNOWN,

    /* 关键字 - 空指令 */
    KEYWORD_HCF,
    KEYWORD_YIELD,

    /* 关键字 - 预处理指令 */
    KEYWORD_ALIAS,
    KEYWORD_DEFINE,

    /* 关键字 - 一元指令 */
    KEYWORD_PEEK,
    KEYWORD_POP,
    KEYWORD_PUSH,
    KEYWORD_CLR,
    KEYWORD_J,
    KEYWORD_JAL,
    KEYWORD_JR,
    KEYWORD_RAND,
    KEYWORD_SLEEP,
    KEYWORD_CLRD,

    /* 关键字 - 二元指令 */
    KEYWORD_ABS,
    KEYWORD_ACOS,
    KEYWORD_ADD,
    KEYWORD_ASIN,
    KEYWORD_ATAN,
    KEYWORD_ATAN2,
    KEYWORD_CEIL,
    KEYWORD_COS,
    KEYWORD_DIV,
    KEYWORD_EXP,
    KEYWORD_FLOOR,
    KEYWORD_LOG,
    KEYWORD_MAX,
    KEYWORD_MIN,
    KEYWORD_MOD,
    KEYWORD_MUL,
    KEYWORD_POW,
    KEYWORD_ROUND,
    KEYWORD_SIN,
    KEYWORD_SQRT,
    KEYWORD_SGN,
    KEYWORD_SUB,
    KEYWORD_TAN,
    KEYWORD_TRUNC,
    KEYWORD_NOT,
    KEYWORD_MOVE,
    KEYWORD_POKE,
    KEYWORD_BEQZ,
    KEYWORD_BEQZAL,
    KEYWORD_BNEZ,
    KEYWORD_BNEZAL,
    KEYWORD_BGEZ,
    KEYWORD_BGEZAL,
    KEYWORD_BGTZ,
    KEYWORD_BGTZAL,
    KEYWORD_BLEZ,
    KEYWORD_BLEZAL,
    KEYWORD_BLTZ,
    KEYWORD_BLTZAL,
    KEYWORD_BNAN,
    KEYWORD_BDNS,
    KEYWORD_BDNSAL,
    KEYWORD_BDSE,
    KEYWORD_BDSEAL,
    KEYWORD_BREQZ,
    KEYWORD_BRNEZ,
    KEYWORD_BRGEZ,
    KEYWORD_BRGTZ,
    KEYWORD_BRLEZ,
    KEYWORD_BRLTZ,
    KEYWORD_BRNAN,
    KEYWORD_BRDNS,
    KEYWORD_BRDSE,
    KEYWORD_SEQZ,
    KEYWORD_SNEZ,
    KEYWORD_SGEZ,
    KEYWORD_SGTZ,
    KEYWORD_SLEZ,
    KEYWORD_SLTZ,
    KEYWORD_SNAN,
    KEYWORD_SNANZ,
    KEYWORD_SDNS,
    KEYWORD_SDSE,

    /* 关键字 - 三元指令 */
    KEYWORD_AND,
    KEYWORD_NOR,
    KEYWORD_OR,
    KEYWORD_SLA,
    KEYWORD_SLL,
    KEYWORD_SRA,
    KEYWORD_SRL,
    KEYWORD_XOR,
    KEYWORD_GET,
    KEYWORD_PUT,
    KEYWORD_L,
    KEYWORD_LS,
    KEYWORD_LR,
    KEYWORD_S,
    KEYWORD_SB,
    KEYWORD_ROL,
    KEYWORD_ROR,
    KEYWORD_RMAP,
    KEYWORD_BEQ,
    KEYWORD_BEQAL,
    KEYWORD_BNE,
    KEYWORD_BNEAL,
    KEYWORD_BGE,
    KEYWORD_BGEAL,
    KEYWORD_BGT,
    KEYWORD_BGTAL,
    KEYWORD_BLE,
    KEYWORD_BLEAL,
    KEYWORD_BLT,
    KEYWORD_BLTAL,
    KEYWORD_BAPZ,
    KEYWORD_BAPZAL,
    KEYWORD_BNAZ,
    KEYWORD_BNAZAL,
    KEYWORD_BDNVL,
    KEYWORD_BDNVS,
    KEYWORD_BREQ,
    KEYWORD_BRNE,
    KEYWORD_BRGE,
    KEYWORD_BRGT,
    KEYWORD_BRLE,
    KEYWORD_BRLT,
    KEYWORD_BRAPZ,
    KEYWORD_BRNAZ,
    KEYWORD_SAPZ,
    KEYWORD_SNAZ,
    KEYWORD_SEQ,
    KEYWORD_SNE,
    KEYWORD_SGE,
    KEYWORD_SGT,
    KEYWORD_SLE,
    KEYWORD_SLT,

    /* 关键字 - 四元指令 */
    KEYWORD_LERP,
    KEYWORD_CLAMP,
    KEYWORD_EXT,
    KEYWORD_INS,
    KEYWORD_SS,
    KEYWORD_LB,
    KEYWORD_SBN,
    KEYWORD_SBS,
    KEYWORD_BAP,
    KEYWORD_BAPAL,
    KEYWORD_BNA,
    KEYWORD_BNAAL,
    KEYWORD_BRAP,
    KEYWORD_BRNA,
    KEYWORD_SAP,
    KEYWORD_SNA,
    KEYWORD_SELECT,

    /* 关键字 - 五元指令 */
    KEYWORD_LBN,
    KEYWORD_LBS,

    /* 关键字 - 六元指令 */
    KEYWORD_LBNS
}


/**
 * IC10 词法标记类别枚举。
 *
 * 对应 C++ `ic10::TokenCategory`，值从 0 开始递增。
 * 运行时由 C++ 端通过 `enum_to_str` 编译期反射导出，
 * 保证与 C++ 枚举定义自动同步。
 *
 * @public
 */
export enum TokenCategory {
    /** 字面值（数字、字符串、关键字等） */
    LITERAL = 0,
    /** 符号（括号、冒号等） */
    SYMBOL,
    /** 注释 */
    COMMENT,
    /** 注解（文档注释、类型提示） */
    ANNOTATION,
    /** 空白符 */
    WHITESPACE,
    /** 文件结束 */
    END,
    /** 无效标记 */
    INVALID
}


/**
 * @summary 词法单元类
 *
 * @desc 表示 IC10 源代码中的最小语义单元。
 * 词法单元包括关键字（如 `move`、`add`）、标识符、数字、运算符等。
 *
 * Token 是词法分析的结果，作为语法分析器的输入。
 *
 * @public
 */
export class Token {
    /**
     * @summary 构造函数
     *
     * @param type - Token 类型（对应 ic10::TokenType 枚举）
     * @param pos - 位置对象，记录该 Token 在源代码中的位置
     * @param lexeme - 词素，即源代码中的原始字符串
     * @param category - Token 类别（对应 ic10::TokenCategory 枚举，可选）
     *
     * @example
     * ```typescript
     * const pos = new ic10.Pos();
     * pos.line = 0;
     * pos.column = 0;
     * const token = new ic10.Token(TokenType.KEYWORD_MOVE, pos, 'move', TokenCategory.LITERAL);
     * ```
     */
    constructor(type: TokenType, pos: Pos, lexeme?: string, category?: TokenCategory);

    /**
     * @summary Token 类型
     * @desc 对应 ic10::TokenType 枚举值，表示 Token 的语法类别
     */
    type: TokenType;

    /**
     * @summary 位置信息
     * @desc 该 Token 在源代码中的起始位置
     */
    pos: Pos;

    /**
     * @summary 词素
     * @desc 源代码中匹配该 Token 的原始字符串
     */
    lexeme: string;

    /**
     * @summary Token 类别
     * @desc 对应 ic10::TokenCategory 枚举值，提供更细粒度的分类
     */
    category: TokenCategory;

    /**
     * @summary 返回可读字符串表示
     *
     * @returns 格式化的字符串，如 `Token{type=1, lexeme="move"}`
     *
     * @desc 用于调试目的，返回人类可读的 Token 描述。
     */
    toString(): string;

    /**
     * @summary 返回 JSON 字符串表示
     *
     * @returns JSON 格式的 Token 表示
     *
     * @desc 返回包含 type、pos、lexeme、category 的 JSON 对象字符串。
     */
    toJSON(): string;
}