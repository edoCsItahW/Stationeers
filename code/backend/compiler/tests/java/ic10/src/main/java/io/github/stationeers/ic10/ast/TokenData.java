// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

/**
 * Token data embedded in {@link ErrorNode}.
 * Mirrors the C++ {@code Token::toJSON()} output.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class TokenData {

    private int type;        // TokenType enum value
    private Position pos;
    private String lexeme;
    private int category;    // TokenCategory enum value

    public TokenData() {}

    @JsonCreator
    public TokenData(
            @JsonProperty("type") int type,
            @JsonProperty("pos") Position pos,
            @JsonProperty("lexeme") String lexeme,
            @JsonProperty("category") int category) {
        this.type = type;
        this.pos = pos;
        this.lexeme = lexeme;
        this.category = category;
    }

    public int getType() { return type; }
    public void setType(int type) { this.type = type; }

    public Position getPos() { return pos; }
    public void setPos(Position pos) { this.pos = pos; }

    public String getLexeme() { return lexeme; }
    public void setLexeme(String lexeme) { this.lexeme = lexeme; }

    public int getCategory() { return category; }
    public void setCategory(int category) { this.category = category; }

    @Override
    public String toString() {
        return lexeme;
    }
}
