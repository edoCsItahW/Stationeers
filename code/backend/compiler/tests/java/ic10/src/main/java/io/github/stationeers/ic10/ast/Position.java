// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

/**
 * Source position (line, column) in IC10 source code.
 * Mirrors the C++ {@code Pos} structure serialized in JSON.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class Position {

    private int line;
    private int column;

    public Position() {}

    @JsonCreator
    public Position(
            @JsonProperty("line") int line,
            @JsonProperty("column") int column) {
        this.line = line;
        this.column = column;
    }

    public int getLine() { return line; }
    public void setLine(int line) { this.line = line; }

    public int getColumn() { return column; }
    public void setColumn(int column) { this.column = column; }

    @Override
    public String toString() {
        return line + ":" + column;
    }
}
