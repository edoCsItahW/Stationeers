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
 * - file IC10Local.java
 * - date 2026/08/02
 * - copyright CC BY-NC-SA 2026. All rights reserved.
 */

public class IC10Local {

    public static native void setLanguage(String name);

    static {
        System.loadLibrary("ic10_java");
    }
}
