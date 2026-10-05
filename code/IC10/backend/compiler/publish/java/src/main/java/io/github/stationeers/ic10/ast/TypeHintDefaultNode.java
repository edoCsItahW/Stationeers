// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * One {@code @default} of a type hint ("TypeHintDefault") — {@code @default <category> <name> <value>}
 * for a device member, {@code @default <value>} for a register (both {@code category} and
 * {@code name} are then absent).
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class TypeHintDefaultNode extends ASTNode {
    /** Group the default belongs to: {@code logic} / {@code logic-slot} / {@code slot}; null for registers. */
    private String category;

    /** Member name the default applies to; null for registers. */
    private String name;

    /** Default value as written in the source. */
    private String value;
}
