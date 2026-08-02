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
 * - file Linker.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */
import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;

/**
 * IC10 linker.
 * Merges symbol tables from multiple compilation units into a unified global symbol table,
 * performing cross-unit symbol resolution and type checking.
 *
 * <p>Usage:
 * <pre>{@code
 * Linker linker = new Linker();
 * // Add compiled Program with path
 * linker.addUnitProgram(program, "file.ic");
 * // Add source code string with path
 * linker.addUnitSource("source code", "file2.ic");
 * // Perform linking
 * SymbolTable symbolTable = linker.link();
 * // Get diagnostics
 * Diagnostic[] diagnostics = linker.getDiagnostics();
 * // Get global type table
 * TypeTable typeTable = linker.getTypeTable();
 * }</pre>
 */
public class Linker {

    @DoNotStrip
    private HybridData mHybridData;

    public Linker() {
        mHybridData = initHybrid();
    }

    private Linker(HybridData hd) {
        mHybridData = hd;
    }

    /**
     * Add compilation unit (compiled Program with path).
     *
     * @param program Parsed Program object
     * @param path Source file path for diagnostic location
     */
    public native void addUnitProgram(Program program, String path);

    /**
     * Add compilation unit (compiled Program without path).
     *
     * @param program Parsed Program object
     */
    public native void addUnitProgramNoPath(Program program);

    /**
     * Add compilation unit (source code string with path).
     *
     * @param source IC10 source code string
     * @param path Source file path for diagnostic location
     */
    public native void addUnitSource(String source, String path);

    /**
     * Add compilation unit (source code string without path).
     *
     * @param source IC10 source code string
     */
    public native void addUnitSourceNoPath(String source);

    /**
     * Perform linking.
     * Executes the complete linking flow for all added compilation units.
     *
     * @return The merged global symbol table
     */
    public native SymbolTable link();

    /**
     * Get all diagnostics from all compilation units.
     *
     * @return Array of diagnostic information
     */
    public native Diagnostic[] getDiagnostics();

    /**
     * Get the merged global type table.
     * Should be called after {@link #link()} to get complete results.
     *
     * @return The global type table
     */
    public native TypeTable getTypeTable();

    @DoNotStrip
    private native HybridData initHybrid();

    static {
        System.loadLibrary("ic10_java");
    }
}
