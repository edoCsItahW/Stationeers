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
 * - file IncCompiler.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */
import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;

/**
 * IC10 incremental compiler.
 * Top-level wrapper integrating incremental lexer and incremental parser.
 * Suitable for frequent small edits in editor scenarios, providing a one-stop
 * incremental compilation interface.
 *
 * <p>Usage:
 * <pre>{@code
 * IncCompiler compiler = new IncCompiler();
 * // First full compilation
 * String result = compiler.compileFull(source);
 * // Subsequent incremental compilation
 * result = compiler.compileInc(newSource);
 * // Check cache state
 * boolean hasCache = compiler.hasCache();
 * // Clear cache when needed
 * compiler.clear();
 * }</pre>
 *
 * <p>Result JSON format:
 * <pre>{@code
 * {
 *   "tokens": [...],
 *   "ast": {...},
 *   "incremental": false,
 *   "relexedLines": 0,
 *   "reparsedStmts": 0
 * }
 * }</pre>
 */
public class IncCompiler {

    @DoNotStrip
    private HybridData mHybridData;

    public IncCompiler() {
        mHybridData = initHybrid();
    }

    private IncCompiler(HybridData hd) {
        mHybridData = hd;
    }

    /**
     * Full compilation.
     * Perform full lexical and syntactic analysis on complete source, building caches.
     *
     * @param source Source code string
     * @return JSON format result string
     */
    public native String compileFull(String source);

    /**
     * Incremental compilation.
     * Perform incremental compilation based on previous cache.
     * Falls back to full compilation if cache is empty.
     *
     * @param newSource New source code string
     * @return JSON format result string
     */
    public native String compileInc(String newSource);

    /**
     * Check if there is valid cache.
     *
     * @return true if both lexer and parser have cache
     */
    public native boolean hasCache();

    /**
     * Clear all caches.
     */
    public native void clear();

    @DoNotStrip
    private native HybridData initHybrid();

    static {
        IC10Native.ensureLoaded();
    }
}
