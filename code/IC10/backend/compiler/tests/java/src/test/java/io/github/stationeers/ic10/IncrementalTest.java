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
        IC10Local.setLanguage("zh-hans");
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
        @DisplayName("hasCache initially false")
        void hasCacheInitiallyFalse() {
            IncLexer lexer = new IncLexer();
            assertFalse(lexer.hasCache());
        }

        @Test
        @DisplayName("hasCache true after tokenizeFull")
        void hasCacheAfterFull() {
            IncLexer lexer = new IncLexer();
            lexer.tokenizeFull(SRC_BASELINE);
            assertTrue(lexer.hasCache());
        }

        @Test
        @DisplayName("clear removes cache")
        void clearRemovesCache() {
            IncLexer lexer = new IncLexer();
            lexer.tokenizeFull(SRC_BASELINE);
            assertTrue(lexer.hasCache());
            lexer.clear();
            assertFalse(lexer.hasCache());
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

        @Test
        @DisplayName("parseInc with changedStartLine=0 falls back to full")
        void parseIncUnchanged() {
            // changedStartLine=0 触发回退全量解析（对齐 C++ inc_parser.cpp:68）
            // parseFull 返回 incremental=false
            IncParser parser = new IncParser();
            Token[] tokens = tokenize(SRC_BASELINE);
            parser.parseFull(tokens);
            String json = parser.parseInc(tokens, 0);

            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":false"));
        }

        @Test
        @DisplayName("hasCache initially false")
        void hasCacheInitiallyFalse() {
            IncParser parser = new IncParser();
            assertFalse(parser.hasCache());
        }

        @Test
        @DisplayName("hasCache true after parseFull")
        void hasCacheAfterFull() {
            IncParser parser = new IncParser();
            Token[] tokens = tokenize(SRC_BASELINE);
            parser.parseFull(tokens);
            assertTrue(parser.hasCache());
        }

        @Test
        @DisplayName("clear removes cache")
        void clearRemovesCache() {
            IncParser parser = new IncParser();
            Token[] tokens = tokenize(SRC_BASELINE);
            parser.parseFull(tokens);
            assertTrue(parser.hasCache());
            parser.clear();
            assertFalse(parser.hasCache());
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
        @DisplayName("compileInc with unchanged source")
        void compileIncUnchanged() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC_BASELINE);
            String json = compiler.compileInc(SRC_BASELINE);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
        }

        @Test
        @DisplayName("compileInc detects single-line change")
        void compileIncSingleLineChange() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC_BASELINE);

            String modified = String.join("\n",
                    "alias ic d0",
                    "alias something r0",
                    "move r0 99",
                    "add r0 r0 1",
                    "yield"
            );
            String json = compiler.compileInc(modified);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
        }

        @Test
        @DisplayName("compileInc detects inserted lines")
        void compileIncInserted() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC_BASELINE);

            String inserted = String.join("\n",
                    "alias ic d0",
                    "alias something r0",
                    "move r0 42",
                    "add r0 r0 1",
                    "hcf",
                    "yield"
            );
            String json = compiler.compileInc(inserted);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
        }

        @Test
        @DisplayName("compileInc detects deleted lines")
        void compileIncDeleted() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC_BASELINE);

            String deleted = String.join("\n",
                    "alias ic d0",
                    "move r0 42",
                    "add r0 r0 1",
                    "yield"
            );
            String json = compiler.compileInc(deleted);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
        }

        @Test
        @DisplayName("compileInc detects appended lines")
        void compileIncAppend() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC_BASELINE);

            String appended = SRC_BASELINE + "\nmove r1 10";
            String json = compiler.compileInc(appended);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
        }

        @Test
        @DisplayName("compileInc falls back when cache empty")
        void compileIncFallback() {
            IncCompiler compiler = new IncCompiler();
            String json = compiler.compileInc(SRC_BASELINE);
            assertNotNull(json);
            assertTrue(json.contains("\"incremental\":false"));
        }

        @Test
        @DisplayName("hasCache initially false")
        void hasCacheInitiallyFalse() {
            IncCompiler compiler = new IncCompiler();
            assertFalse(compiler.hasCache());
        }

        @Test
        @DisplayName("hasCache true after compileFull")
        void hasCacheAfterFull() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC_BASELINE);
            assertTrue(compiler.hasCache());
        }

        @Test
        @DisplayName("clear removes cache")
        void clearRemovesCache() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC_BASELINE);
            assertTrue(compiler.hasCache());
            compiler.clear();
            assertFalse(compiler.hasCache());
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
