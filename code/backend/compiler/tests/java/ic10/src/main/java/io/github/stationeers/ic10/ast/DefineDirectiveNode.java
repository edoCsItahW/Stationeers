// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;

/**
 * Define directive node ("DefineDirective") — "define NAME value".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@JsonInclude(JsonInclude.Include.NON_NULL)
public class DefineDirectiveNode extends ASTNode {
    private ASTNode identifier;  // IdentifierNode or ErrorNode
    private ASTNode number;      // NumberValue (variant)
    private String typeName;     // optional
    private String desc;         // optional

    public ASTNode getIdentifier() { return identifier; }
    public void setIdentifier(ASTNode identifier) { this.identifier = identifier; }

    public ASTNode getNumber() { return number; }
    public void setNumber(ASTNode number) { this.number = number; }

    public String getTypeName() { return typeName; }
    public void setTypeName(String typeName) { this.typeName = typeName; }

    public String getDesc() { return desc; }
    public void setDesc(String desc) { this.desc = desc; }
}
