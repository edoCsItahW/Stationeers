// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Nullary instruction (0 operands): hcf, yield.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class NullaryInstruction extends InstructionNode {

    @Override
    public int getArity() { return 0; }

    @Override
    public ASTNode getOperand(int n) { return null; }

    @Override
    public int getOperandType(int n) { return -1; }

    @Override
    public int length() {
        return getKeyword().length();
    }
}
