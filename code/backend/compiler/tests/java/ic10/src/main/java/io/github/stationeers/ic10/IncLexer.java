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
 * - file IncLexer.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */
import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;

/**
 * IC10 incremental lexer.
 * Line-hash-cache based incremental lexer, suitable for frequent small edits in editor scenarios.
 * Uses two-pointer diff detection to only re-lex the changed range, significantly reducing
 * redundant computation.
 *
 * <p>Usage:
 * <pre>{@code
 * IncLexer lexer = new IncLexer();
 * // First full lexing
 * String result = lexer.tokenizeFull(source);
 * // Subsequent incremental lexing
 * result = lexer.tokenizeInc(newSource);
 * // Check cache state
 * boolean hasCache = lexer.hasCache();
 * // Clear cache when needed
 * lexer.clear();
 * }</pre>
 *
 * <p>Result JSON format:
 * <pre>{@code
 * {
 *   "tokens": [...],
 *   "incremental": false,
 *   "relexedLines": 0,
 *   "changedStartLine": 0,
 *   "oldChangedEndLine": 0,
 *   "newChangedEndLine": 0
 * }
 * }</pre>
 */
public class IncLexer {

    @DoNotStrip
    private HybridData mHybridData;

    public IncLexer() {
        mHybridData = initHybrid();
    }

    private IncLexer(HybridData hd) {
        mHybridData = hd;
    }

    /**
     * Full lexical analysis.
     * Perform full lexical analysis on complete source, building line cache.
     *
     * @param source Source code string
     * @return JSON format result string
     */
    public native String tokenizeFull(String source);

    /**
     * Incremental lexical analysis.
     * Perform incremental lexical analysis based on previous cache.
     * Falls back to full analysis if cache is empty.
     *
     * @param newSource New source code string
     * @return JSON format result string
     */
    public native String tokenizeInc(String newSource);

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
