// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Alias directive node ("AliasDirective") — "alias NAME d0/Register".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class AliasDirectiveNode extends ASTNode {
    private ASTNode identifier;         // IdentifierNode or ErrorNode
    private ASTNode registerOrDevice;   // RegisterNode, DeviceNode, IdentifierNode, or ErrorNode
    private String typeName;            // optional (@type)
    private String desc;                // optional (@desc)


    @Override
    public int length() {
        if (registerOrDevice.getType().equals("Error"))
            return -1;

        return registerOrDevice.getPosition().getColumn() - getPosition().getColumn() + registerOrDevice.length();
    }
}
