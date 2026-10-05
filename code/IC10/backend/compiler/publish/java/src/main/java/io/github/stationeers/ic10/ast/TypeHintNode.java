// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * Type hint node ("TypeHint") — the {@code #: @type ... @desc ... @default ... @builtin} comment
 * attached to {@code alias} / {@code define} directives.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class TypeHintNode extends ASTNode {
    /** Type name given by {@code @type}; null when absent. */
    private String type;

    /** Description given by {@code @desc} ({@link StringNode}, {@link LinkNode} or {@link ErrorNode}); null when absent. */
    private ASTNode desc;

    /** {@code @default} entries (empty list when there are none). */
    private List<TypeHintDefaultNode> defaults;

    /** Whether {@code @builtin} marked this hint as a predefined constant. */
    private boolean builtin;
}
