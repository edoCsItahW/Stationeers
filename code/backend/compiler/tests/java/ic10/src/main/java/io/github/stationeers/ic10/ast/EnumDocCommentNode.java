// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * Enum doc comment node ("EnumDocComment") — "#> @enum ... #> @end-enum".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class EnumDocCommentNode extends ASTNode {
    private String name;
    private String desc;        // optional
    private List<ValueEntry> values;

    @Data
    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class ValueEntry {
        private String name;
        private String value;
        private String desc;  // optional
    }
}
