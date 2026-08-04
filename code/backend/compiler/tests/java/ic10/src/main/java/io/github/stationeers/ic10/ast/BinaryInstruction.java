// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Binary instruction (2 operands): abs, acos, asin, atan, ceil, cos,
 * exp, floor, log, round, sin, sqrt, tan, trunc, not, move, sgn,
 * seqz, snez, sgez, sgtz, slez, sltz, snan, snanz, bdns, bdnsal,
 * bdse, bdseal, brdns, brdse, sdns, sdse, poke, beqz, beqzal,
 * bnez, bnezal, bgez, bgezal, bgtz, bgtzal, blez, blezal, bltz,
 * bltzal, bnan, breqz, brnez, brgez, brgtz, brlez, brltz, brnan.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class BinaryInstruction extends UnaryInstruction {
    private ASTNode operand2;
    private int type2;

    public ASTNode getOperand2() { return operand2; }
    public void setOperand2(ASTNode operand2) { this.operand2 = operand2; }

    public int getType2() { return type2; }
    public void setType2(int type2) { this.type2 = type2; }

    @Override
    public int getArity() { return 2; }

    @Override
    public ASTNode getOperand(int n) {
        if (n == 1) return getOperand1();
        if (n == 2) return operand2;
        return null;
    }

    @Override
    public int getOperandType(int n) {
        if (n == 1) return getType1();
        if (n == 2) return type2;
        return -1;
    }
}
