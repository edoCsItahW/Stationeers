package io.github.stationeers.jetbrains.ic10;

import com.intellij.openapi.application.ApplicationManager;
import com.intellij.openapi.components.PersistentStateComponent;
import com.intellij.openapi.components.Service;
import com.intellij.openapi.components.State;
import com.intellij.openapi.components.Storage;
import org.jetbrains.annotations.NotNull;

/**
 * Application-level settings for the IC10 language server executable.
 *
 * <p>By default the executable comes from the companion plugin that carries it; {@link #getExecutablePath()}
 * is only an escape hatch for a user who has their own build.
 */
@Service(Service.Level.APP)
@State(name = "IC10LspSettings", storages = @Storage("ic10-lsp.xml"))
public final class IC10LspSettings implements PersistentStateComponent<IC10LspSettings.State> {

    /** Persisted state. Fields must stay public for the platform serializer. */
    public static final class State {
        public String executablePath = "";
    }

    private State state = new State();

    public static IC10LspSettings getInstance() {
        return ApplicationManager.getApplication().getService(IC10LspSettings.class);
    }

    @Override
    public @NotNull State getState() {
        return state;
    }

    @Override
    public void loadState(@NotNull State state) {
        this.state = state;
    }

    public @NotNull String getExecutablePath() {
        return state.executablePath == null ? "" : state.executablePath.trim();
    }

    public void setExecutablePath(@NotNull String value) {
        state.executablePath = value.trim();
    }
}
