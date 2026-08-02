// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package com.stationeers.ic10;

/**
 * Operand type classification for IC10 instructions.
 * <p>
 * Mirrors the C++ {@code enum class OperandType} in {@code ic10/parser/ast/ast_nullary_ins.hpp}.
 * Each value identifies what kinds of tokens are accepted at a particular operand position
 * in an instruction.
 *
 * @author edocsitahw
 * @version 1.0.0
 * @since 1.0.0
 */
public enum OperandType {
    /** Register or Identifier */
    REG_IDENT(0),
    /** Device or Identifier (device alias reference) */
    DEV_ALIAS(1),
    /** Register, Identifier, or numeric literal (incl. constants and macro calls) */
    REG_NUM(2),
    /** Device, Register, or Identifier (device reference) */
    DEV_REF(3),
    /** Logic slot name or numeric literal */
    LOGIC_SLOT(4),
    /** Reagent mode name or numeric literal */
    REAGENT_MODE(5),
    /** Jump target: numeric literal, constant, macro call, or identifier */
    JUMP_TARGET(6),
    /** Logic type name or numeric literal */
    LOGIC_TYPE(7),
    /** Slot index: numeric literal only */
    SLOT_IDX(8),
    /** Batch mode name or numeric literal */
    BATCH_MODE(9);

    private final int value;

    OperandType(int value) {
        this.value = value;
    }

    /**
     * Returns the integer value matching the C++ enum value.
     *
     * @return the numeric value of this operand type
     */
    public int getValue() {
        return value;
    }

    /**
     * Resolves an {@link OperandType} from its integer value.
     *
     * @param value the integer value (0-9)
     * @return the matching operand type
     * @throws IllegalArgumentException if the value is out of range
     */
    public static OperandType fromValue(int value) {
        for (OperandType type : values()) {
            if (type.value == value) {
                return type;
            }
        }
        throw new IllegalArgumentException("Invalid OperandType value: " + value);
    }
}
