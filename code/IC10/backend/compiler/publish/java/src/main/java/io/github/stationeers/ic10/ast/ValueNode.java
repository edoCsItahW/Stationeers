// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Marker interface for AST nodes that carry a scalar value.
 * Includes literal nodes (Integer, Float, Register, Device, etc.)
 * and ConstantNode.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public interface ValueNode {

    /**
     * Returns the string representation of this node's value.
     *
     * @return the value as a string (e.g. "42", "r0", "d0", "nan")
     */
    String getValue();
}
