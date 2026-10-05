// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Dynamically addressed register node ("DynamicRegister") — the register is named by another
 * operand ({@code r r0} style indirection).
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class DynamicRegisterNode extends ASTNode {
    /** Operand holding the register to read ({@link DynamicRegisterNode}, a static register or an error). */
    private ASTNode register;
}
