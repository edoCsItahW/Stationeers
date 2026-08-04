// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Device reference node (e.g. "d0", "Device").
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class DeviceNode extends ASTNode implements ValueNode {
    private String value;

    public String getValue() { return value; }
    public void setValue(String value) { this.value = value; }
}
