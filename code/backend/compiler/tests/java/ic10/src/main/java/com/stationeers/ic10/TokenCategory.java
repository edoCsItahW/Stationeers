// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package com.stationeers.ic10;

/**
 * Token category classification.
 * <p>
 * Mirrors the C++ {@code enum class TokenCategory} in {@code ic10/lexer/token.hpp}.
 * Values are contiguous starting from 0, matching the C++ enum layout exactly.
 * Use {@link #getValue()} to compare with the integer returned by {@link Token#getCategory()}.
 *
 * @author edocsitahw
 * @version 1.0.0
 * @since 1.0.0
 */
public enum TokenCategory {
    /** Literal value (number, string) */
    LITERAL(0),
    /** Symbol (parenthesis, colon, etc.) */
    SYMBOL(1),
    /** Comment (# or //) */
    COMMENT(2),
    /** Annotation (doc comment #&gt; or type hint #:) */
    ANNOTATION(3),
    /** Whitespace (newline) */
    WHITESPACE(4),
    /** End of file */
    END(5),
    /** Invalid token */
    INVALID(6);

    private final int value;

    TokenCategory(int value) {
        this.value = value;
    }

    /**
     * Returns the integer value matching the C++ enum value.
     *
     * @return the numeric value of this category
     */
    public int getValue() {
        return value;
    }

    /**
     * Resolves a {@link TokenCategory} from its integer value.
     *
     * @param value the integer value (0-6)
     * @return the matching category
     * @throws IllegalArgumentException if the value is out of range
     */
    public static TokenCategory fromValue(int value) {
        for (TokenCategory category : values()) {
            if (category.value == value) {
                return category;
            }
        }
        throw new IllegalArgumentException("Invalid TokenCategory value: " + value);
    }
}
