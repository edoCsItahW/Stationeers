package io.github.stationeers.jetbrains.ic10;

import com.intellij.extapi.psi.PsiFileBase;
import com.intellij.openapi.fileTypes.FileType;
import com.intellij.psi.FileViewProvider;
import org.jetbrains.annotations.NotNull;


/**
 * @author edocsitahw
 * @version 1.0.0
 * - file null.java
 * - date 2026/08/04 16:56
 * - copyrigh CC BY-NC-SA 2026. All rights reserved.
 */

public class IC10File extends PsiFileBase {

    public IC10File(@NotNull FileViewProvider viewProvider) {
        super(viewProvider, IC10Language.INSTANCE);
    }

    @NotNull
    @Override
    public FileType getFileType() {
        return IC10FileType.INSTANCE;
    }

    @Override
    public String toString() {
        return "IC10 File";
    }

}
