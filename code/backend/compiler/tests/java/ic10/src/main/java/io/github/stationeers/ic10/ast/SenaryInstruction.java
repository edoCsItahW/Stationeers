// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Senary instruction (6 operands): lbns.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class SenaryInstruction extends QuinaryInstruction {
    private ASTNode operand6;
    private int type6;

    public ASTNode getOperand6() { return operand6; }
    public void setOperand6(ASTNode operand6) { this.operand6 = operand6; }

    public int getType6() { return type6; }
    public void setType6(int type6) { this.type6 = type6; }

    @Override
    public int getArity() { return 6; }

    @Override
    public ASTNode getOperand(int n) {
        if (n == 1) return getOperand1();
        if (n == 2) return getOperand2();
        if (n == 3) return getOperand3();
        if (n == 4) return getOperand4();
        if (n == 5) return getOperand5();
        if (n == 6) return operand6;
        return null;
    }

    @Override
    public int getOperandType(int n) {
        if (n == 1) return getType1();
        if (n == 2) return getType2();
        if (n == 3) return getType3();
        if (n == 4) return getType4();
        if (n == 5) return getType5();
        if (n == 6) return type6;
        return -1;
    }
}
