// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

/**
 * Ternary instruction (3 operands): add, atan2, div, max, min, mod,
 * mul, pow, sub, rol, ror, and, nor, or, sla, sll, sra, srl, xor,
 * sapz, snaz, seq, sne, sge, sgt, sle, slt, get, rmap, put, l, s,
 * sb, bdnvl, bdnvs, beq, beqal, bne, bneal, bge, bgeal, bgt,
 * bgtal, ble, bleal, blt, bltal, bapz, bapzal, bnaz, bnazal, breq,
 * brne, brge, brgt, brle, brlt, brapz, brnaz.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Getter
@Setter
@EqualsAndHashCode(callSuper = true)
public class TernaryInstruction extends BinaryInstruction {
    private ASTNode operand3;
    private int type3;

    @Override
    public int getArity() { return 3; }

    @Override
    public ASTNode getOperand(int n) {
        if (n == 1) return getOperand1();
        if (n == 2) return getOperand2();
        if (n == 3) return operand3;
        return null;
    }

    @Override
    public int getOperandType(int n) {
        if (n == 1) return getType1();
        if (n == 2) return getType2();
        if (n == 3) return type3;
        return -1;
    }
}
