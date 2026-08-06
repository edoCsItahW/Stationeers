package io.github.stationeers.jetbrains.ic10;

import com.intellij.openapi.project.Project;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.platform.lsp.api.LspIntegrationProvider;
import org.jetbrains.annotations.NotNull;

/**
 * IC10 LSP integration provider. Starts the IC10 language server when an
 * IC10 file is opened in the editor.
 */
public class IC10LspIntegrationProvider implements LspIntegrationProvider {

    @Override
    public void fileOpened(
            @NotNull Project project,
            @NotNull VirtualFile file,
            @NotNull LspClientStarter clientStarter) {
        if ("ic".equals(file.getExtension()) || "ic10".equals(file.getExtension())) {
            var descriptor = new IC10LspClientDescriptor(project);
            clientStarter.ensureClientStarted(descriptor);
        }
    }
}
