// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

/**
 * Quinary instruction (5 operands): lbn, lbs.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Getter
@Setter
@EqualsAndHashCode(callSuper = true)
public class QuinaryInstruction extends QuaternaryInstruction {
    private ASTNode operand5;
    private int type5;

    @Override
    public int getArity() { return 5; }

    @Override
    public ASTNode getOperand(int n) {
        if (n == 1) return getOperand1();
        if (n == 2) return getOperand2();
        if (n == 3) return getOperand3();
        if (n == 4) return getOperand4();
        if (n == 5) return operand5;
        return null;
    }

    @Override
    public int getOperandType(int n) {
        if (n == 1) return getType1();
        if (n == 2) return getType2();
        if (n == 3) return getType3();
        if (n == 4) return getType4();
        if (n == 5) return type5;
        return -1;
    }
}
