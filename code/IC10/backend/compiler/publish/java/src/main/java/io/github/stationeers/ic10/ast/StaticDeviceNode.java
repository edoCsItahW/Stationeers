// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Statically spelled device node ("StaticDevice") — a device port such as {@code d0} or the
 * self-reference device {@code db}, optionally with a pin ({@code d0:1}).
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class StaticDeviceNode extends ASTNode {
    /** The port itself ({@link StaticDevicePortNode}). */
    private ASTNode device;

    /** Optional pin number ({@code 1} in {@code d0:1}). */
    private ASTNode pin;
}
