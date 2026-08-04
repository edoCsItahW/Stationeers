// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Define directive node ("DefineDirective") — "define NAME value".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class DefineDirectiveNode extends ASTNode {
    private ASTNode identifier;  // IdentifierNode or ErrorNode
    private ASTNode number;      // NumberValue (variant)
    private String typeName;     // optional
    private String desc;         // optional
}
