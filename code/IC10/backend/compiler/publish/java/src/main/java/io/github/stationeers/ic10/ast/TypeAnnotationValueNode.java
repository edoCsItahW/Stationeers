// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * Value-only annotation line of a {@code #>} device block — {@code "DeviceAnnotationDeviceHash"}
 * ({@code @device-hash}), {@code "DeviceAnnotationNameHash"} ({@code @name-hash}) or
 * {@code "DeviceAnnotationReagentHash"} ({@code @reagent-hash}).
 * <p>
 * The three share one JSON shape, so they share this class; the discriminator tells them apart.
 * </p>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class TypeAnnotationValueNode extends ASTNode implements ValueNode {
    /** Value as written (e.g. a hash literal). */
    private String value;
}
