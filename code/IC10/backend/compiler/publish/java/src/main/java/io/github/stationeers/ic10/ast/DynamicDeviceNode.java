// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Dynamically addressed device node ("DynamicDevice") — the port is held by a register
 * ({@code d r0} style indirection).
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class DynamicDeviceNode extends ASTNode {
    /** Operand holding the device port ({@link DynamicRegisterNode}, a static register or an error). */
    private ASTNode register;
}
