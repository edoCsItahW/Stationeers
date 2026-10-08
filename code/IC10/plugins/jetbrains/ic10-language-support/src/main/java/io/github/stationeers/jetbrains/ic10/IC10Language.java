package io.github.stationeers.jetbrains.ic10;

import com.intellij.lang.Language;

/**
 * @author edocsitahw
 * @version 1.0.0
 * - file null.java
 * - date 2026/08/03 21:45
 * - copyrigh CC BY-NC-SA 2026. All rights reserved.
 */

public class IC10Language extends Language {

    public static final IC10Language INSTANCE = new IC10Language();

    private IC10Language() {
        super("IC10");
    }

}
