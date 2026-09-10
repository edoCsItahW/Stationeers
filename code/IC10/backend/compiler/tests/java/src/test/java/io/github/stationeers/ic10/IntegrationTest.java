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
 * Integration tests for the IC10 Java binding.
 * <p>
 * Mirrors the Node.js {@code integration.test.ts} and Python
 * {@code test_integration.py} test suites. Validates data flow across
 * the Lexer → Parser → Analyser → Linker pipeline, diagnostic
 * propagation, and consistency between full and incremental compilation.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("Integration tests")
class IntegrationTest {

    @BeforeAll
    static void setupLanguage() {
        IC10CompilerLocal.setLanguage("zh-hans");
    }

    // ---- Helpers ----

    private Token[] lex(String src) {
        return Lexer.tokenize(src, false);
    }

    private Program parse(String src) {
        return Parser.parsing(lex(src), false);
    }

    private Analyser analyse(String src) {
        Program program = parse(src);
        Analyser analyser = new Analyser();
        analyser.visit(program);
        return analyser;
    }

    // ============================================================
    // Lexer -> Parser data flow
    // ============================================================

    @Nested
    @DisplayName("Lexer → Parser data flow")
    class LexerToParser {

        @Test
        @DisplayName("tokens flow from Lexer to Parser correctly")
        void tokenFlow() {
            String src = "alias ic d0\nmove r0 42\nhcf\n";
            Token[] tokens = lex(src);
            assertTrue(tokens.length > 0);
            assertEquals(TokenType.END.getValue(), tokens[tokens.length - 1].getType());

            Program program = Parser.parsing(tokens, false);
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        

        @Test
        @DisplayName("empty token stream handled")
        void emptyTokenStream() {
            Token[] tokens = lex("");
            assertEquals(1, tokens.length); // only END
            Program program = Parser.parsing(tokens, false);
            assertNotNull(program);
        }
    }

    // ============================================================
    // Parser -> Analyser data flow
    // ============================================================

    @Nested
    @DisplayName("Parser → Analyser data flow")
    class ParserToAnalyser {

        @Test
        @DisplayName("AST flows into Analyser")
        void astFlow() {
            Program program = parse("alias ic d0\nhcf\n");
            Analyser analyser = new Analyser();
            analyser.visit(program);

            SymbolTable st = analyser.getSymbolTable();
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("ic"), "symbol table should contain alias");
        }

        @Test
        @DisplayName("analyser type table populated")
        void analyserTypeTable() {
            Analyser analyser = analyse(
                    "#> @device\n" +
                    "#> @name Furnace\n" +
                    "#> @end-device\n" +
                    "alias f d0 #: @type Furnace\n" +
                    "hcf\n");
            TypeTable tt = analyser.getTypeTable();
            assertNotNull(tt);
            String json = tt.toJSON();
            assertTrue(json.contains("Furnace"));
        }

        @Test
        @DisplayName("analyser handles empty program")
        void analyserEmptyProgram() {
            Analyser analyser = analyse("");
            assertNotNull(analyser.getSymbolTable());
            assertNotNull(analyser.getSymbolTable().toJSON());
        }

        @Test
        @DisplayName("static Analyser.analyse does not throw")
        void staticAnalyse() {
            Program program = parse("alias ic d0\nhcf\n");
            assertDoesNotThrow(() -> Analyser.analyse(program));
        }
    }

    // ============================================================
    // End-to-end pipeline
    // ============================================================

    @Nested
    @DisplayName("End-to-end pipeline")
    class EndToEnd {

        @Test
        @DisplayName("full pipeline: source -> tokens -> ast -> symboltable")
        void fullPipeline() {
            String src = String.join("\n",
                    "alias ic d0",
                    "alias something r0",
                    "move r0 42",
                    "add r0 r0 1",
                    "yield"
            );
            Token[] tokens = lex(src);
            Program program = Parser.parsing(tokens, false);
            Analyser analyser = new Analyser();
            analyser.visit(program);

            SymbolTable st = analyser.getSymbolTable();
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("ic"));
            assertTrue(json.contains("something"));
        }

        @Test
        @DisplayName("linker integrates symbol tables from multiple compilations")
        void linkerIntegration() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("alias a r0\nhcf\n");
            linker.addUnitSourceNoPath("alias b r1\nhcf\n");
            SymbolTable st = linker.link();
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("a") && json.contains("b"));
        }

        @Test
        @DisplayName("analyser and linker type table consistent")
        void analyserLinkerTypeConsistency() {
            String deviceDef =
                    "#> @device\n" +
                    "#> @name Furnace\n" +
                    "#> @end-device\n";

            // via Analyser
            Analyser analyser = analyse(deviceDef + "alias f d0 #: @type Furnace\nhcf\n");
            String analyserTypeJson = analyser.getTypeTable().toJSON();

            // via Linker
            Linker linker = new Linker();
            linker.addUnitSourceNoPath(deviceDef);
            linker.addUnitSourceNoPath("alias f d0 #: @type Furnace\nhcf\n");
            linker.link();
            String linkerTypeJson = linker.getTypeTable().toJSON();

            assertTrue(analyserTypeJson.contains("Furnace"));
            assertTrue(linkerTypeJson.contains("Furnace"));
        }
    }

    // ============================================================
    // Diagnostic propagation
    // ============================================================

    @Nested
    @DisplayName("Diagnostic propagation")
    class DiagnosticPropagation {

        @Test
        @DisplayName("lexer diagnostics are accessible")
        void lexerDiagnostics() {
            Lexer lexer = new Lexer();
            lexer.init("100abc", false);
            lexer.scan();
            Diagnostic[] diags = lexer.getDiagnostics();
            assertTrue(diags.length > 0);
        }

        

        

        

        @Test
        @DisplayName("diagnostic fields accessible across stages")
        void diagnosticFieldsAcrossStages() {
            // lexer diagnostic
            Lexer lexer = new Lexer();
            lexer.init("\"unclosed", false);
            lexer.scan();
            Diagnostic[] lexDiags = lexer.getDiagnostics();
            assertTrue(lexDiags.length > 0);
            assertNotNull(lexDiags[0].id);
            assertNotNull(lexDiags[0].message);
            assertNotNull(lexDiags[0].start);
        }
    }

    // ============================================================
    // Full vs incremental consistency
    // ============================================================

    @Nested
    @DisplayName("Full vs incremental consistency")
    class FullVsIncremental {

        private static final String SRC = String.join("\n",
                "alias ic d0",
                "move r0 42",
                "hcf"
        );

        @Test
        @DisplayName("IncCompiler full produces valid tokens and AST")
        void incCompilerFull() {
            IncCompiler compiler = new IncCompiler();
            String json = compiler.compileFull(SRC);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
        }

        

        @Test
        @DisplayName("IncCompiler inc on modified source produces valid output")
        void incCompilerIncModified() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC);

            String modified = String.join("\n",
                    "alias ic d0",
                    "move r0 99",
                    "hcf"
            );
            String json = compiler.compileInc(modified);
            assertNotNull(json);
            assertTrue(json.contains("\"tokens\""));
            assertTrue(json.contains("\"ast\""));
        }

        
    }

    // ============================================================
    // Multi-program / system tests
    // ============================================================

    @Nested
    @DisplayName("Program-level semantics")
    class ProgramSemantics {

        @Test
        @DisplayName("Program start/end positions accessible")
        void programStartEnd() {
            Program program = parse("hcf\n");
            assertNotNull(program.getStart());
            assertNotNull(program.getEnd());
        }

        @Test
        @DisplayName("multiple programs isolated")
        void multipleProgramsIsolated() {
            Program p1 = parse("alias a r0\n");
            Program p2 = parse("alias b r1\n");
            assertNotNull(p1);
            assertNotNull(p2);
            // verify both produce valid JSON
            assertNotNull(p1.toJSON());
            assertNotNull(p2.toJSON());
        }

        @Test
        @DisplayName("TypeOfNode mapping is comprehensive")
        void typeOfNodeMapping() {
            String json = TypeOfNode.toJSON();
            assertNotNull(json);
            assertFalse(json.isEmpty());
            assertTrue(json.length() > 50);
        }
    }

    // ============================================================
    // Type inference and device context
    // 对齐 Node integration.test.ts 同名分组
    // ============================================================

    
}
