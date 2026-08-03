package io.github.stationeers.jetbrains.ic10;

import com.intellij.lang.ASTNode;
import com.intellij.lang.ParserDefinition;
import com.intellij.lang.PsiParser;
import com.intellij.openapi.project.Project;
import com.intellij.psi.FileViewProvider;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import com.intellij.psi.impl.source.tree.CompositeElement;
import com.intellij.psi.tree.IFileElementType;
import com.intellij.psi.tree.TokenSet;
import org.jetbrains.annotations.NotNull;

import io.github.stationeers.ic10.*;

/**
 * @author edocsitahw
 * @version 1.0.0
 * - file null.java
 * - date 2026/08/03 23:22
 * - copyrigh CC BY-NC-SA 2026. All rights reserved.
 */

public class IC10ParserDefinition implements ParserDefinition {

    public static final IFileElementType FILE = new IFileElementType(IC10Language.INSTANCE);

    @Override
    public @NotNull com.intellij.lexer.Lexer createLexer(Project project) {
        return null;
    }

    @NotNull
    @Override
    public PsiParser createParser(final Project project) {
        return (rootElementType, builder) -> {
            String source = builder.getOriginalText().toString();

            Token[] tokens = Lexer.tokenize(source, false);

            Program program = Parser.parsing(tokens, false);

            CompositeElement root =
        };
    }

    @Override
    public @NotNull IFileElementType getFileNodeType() {
        return FILE;
    }

    @Override
    public @NotNull TokenSet getCommentTokens() {
        return null;
    }

    @Override
    public @NotNull TokenSet getStringLiteralElements() {
        return null;
    }

    @Override
    public @NotNull PsiElement createElement(ASTNode astNode) {
        return null;
    }

    @Override
    public @NotNull PsiFile createFile(@NotNull FileViewProvider fileViewProvider) {
        return null;
    }

}
