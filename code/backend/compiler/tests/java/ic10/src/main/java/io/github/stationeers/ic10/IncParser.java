// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

package io.github.stationeers.ic10;

/**
 * @author edocsitahw
 * @version 1.0.0
 * - file IncParser.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */
import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;

/**
 * IC10 incremental parser.
 * Statement-cache based incremental parser, suitable for frequent small edits in editor scenarios.
 * Re-parses from changed line to end of file, prefix statements are reused directly.
 *
 * <p>Usage:
 * <pre>{@code
 * IncParser parser = new IncParser();
 * // First full parsing
 * String result = parser.parseFull(tokens);
 * // Subsequent incremental parsing
 * result = parser.parseInc(newTokens, changedStartLine);
 * // Check cache state
 * boolean hasCache = parser.hasCache();
 * // Clear cache when needed
 * parser.clear();
 * }</pre>
 *
 * <p>Result JSON format:
 * <pre>{@code
 * {
 *   "ast": {...},
 *   "incremental": false,
 *   "reparsedStmts": 0,
 *   "affectedStmtStart": 0
 * }
 * }</pre>
 */
public class IncParser {

    @DoNotStrip
    private HybridData mHybridData;

    public IncParser() {
        mHybridData = initHybrid();
    }

    private IncParser(HybridData hd) {
        mHybridData = hd;
    }

    /**
     * Full parsing.
     * Perform full parsing on complete token sequence, building AST cache.
     *
     * @param tokens Complete token sequence
     * @return JSON format result string
     */
    public native String parseFull(Token[] tokens);

    /**
     * Incremental parsing.
     * Perform incremental parsing based on previous cache.
     * Falls back to full parsing if cache is invalid or change is too large.
     *
     * @param tokens New complete token sequence
     * @param changedStartLine Changed start line (1-based, inclusive)
     * @return JSON format result string
     */
    public native String parseInc(Token[] tokens, int changedStartLine);

    /**
     * Check if there is valid cache.
     *
     * @return true if cache exists
     */
    public native boolean hasCache();

    /**
     * Clear cache.
     */
    public native void clear();

    @DoNotStrip
    private native HybridData initHybrid();

    static {
        System.loadLibrary("ic10_java");
    }
}
