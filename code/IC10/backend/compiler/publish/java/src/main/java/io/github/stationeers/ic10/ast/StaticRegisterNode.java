// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Statically spelled register node — {@code "GeneralPurposeRegister"} ({@code r0}-{@code r15}),
 * {@code "AddressRegister"} ({@code ra}) or {@code "StackPointerRegister"} ({@code sp}).
 * <p>
 * The three spellings share one JSON shape (only {@code value} differs), so they share this class;
 * the discriminator tells them apart. Indirect addressing is {@link DynamicRegisterNode} instead.
 * </p>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class StaticRegisterNode extends ASTNode implements ValueNode {
    /** Register spelling (e.g. {@code "r0"}, {@code "ra"}, {@code "sp"}). */
    private String value;
}
