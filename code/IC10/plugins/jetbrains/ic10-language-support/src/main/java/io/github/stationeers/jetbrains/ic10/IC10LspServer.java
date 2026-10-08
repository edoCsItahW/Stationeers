package io.github.stationeers.jetbrains.ic10;

import com.intellij.ide.plugins.IdeaPluginDescriptor;
import com.intellij.ide.plugins.PluginManagerCore;
import com.intellij.notification.Notification;
import com.intellij.notification.NotificationType;
import com.intellij.notification.Notifications;
import com.intellij.openapi.extensions.PluginId;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.util.SystemInfo;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.nio.file.Files;
import java.nio.file.Path;

/**
 * Locates the IC10 language server executable.
 *
 * <p>The executable is part of this plugin's own distribution: the build stages the SEA artifact under
 * {@code bin/}, which is why the plugin is a fairly large download (a standalone language server embeds a
 * full JavaScript runtime). Keeping it in the plugin — rather than downloading it at runtime — is what
 * makes installation work in regions where the release host is not reachable. A path configured by the user
 * overrides it.
 */
public final class IC10LspServer {

    private static final String EXE_NAME = "ic10-lsp.exe";
    private static final String NOTIFICATION_GROUP = "IC10 Language Server";

    /** Must match {@code <id>} in {@code META-INF/plugin.xml}; the two are the plugin's identity. */
    private static final String PLUGIN_ID = "org.stationeers.ic10-language-support";

    private IC10LspServer() {
    }

    /**
     * Resolves the executable to run.
     *
     * @return the executable, or {@code null} when it is unavailable (the user has already been notified).
     */
    public static @Nullable Path ensureAvailable(@Nullable Project project) {
        String override = IC10LspSettings.getInstance().getExecutablePath();
        if (!override.isEmpty()) {
            Path path = Path.of(override);
            if (Files.isRegularFile(path)) {
                return path;
            }
            report(project, MyMessageBundle.message("lsp.error.overrideMissing", override));
            return null;
        }

        if (!SystemInfo.isWindows) {
            report(project, MyMessageBundle.message("lsp.error.windowsOnly"));
            return null;
        }

        Path bundled = bundledExecutable();
        if (bundled != null && Files.isRegularFile(bundled)) {
            return bundled;
        }
        report(project, MyMessageBundle.message("lsp.error.serverMissing"));
        return null;
    }

    /**
     * The executable inside this plugin's own distribution directory ({@code bin/ic10-lsp.exe}).
     *
     * <p>Note: {@code PluginManagerCore.getPluginByClassName} would avoid repeating the plugin id here, but
     * it is deprecated in this platform version.
     */
    private static @Nullable Path bundledExecutable() {
        IdeaPluginDescriptor descriptor = PluginManagerCore.getPlugin(PluginId.getId(PLUGIN_ID));
        if (descriptor == null) {
            return null;
        }

        Path pluginPath = descriptor.getPluginPath();
        return pluginPath == null ? null : pluginPath.resolve("bin").resolve(EXE_NAME);
    }

    private static void report(@Nullable Project project, @NotNull String content) {
        Notifications.Bus.notify(
                new Notification(NOTIFICATION_GROUP, MyMessageBundle.message("lsp.notification.title"), content,
                        NotificationType.ERROR),
                project);
    }
}
