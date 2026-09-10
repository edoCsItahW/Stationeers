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
    DOT,
    SUB,
    DIV,

    /* 注释 */
    HEX_COMMENT,
    SLASH_COMMENT,

    /* 换行 */
    NEWLINE,

    /* 关键字 */
    KEYWORD,
    KEYWORD_HASH,
    KEYWORD_STR,
    KEYWORD_ALIAS,
    KEYWORD_DEFINE,

    /* 文件结束标记 */
    END,

    /* 未知标记 */
    UNKNOWN,

    /* 类型提示/注解前缀与标签 */
    TYPE_HINT_PREFIX,
    TYPE_ANNOTATION_PREFIX,
    TAG
}

/**
 * IC10 指令关键字枚举。
 *
 * 对应 C++ `ic10::InstructionKeyword`，值从 0 开始递增。
 * 指令关键字不再拥有独立的 TokenType，而是统一由 `TokenType.KEYWORD` 承载，
 * 具体指令通过 `Token.keyword` 字段（`InstructionKeyword`）区分。
 *
 * @public
 */
export enum InstructionKeyword {
    /* 空指令 */
    HCF = 0,
    YIELD,

    /* 一元指令 */
    PEEK,
    POP,
    PUSH,
    CLR,
    J,
    JAL,
    JR,
    RAND,
    SLEEP,
    CLRD,

    /* 二元指令 */
    ABS,
    ACOS,
    ASIN,
    ATAN,
    ATAN2,
    CEIL,
    COS,
    DIV,
    EXP,
    FLOOR,
    LOG,
    MAX,
    MIN,
    MOD,
    MUL,
    POW,
    ROUND,
    SIN,
    SQRT,
    SGN,
    SUB,
    TAN,
    TRUNC,
    NOT,
    MOVE,
    POKE,
    BEQZ,
    BEQZAL,
    BNEZ,
    BNEZAL,
    BGEZ,
    BGEZAL,
    BGTZ,
    BGTZAL,
    BLEZ,
    BLEZAL,
    BLTZ,
    BLTZAL,
    BNAN,
    BDNS,
    BDNSAL,
    BDSE,
    BDSEAL,
    BREQZ,
    BRGEZ,
    BRGTZ,
    BRLEZ,
    BRLTZ,
    BRNAN,
    BRNEZ,
    BRDNS,
    BRDSE,
    SEQZ,
    SNEZ,
    SGEZ,
    SGTZ,
    SLEZ,
    SLTZ,
    SNAN,
    SNANZ,
    SDNS,
    SDSE,

    /* 三元指令 */
    ADD,
    AND,
    NOR,
    OR,
    SLA,
    SLL,
    SRA,
    SRL,
    XOR,
    GET,
    PUT,
    L,
    LS,
    LR,
    S,
    SB,
    ROL,
    ROR,
    RMAP,
    BEQ,
    BEQAL,
    BNE,
    BNEAL,
    BGE,
    BGEAL,
    BGT,
    BGTAL,
    BLE,
    BLEAL,
    BLT,
    BLTAL,
    BAPZ,
    BAPZAL,
    BNAZ,
    BNAZAL,
    BDNVL,
    BDNVS,
    BREQ,
    BRNE,
    BRGE,
    BRGT,
    BRLE,
    BRLT,
    BRAPZ,
    BRNAZ,
    SAPZ,
    SNAZ,
    SEQ,
    SNE,
    SGE,
    SGT,
    SLE,
    SLT,

    /* 四元指令 */
    CLAMP,
    LERP,
    EXT,
    INS,
    SS,
    LB,
    SBN,
    SBS,
    BAP,
    BAPAL,
    BNA,
    BNAAL,
    BRAP,
    BRNA,
    SAP,
    SNA,
    SELECT,

    /* 五元指令 */
    LBN,
    LBS,

    /* 六元指令 */
    LBNS
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
     * const token = new ic10.Token(TokenType.KEYWORD, pos, 'move', TokenCategory.LITERAL, InstructionKeyword.MOVE);
     * ```
     */
    constructor(
        type: TokenType,
        pos: Pos,
        lexeme?: string,
        category?: TokenCategory,
        keyword?: InstructionKeyword
    );

    /**
     * @summary Token 类型
     * @desc 对应 ic10::TokenType 枚举值，表示 Token 的语法类别
     */
    type: TokenType;

    /**
     * @summary 指令关键字（可选）
     * @desc 当 type 为 TokenType.KEYWORD 时，此字段表示具体的指令关键字
     *       （对应 ic10::InstructionKeyword 枚举值），其余情况为 undefined。
     */
    keyword?: InstructionKeyword;

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