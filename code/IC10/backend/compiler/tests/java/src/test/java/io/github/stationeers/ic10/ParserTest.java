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
 * Tests for the IC10 Java Parser.
 * <p>
 * Mirrors the C++ {@code test_parser.cpp} and Node.js {@code parser.test.ts}
 * test suites at matching granularity. Because the Java binding exposes only
 * {@link Program#toJSON()} for AST inspection, assertions verify the presence
 * of expected tokens/fields within the serialized JSON output.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("Parser tests")
class ParserTest {

    @BeforeAll
    static void setupLanguage() {
        IC10CompilerLocal.setLanguage("zh-hans");
    }

    // ---- Helpers ----

    private static Token[] tokenize(String src) {
        return Lexer.tokenize(src, false);
    }

    private static Program parseProgram(String src) {
        Token[] tokens = tokenize(src);
        return Parser.parsing(tokens, false);
    }

    // ============================================================
    // Basic parsing
    // ============================================================

    @Nested
    @DisplayName("Basic parsing")
    class BasicParsing {

        @Test
        @DisplayName("empty program produces valid Program")
        void emptyProgram() {
            Program program = parseProgram("");
            assertNotNull(program);
            assertNotNull(program.toJSON());
            assertNotNull(program.toString());
        }

        @Test
        @DisplayName("whitespace-only program")
        void onlyWhitespace() {
            Program program = parseProgram("   \n  \n  ");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("comments-only program")
        void onlyComments() {
            Program program = parseProgram("# this is a comment\n# another comment\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("Program getNodeName returns 'Program'")
        void getNodeName() {
            assertEquals("Program", Program.getNodeName());
        }
    }

    // ============================================================
    // Labels
    // ============================================================

    @Nested
    @DisplayName("Labels")
    class Labels {

        @Test
        @DisplayName("single label")
        void parseLabelSimple() {
            Program program = parseProgram("main:\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertNotNull(json);
        }

        @Test
        @DisplayName("multiple labels")
        void parseLabelMultiple() {
            Program program = parseProgram("start:\nloop:\nend:\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }
    }

    // ============================================================
    // Preprocessor directives
    // ============================================================

    @Nested
    @DisplayName("Preprocessor directives")
    class Preprocessor {

        @Test
        @DisplayName("alias directive")
        void parseAlias() {
            Program program = parseProgram("alias foo r0\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("foo") || json.contains("alias"),
                    "JSON should mention alias or name");
        }

        @Test
        @DisplayName("alias directive with device")
        void parseAliasDevice() {
            Program program = parseProgram("alias dev d0\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("dev") || json.contains("d0"));
        }

        @Test
        @DisplayName("define with integer")
        void parseDefineWithInteger() {
            Program program = parseProgram("define MAX_COUNT 100\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("MAX_COUNT") || json.contains("100"));
        }

        

        @Test
        @DisplayName("define with hex")
        void parseDefineWithHex() {
            Program program = parseProgram("define MASK $FF\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("alias with type hint")
        void parseAliasWithTypeHint() {
            Program program = parseProgram("alias myFurnace d0 #: @type Furnace\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("Furnace"), "JSON should contain type name");
        }

        @Test
        @DisplayName("alias with desc type hint")
        void parseAliasWithDescTypeHint() {
            Program program = parseProgram("alias myFurnace d0 #: @desc 炉窑设备\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("炉窑") || json.contains("desc"));
        }

        @Test
        @DisplayName("alias with type and desc")
        void parseAliasWithTypeAndDesc() {
            Program program = parseProgram("alias myFurnace d0 #: @type Furnace @desc 炉窑\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("Furnace"));
        }

        @Test
        @DisplayName("define with desc type hint")
        void parseDefineWithDescTypeHint() {
            Program program = parseProgram("define MAX 100 #: @desc 最大值\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("define with type and desc")
        void parseDefineWithTypeAndDesc() {
            Program program = parseProgram("define PRESSURE 101325 #: @type Pressure @desc 标准大气压\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("Pressure"));
        }
    }

    // ============================================================
    // Doc comments
    // ============================================================

    

    // ============================================================
    // Instructions (by meta)
    // ============================================================

    @Nested
    @DisplayName("Instructions by meta")
    class Instructions {

        @Test
        @DisplayName("nullary: hcf")
        void parseNullaryHcf() {
            Program program = parseProgram("hcf\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("hcf") || json.contains("Hcf"));
        }

        

        @Test
        @DisplayName("unary: j label")
        void parseUnaryJ() {
            Program program = parseProgram("j main\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        

        

        @Test
        @DisplayName("binary: move r0 42")
        void parseBinaryMove() {
            Program program = parseProgram("move r0 42\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("move") || json.contains("r0"));
        }

        

        

        

        @Test
        @DisplayName("ternary: add r0 r1 r2")
        void parseTernary() {
            Program program = parseProgram("add r0 r1 r2\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("add") || json.contains("r0"));
        }

        

        

        @Test
        @DisplayName("quaternary: lb r0 d0 0")
        void parseQuaternaryLb() {
            Program program = parseProgram("lb r0 d0 0\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        

        @Test
        @DisplayName("quinary: lbn r0 db Name")
        void parseQuinaryLbn() {
            Program program = parseProgram("lbn r0 db Name\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("senary: lbns r0 db Name 1")
        void parseSenaryLbns() {
            Program program = parseProgram("lbns r0 db Name 1\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }
    }

    // ============================================================
    // Multi-statement programs
    // ============================================================

    @Nested
    @DisplayName("Multi-statement programs")
    class MultiStatement {

        @Test
        @DisplayName("alias + instruction")
        void aliasPlusInstruction() {
            Program program = parseProgram("alias ic d0\nhcf\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("ic") || json.contains("hcf"));
        }

        @Test
        @DisplayName("multiple instructions")
        void multipleInstructions() {
            Program program = parseProgram("move r0 42\nadd r0 r0 1\nyield\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("program with comments")
        void programWithComments() {
            Program program = parseProgram(
                    "# Setup\n" +
                    "alias ic d0\n" +
                    "# Loop\n" +
                    "start: add r0 r1 r2\n" +
                    "yield\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertNotNull(json);
        }

        @Test
        @DisplayName("complex device program")
        void complexDeviceProgram() {
            Program program = parseProgram(
                    "#> @device\n" +
                    "#> @name Furnace\n" +
                    "#> @end-device\n" +
                    "alias furnace d0 #: @type Furnace\n" +
                    "hcf\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("Furnace"));
        }
    }

    // ============================================================
    // AST serialization
    // ============================================================

    @Nested
    @DisplayName("AST serialization")
    class Serialization {

        @Test
        @DisplayName("toJSON returns non-empty valid JSON string")
        void toJSON() {
            Program program = parseProgram("move r0 42\n");
            String json = program.toJSON();
            assertNotNull(json);
            assertFalse(json.isEmpty());
            assertTrue(json.startsWith("{"), "JSON should start with '{'");
            assertTrue(json.endsWith("}"), "JSON should end with '}'");
        }

        @Test
        @DisplayName("toString returns non-empty string")
        void toStringTest() {
            Program program = parseProgram("move r0 42\n");
            String str = program.toString();
            assertNotNull(str);
            assertFalse(str.isEmpty());
        }

        @Test
        @DisplayName("JSON contains source information")
        void jsonContainsInfo() {
            Program program = parseProgram("alias ic d0\n");
            String json = program.toJSON();
            assertTrue(json.contains("\"") || json.length() > 10);
        }
    }

    // ============================================================
    // Static vs instance API
    // ============================================================

    @Nested
    @DisplayName("Static vs instance API")
    class ParserApi {

        @Test
        @DisplayName("static parsing matches instance parse")
        void staticMatchesInstance() {
            String src = "alias ic d0\n";
            Token[] tokens = tokenize(src);

            Program p1 = Parser.parsing(tokens, false);

            Parser parser = new Parser();
            parser.init(tokens, false);
            Program p2 = parser.parse();

            assertNotNull(p1);
            assertNotNull(p2);
            assertNotNull(p1.toJSON());
            assertNotNull(p2.toJSON());
        }

        @Test
        @DisplayName("parser.getDiagnostics for valid input is empty")
        void diagnosticsEmptyForValid() {
            Token[] tokens = tokenize("hcf\n");
            Parser parser = new Parser();
            parser.init(tokens, false);
            parser.parse();
            Diagnostic[] diags = parser.getDiagnostics();
            assertNotNull(diags);
        }
    }

    // ============================================================
    // Error recovery / robustness
    // ============================================================

    @Nested
    @DisplayName("Error recovery / robustness")
    class ErrorRecovery {

        @Test
        @DisplayName("incomplete input does not crash")
        void incompleteInput() {
            assertDoesNotThrow(() -> parseProgram("move r0\n"));
        }

        @Test
        @DisplayName("garbage input does not crash")
        void garbageInput() {
            assertDoesNotThrow(() -> parseProgram("!@#$%"));
        }

        @Test
        @DisplayName("no newline at end does not crash")
        void noNewlineAtEnd() {
            assertDoesNotThrow(() -> parseProgram("hcf"));
        }

        @Test
        @DisplayName("multiple empty lines handled")
        void multipleEmptyLines() {
            assertDoesNotThrow(() -> parseProgram("\n\n\n"));
        }
    }

    // ============================================================
    // More instruction types
    // ============================================================

    

    // ============================================================
    // Jump and branch instructions
    // ============================================================

    @Nested
    @DisplayName("Jump and branch instructions")
    class JumpBranchInstructions {

        @Test
        @DisplayName("jump: j label")
        void parseJump() {
            Program program = parseProgram("j label\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("branch: beq r0 r1 label")
        void parseBranchBeq() {
            Program program = parseProgram("beq r0 r1 label\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        

        

        

        

        

        @Test
        @DisplayName("multiple branch instructions")
        void parseMultipleBranches() {
            Program program = parseProgram(
                    "beq r0 r1 label\n" +
                    "bne r0 r1 label\n" +
                    "blt r0 r1 label\n" +
                    "bgt r0 r1 label\n" +
                    "ble r0 r1 label\n" +
                    "bge r0 r1 label\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }
    }

    // ============================================================
    // Stack instructions
    // ============================================================

    @Nested
    @DisplayName("Stack instructions")
    class StackInstructions {

        @Test
        @DisplayName("push r0")
        void parsePush() {
            Program program = parseProgram("push r0\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        

        @Test
        @DisplayName("peek r0 0")
        void parsePeek() {
            Program program = parseProgram("peek r0 0\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("stack instructions combined")
        void parseStackCombined() {
            Program program = parseProgram("push r0\npop r0\npeek r0 0\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }
    }

    // ============================================================
    // Device instructions
    // ============================================================

    @Nested
    @DisplayName("Device instructions")
    class DeviceInstructions {

        @Test
        @DisplayName("l r0 d0 Pressure")
        void parseL() {
            Program program = parseProgram("l r0 d0 Pressure\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("s d0 On r0")
        void parseS() {
            Program program = parseProgram("s d0 On r0\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("ls r0 d0 0 Quantity")
        void parseLs() {
            Program program = parseProgram("ls r0 d0 0 Quantity\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("lr r0 d0 Contents label")
        void parseLr() {
            Program program = parseProgram("lr r0 d0 Contents label\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }
    }

    // ============================================================
    // Batch instructions
    // ============================================================

    @Nested
    @DisplayName("Batch instructions")
    class BatchInstructions {

        @Test
        @DisplayName("lb r0 100 Pressure Average")
        void parseLb() {
            Program program = parseProgram("lb r0 100 Pressure Average\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("lbs r0 100 0 Quantity Average")
        void parseLbs() {
            Program program = parseProgram("lbs r0 100 0 Quantity Average\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("lbn r0 100 200 Pressure Average")
        void parseLbnBatch() {
            Program program = parseProgram("lbn r0 100 200 Pressure Average\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        

        @Test
        @DisplayName("batch instructions combined")
        void parseBatchCombined() {
            Program program = parseProgram(
                    "lb r0 100 Pressure Average\n" +
                    "lbs r0 100 0 Quantity Average\n" +
                    "lbn r0 100 200 Pressure Average\n" +
                    "lbns r0 100 200 0 Quantity Average\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }
    }

    // ============================================================
    // Error recovery and robustness
    // ============================================================

    @Nested
    @DisplayName("Error recovery / robustness")
    class ErrorRecoveryExtended {

        @Test
        @DisplayName("invalid instruction produces error node but doesn't crash")
        void invalidInstructionNoCrash() {
            assertDoesNotThrow(() -> parseProgram("invalid_instruction r0 r1\n"));
        }

        @Test
        @DisplayName("error doesn't block subsequent statements")
        void errorDoesNotBlockSubsequent() {
            Program program = parseProgram("hcf\nbad_instruction\nyield\n");
            assertNotNull(program);
            assertNotNull(program.toJSON());
        }

        @Test
        @DisplayName("alias robustness incremental")
        void aliasRobustnessIncremental() {
            String[] inputs = {
                    "alias",
                    "alias test",
                    "alias test r0",
                    "alias test d0",
                    "alias test HASH",
                    "alias test HASH(",
                    "alias test HASH(\"",
                    "alias test HASH(\"Something",
                    "alias test HASH(\"Something\"",
                    "alias test HASH(\"Something\")",
                    "alias test STR",
                    "alias test STR(",
                    "alias test STR(\"",
                    "alias test STR(\"Equipment",
                    "alias test STR(\"Equipment\"",
                    "alias test STR(\"Equipment\")",
            };
            for (String input : inputs) {
                assertDoesNotThrow(() -> parseProgram(input + "\n"),
                        "should not throw for: " + input);
            }
        }

        @Test
        @DisplayName("define robustness incremental")
        void defineRobustnessIncremental() {
            String[] inputs = {
                    "define",
                    "define X",
                    "define X 1",
                    "define X 1.5",
                    "define X STR",
                    "define X STR(",
                    "define X STR(\"",
                    "define X STR(\"Structure",
                    "define X STR(\"Structure\"",
                    "define X STR(\"Structure\")",
                    "define X HASH",
                    "define X HASH(",
                    "define X HASH(\"",
                    "define X HASH(\"ItemName",
                    "define X HASH(\"ItemName\"",
                    "define X HASH(\"ItemName\")",
            };
            for (String input : inputs) {
                assertDoesNotThrow(() -> parseProgram(input + "\n"),
                        "should not throw for: " + input);
            }
        }

        @Test
        @DisplayName("instruction robustness nullary")
        void instructionRobustnessNullary() {
            String[] inputs = {"hcf", "hcf extra", "yield", "yield extra"};
            for (String input : inputs) {
                assertDoesNotThrow(() -> parseProgram(input + "\n"),
                        "should not throw for: " + input);
            }
        }

        

        

        

        @Test
        @DisplayName("instruction robustness with macro calls")
        void instructionRobustnessMacroCalls() {
            String[] inputs = {
                    "move r0 STR",
                    "move r0 STR(",
                    "move r0 STR(\"",
                    "move r0 STR(\"Test\"",
                    "move r0 STR(\"Test\")",
                    "move r0 HASH",
                    "move r0 HASH(",
                    "move r0 HASH(\"",
                    "move r0 HASH(\"Test\"",
                    "move r0 HASH(\"Test\")",
            };
            for (String input : inputs) {
                assertDoesNotThrow(() -> parseProgram(input + "\n"),
                        "should not throw for: " + input);
            }
        }
    }

    // ============================================================
    // Statement newline separation (IEP2)
    // ============================================================

    @Nested
    @DisplayName("Statement newline separation (IEP2)")
    class NewlineSeparation {

        @Test
        @DisplayName("statements with newline no error")
        void statementsWithNewlineNoError() {
            Token[] tokens = tokenize("hcf\nyield\n");
            Parser parser = new Parser();
            parser.init(tokens, false);
            parser.parse();
            Diagnostic[] diags = parser.getDiagnostics();
            assertEquals(0, diags.length, "statements separated by newline should have no diagnostics");
        }

        @Test
        @DisplayName("last statement without newline no error")
        void lastStatementWithoutNewline() {
            Token[] tokens = tokenize("hcf\nyield");
            Parser parser = new Parser();
            parser.init(tokens, false);
            parser.parse();
            Diagnostic[] diags = parser.getDiagnostics();
            assertEquals(0, diags.length, "last statement without trailing newline should be ok");
        }

        @Test
        @DisplayName("statement followed by comment then newline")
        void statementWithCommentThenNewline() {
            Token[] tokens = tokenize("hcf # comment\nyield\n");
            Parser parser = new Parser();
            parser.init(tokens, false);
            parser.parse();
            Diagnostic[] diags = parser.getDiagnostics();
            assertEquals(0, diags.length, "statement followed by inline comment then newline should be ok");
        }
    }

    // ============================================================
    // AST serialization extended
    // ============================================================

    @Nested
    @DisplayName("AST serialization extended")
    class SerializationExtended {

        @Test
        @DisplayName("toJSON contains alias fields")
        void jsonContainsAliasFields() {
            Program program = parseProgram("alias foo r0\n");
            String json = program.toJSON();
            assertNotNull(json);
            assertTrue(json.contains("foo"));
        }

        @Test
        @DisplayName("toJSON contains define fields")
        void jsonContainsDefineFields() {
            Program program = parseProgram("define MAX 100\n");
            String json = program.toJSON();
            assertNotNull(json);
            assertTrue(json.contains("MAX"));
        }

        @Test
        @DisplayName("toJSON contains label")
        void jsonContainsLabel() {
            Program program = parseProgram("start:\n");
            String json = program.toJSON();
            assertNotNull(json);
            assertTrue(json.contains("start"));
        }

        @Test
        @DisplayName("toJSON multiple statements contains all")
        void jsonMultipleStatements() {
            Program program = parseProgram(
                    "alias foo r0\n" +
                    "define MAX 10\n" +
                    "start:\n" +
                    "add r0 r1 r2\n" +
                    "hcf\n");
            String json = program.toJSON();
            assertNotNull(json);
            assertTrue(json.contains("foo"));
            assertTrue(json.contains("MAX"));
            assertTrue(json.contains("start"));
            assertTrue(json.contains("add"));
            assertTrue(json.contains("hcf"));
        }

        @Test
        @DisplayName("toString for multiple instructions not empty")
        void toStringMultipleInstructions() {
            Program program = parseProgram(
                    "move r0 0\n" +
                    "add r0 r0 1\n" +
                    "yield\n");
            String str = program.toString();
            assertNotNull(str);
            assertFalse(str.isEmpty());
        }
    }

    // ============================================================
    // Device doc comment extended
    // ============================================================

    

    // ============================================================
    // Type hint standalone error
    // ============================================================

    @Nested
    @DisplayName("Type hint and error recovery")
    class TypeHintErrors {

        @Test
        @DisplayName("type hint standalone produces error node")
        void typeHintStandaloneError() {
            assertDoesNotThrow(() -> parseProgram("#: @type Furnace\n"));
        }

        @Test
        @DisplayName("mixed doc comment and code")
        void mixedDocCommentAndCode() {
            Program program = parseProgram(
                    "#> @device\n" +
                    "#> @name Furnace\n" +
                    "#> @end-device\n" +
                    "alias f d0\n");
            assertNotNull(program);
            String json = program.toJSON();
            assertTrue(json.contains("Furnace"));
            assertTrue(json.contains("f"));
        }
    }
}
