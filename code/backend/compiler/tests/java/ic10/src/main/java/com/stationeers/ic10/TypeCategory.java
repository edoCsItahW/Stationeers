// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package com.stationeers.ic10;

/**
 * Type category classification for IC10 semantic analysis.
 * <p>
 * Mirrors the C++ {@code enum class TypeCategory} in {@code ic10/semantic/types.hpp}.
 * Identifies the semantic category of a type (label, macro call, constant, etc.).
 *
 * @author edocsitahw
 * @version 1.0.0
 * @since 1.0.0
 */
public enum TypeCategory {
    /** Label category */
    LABEL(0),
    /** STR macro call category */
    STR_CALL(1),
    /** HASH macro call category */
    HASH_CALL(2),
    /** Constant category */
    CONSTANT(3),
    /** Number category */
    NUMBER(4),
    /** Basic category */
    BASIC(5);

    private final int value;

    TypeCategory(int value) {
        this.value = value;
    }

    /**
     * Returns the integer value matching the C++ enum value.
     *
     * @return the numeric value of this type category
     */
    public int getValue() {
        return value;
    }

    /**
     * Resolves a {@link TypeCategory} from its integer value.
     *
     * @param value the integer value (0-5)
     * @return the matching type category
     * @throws IllegalArgumentException if the value is out of range
     */
    public static TypeCategory fromValue(int value) {
        for (TypeCategory category : values()) {
            if (category.value == value) {
                return category;
            }
        }
        throw new IllegalArgumentException("Invalid TypeCategory value: " + value);
    }
}
