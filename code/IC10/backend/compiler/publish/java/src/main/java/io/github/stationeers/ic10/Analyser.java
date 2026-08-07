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
 * - file Analyser.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */
import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;

public class Analyser {

    @DoNotStrip
    private HybridData mHybridData;

    public Analyser() {
        mHybridData = initHybrid();
    }

    private Analyser(HybridData hd) {
        mHybridData = hd;
    }

    public native void visit(Program program);

    public native Diagnostic[] getDiagnostics();

    public native SymbolTable getSymbolTable();

    public native TypeTable getTypeTable();

    public static native void analyse(Program program);

    @DoNotStrip
    private native HybridData initHybrid();

    static {
        IC10Native.ensureLoaded();
    }
}
