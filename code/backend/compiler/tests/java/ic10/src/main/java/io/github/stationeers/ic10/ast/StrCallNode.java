// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * String macro call node ("StrCall") — STR("...").
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class StrCallNode extends ASTNode {
    private ASTNode value;  // StringNode or ErrorNode

    public ASTNode getValue() { return value; }
    public void setValue(ASTNode value) { this.value = value; }
}
