// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Label definition node ("LabelDef") — "labelName:".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class LabelDefNode extends ASTNode {
    private ASTNode identifier;  // IdentifierNode or ErrorNode

    public ASTNode getIdentifier() { return identifier; }
    public void setIdentifier(ASTNode identifier) { this.identifier = identifier; }
}
