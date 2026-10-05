// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Statically spelled device port node — {@code "SelfReferenceDevice"} ({@code db}, the IC10 the
 * program runs on) or {@code "OrdinaryDevice"} ({@code d0}-{@code d5}).
 * <p>
 * Both spellings share one JSON shape (only {@code value} differs); the discriminator tells them
 * apart. Both only ever appear inside {@link StaticDeviceNode#getDevice()}.
 * </p>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class StaticDevicePortNode extends ASTNode implements ValueNode {
    /** Device spelling (e.g. {@code "d0"}, {@code "db"}). */
    private String value;
}
