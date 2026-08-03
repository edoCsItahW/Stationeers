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
 * - file Pos.java
 * - date 2026/08/02 19:31
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */
import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;

public class Pos {

    @DoNotStrip
    private HybridData mHybridData;

    public Pos() {
        mHybridData = initHybrid();
    }

    private Pos(HybridData hd) {
        mHybridData = hd;
    }

    public native int getLine();

    public native int getColumn();

    public native int getOffset();

    public native void newline();

    public native void next(int byteValue);

    public native void move(int charOffset, int byteOffset);

    @DoNotStrip
    private native HybridData initHybrid();

    static {
        System.loadLibrary("ic10_java");
    }
}
