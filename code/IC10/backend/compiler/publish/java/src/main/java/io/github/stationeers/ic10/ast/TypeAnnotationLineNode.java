// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * Named annotation line of a {@code #>} device block — {@code "DeviceAnnotationLogic"}
 * ({@code @logic}), {@code "DeviceAnnotationLogicSlot"} ({@code @logic-slot}) or
 * {@code "DeviceAnnotationSlot"} ({@code @slot}).
 * <p>
 * The three share one JSON shape, so they share this class; the discriminator
 * ({@link #getNodeName()}) tells them apart. Only {@code @logic} lines can carry {@code access}, only
 * {@code @logic-slot} lines can carry {@code slotIndices} — the other lines simply never emit those keys.
 * </p>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class TypeAnnotationLineNode extends ASTNode {
    /** Member name (e.g. the logic property {@code Pressure}). */
    private String name;

    /** Value (e.g. a pin or slot index). */
    private String value;

    /** Read/write access: {@code "r"}, {@code "w"} or {@code "rw"}; null when not declared. */
    private String access;

    /** Applicable slot indices; null when not declared, empty list for {@code ()}. */
    private List<Integer> slotIndices;

    /** Default value appended to the line; null when not declared. */
    private String defaultValue;

    /** Description ({@link StringNode}, {@link LinkNode} or {@link ErrorNode}); null when absent. */
    private ASTNode desc;
}
