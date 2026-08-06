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
 * Tests for the IC10 Java Lexer.
 * <p>
 * Mirrors the C++ {@code test_lexer.cpp} and Node.js {@code lexer.test.ts}
 * test suites at matching granularity. Covers token recognition (literals,
 * identifiers, registers, devices, strings, comments, annotations, keywords,
 * symbols), position tracking, diagnostics, and boundary/error-recovery cases.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("Lexer tests")
class LexerTest {

    @BeforeAll
    static void setupLanguage() {
        IC10Local.setLanguage("zh-hans");
    }

    // ---- Helpers ----

    /** Returns all tokens except NEWLINE and END. */
    private static Token[] meaningful(Token[] tokens) {
        int count = 0;
        for (Token t : tokens) {
            int type = t.getType();
            if (type != TokenType.NEWLINE.getValue() && type != TokenType.END.getValue()) {
                count++;
            }
        }
        Token[] out = new Token[count];
        int i = 0;
        for (Token t : tokens) {
            int type = t.getType();
            if (type != TokenType.NEWLINE.getValue() && type != TokenType.END.getValue()) {
                out[i++] = t;
            }
        }
        return out;
    }

    // ============================================================
    // Empty / whitespace input
    // ============================================================

    @Nested
    @DisplayName("Empty / whitespace input")
    class EmptyInput {

        @Test
        @DisplayName("empty input produces only END token")
        void emptyInputProducesOnlyEnd() {
            Token[] tokens = Lexer.tokenize("", false);
            assertEquals(1, tokens.length);
            assertEquals(TokenType.END.getValue(), tokens[0].getType());
        }

        @Test
        @DisplayName("whitespace-only input produces only END token")
        void whitespaceOnlyInput() {
            Token[] tokens = Lexer.tokenize("   \t  ", false);
            assertEquals(1, tokens.length);
            assertEquals(TokenType.END.getValue(), tokens[0].getType());
        }
    }

    // ============================================================
    // Numbers
    // ============================================================

    @Nested
    @DisplayName("Numbers")
    class Numbers {

        @Test
        @DisplayName("integer simple")
        void integerSimple() {
            Token[] t = meaningful(Lexer.tokenize("123", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.INTEGER.getValue(), t[0].getType());
            assertEquals(TokenCategory.LITERAL.getValue(), t[0].getCategory());
            assertEquals("123", t[0].getLexeme());
        }

        @Test
        @DisplayName("integer zero")
        void integerZero() {
            Token[] t = meaningful(Lexer.tokenize("0", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.INTEGER.getValue(), t[0].getType());
            assertEquals("0", t[0].getLexeme());
        }

        @Test
        @DisplayName("integer large")
        void integerLarge() {
            Token[] t = meaningful(Lexer.tokenize("999999999", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.INTEGER.getValue(), t[0].getType());
            assertEquals("999999999", t[0].getLexeme());
        }

        @Test
        @DisplayName("float simple")
        void floatSimple() {
            Token[] t = meaningful(Lexer.tokenize("3.14", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.FLOAT.getValue(), t[0].getType());
            assertEquals("3.14", t[0].getLexeme());
        }

        @Test
        @DisplayName("float leading zero")
        void floatLeadingZero() {
            Token[] t = meaningful(Lexer.tokenize("0.5", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.FLOAT.getValue(), t[0].getType());
            assertEquals("0.5", t[0].getLexeme());
        }

        @Test
        @DisplayName("float trailing digits")
        void floatTrailingDigits() {
            Token[] t = meaningful(Lexer.tokenize("42.0", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.FLOAT.getValue(), t[0].getType());
            assertEquals("42.0", t[0].getLexeme());
        }

        @Test
        @DisplayName("float scientific e notation")
        void floatScientificNotation() {
            Token[] t = meaningful(Lexer.tokenize("1.5e10", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.FLOAT.getValue(), t[0].getType());
            assertEquals("1.5e10", t[0].getLexeme());
        }

        @Test
        @DisplayName("float scientific with plus")
        void floatScientificWithPlus() {
            Token[] t = meaningful(Lexer.tokenize("2.0e+5", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.FLOAT.getValue(), t[0].getType());
            assertEquals("2.0e+5", t[0].getLexeme());
        }

        @Test
        @DisplayName("float scientific with minus")
        void floatScientificWithMinus() {
            Token[] t = meaningful(Lexer.tokenize("3.14e-2", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.FLOAT.getValue(), t[0].getType());
            assertEquals("3.14e-2", t[0].getLexeme());
        }

        @Test
        @DisplayName("float scientific uppercase E")
        void floatScientificUpperCaseE() {
            Token[] t = meaningful(Lexer.tokenize("1.0E100", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.FLOAT.getValue(), t[0].getType());
            assertEquals("1.0E100", t[0].getLexeme());
        }

        @Test
        @DisplayName("hex number simple")
        void hexNumberSimple() {
            Token[] t = meaningful(Lexer.tokenize("$FF", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_NUMBER.getValue(), t[0].getType());
            assertEquals("$FF", t[0].getLexeme());
        }

        @Test
        @DisplayName("hex number lowercase")
        void hexNumberLowercase() {
            Token[] t = meaningful(Lexer.tokenize("$a1b2c3", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_NUMBER.getValue(), t[0].getType());
            assertEquals("$a1b2c3", t[0].getLexeme());
        }

        @Test
        @DisplayName("hex number single digit")
        void hexNumberSingleDigit() {
            Token[] t = meaningful(Lexer.tokenize("$0", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_NUMBER.getValue(), t[0].getType());
            assertEquals("$0", t[0].getLexeme());
        }

        @Test
        @DisplayName("hex number mixed case")
        void hexNumberMixedCase() {
            Token[] t = meaningful(Lexer.tokenize("$AbCdEf", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_NUMBER.getValue(), t[0].getType());
            assertEquals("$AbCdEf", t[0].getLexeme());
        }

        @Test
        @DisplayName("binary number simple")
        void binaryNumberSimple() {
            Token[] t = meaningful(Lexer.tokenize("%1010", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.BINARY_NUMBER.getValue(), t[0].getType());
            assertEquals("%1010", t[0].getLexeme());
        }

        @Test
        @DisplayName("binary number all zeros")
        void binaryNumberAllZeros() {
            Token[] t = meaningful(Lexer.tokenize("%0000", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.BINARY_NUMBER.getValue(), t[0].getType());
            assertEquals("%0000", t[0].getLexeme());
        }

        @Test
        @DisplayName("binary number all ones")
        void binaryNumberAllOnes() {
            Token[] t = meaningful(Lexer.tokenize("%11111111", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.BINARY_NUMBER.getValue(), t[0].getType());
            assertEquals("%11111111", t[0].getLexeme());
        }
    }

    // ============================================================
    // Identifiers
    // ============================================================

    @Nested
    @DisplayName("Identifiers")
    class Identifiers {

        @Test
        @DisplayName("identifier simple")
        void identifierSimple() {
            Token[] t = meaningful(Lexer.tokenize("myVar", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("myVar", t[0].getLexeme());
        }

        @Test
        @DisplayName("identifier starting with underscore")
        void identifierStartingWithUnderscore() {
            Token[] t = meaningful(Lexer.tokenize("_temp", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("_temp", t[0].getLexeme());
        }

        @Test
        @DisplayName("identifier with numbers")
        void identifierWithNumbers() {
            Token[] t = meaningful(Lexer.tokenize("var123", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("var123", t[0].getLexeme());
        }

        @Test
        @DisplayName("identifier single letter")
        void identifierSingleLetter() {
            Token[] t = meaningful(Lexer.tokenize("x", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("x", t[0].getLexeme());
        }

        @Test
        @DisplayName("identifier underscore only")
        void identifierUnderscoreOnly() {
            Token[] t = meaningful(Lexer.tokenize("_", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("_", t[0].getLexeme());
        }
    }

    // ============================================================
    // Registers
    // ============================================================

    @Nested
    @DisplayName("Registers")
    class Registers {

        @Test
        @DisplayName("register r0")
        void registerR0() {
            Token[] t = meaningful(Lexer.tokenize("r0", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.REGISTER.getValue(), t[0].getType());
            assertEquals("r0", t[0].getLexeme());
        }

        @Test
        @DisplayName("register r9")
        void registerR9() {
            Token[] t = meaningful(Lexer.tokenize("r9", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.REGISTER.getValue(), t[0].getType());
            assertEquals("r9", t[0].getLexeme());
        }

        @Test
        @DisplayName("register r10")
        void registerR10() {
            Token[] t = meaningful(Lexer.tokenize("r10", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.REGISTER.getValue(), t[0].getType());
            assertEquals("r10", t[0].getLexeme());
        }

        @Test
        @DisplayName("register r15")
        void registerR15() {
            Token[] t = meaningful(Lexer.tokenize("r15", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.REGISTER.getValue(), t[0].getType());
            assertEquals("r15", t[0].getLexeme());
        }

        @Test
        @DisplayName("register in context r0 r1 r2")
        void registerInContext() {
            Token[] t = meaningful(Lexer.tokenize("r0 r1 r2", false));
            assertEquals(3, t.length);
            for (Token tok : t) {
                assertEquals(TokenType.REGISTER.getValue(), tok.getType());
            }
            assertEquals("r0", t[0].getLexeme());
            assertEquals("r1", t[1].getLexeme());
            assertEquals("r2", t[2].getLexeme());
        }

        @Test
        @DisplayName("r16 is identifier (out of range)")
        void registerR16IsIdentifier() {
            Token[] t = meaningful(Lexer.tokenize("r16", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("r16", t[0].getLexeme());
        }

        @Test
        @DisplayName("all registers r0-r15")
        void allRegisters() {
            for (int i = 0; i < 16; i++) {
                Token[] t = meaningful(Lexer.tokenize("move r" + i + " 0", false));
                boolean foundRegister = false;
                for (Token tok : t) {
                    if (tok.getType() == TokenType.REGISTER.getValue()
                            && tok.getLexeme().equals("r" + i)) {
                        foundRegister = true;
                        break;
                    }
                }
                assertTrue(foundRegister, "r" + i + " should be recognized as REGISTER");
            }
        }
    }

    // ============================================================
    // Devices
    // ============================================================

    @Nested
    @DisplayName("Devices")
    class Devices {

        @Test
        @DisplayName("device d0")
        void deviceD0() {
            Token[] t = meaningful(Lexer.tokenize("d0", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DEVICE.getValue(), t[0].getType());
            assertEquals("d0", t[0].getLexeme());
        }

        @Test
        @DisplayName("device d5")
        void deviceD5() {
            Token[] t = meaningful(Lexer.tokenize("d5", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DEVICE.getValue(), t[0].getType());
            assertEquals("d5", t[0].getLexeme());
        }

        @Test
        @DisplayName("d6 is identifier (out of range)")
        void deviceD6IsIdentifier() {
            Token[] t = meaningful(Lexer.tokenize("d6", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("d6", t[0].getLexeme());
        }

        @Test
        @DisplayName("all devices d0-d5")
        void allDevices() {
            for (int i = 0; i < 6; i++) {
                Token[] t = meaningful(Lexer.tokenize("alias dev d" + i, false));
                boolean foundDevice = false;
                for (Token tok : t) {
                    if (tok.getType() == TokenType.DEVICE.getValue()
                            && tok.getLexeme().equals("d" + i)) {
                        foundDevice = true;
                        break;
                    }
                }
                assertTrue(foundDevice, "d" + i + " should be recognized as DEVICE");
            }
        }
    }

    // ============================================================
    // Strings
    // ============================================================

    @Nested
    @DisplayName("Strings")
    class Strings {

        @Test
        @DisplayName("string simple")
        void stringSimple() {
            Token[] t = meaningful(Lexer.tokenize("\"hello\"", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.STRING.getValue(), t[0].getType());
            assertEquals("\"hello\"", t[0].getLexeme());
        }

        @Test
        @DisplayName("string empty")
        void stringEmpty() {
            Token[] t = meaningful(Lexer.tokenize("\"\"", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.STRING.getValue(), t[0].getType());
            assertEquals("\"\"", t[0].getLexeme());
        }

        @Test
        @DisplayName("string with spaces")
        void stringWithSpaces() {
            Token[] t = meaningful(Lexer.tokenize("\"hello world\"", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.STRING.getValue(), t[0].getType());
            assertEquals("\"hello world\"", t[0].getLexeme());
        }

        @Test
        @DisplayName("string with numbers")
        void stringWithNumbers() {
            Token[] t = meaningful(Lexer.tokenize("\"test123\"", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.STRING.getValue(), t[0].getType());
            assertEquals("\"test123\"", t[0].getLexeme());
        }

        @Test
        @DisplayName("unclosed string produces UNKNOWN and diagnostics")
        void stringUnclosedProducesError() {
            Lexer lexer = new Lexer();
            lexer.init("\"unclosed", false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length >= 1);
            assertEquals(TokenType.UNKNOWN.getValue(), tokens[0].getType());
            assertTrue(lexer.getDiagnostics().length > 0,
                    "unclosed string should produce diagnostics");
        }

        @Test
        @DisplayName("unclosed string multiline recovery")
        void stringUnclosedMultiLineRecovery() {
            Lexer lexer = new Lexer();
            lexer.init("\"unclosed\nadd r0 r1 r2\n", false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length >= 1);
            // first token is UNKNOWN for the unclosed string
            assertEquals(TokenType.UNKNOWN.getValue(), tokens[0].getType());
            assertTrue(lexer.getDiagnostics().length > 0);
        }
    }

    // ============================================================
    // Comments
    // ============================================================

    @Nested
    @DisplayName("Comments")
    class Comments {

        @Test
        @DisplayName("hash comment simple")
        void hexCommentSimple() {
            Token[] t = meaningful(Lexer.tokenize("# this is a comment", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_COMMENT.getValue(), t[0].getType());
            assertEquals("# this is a comment", t[0].getLexeme());
            assertEquals(TokenCategory.COMMENT.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("hash comment empty")
        void hexCommentEmpty() {
            Token[] t = meaningful(Lexer.tokenize("#", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_COMMENT.getValue(), t[0].getType());
            assertEquals("#", t[0].getLexeme());
        }

        @Test
        @DisplayName("slash comment simple")
        void slashCommentSimple() {
            Token[] t = meaningful(Lexer.tokenize("// this is a comment", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.SLASH_COMMENT.getValue(), t[0].getType());
            assertEquals("// this is a comment", t[0].getLexeme());
            assertEquals(TokenCategory.COMMENT.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("slash comment empty")
        void slashCommentEmpty() {
            Token[] t = meaningful(Lexer.tokenize("//", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.SLASH_COMMENT.getValue(), t[0].getType());
            assertEquals("//", t[0].getLexeme());
        }

        @Test
        @DisplayName("comment after code")
        void commentAfterCode() {
            Token[] t = meaningful(Lexer.tokenize("add r0 r1 r2 # comment", false));
            assertEquals(5, t.length);
            assertEquals(TokenType.KEYWORD_ADD.getValue(), t[0].getType());
            assertEquals(TokenType.REGISTER.getValue(), t[1].getType());
            assertEquals(TokenType.REGISTER.getValue(), t[2].getType());
            assertEquals(TokenType.REGISTER.getValue(), t[3].getType());
            assertEquals(TokenType.HEX_COMMENT.getValue(), t[4].getType());
        }
    }

    // ============================================================
    // Doc comments / type hints
    // ============================================================

    @Nested
    @DisplayName("Doc comments / type hints")
    class DocComments {

        @Test
        @DisplayName("doc comment device")
        void docCommentDevice() {
            Token[] t = meaningful(Lexer.tokenize("#> @device", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DOC_COMMENT.getValue(), t[0].getType());
            assertEquals("#> @device", t[0].getLexeme());
            assertEquals(TokenCategory.ANNOTATION.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("doc comment enum")
        void docCommentEnum() {
            Token[] t = meaningful(Lexer.tokenize("#> @enum", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DOC_COMMENT.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("doc comment name")
        void docCommentName() {
            Token[] t = meaningful(Lexer.tokenize("#> @name Furnace", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DOC_COMMENT.getValue(), t[0].getType());
            assertEquals("#> @name Furnace", t[0].getLexeme());
        }

        @Test
        @DisplayName("doc comment desc")
        void docCommentDesc() {
            Token[] t = meaningful(Lexer.tokenize("#> @desc 炉窑设备", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DOC_COMMENT.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("doc comment value")
        void docCommentValue() {
            Token[] t = meaningful(Lexer.tokenize("#> @value Oxygen 1", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DOC_COMMENT.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("doc comment end-device")
        void docCommentEndDevice() {
            Token[] t = meaningful(Lexer.tokenize("#> @end-device", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DOC_COMMENT.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("doc comment end-enum")
        void docCommentEndEnum() {
            Token[] t = meaningful(Lexer.tokenize("#> @end-enum", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.DOC_COMMENT.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("type hint")
        void typeHint() {
            Token[] t = meaningful(Lexer.tokenize("#: @type Furnace", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.TYPE_HINT.getValue(), t[0].getType());
            assertEquals("#: @type Furnace", t[0].getLexeme());
            assertEquals(TokenCategory.ANNOTATION.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("type hint with desc")
        void typeHintDesc() {
            Token[] t = meaningful(Lexer.tokenize("#: @desc 炉窑设备", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.TYPE_HINT.getValue(), t[0].getType());
            assertEquals(TokenCategory.ANNOTATION.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("type hint multiple tags")
        void typeHintMultipleTags() {
            Token[] t = meaningful(Lexer.tokenize("#: @type Furnace @desc 炉窑", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.TYPE_HINT.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("invalid doc comment fallback to HEX_COMMENT")
        void invalidDocCommentFallback() {
            Token[] t = meaningful(Lexer.tokenize("#> not a tag", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_COMMENT.getValue(), t[0].getType());
            assertEquals(TokenCategory.COMMENT.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("invalid type hint fallback to HEX_COMMENT")
        void invalidTypeHintFallback() {
            Token[] t = meaningful(Lexer.tokenize("#: not type", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_COMMENT.getValue(), t[0].getType());
            assertEquals(TokenCategory.COMMENT.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("hash without annotation is HEX_COMMENT")
        void hashWithoutAnnotation() {
            Token[] t = meaningful(Lexer.tokenize("# normal comment", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.HEX_COMMENT.getValue(), t[0].getType());
            assertEquals(TokenCategory.COMMENT.getValue(), t[0].getCategory());
        }
    }

    // ============================================================
    // Keywords
    // ============================================================

    @Nested
    @DisplayName("Keywords")
    class Keywords {

        @Test
        @DisplayName("keyword alias")
        void keywordAlias() {
            Token[] t = meaningful(Lexer.tokenize("alias", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_ALIAS.getValue(), t[0].getType());
            assertEquals("alias", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword define")
        void keywordDefine() {
            Token[] t = meaningful(Lexer.tokenize("define", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_DEFINE.getValue(), t[0].getType());
            assertEquals("define", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword hcf")
        void keywordHcf() {
            Token[] t = meaningful(Lexer.tokenize("hcf", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_HCF.getValue(), t[0].getType());
            assertEquals("hcf", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword yield")
        void keywordYield() {
            Token[] t = meaningful(Lexer.tokenize("yield", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_YIELD.getValue(), t[0].getType());
            assertEquals("yield", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword add")
        void keywordAdd() {
            Token[] t = meaningful(Lexer.tokenize("add", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_ADD.getValue(), t[0].getType());
            assertEquals("add", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword sub")
        void keywordSub() {
            Token[] t = meaningful(Lexer.tokenize("sub", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_SUB.getValue(), t[0].getType());
            assertEquals("sub", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword move")
        void keywordMove() {
            Token[] t = meaningful(Lexer.tokenize("move", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_MOVE.getValue(), t[0].getType());
            assertEquals("move", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword nan")
        void keywordConstantNan() {
            Token[] t = meaningful(Lexer.tokenize("nan", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_NAN.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("keyword pi")
        void keywordConstantPi() {
            Token[] t = meaningful(Lexer.tokenize("pi", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_PI.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("keyword HASH")
        void keywordHash() {
            Token[] t = meaningful(Lexer.tokenize("HASH", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_HASH.getValue(), t[0].getType());
            assertEquals("HASH", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword STR")
        void keywordStr() {
            Token[] t = meaningful(Lexer.tokenize("STR", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_STR.getValue(), t[0].getType());
            assertEquals("STR", t[0].getLexeme());
        }

        @Test
        @DisplayName("keyword rgas")
        void keywordRgas() {
            Token[] t = meaningful(Lexer.tokenize("rgas", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.KEYWORD_RGAS.getValue(), t[0].getType());
        }
    }

    // ============================================================
    // Symbols
    // ============================================================

    @Nested
    @DisplayName("Symbols")
    class Symbols {

        @Test
        @DisplayName("left paren")
        void symbolLeftParen() {
            Token[] t = meaningful(Lexer.tokenize("(", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.LPAREN.getValue(), t[0].getType());
            assertEquals(TokenCategory.SYMBOL.getValue(), t[0].getCategory());
        }

        @Test
        @DisplayName("right paren")
        void symbolRightParen() {
            Token[] t = meaningful(Lexer.tokenize(")", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.RPAREN.getValue(), t[0].getType());
        }

        @Test
        @DisplayName("colon")
        void symbolColon() {
            Token[] t = meaningful(Lexer.tokenize(":", false));
            assertEquals(1, t.length);
            assertEquals(TokenType.COLON.getValue(), t[0].getType());
        }
    }

    // ============================================================
    // Newlines
    // ============================================================

    @Nested
    @DisplayName("Newlines")
    class Newlines {

        @Test
        @DisplayName("newline single")
        void newlineSingle() {
            Token[] tokens = Lexer.tokenize("\n", false);
            int newlineCount = 0;
            for (Token tok : tokens) {
                if (tok.getType() == TokenType.NEWLINE.getValue()) {
                    newlineCount++;
                }
            }
            assertEquals(1, newlineCount);
            // get the NEWLINE token
            for (Token tok : tokens) {
                if (tok.getType() == TokenType.NEWLINE.getValue()) {
                    assertEquals(TokenCategory.WHITESPACE.getValue(), tok.getCategory());
                    break;
                }
            }
        }

        @Test
        @DisplayName("newline multiple")
        void newlineMultiple() {
            Token[] tokens = Lexer.tokenize("\n\n\n", false);
            int newlineCount = 0;
            for (Token tok : tokens) {
                if (tok.getType() == TokenType.NEWLINE.getValue()) {
                    newlineCount++;
                }
            }
            assertEquals(3, newlineCount);
        }
    }

    // ============================================================
    // Position information
    // ============================================================

    @Nested
    @DisplayName("Position information")
    class Positions {

        @Test
        @DisplayName("first token position line 1 col 1")
        void positionFirstToken() {
            Token[] t = meaningful(Lexer.tokenize("add", false));
            assertEquals(1, t.length);
            assertEquals(1, t[0].getPos().getLine());
            assertEquals(1, t[0].getPos().getColumn());
            assertEquals(0, t[0].getPos().getOffset());
        }

        @Test
        @DisplayName("position after spaces")
        void positionAfterSpaces() {
            Token[] t = meaningful(Lexer.tokenize("   add", false));
            assertEquals(1, t.length);
            assertEquals(1, t[0].getPos().getLine());
            assertEquals(4, t[0].getPos().getColumn());
            assertEquals(3, t[0].getPos().getOffset());
        }

        @Test
        @DisplayName("position on second line")
        void positionSecondLine() {
            Token[] tokens = Lexer.tokenize("add\nsub", false);
            int subIndex = -1;
            for (int i = 0; i < tokens.length; i++) {
                if (tokens[i].getType() == TokenType.KEYWORD_SUB.getValue()) {
                    subIndex = i;
                    break;
                }
            }
            assertTrue(subIndex >= 0, "should find 'sub' token");
            assertEquals(2, tokens[subIndex].getPos().getLine());
            assertEquals(1, tokens[subIndex].getPos().getColumn());
        }
    }

    // ============================================================
    // Multi-token combinations
    // ============================================================

    @Nested
    @DisplayName("Multi-token combinations")
    class MultiToken {

        @Test
        @DisplayName("multiple integers")
        void multipleIntegers() {
            Token[] t = meaningful(Lexer.tokenize("1 2 3", false));
            assertEquals(3, t.length);
            for (Token tok : t) {
                assertEquals(TokenType.INTEGER.getValue(), tok.getType());
            }
            assertEquals("1", t[0].getLexeme());
            assertEquals("2", t[1].getLexeme());
            assertEquals("3", t[2].getLexeme());
        }

        @Test
        @DisplayName("simple instruction add r0 r1 r2")
        void simpleInstructionAdd() {
            Token[] tokens = Lexer.tokenize("add r0 r1 r2\n", false);
            // tokens: add, r0, r1, r2, NEWLINE, END
            assertEquals(6, tokens.length);
            assertEquals(TokenType.KEYWORD_ADD.getValue(), tokens[0].getType());
            assertEquals(TokenType.REGISTER.getValue(), tokens[1].getType());
            assertEquals(TokenType.REGISTER.getValue(), tokens[2].getType());
            assertEquals(TokenType.REGISTER.getValue(), tokens[3].getType());
            assertEquals(TokenType.NEWLINE.getValue(), tokens[4].getType());
        }

        @Test
        @DisplayName("alias directive")
        void aliasDirective() {
            Token[] tokens = Lexer.tokenize("alias foo r0\n", false);
            assertEquals(5, tokens.length);
            assertEquals(TokenType.KEYWORD_ALIAS.getValue(), tokens[0].getType());
            assertEquals(TokenType.IDENTIFIER.getValue(), tokens[1].getType());
            assertEquals(TokenType.REGISTER.getValue(), tokens[2].getType());
            assertEquals(TokenType.NEWLINE.getValue(), tokens[3].getType());
        }

        @Test
        @DisplayName("label definition")
        void labelDefinition() {
            Token[] t = meaningful(Lexer.tokenize("myLabel:", false));
            assertEquals(2, t.length);
            assertEquals(TokenType.IDENTIFIER.getValue(), t[0].getType());
            assertEquals("myLabel", t[0].getLexeme());
            assertEquals(TokenType.COLON.getValue(), t[1].getType());
        }

        @Test
        @DisplayName("HASH call syntax")
        void hashCallSyntax() {
            Token[] t = meaningful(Lexer.tokenize("HASH(\"test\")", false));
            assertEquals(4, t.length);
            assertEquals(TokenType.KEYWORD_HASH.getValue(), t[0].getType());
            assertEquals(TokenType.LPAREN.getValue(), t[1].getType());
            assertEquals(TokenType.STRING.getValue(), t[2].getType());
            assertEquals(TokenType.RPAREN.getValue(), t[3].getType());
        }
    }

    // ============================================================
    // END token
    // ============================================================

    @Nested
    @DisplayName("END token")
    class EndToken {

        @Test
        @DisplayName("END token always present at end")
        void endTokenAlwaysPresent() {
            Token[] tokens = Lexer.tokenize("add r0 r1 r2", false);
            assertTrue(tokens.length >= 1);
            assertEquals(TokenType.END.getValue(), tokens[tokens.length - 1].getType());
            assertEquals(TokenCategory.END.getValue(), tokens[tokens.length - 1].getCategory());
        }

        @Test
        @DisplayName("END token position is end of input")
        void endTokenPosition() {
            Token[] tokens = Lexer.tokenize("abc", false);
            Token end = tokens[tokens.length - 1];
            assertEquals(TokenType.END.getValue(), end.getType());
            assertEquals(3, end.getPos().getOffset());
        }
    }

    // ============================================================
    // Token boundary checks (IEL3_2)
    // ============================================================

    @Nested
    @DisplayName("Token boundary checks (IEL3_2)")
    class BoundaryChecks {

        @Test
        @DisplayName("integer followed by identifier without space produces diagnostics")
        void integerFollowedByIdentifierNoSpace() {
            Lexer lexer = new Lexer();
            lexer.init("100abc", false);
            lexer.scan();
            assertTrue(lexer.getDiagnostics().length > 0,
                    "should emit diagnostics for '100abc'");
        }

        @Test
        @DisplayName("integer followed by identifier with space is ok")
        void integerFollowedByIdentifierWithSpace() {
            Lexer lexer = new Lexer();
            lexer.init("100 abc", false);
            lexer.scan();
            assertEquals(0, lexer.getDiagnostics().length,
                    "should not emit diagnostics for '100 abc'");
        }

        @Test
        @DisplayName("hex number followed by non-hex char produces diagnostics")
        void hexNumberFollowedByNonHexChar() {
            Lexer lexer = new Lexer();
            lexer.init("$ffg", false);
            lexer.scan();
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("binary number followed by identifier produces diagnostics")
        void binaryNumberFollowedByIdentifier() {
            Lexer lexer = new Lexer();
            lexer.init("%101abc", false);
            lexer.scan();
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("string followed by identifier without space produces diagnostics")
        void stringFollowedByIdentifierNoSpace() {
            Lexer lexer = new Lexer();
            lexer.init("\"hello\"world", false);
            lexer.scan();
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("number followed by symbol (colon) no diagnostic")
        void numberFollowedBySymbolNoError() {
            Lexer lexer = new Lexer();
            lexer.init("main:", false);
            lexer.scan();
            assertEquals(0, lexer.getDiagnostics().length,
                    "symbol does not need whitespace separator");
        }
    }

    // ============================================================
    // Token API
    // ============================================================

    @Nested
    @DisplayName("Token API")
    class TokenApi {

        @Test
        @DisplayName("token toString not empty")
        void tokenToStringNotEmpty() {
            Token[] t = meaningful(Lexer.tokenize("add", false));
            assertTrue(t[0].toString() != null && !t[0].toString().isEmpty());
        }

        @Test
        @DisplayName("token toJSON not empty and contains lexeme")
        void tokenToJSON() {
            Token[] t = meaningful(Lexer.tokenize("42", false));
            String json = t[0].toJSON();
            assertNotNull(json);
            assertFalse(json.isEmpty());
            assertTrue(json.contains("42"));
        }
    }

    // ============================================================
    // Pos API
    // ============================================================

    @Nested
    @DisplayName("Pos API")
    class PosApi {

        @Test
        @DisplayName("default pos is at start (1,1,0)")
        void defaultPos() {
            Pos pos = new Pos();
            assertEquals(1, pos.getLine());
            assertEquals(1, pos.getColumn());
            assertEquals(0, pos.getOffset());
        }

        @Test
        @DisplayName("pos next advances column and offset")
        void posNext() {
            Pos pos = new Pos();
            pos.next(0x00);
            assertEquals(1, pos.getLine());
            assertEquals(2, pos.getColumn());
            assertEquals(1, pos.getOffset());
        }

        @Test
        @DisplayName("pos newline resets column and advances line")
        void posNewline() {
            Pos pos = new Pos();
            pos.newline();
            assertEquals(2, pos.getLine());
            assertEquals(1, pos.getColumn());
            assertTrue(pos.getOffset() > 0);
        }

        @Test
        @DisplayName("pos move advances by offset")
        void posMove() {
            Pos pos = new Pos();
            pos.move(5, 5);
            assertEquals(6, pos.getColumn());
            assertEquals(5, pos.getOffset());
        }
    }

    // ============================================================
    // Symbol tolerance
    // ============================================================

    @Nested
    @DisplayName("Symbol tolerance")
    class SymbolTolerance {

        private static final String SPECIAL_CHARS = "`~!@$%^&*()+{}|:\"<>?-=[]\\;',./";

        @Test
        @DisplayName("bare special chars do not crash and produce diagnostics")
        void bareSpecialChars() {
            Lexer lexer = new Lexer();
            lexer.init(SPECIAL_CHARS, false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length > 1);
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("special chars after alias produce diagnostics")
        void specialCharsAfterAlias() {
            Lexer lexer = new Lexer();
            lexer.init("alias " + SPECIAL_CHARS, false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length > 1);
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("special chars after define produce diagnostics")
        void specialCharsAfterDefine() {
            Lexer lexer = new Lexer();
            lexer.init("define " + SPECIAL_CHARS, false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length > 1);
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("special chars after define with name produce diagnostics")
        void specialCharsAfterDefineWithName() {
            Lexer lexer = new Lexer();
            lexer.init("define test2 " + SPECIAL_CHARS, false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length > 1);
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("special chars after j instruction produce diagnostics")
        void specialCharsAfterJump() {
            Lexer lexer = new Lexer();
            lexer.init("j " + SPECIAL_CHARS, false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length > 1);
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("special chars after j with target produce diagnostics")
        void specialCharsAfterJumpWithTarget() {
            Lexer lexer = new Lexer();
            lexer.init("j xxx " + SPECIAL_CHARS, false);
            Token[] tokens = lexer.scan();
            assertTrue(tokens.length > 1);
            assertTrue(lexer.getDiagnostics().length > 0);
        }
    }

    // ============================================================
    // Lexer instance API
    // ============================================================

    @Nested
    @DisplayName("Lexer instance API")
    class LexerInstance {

        @Test
        @DisplayName("scan instance matches static tokenize")
        void scanInstanceMatchesStatic() {
            String src = "alias ic d0";
            Token[] staticTokens = Lexer.tokenize(src, false);

            Lexer lexer = new Lexer();
            lexer.init(src, false);
            Token[] instanceTokens = lexer.scan();

            assertEquals(staticTokens.length, instanceTokens.length);
            for (int i = 0; i < staticTokens.length; i++) {
                assertEquals(staticTokens[i].getType(), instanceTokens[i].getType(),
                        "type mismatch at token " + i);
                assertEquals(staticTokens[i].getLexeme(), instanceTokens[i].getLexeme(),
                        "lexeme mismatch at token " + i);
            }
        }

        @Test
        @DisplayName("diagnostics empty for valid input")
        void diagnosticsEmptyForValidInput() {
            Lexer lexer = new Lexer();
            lexer.init("add r0 r1 r2", false);
            lexer.scan();
            assertEquals(0, lexer.getDiagnostics().length);
        }

        @Test
        @DisplayName("diagnostics non-empty for invalid input")
        void diagnosticsNonEmptyForInvalidInput() {
            Lexer lexer = new Lexer();
            lexer.init("100abc", false);
            lexer.scan();
            assertTrue(lexer.getDiagnostics().length > 0);
        }

        @Test
        @DisplayName("diagnostic fields are accessible")
        void diagnosticFieldsAccessible() {
            Lexer lexer = new Lexer();
            lexer.init("\"unclosed", false);
            lexer.scan();
            Diagnostic[] diags = lexer.getDiagnostics();
            assertTrue(diags.length > 0);
            Diagnostic d = diags[0];
            assertNotNull(d.id);
            assertNotNull(d.message);
        }
    }

    // ============================================================
    // Token unit tests
    // ============================================================

    @Nested
    @DisplayName("Token unit tests")
    class TokenUnitTests {

        @Test
        @DisplayName("token fields accessible from lexer output")
        void tokenFieldsAccessible() {
            Token[] tokens = Lexer.tokenize("add r0 42", false);
            assertTrue(tokens.length >= 4);
            Token first = tokens[0];
            assertEquals(TokenType.KEYWORD_ADD.getValue(), first.getType());
            assertEquals(TokenCategory.LITERAL.getValue(), first.getCategory());
            assertEquals("add", first.getLexeme());
        }

        @Test
        @DisplayName("token toString not empty")
        void tokenToStringNotEmpty() {
            Token[] tokens = Lexer.tokenize("add", false);
            Token tok = tokens[0];
            String str = tok.toString();
            assertNotNull(str);
            assertFalse(str.isEmpty());
        }

        @Test
        @DisplayName("token toJSON is valid")
        void tokenToJSONIsValid() {
            Token[] tokens = Lexer.tokenize("42", false);
            Token tok = tokens[0];
            String json = tok.toJSON();
            assertNotNull(json);
            assertFalse(json.isEmpty());
            assertTrue(json.contains("42"));
        }

        @Test
        @DisplayName("token pos accessible")
        void tokenPosAccessible() {
            Token[] tokens = Lexer.tokenize("add", false);
            Token tok = tokens[0];
            Pos pos = tok.getPos();
            assertNotNull(pos);
            assertEquals(1, pos.getLine());
            assertEquals(1, pos.getColumn());
            assertEquals(0, pos.getOffset());
        }
    }

    // ============================================================
    // Position unit tests (extended)
    // ============================================================

    @Nested
    @DisplayName("Position unit tests (extended)")
    class PositionExtended {

        @Test
        @DisplayName("position move updates column and offset only")
        void paramConstructor() {
            // Pos::move(charOffset, byteOffset) 只修改 column_ 和 offset_，不修改 line_
            // 对齐 C++ Pos::move 实现 (common/src/utils/position.cpp:45-48)
            Pos pos = new Pos();
            pos.move(20, 20);
            assertEquals(1, pos.getLine());
            assertEquals(21, pos.getColumn());
            assertEquals(20, pos.getOffset());
        }

        @Test
        @DisplayName("multiple next calls advance correctly")
        void multipleNextCalls() {
            Pos pos = new Pos();
            for (int i = 0; i < 10; i++) {
                pos.next(0x00);
            }
            assertEquals(1, pos.getLine());
            assertEquals(11, pos.getColumn());
            assertEquals(10, pos.getOffset());
        }

        @Test
        @DisplayName("newline then next advances correctly")
        void newlineThenNext() {
            Pos pos = new Pos();
            pos.newline();
            pos.next(0x00);
            assertEquals(2, pos.getLine());
            assertEquals(2, pos.getColumn());
            assertEquals(2, pos.getOffset());
        }

        @Test
        @DisplayName("newline resets column and advances line")
        void newlineResetsColumn() {
            Pos pos = new Pos();
            for (int i = 0; i < 9; i++) {
                pos.next(0x00);
            }
            pos.newline();
            assertEquals(2, pos.getLine());
            assertEquals(1, pos.getColumn());
            assertEquals(10, pos.getOffset());
        }

        @Test
        @DisplayName("pos move advances by offset")
        void posMove() {
            Pos pos = new Pos();
            pos.move(5, 5);
            assertEquals(6, pos.getColumn());
            assertEquals(5, pos.getOffset());
        }

        @Test
        @DisplayName("pos default is at start")
        void defaultPos() {
            Pos pos = new Pos();
            assertEquals(1, pos.getLine());
            assertEquals(1, pos.getColumn());
            assertEquals(0, pos.getOffset());
        }
    }
}
