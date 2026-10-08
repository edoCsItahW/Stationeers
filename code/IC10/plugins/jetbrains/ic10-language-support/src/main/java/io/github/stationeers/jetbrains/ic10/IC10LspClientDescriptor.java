package io.github.stationeers.jetbrains.ic10;

import com.intellij.execution.configurations.GeneralCommandLine;
import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.editor.markup.TextAttributes;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.platform.lsp.api.ProjectWideLspClientDescriptor;
import com.intellij.platform.lsp.api.customization.LspCompletionCustomizer;
import com.intellij.platform.lsp.api.customization.LspCompletionSupport;
import com.intellij.platform.lsp.api.customization.LspCustomization;
import com.intellij.platform.lsp.api.customization.LspSemanticTokensCustomizer;
import com.intellij.platform.lsp.api.customization.LspSemanticTokensSupport;
import com.intellij.psi.PsiFile;
import com.intellij.ui.JBColor;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.awt.Color;
import java.awt.Font;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * IC10 LSP client descriptor. Provides the connection configuration for the
 * IC10 language server.
 */
public class IC10LspClientDescriptor extends ProjectWideLspClientDescriptor {

    /**
     * Maps IC10 token type names (both Title Case and lowercase) to IntelliJ text attributes.
     *
     * <p>These are keys <em>owned by this plugin</em> rather than {@code DefaultLanguageHighlighterColors}
     * constants, because the latter cannot colour several IC10 token types at all: {@code DEFAULT_PARAMETER}
     * (registers) is not defined in any bundled scheme and its fallback chain ends at the plain text colour,
     * while {@code DEFAULT_LABEL} carries no foreground in the light scheme. A key created with default
     * attributes has no fallback, so its colour is used in every theme.
     *
     * <p>The dark values are the palette the VS Code extension ships in its {@code package.json}
     * ({@code editor.semanticTokenColorCustomizations}), so both clients agree; the light values are the
     * matching hues darkened for a light background.
     */
    private static final Map<String, TextAttributesKey> TOKEN_COLORS = new HashMap<>();
    static {
        putTokenColor("Keyword", key("IC10_KEYWORD", 0x000080, 0xCF8E6D, Font.BOLD));
        putTokenColor("Register", key("IC10_REGISTER", 0x7A3E2A, 0xB4654A));
        putTokenColor("RegisterIdentifier", key("IC10_REGISTER_IDENTIFIER", 0x5A2C1E, 0x7E4633));
        putTokenColor("Macro", key("IC10_MACRO", 0x005A9C, 0x56A8F5));
        putTokenColor("Device", key("IC10_DEVICE", 0x7A1FA2, 0xB267D6));
        putTokenColor("DeviceIdentifier", key("IC10_DEVICE_IDENTIFIER", 0x55146E, 0x77458F));
        putTokenColor("Number", key("IC10_NUMBER", 0x0000FF, 0x2AACB8));
        putTokenColor("NumberIdentifier", key("IC10_NUMBER_IDENTIFIER", 0x00666F, 0x1F8088));
        putTokenColor("String", key("IC10_STRING", 0x008000, 0x6AAB73));
        putTokenColor("Constant", key("IC10_CONSTANT", 0x660E7A, 0xC77DBB));
        putTokenColor("Comment", key("IC10_COMMENT", 0x808080, 0x7A7E85));
        putTokenColor("Decorator", key("IC10_DECORATOR", 0x3F6B4A, 0x5F826B));
        putTokenColor("AnnotationTag", key("IC10_ANNOTATION_TAG", 0x2E7D46, 0x67A37C));
        putTokenColor("Label", key("IC10_LABEL", 0x8A6D1F, 0xE8BF6A));
        putTokenColor("LabelIdentifier", key("IC10_LABEL_IDENTIFIER", 0x6E5A2A, 0xA3874C));
        putTokenColor("Enum", key("IC10_ENUM", 0x0F7A6A, 0x4EC9B0));
        putTokenColor("EnumMember", key("IC10_ENUM_MEMBER", 0x8A3F9E, 0xD9A0DC));
        putTokenColor("Unknown", key("IC10_UNKNOWN", 0xC0272D, 0xF75464));
    }

    private static TextAttributesKey key(String name, int light, int dark) {
        return key(name, light, dark, Font.PLAIN);
    }

    /**
     * The {@code (String, TextAttributes)} overload is deprecated in favour of declaring colours in
     * {@code additionalTextAttributes} scheme files. It is kept on purpose: a key created with inline
     * defaults has no fallback key, so {@code DefaultColorsScheme#getAttributes} falls through to
     * {@code getKeyDefaults()} and paints the colour in every theme, whereas a scheme file silently paints
     * nothing when its scheme name or file does not match — which is exactly the symptom being fixed here.
     * Switching to scheme files is a follow-up, not a prerequisite.
     */
    @SuppressWarnings("deprecation")
    private static TextAttributesKey key(String name, int light, int dark, int fontType) {
        TextAttributes attributes =
                new TextAttributes(new JBColor(new Color(light), new Color(dark)), null, null, null, fontType);

        return TextAttributesKey.createTextAttributesKey(name, attributes);
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
        Path executable = IC10LspServer.ensureAvailable(project);
        if (executable == null) {
            // ensureAvailable() has already told the user what to fix; fail loudly instead of starting a
            // client whose executable does not exist.
            throw new IllegalStateException(MyMessageBundle.message("lsp.error.notConfigured"));
        }
        // 服务器默认按 VS Code 的渲染能力出内容（SVG 悬停卡片、行尾双空格的 Markdown 换行、带等宽对齐的
        // 内联提示），IntelliJ 三样都用不出来：悬停改成原生 Markdown、换行改用行内 `<br />`（它的 quick doc
        // 会逐行 trimEnd，双空格形同不存在）、内联提示则干脆不声明。开关由服务端解析，见 packages/server 的
        // cliOptions.ts；不传参数时行为与 VS Code 完全一致。
        // The server renders for VS Code by default (an SVG hover card, two-trailing-space Markdown breaks and
        // monospace-aligned inlay hints), none of which IntelliJ can display: hover switches to native Markdown,
        // breaks switch to inline `<br />` (its quick doc trims trailing whitespace per line, so the two spaces
        // simply never arrive) and inlay hints are not advertised at all. The switches are parsed server-side
        // (cliOptions.ts under packages/server); omitting them keeps the VS Code behaviour exactly as it is.
        return new GeneralCommandLine(
                executable.toString(),
                "--stdio",
                "--hover-renderer=markdown",
                "--hover-breaks=html",
                "--inlay-hints=off");
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
                    /**
                     * Accepts every character the server advertises as a trigger.
                     *
                     * <p>Returning {@code true} widens nothing by itself: the platform's own auto-popup handler
                     * runs first and already covers letters, digits and {@code '_'}, and the LSP typed handler
                     * still checks the server's {@code triggerCharacters} before asking. Overriding this with
                     * {@code Character.isLetter(c)} — as this plugin used to — therefore had exactly one
                     * effect: the space, {@code ':'}, {@code '.'} and {@code '"'} the server does list were
                     * silently dropped, so completion never popped up after them.
                     */
                    @Override
                    public boolean isTriggerCharacterRespected(char c) {
                        return true;
                    }
                };
            }
        };
    }
}
