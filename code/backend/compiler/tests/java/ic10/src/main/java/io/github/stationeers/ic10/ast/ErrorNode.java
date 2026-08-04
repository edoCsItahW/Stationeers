// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Error node ("Error") — produced by the parser when it encounters invalid syntax.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class ErrorNode extends ASTNode {
    private TokenData token;
    private String message;

    public TokenData getToken() { return token; }
    public void setToken(TokenData token) { this.token = token; }

    public String getMessage() { return message; }
    public void setMessage(String message) { this.message = message; }
}
