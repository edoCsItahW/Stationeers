// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * Device annotation node ("DeviceAnnotation") — the whole {@code #> @device ... #> @end-device}
 * block. It is the data source the semantic stage uses to decide whether a device member name is
 * legal, so the entry lists mirror the C++ member names exactly.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class DeviceAnnotationNode extends ASTNode {
    /** Device type name. */
    private String name;

    /** Description ({@link StringNode}, {@link LinkNode} or {@link ErrorNode}); null when absent. */
    private ASTNode desc;

    /** Optional {@code @device-hash} line. */
    private ASTNode deviceHash;

    /** Optional {@code @name-hash} line. */
    private ASTNode nameHash;

    /** {@code @logic} lines (empty list when there are none). */
    private List<TypeAnnotationLineNode> logics;

    /** {@code @logic-slot} lines (empty list when there are none). */
    private List<TypeAnnotationLineNode> logicSlots;

    /** {@code @reagent-hash} lines (empty list when there are none). */
    private List<TypeAnnotationValueNode> reagentHashes;

    /** {@code @slot} lines (empty list when there are none). */
    private List<TypeAnnotationLineNode> slots;
}
