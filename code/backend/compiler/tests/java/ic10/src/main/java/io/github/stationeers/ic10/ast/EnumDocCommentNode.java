// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import java.util.List;

/**
 * Enum doc comment node ("EnumDocComment") — "#> @enum ... #> @end-enum".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@JsonInclude(JsonInclude.Include.NON_NULL)
public class EnumDocCommentNode extends ASTNode {
    private String name;
    private String desc;        // optional
    private List<ValueEntry> values;

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getDesc() { return desc; }
    public void setDesc(String desc) { this.desc = desc; }

    public List<ValueEntry> getValues() { return values; }
    public void setValues(List<ValueEntry> values) { this.values = values; }

    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class ValueEntry {
        private String name;
        private String value;
        private String desc;  // optional

        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
        public String getValue() { return value; }
        public void setValue(String value) { this.value = value; }
        public String getDesc() { return desc; }
        public void setDesc(String desc) { this.desc = desc; }
    }
}
