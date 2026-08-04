// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Hexadecimal number literal node ("HexNumber").
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class HexNumberNode extends ASTNode implements ValueNode {
    private String value;
}
