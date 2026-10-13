package io.github.stationeers.jetbrains.ic10;

import com.intellij.openapi.fileTypes.LanguageFileType;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;

/**
 * @author edocsitahw
 * @version 1.0.0
 * - file null.java
 * - date 2026/08/03 21:41
 * - copyrigh CC BY-NC-SA 2026. All rights reserved.
 */

public class IC10FileType extends LanguageFileType {

    public static final IC10FileType INSTANCE = new IC10FileType();

    private IC10FileType() {
        super(IC10Language.INSTANCE);
    }

    @NotNull
    @Override
    public String getName() {
        return "IC10 File";
    }

    @NotNull
    @Override
    public String getDescription() {
        return "IC10 language file";
    }

    @NotNull
    @Override
    public String getDefaultExtension() {
        return "ic";
    }

    @Override
    public Icon getIcon() {
        return IC10Icons.FILE;
    }

}
