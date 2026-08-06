// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Integer literal node ("Integer").
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class IntegerNode extends ASTNode implements ValueNode {
    private String value;

    /**
     * Returns the integer value as a parsed long.
     *
     * @return the integer value
     */
    public long asLong() { return Long.parseLong(value); }
}
