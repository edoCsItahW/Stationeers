// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Enum reference node ("Enum") — an operand written {@code Foo.Bar}, where {@code name} is the
 * enum type and {@code value} the member.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class EnumNode extends ASTNode {
    /** Enum type name ({@code Foo} in {@code Foo.Bar}). */
    private ASTNode name;

    /** Enum member name ({@code Bar} in {@code Foo.Bar}). */
    private ASTNode value;
}
