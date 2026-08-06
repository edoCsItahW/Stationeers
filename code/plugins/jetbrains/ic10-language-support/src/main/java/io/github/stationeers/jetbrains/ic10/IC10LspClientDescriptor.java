package io.github.stationeers.jetbrains.ic10;

import com.intellij.execution.configurations.GeneralCommandLine;
import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.editor.DefaultLanguageHighlighterColors;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.platform.lsp.api.ProjectWideLspClientDescriptor;
import com.intellij.platform.lsp.api.customization.LspCompletionCustomizer;
import com.intellij.platform.lsp.api.customization.LspCompletionSupport;
import com.intellij.platform.lsp.api.customization.LspCustomization;
import com.intellij.platform.lsp.api.customization.LspSemanticTokensCustomizer;
import com.intellij.platform.lsp.api.customization.LspSemanticTokensSupport;
import com.intellij.psi.PsiFile;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * IC10 LSP client descriptor. Provides the connection configuration for the
 * IC10 language server.
 */
public class IC10LspClientDescriptor extends ProjectWideLspClientDescriptor {

    /** Maps IC10 token type names (both Title Case and lowercase) to IntelliJ TextAttributesKey colors. */
    private static final Map<String, TextAttributesKey> TOKEN_COLORS = new HashMap<>();
    static {
        putTokenColor("Keyword", DefaultLanguageHighlighterColors.KEYWORD);
        putTokenColor("Register", DefaultLanguageHighlighterColors.PARAMETER);
        putTokenColor("RegisterIdentifier", DefaultLanguageHighlighterColors.PARAMETER);
        putTokenColor("Macro", DefaultLanguageHighlighterColors.METADATA);
        putTokenColor("Device", DefaultLanguageHighlighterColors.INSTANCE_FIELD);
        putTokenColor("DeviceIdentifier", DefaultLanguageHighlighterColors.INSTANCE_FIELD);
        putTokenColor("Number", DefaultLanguageHighlighterColors.NUMBER);
        putTokenColor("NumberIdentifier", DefaultLanguageHighlighterColors.NUMBER);
        putTokenColor("String", DefaultLanguageHighlighterColors.STRING);
        putTokenColor("Constant", DefaultLanguageHighlighterColors.CONSTANT);
        putTokenColor("Comment", DefaultLanguageHighlighterColors.LINE_COMMENT);
        putTokenColor("Decorator", DefaultLanguageHighlighterColors.METADATA);
        putTokenColor("Label", DefaultLanguageHighlighterColors.LABEL);
        putTokenColor("LabelIdentifier", DefaultLanguageHighlighterColors.LABEL);
    }

    private static void putTokenColor(String name, TextAttributesKey key) {
        TOKEN_COLORS.put(name, key);
        TOKEN_COLORS.put(name.toLowerCase(), key);
    }

    @NotNull
    private final Project project;

    public IC10LspClientDescriptor(@NotNull Project project) {
        super(project, "IC10 LSP Server");
        this.project = project;
    }

    @Override
    public boolean isSupportedFile(@NotNull VirtualFile file) {
        return Objects.equals(file.getExtension(), "ic10") || Objects.equals(file.getExtension(), "ic");
    }

    @NotNull
    @Override
    public GeneralCommandLine createCommandLine() {
        return new GeneralCommandLine("ic10-lsp", "--stdio");
    }

    @Override
    public @NotNull LspCustomization getLspCustomization() {
        return new LspCustomization() {
            @Override
            public @NotNull LspSemanticTokensCustomizer getSemanticTokensCustomizer() {
                return new LspSemanticTokensSupport() {
                    @Override
                    public boolean shouldAskServerForSemanticTokens(@NotNull PsiFile psiFile) {
                        VirtualFile vf = psiFile.getVirtualFile();
                        return vf != null && isSupportedFile(vf);
                    }

                    @Nullable
                    @Override
                    public TextAttributesKey getTextAttributesKey(
                            @NotNull String tokenType,
                            @NotNull List<String> tokenModifiers) {
                        // IC10 server sends Title Case token type names (e.g., "Keyword", "Register").
                        // IntelliJ's default mapping only matches lowercase standard names.
                        TextAttributesKey key = TOKEN_COLORS.get(tokenType);
                        if (key != null)
                            return key;

                        // Try lowercase as well (for standard LSP names)
                        return TOKEN_COLORS.get(tokenType.toLowerCase());
                    }
                };
            }

            @Override
            public @NotNull LspCompletionCustomizer getCompletionCustomizer() {
                return new LspCompletionSupport() {
                    @Override
                    public boolean isTriggerCharacterRespected(char c) {
                        return Character.isLetter(c);
                    }
                };
            }
        };
    }
}
