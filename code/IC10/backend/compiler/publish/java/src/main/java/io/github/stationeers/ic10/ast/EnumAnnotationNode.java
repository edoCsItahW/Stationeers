// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * Enum annotation node ("EnumAnnotation") — the whole {@code #> @enum ... #> @end-enum} block.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class EnumAnnotationNode extends ASTNode {
    /** Enum type name. */
    private String name;

    /** Description ({@link StringNode}, {@link LinkNode} or {@link ErrorNode}); null when absent. */
    private ASTNode desc;

    /** {@code @value} lines (empty list when there are none). */
    private List<EnumAnnotationValueNode> values;
}
