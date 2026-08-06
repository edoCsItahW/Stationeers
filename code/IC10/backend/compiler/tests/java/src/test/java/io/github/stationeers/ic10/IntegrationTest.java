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
        IC10Local.setLanguage("zh-hans");
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
        @DisplayName("token count preserved between lex and parse")
        void tokenCountPreserved() {
            Token[] tokens = lex("hcf\n");
            assertTrue(tokens.length >= 3, "hcf + newline + end");
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
        @DisplayName("pipeline with device type definition")
        void pipelineWithDeviceType() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure rw",
                    "#> @end-device",
                    "alias s d0 #: @type Sensor",
                    "hcf"
            );
            Token[] tokens = lex(src);
            Program program = Parser.parsing(tokens, false);
            Analyser analyser = new Analyser();
            analyser.visit(program);

            String typeJson = analyser.getTypeTable().toJSON();
            assertTrue(typeJson.contains("Sensor"));
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
        @DisplayName("parser diagnostics are accessible")
        void parserDiagnostics() {
            Token[] tokens = lex("hcf\n");
            Parser parser = new Parser();
            parser.init(tokens, false);
            parser.parse();
            Diagnostic[] diags = parser.getDiagnostics();
            assertNotNull(diags);
        }

        @Test
        @DisplayName("analyser diagnostics are accessible")
        void analyserDiagnostics() {
            Analyser analyser = analyse("alias ic d0\nhcf\n");
            Diagnostic[] diags = analyser.getDiagnostics();
            assertNotNull(diags);
        }

        @Test
        @DisplayName("linker diagnostics are accessible")
        void linkerDiagnostics() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("alias a r0\nhcf\n");
            linker.link();
            Diagnostic[] diags = linker.getDiagnostics();
            assertNotNull(diags);
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
        @DisplayName("IncCompiler inc on unchanged source produces valid output")
        void incCompilerIncUnchanged() {
            IncCompiler compiler = new IncCompiler();
            compiler.compileFull(SRC);
            String json = compiler.compileInc(SRC);
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

        @Test
        @DisplayName("IncCompiler inc on empty source falls back to full")
        void incCompilerIncEmpty() {
            IncCompiler compiler = new IncCompiler();
            String json = compiler.compileInc("");
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

    @Nested
    @DisplayName("Type inference and device context")
    class TypeInferenceAndDeviceContext {

        @Test
        @DisplayName("validate logic names against device type via doc comment")
        void validateLogicNamesViaDocComment() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure rw",
                    "#> @logic Temperature rw",
                    "#> @end-device",
                    "alias sensor d0 #: @type Sensor",
                    "l r0 sensor Pressure",
                    "l r1 sensor Temperature",
                    "hcf"
            );
            Parser parser = new Parser();
            parser.init(lex(src), false);
            parser.parse();
            assertEquals(0, parser.getDiagnostics().length,
                    "parser should produce no diagnostics");

            Analyser analyser = analyse(src);
            assertEquals(0, analyser.getDiagnostics().length,
                    "analyser should produce no diagnostics for valid logic names");
        }

        @Test
        @DisplayName("report error for invalid logic name on typed device")
        void reportErrorForInvalidLogicName() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure rw",
                    "#> @end-device",
                    "alias sensor d0 #: @type Sensor",
                    "l r0 sensor InvalidLogic",
                    "hcf"
            );
            Parser parser = new Parser();
            parser.init(lex(src), false);
            parser.parse();
            assertEquals(0, parser.getDiagnostics().length);

            Analyser analyser = analyse(src);
            assertTrue(analyser.getDiagnostics().length > 0,
                    "analyser should report diagnostic for invalid logic name");
            boolean foundIwa14_2 = false;
            for (Diagnostic d : analyser.getDiagnostics()) {
                if ("IWA14_2".equals(d.id)) {
                    foundIwa14_2 = true;
                    break;
                }
            }
            assertTrue(foundIwa14_2, "should report IWA14_2 for invalid logic name");
        }

        @Test
        @DisplayName("do not duplicate diagnostics for same invalid identifier")
        void noDuplicateDiagnosticsForSameInvalidIdentifier() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure rw",
                    "#> @end-device",
                    "alias sensor d0 #: @type Sensor",
                    "l r0 sensor BadLogic",
                    "hcf"
            );
            Analyser analyser = analyse(src);
            int badLogicCount = 0;
            for (Diagnostic d : analyser.getDiagnostics()) {
                if (d.message != null && d.message.contains("BadLogic")) {
                    badLogicCount++;
                }
            }
            assertTrue(badLogicCount <= 1,
                    "should not duplicate diagnostics for same invalid identifier");
        }

        @Test
        @DisplayName("pass device context across operands in same instruction")
        void passDeviceContextAcrossOperands() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Furnace",
                    "#> @logic Temperature r",
                    "#> @logic Active rw",
                    "#> @end-device",
                    "alias furnace d0 #: @type Furnace",
                    "s furnace Active r0",
                    "hcf"
            );
            Parser parser = new Parser();
            parser.init(lex(src), false);
            parser.parse();
            assertEquals(0, parser.getDiagnostics().length);

            Analyser analyser = analyse(src);
            assertEquals(0, analyser.getDiagnostics().length,
                    "device context should pass across operands in same instruction");
        }

        @Test
        @DisplayName("reset device context between instructions")
        void resetDeviceContextBetweenInstructions() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @logic Pressure rw",
                    "#> @end-device",
                    "alias sensor d0 #: @type Sensor",
                    "l r0 sensor Pressure",
                    "move r1 42",
                    "hcf"
            );
            Parser parser = new Parser();
            parser.init(lex(src), false);
            parser.parse();
            assertEquals(0, parser.getDiagnostics().length);

            Analyser analyser = analyse(src);
            assertEquals(0, analyser.getDiagnostics().length,
                    "device context should reset between instructions");
        }

        @Test
        @DisplayName("validate slot index against device type")
        void validateSlotIndexAgainstDeviceType() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Stacker",
                    "#> @slot 0 ore",
                    "#> @slot 1 ingot",
                    "#> @logicSlot Occupied",
                    "#> @end-device",
                    "alias stacker d0 #: @type Stacker",
                    "ls r0 stacker 0 Occupied",
                    "hcf"
            );
            Parser parser = new Parser();
            parser.init(lex(src), false);
            parser.parse();
            assertEquals(0, parser.getDiagnostics().length,
                    "parser should produce no diagnostics for valid slot index");
        }

        @Test
        @DisplayName("validate reagent mode via enum doc comment")
        void validateReagentModeViaEnumDocComment() {
            String src = String.join("\n",
                    "#> @enum",
                    "#> @name ReagentMode",
                    "#> @value Contents 0",
                    "#> @value Required 1",
                    "#> @end-enum",
                    "#> @device",
                    "#> @name Filter",
                    "#> @end-device",
                    "alias filter d0 #: @type Filter",
                    "lr r0 filter Contents Oxygen",
                    "hcf"
            );
            Parser parser = new Parser();
            parser.init(lex(src), false);
            parser.parse();
            assertEquals(0, parser.getDiagnostics().length,
                    "parser should produce no diagnostics for valid reagent mode");
        }

        @Test
        @DisplayName("include type information in symbol table for typed alias")
        void includeTypeInformationInSymbolTable() {
            String src = String.join("\n",
                    "#> @device",
                    "#> @name Sensor",
                    "#> @end-device",
                    "alias sensor d0 #: @type Sensor",
                    "hcf"
            );
            Analyser analyser = analyse(src);
            String symJson = analyser.getSymbolTable().toJSON();
            assertNotNull(symJson);
            assertTrue(symJson.contains("sensor"),
                    "symbol table should contain 'sensor' alias");
            // BasicType.DEVICE 序列化为数值 4（对齐 C++ BasicType::DEVICE = 4）
            assertTrue(symJson.contains("\"type\":4") || symJson.contains("\"type\": 4"),
                    "sensor symbol type should be DEVICE (4)");
            assertTrue(symJson.contains("Sensor"),
                    "sensor symbol typeName should be 'Sensor'");
        }
    }
}
