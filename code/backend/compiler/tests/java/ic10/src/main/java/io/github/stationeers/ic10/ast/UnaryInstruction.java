// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

/**
 * Unary instruction (1 operand): peek, rand, pop, clr, sleep,
 * clrd, push, jal, jr, j.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Setter
@Getter
@EqualsAndHashCode(callSuper = true)
public class UnaryInstruction extends NullaryInstruction {
    private ASTNode operand1;
    private int type1;

    @Override
    public int getArity() { return 1; }

    @Override
    public ASTNode getOperand(int n) { return n == 1 ? operand1 : null; }

    @Override
    public int getOperandType(int n) { return n == 1 ? type1 : -1; }
}
