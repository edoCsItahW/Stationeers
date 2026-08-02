// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package com.stationeers.ic10;

/**
 * @author edocsitahw
 * @version 1.0.0
 * - file TypeOfNode.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */

/**
 * AST node type mapping utility.
 * Provides static access to the mapping from AST node type names to their
 * BasicType (kind) and TypeCategory (category) enum values.
 * The mapping is generated at compile time from C++ template metaprogramming
 * and returned as a JSON string for Java-side parsing.
 */
public class TypeOfNode {

    /**
     * Returns the JSON string representation of all node type mappings.
     * The JSON format is: {"NodeName":{"kind":int,"category":int},...}
     * Keys are AST node type names (e.g. "Integer", "Float", "Register").
     * Values are objects with "kind" (BasicType enum value) and "category" (TypeCategory enum value).
     *
     * @return JSON string of node type mappings
     */
    public static native String toJSON();

    static {
        System.loadLibrary("ic10_java");
    }
}
