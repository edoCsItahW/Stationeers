// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Base class for all instruction nodes (any arity).
 * <p>
 * There are 147 IC10 instructions, grouped by the number of operands (arity 0-6).
 * All instructions share the {@code keyword} field, and each arity adds
 * {@code operandN} / {@code typeN} pairs.
 * </p>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public abstract class InstructionNode extends ASTNode {

    /** The instruction mnemonic (e.g. "add", "beq", "lbns"). */
    private String keyword;

    /**
     * Returns the number of operands for this instruction.
     *
     * @return 0-6, the arity of the instruction
     */
    public abstract int getArity();

    /**
     * Returns the operand at the given (1-based) index.
     *
     * @param n 1-based operand index
     * @return the operand AST node, or null if n > arity
     */
    public abstract ASTNode getOperand(int n);

    /**
     * Returns the OperandType enum ordinal for the given (1-based) operand index.
     *
     * @param n 1-based operand index
     * @return the OperandType ordinal, or -1 if n > arity
     */
    public abstract int getOperandType(int n);
}
