// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;

/**
 * Alias directive node ("AliasDirective") — "alias NAME d0/Register".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@JsonInclude(JsonInclude.Include.NON_NULL)
public class AliasDirectiveNode extends ASTNode {
    private ASTNode identifier;         // IdentifierNode or ErrorNode
    private ASTNode registerOrDevice;   // RegisterNode, DeviceNode, IdentifierNode, or ErrorNode
    private String typeName;            // optional (@type)
    private String desc;                // optional (@desc)

    public ASTNode getIdentifier() { return identifier; }
    public void setIdentifier(ASTNode identifier) { this.identifier = identifier; }

    public ASTNode getRegisterOrDevice() { return registerOrDevice; }
    public void setRegisterOrDevice(ASTNode registerOrDevice) { this.registerOrDevice = registerOrDevice; }

    public String getTypeName() { return typeName; }
    public void setTypeName(String typeName) { this.typeName = typeName; }

    public String getDesc() { return desc; }
    public void setDesc(String desc) { this.desc = desc; }
}
