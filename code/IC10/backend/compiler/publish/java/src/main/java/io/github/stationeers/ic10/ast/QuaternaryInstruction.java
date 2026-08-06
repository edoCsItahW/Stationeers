// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

/**
 * Quaternary instruction (4 operands): clamp, lerp, ext, ins, sap,
 * sna, select, ss, lb, sbn, sbs, bap, bapal, bna, bnaal, brap,
 * brna, ls, lr.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Getter
@Setter
@EqualsAndHashCode(callSuper = true)
public class QuaternaryInstruction extends TernaryInstruction {
    private ASTNode operand4;
    private int type4;

    @Override
    public int getArity() { return 4; }

    @Override
    public ASTNode getOperand(int n) {
        if (n == 1) return getOperand1();
        if (n == 2) return getOperand2();
        if (n == 3) return getOperand3();
        if (n == 4) return operand4;
        return null;
    }

    @Override
    public int getOperandType(int n) {
        if (n == 1) return getType1();
        if (n == 2) return getType2();
        if (n == 3) return getType3();
        if (n == 4) return type4;
        return -1;
    }
}
