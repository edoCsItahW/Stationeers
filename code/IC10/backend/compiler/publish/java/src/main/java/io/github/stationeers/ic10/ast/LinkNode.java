// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * Link-style description node ("Link") — the {@code .Device.Sensor.Pressure} form inside a
 * {@code #:} type hint: {@code paths} holds the {@code /}-separated segments and {@code fields}
 * the {@code .}-separated ones.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
public class LinkNode extends ASTNode {
    /** Path segments (the {@code /}-separated part). */
    private List<String> paths;

    /** Fields (the {@code .}-separated part). */
    private List<String> fields;
}
