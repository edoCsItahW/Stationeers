// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * Centralized native library loader for the IC10 Java binding.
 * <p>
 * All JNI wrapper classes call {@link #ensureLoaded()} in their static
 * initializers instead of calling {@link System#loadLibrary(String)}
 * directly. This allows the library to extract the native DLL from its
 * own JAR when it is not present in {@code java.library.path}.
 * </p>
 *
 * @author edocsitahw
 * @version 1.0.0
 */
public final class IC10Native {

    private static volatile boolean loaded;

    private IC10Native() {}

    /**
     * Guarantees the native library is loaded exactly once.
     * Safe to call multiple times from multiple class static initializers.
     */
    public static synchronized void ensureLoaded() {
        if (loaded) return;
        loaded = true;

        String libFileName = mapLibName();

        // 1. Try to extract from classpath (JAR contains /native/<dll>)
        InputStream is = IC10Native.class.getResourceAsStream("/native/" + libFileName);
        if (is != null) {
            try (is) {
                Path tmpDir = Files.createTempDirectory("ic10_native");
                Path libPath = tmpDir.resolve(libFileName);
                tmpDir.toFile().deleteOnExit();
                libPath.toFile().deleteOnExit();

                Files.copy(is, libPath, StandardCopyOption.REPLACE_EXISTING);
                System.load(libPath.toAbsolutePath().toString());
                return;
            } catch (IOException e) {
                throw new RuntimeException("Failed to extract native library /native/" + libFileName, e);
            }
        }

        // 2. Fallback: try java.library.path (development mode)
        System.loadLibrary("ic10c_java");
    }

    private static String mapLibName() {
        String os = System.getProperty("os.name", "").toLowerCase();
        if (os.contains("windows")) return "ic10c_java.dll";
        if (os.contains("mac"))     return "libic10c_java.dylib";
        return "libic10c_java.so";
    }
}
