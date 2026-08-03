// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package io.github.stationeers.ic10;

/**
 * Basic type classification for IC10 semantic analysis.
 * <p>
 * Mirrors the C++ {@code enum class BasicType} in {@code ic10/semantic/types.hpp}.
 * Identifies the fundamental data type of a symbol or expression.
 *
 * @author edocsitahw
 * @version 1.0.0
 * @since 1.0.0
 */
public enum BasicType {
    /** String type */
    STRING(0),
    /** Integer type */
    INTEGER(1),
    /** Float type */
    FLOAT(2),
    /** Register type (r0-r15) */
    REGISTER(3),
    /** Device type (starts with @) */
    DEVICE(4),
    /** Unknown type (parse error) */
    UNKNOWN(5),
    /** Enum type */
    ENUM(6);

    private final int value;

    BasicType(int value) {
        this.value = value;
    }

    /**
     * Returns the integer value matching the C++ enum value.
     *
     * @return the numeric value of this basic type
     */
    public int getValue() {
        return value;
    }

    /**
     * Resolves a {@link BasicType} from its integer value.
     *
     * @param value the integer value (0-6)
     * @return the matching basic type
     * @throws IllegalArgumentException if the value is out of range
     */
    public static BasicType fromValue(int value) {
        for (BasicType type : values()) {
            if (type.value == value) {
                return type;
            }
        }
        throw new IllegalArgumentException("Invalid BasicType value: " + value);
    }
}
