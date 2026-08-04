// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

/**
 * Keyword constant node ("Constant") — nan, pinf, pi, deg2rad, etc.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public class ConstantNode extends ASTNode implements ValueNode {
    private String keyword;

    @Override
    public String getValue() { return keyword; }
    public void setKeyword(String keyword) { this.keyword = keyword; }
    public String getKeyword() { return keyword; }
}
