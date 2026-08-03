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
 * - file Parser.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */
import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;

public class Parser {

    @DoNotStrip
    private HybridData mHybridData;

    public Parser() {
        mHybridData = initHybrid();
    }

    private Parser(HybridData hd) {
        mHybridData = hd;
    }

    public native void init(Token[] tokens, boolean debug);

    public native Program parse();

    public native Diagnostic[] getDiagnostics();

    public static native Program parsing(Token[] tokens, boolean debug);

    @DoNotStrip
    private native HybridData initHybrid();

    static {
        System.loadLibrary("ic10_java");
    }
}
