/*
 * Copyright (c) 2026. All rights reserved.
 *
 * Stub NativeLoader implementation for fbjni.
 *
 * fbjni 的 C++ 部分已静态链接到 ic10_java 动态库中，
 * 因此 NativeLoader.loadLibrary("fbjni") 是一个空操作。
 * ic10_java 动态库通过各 IC10 Java 类的 static { System.loadLibrary("ic10_java"); } 加载。
 */
package com.facebook.soloader.nativeloader;

/**
 * No-op NativeLoader stub.
 * <p>
 * fbjni native code is statically linked into the ic10_java dynamic library,
 * so loading "fbjni" separately is unnecessary.
 */
public class NativeLoader {

    /**
     * No-op library loader.
     *
     * @param name library name (ignored)
     */
    public static void loadLibrary(String name) {
        // No-op: fbjni 已静态链接到 ic10_java.dll
    }
}
