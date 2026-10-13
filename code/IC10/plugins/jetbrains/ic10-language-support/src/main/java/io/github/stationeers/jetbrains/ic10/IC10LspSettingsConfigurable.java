package io.github.stationeers.jetbrains.ic10;

import com.intellij.openapi.options.Configurable;
import com.intellij.ui.components.JBLabel;
import com.intellij.ui.components.JBTextField;
import com.intellij.util.ui.FormBuilder;
import com.intellij.util.ui.JBUI;
import org.jetbrains.annotations.Nullable;

import javax.swing.JComponent;
import javax.swing.JPanel;

/**
 * Settings UI for {@link IC10LspSettings}: an optional override for the language server executable.
 */
public final class IC10LspSettingsConfigurable implements Configurable {

    private final JBTextField executablePath = new JBTextField();

    @Override
    public String getDisplayName() {
        return MyMessageBundle.message("settings.displayName");
    }

    @Override
    public @Nullable JComponent createComponent() {
        reset();
        JPanel panel = FormBuilder.createFormBuilder()
                .addLabeledComponent(new JBLabel(MyMessageBundle.message("settings.executablePath")), executablePath, 1, false)
                .addComponent(new JBLabel(MyMessageBundle.message("settings.executablePath.note")))
                .addComponentFillVertically(new JPanel(), 0)
                .getPanel();
        panel.setBorder(JBUI.Borders.empty(10));
        return panel;
    }

    @Override
    public boolean isModified() {
        return !executablePath.getText().trim().equals(IC10LspSettings.getInstance().getExecutablePath());
    }

    @Override
    public void apply() {
        IC10LspSettings.getInstance().setExecutablePath(executablePath.getText());
    }

    @Override
    public void reset() {
        executablePath.setText(IC10LspSettings.getInstance().getExecutablePath());
    }
}
