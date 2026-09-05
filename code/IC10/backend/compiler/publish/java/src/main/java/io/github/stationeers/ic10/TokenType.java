// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package io.github.stationeers.ic10;

/**
 * Token type classification for IC10 lexer tokens.
 * <p>
 * Mirrors the C++ {@code enum class TokenType} in {@code ic10/lexer/token.hpp}.
 * Values are contiguous starting from 0, matching the C++ enum layout exactly.
 * Use {@link #getValue()} to compare with the integer returned by {@link Token#getType()}.
 * <p>
 * Instruction keywords are NOT separate token types: they are a single
 * {@link #KEYWORD} type whose specific instruction is carried by
 * {@link Token#getKeyword()} ({@link InstructionKeyword}).
 *
 * @author edocsitahw
 * @version 2.0.0
 */
public enum TokenType {
    // ---- 数字 ----
    INTEGER(0),
    FLOAT(1),
    HEX_NUMBER(2),
    BINARY_NUMBER(3),
    STRING(4),

    // ---- 变量名 ----
    IDENTIFIER(5),

    // ---- 寄存器设备 ----
    REGISTER(6),
    DEVICE(7),

    // ---- 符号 ----
    LPAREN(8),
    RPAREN(9),
    COLON(10),
    DOT(11),
    SUB(12),
    DIV(13),

    // ---- 注释 ----
    HEX_COMMENT(14),
    SLASH_COMMENT(15),

    // ---- 换行 ----
    NEWLINE(16),

    // ---- 关键字 ----
    KEYWORD(17),
    KEYWORD_HASH(18),
    KEYWORD_STR(19),
    KEYWORD_ALIAS(20),
    KEYWORD_DEFINE(21),

    // ---- 文件结束 ----
    END(22),

    // ---- 未知标记 ----
    UNKNOWN(23),

    // ---- 注解前缀 ----
    TYPE_HINT_PREFIX(24),
    TYPE_ANNOTATION_PREFIX(25),
    TAG(26);

    private final int value;

    TokenType(int value) {
        this.value = value;
    }

    /**
     * Returns the integer value matching the C++ enum value.
     *
     * @return the numeric value of this token type
     */
    public int getValue() {
        return value;
    }

    /**
     * Resolves a {@link TokenType} from its integer value.
     *
     * @param value the integer value (0-26)
     * @return the matching token type
     * @throws IllegalArgumentException if the value is out of range
     */
    public static TokenType fromValue(int value) {
        TokenType[] all = values();
        if (value >= 0 && value < all.length)
            return all[value];

        throw new IllegalArgumentException("Invalid TokenType value: " + value);
    }
}
