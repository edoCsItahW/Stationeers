// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.*;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

/**
 * Source position (line, column) in IC10 source code.
 * Mirrors the C++ {@code Pos} structure serialized in JSON.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@NoArgsConstructor
@AllArgsConstructor(onConstructor = @__(@JsonCreator))
public class Position {
    @JsonProperty("line") private int line;
    @JsonProperty("column") private int column;
    @JsonProperty("offset") private int offset;

    @Override
    public String toString() {
        return line + ":" + column;
    }
}
