// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

/**
 * Token data embedded in {@link ErrorNode}.
 * Mirrors the C++ {@code Token::toJSON()} output.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@NoArgsConstructor
@AllArgsConstructor(onConstructor = @__(@JsonCreator))
public class TokenData {

    @JsonProperty("type") private int type;        // TokenType enum value
    @JsonProperty("pos") private Position pos;
    @JsonProperty("lexeme") private String lexeme;
    @JsonProperty("category") private int category;    // TokenCategory enum value

    @Override
    public String toString() {
        return lexeme;
    }
}
