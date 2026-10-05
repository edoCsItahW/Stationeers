// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

/**
 * One {@code @value} line of an enum annotation ("EnumAnnotationValue") —
 * {@code #> @value Name 0 description}.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class EnumAnnotationValueNode extends ASTNode {
    /** Member name. */
    private String name;

    /** Member value as written. */
    private String value;

    /** Description ({@link StringNode}, {@link LinkNode} or {@link ErrorNode}); null when absent. */
    private ASTNode desc;

    /** Always {@code "value"} in the JSON — the syntax tag this line was written with. */
    private String tag;
}
