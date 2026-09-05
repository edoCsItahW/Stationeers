// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Tests for the IC10 Java incremental compilation components:
 * {@link IncLexer}, {@link IncParser}, {@link IncCompiler}.
 * <p>
 * Mirrors the C++ {@code test_incremental.cpp} and Node.js
 * {@code incremental.test.ts} test suites at matching granularity.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("Incremental compilation tests")
class IncrementalTest {

    @BeforeAll
    static void setupLanguage() {
        IC10CompilerLocal.setLanguage("zh-hans");
    }

    private static final String SRC_BASELINE = String.join("\n",
            "alias ic d0",
            "alias something r0",
            "move r0 42",
            "add r0 r0 1",
            "yield"
    );

    // ---- IncLexer ----

    @Nested
    @DisplayName("IncLexer")
    class IncLexerTests {

        @Test
        @DisplayName("tokenizeFull returns full result")
        void tokenizeFull() {
            IncLexer lexer = new IncLexer();
            String json = lexer.tokenizeFull(SRC_BASELINE);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"incremental\":false"));
            assertTrue(json.contains("\"relexedLines\""));
            assertTrue(json.contains("\"changedStartLine\""));
        }

        @Test
        @DisplayName("tokenizeInc with unchanged source reuses cache")
        void tokenizeIncUnchanged() {
            IncLexer lexer = new IncLexer();
            lexer.tokenizeFull(SRC_BASELINE);
            String json = lexer.tokenizeInc(SRC_BASELINE);
            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":true"));
            assertTrue(json.contains("\"relexedLines\":0"));
        }

        @Test
        @DisplayName("tokenizeInc detects single-line change")
        void tokenizeIncSingleLineChange() {
            IncLexer lexer = new IncLexer();
            lexer.tokenizeFull(SRC_BASELINE);

            String modified = String.join("\n",
                    "alias ic d0",
                    "alias something r0",
                    "move r0 99",
                    "add r0 r0 1",
                    "yield"
            );
            String json = lexer.tokenizeInc(modified);
            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":true"));
        }

        @Test
        @DisplayName("tokenizeInc detects inserted line")
        void tokenizeIncInsertedLine() {
            IncLexer lexer = new IncLexer();
            lexer.tokenizeFull(SRC_BASELINE);

            String inserted = String.join("\n",
                    "alias ic d0",
                    "alias something r0",
                    "move r0 42",
                    "add r0 r0 1",
                    "hcf",
                    "yield"
            );
            String json = lexer.tokenizeInc(inserted);
            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":true"));
        }

        @Test
        @DisplayName("tokenizeInc detects deleted line")
        void tokenizeIncDeletedLine() {
            IncLexer lexer = new IncLexer();
            lexer.tokenizeFull(SRC_BASELINE);

            String deleted = String.join("\n",
                    "alias ic d0",
                    "move r0 42",
                    "add r0 r0 1",
                    "yield"
            );
            String json = lexer.tokenizeInc(deleted);
            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":true"));
        }

        @Test
        @DisplayName("tokenizeInc detects appended lines")
        void tokenizeIncAppend() {
            IncLexer lexer = new IncLexer();
            lexer.tokenizeFull(SRC_BASELINE);

            String appended = SRC_BASELINE + "\nmove r1 10";
            String json = lexer.tokenizeInc(appended);
            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":true"));
        }

        @Test
        @DisplayName("tokenizeInc falls back to full when cache empty")
        void tokenizeIncFallback() {
            IncLexer lexer = new IncLexer();
            String json = lexer.tokenizeInc(SRC_BASELINE);
            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":false"));
        }

        

        

        

        @Test
        @DisplayName("full-then-inc gives consistent structure")
        void fullThenIncConsistentStructure() {
            IncLexer lexer = new IncLexer();
            String fullJson = lexer.tokenizeFull(SRC_BASELINE);
            lexer.tokenizeInc(SRC_BASELINE); // unchanged second call
            String incJson = lexer.tokenizeInc(SRC_BASELINE);

            assertNotNull(fullJson);
            assertNotNull(incJson);
            // both contain tokens
            assertTrue(fullJson.contains("\"tokens\""));
            assertTrue(incJson.contains("\"tokens\""));
        }
    }

    // ---- IncParser ----

    @Nested
    @DisplayName("IncParser")
    class IncParserTests {

        private Token[] tokenize(String src) {
            return Lexer.tokenize(src, false);
        }

        @Test
        @DisplayName("parseFull produces AST JSON")
        void parseFull() {
            IncParser parser = new IncParser();
            Token[] tokens = tokenize(SRC_BASELINE);
            String json = parser.parseFull(tokens);
            assertNotNull(json);
            assertTrue(json.contains("\"ast\""));
            assertTrue(json.contains("\"incremental\":false"));
            assertTrue(json.contains("\"reparsedStmts\""));
        }

        @Test
        @DisplayName("parseInc reuses cache for unchanged tokens")
        void parseInc() {
            IncParser parser = new IncParser();
            Token[] tokens = tokenize(SRC_BASELINE);
            parser.parseFull(tokens);

            String modified = String.join("\n",
                    "alias ic d0",
                    "alias something r0",
                    "move r0 99",
                    "add r0 r0 1",
                    "yield"
            );
            Token[] newTokens = tokenize(modified);
            String json = parser.parseInc(newTokens, 3);

            assertNotNull(json);
            assertTrue(json.contains("\"ast\""));
            assertTrue(json.contains("\"incremental\":true"));
        }

        

        

        

        
    }

    // ---- IncCompiler ----

    @Nested
    @DisplayName("IncCompiler")
    class IncCompilerTests {

        @Test
        @DisplayName("compileFull returns tokens and AST")
        void compileFull() {
            IncCompiler compiler = new IncCompiler();
            String json = compiler.compileFull(SRC_BASELINE);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
            assertTrue(json.contains("\"incremental\":false"));
        }

        

        

        

        

        

        

        

        

        

        @Test
        @DisplayName("compileInc empty source fallback")
        void compileIncEmptySource() {
            IncCompiler compiler = new IncCompiler();
            String json = compiler.compileInc("");
            assertNotNull(json);
        }
    }
}
