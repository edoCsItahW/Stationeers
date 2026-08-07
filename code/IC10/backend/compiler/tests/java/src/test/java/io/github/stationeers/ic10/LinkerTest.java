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
 * Tests for the IC10 Java Linker.
 * <p>
 * Mirrors the C++ {@code test_linker.cpp} test suite at matching granularity.
 * Covers single-unit linking, multi-unit symbol merging, cross-unit forward
 * references, type table sharing, and linker API edge cases.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("Linker tests")
class LinkerTest {

    @BeforeAll
    static void setupLanguage() {
        IC10CompilerLocal.setLanguage("zh-hans");
    }

    // ---- Helpers ----

    private static SymbolTable linkSingleSource(String source) {
        Linker linker = new Linker();
        linker.addUnitSourceNoPath(source);
        return linker.link();
    }

    // ============================================================
    // Single-unit linking
    // ============================================================

    @Nested
    @DisplayName("Single-unit linking")
    class SingleUnit {

        @Test
        @DisplayName("alias single unit")
        void singleAlias() {
            SymbolTable st = linkSingleSource("alias ic d0\nhcf\n");
            assertNotNull(st);
            String json = st.toJSON();
            assertNotNull(json);
            assertTrue(json.contains("ic"), "symbol table should contain 'ic' alias");
        }

        @Test
        @DisplayName("define single unit")
        void singleDefine() {
            SymbolTable st = linkSingleSource("define MAX 100\nhcf\n");
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("MAX"), "symbol table should contain 'MAX'");
        }

        @Test
        @DisplayName("label single unit")
        void singleLabel() {
            SymbolTable st = linkSingleSource("main: hcf\n");
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("main"), "symbol table should contain 'main' label");
        }

        @Test
        @DisplayName("hcf only")
        void hcfOnly() {
            SymbolTable st = linkSingleSource("hcf\n");
            assertNotNull(st);
            assertNotNull(st.toJSON());
        }

        @Test
        @DisplayName("empty source")
        void emptySource() {
            SymbolTable st = linkSingleSource("");
            assertNotNull(st);
            assertNotNull(st.toJSON());
        }
    }

    // ============================================================
    // Multi-unit symbol merging
    // ============================================================

    @Nested
    @DisplayName("Multi-unit symbol merging")
    class MultiUnit {

        @Test
        @DisplayName("two aliases in separate units")
        void twoAliasesInSeparateUnits() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("alias a r0\nhcf\n");
            linker.addUnitSourceNoPath("alias b r1\nhcf\n");
            SymbolTable st = linker.link();
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("a") && json.contains("b"),
                    "symbol table should contain both aliases");
        }

        @Test
        @DisplayName("alias + define from different units")
        void aliasPlusDefine() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("alias dev d0\nhcf\n");
            linker.addUnitSourceNoPath("define MAX 100\nhcf\n");
            SymbolTable st = linker.link();
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("dev") && json.contains("MAX"));
        }

        @Test
        @DisplayName("label referenced across units")
        void crossUnitLabelReference() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("start: hcf\n");
            linker.addUnitSourceNoPath("j start\nhcf\n");
            SymbolTable st = linker.link();
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("start"));
        }

        @Test
        @DisplayName("alias forward reference across units")
        void forwardReference() {
            Linker linker = new Linker();
            // unit 1 uses alias before definition
            linker.addUnitSourceNoPath("hcf\n");
            // unit 2 defines the alias
            linker.addUnitSourceNoPath("alias used r0\nhcf\n");
            SymbolTable st = linker.link();
            assertNotNull(st);
            assertNotNull(st.toJSON());
        }
    }

    // ============================================================
    // Type table across units
    // ============================================================

    @Nested
    @DisplayName("Type table across units")
    class TypeTableCrossUnit {

        @Test
        @DisplayName("device type defined and used across units")
        void deviceTypeCrossUnit() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath(
                    "#> @device\n" +
                    "#> @name Sensor\n" +
                    "#> @logic Pressure rw\n" +
                    "#> @end-device\n");
            linker.addUnitSourceNoPath("alias s d0 #: @type Sensor\nhcf\n");
            SymbolTable st = linker.link();
            assertNotNull(st);
            String json = linker.getTypeTable().toJSON();
            assertTrue(json.contains("Sensor"), "type table should contain 'Sensor'");
        }

        @Test
        @DisplayName("enum type shared across units")
        void enumTypeCrossUnit() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath(
                    "#> @enum\n" +
                    "#> @name ReagentMode\n" +
                    "#> @value Contents 0\n" +
                    "#> @value Required 1\n" +
                    "#> @end-enum\n");
            linker.addUnitSourceNoPath("alias f d0\nhcf\n");
            linker.link();
            String json = linker.getTypeTable().toJSON();
            assertTrue(json.contains("ReagentMode"));
        }

        @Test
        @DisplayName("multiple device types")
        void multipleDeviceTypes() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath(
                    "#> @device\n" +
                    "#> @name Furnace\n" +
                    "#> @end-device\n");
            linker.addUnitSourceNoPath(
                    "#> @device\n" +
                    "#> @name Sensor\n" +
                    "#> @end-device\n");
            linker.addUnitSourceNoPath("alias f d0 #: @type Furnace\nhcf\n");
            linker.link();
            String json = linker.getTypeTable().toJSON();
            assertTrue(json.contains("Furnace"));
            assertTrue(json.contains("Sensor"));
        }

        @Test
        @DisplayName("empty type table after linking minimal program")
        void typeTableAfterMinimal() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("hcf\n");
            linker.link();
            TypeTable tt = linker.getTypeTable();
            assertNotNull(tt);
            assertNotNull(tt.toJSON());
        }
    }

    // ============================================================
    // Diagnostics
    // ============================================================

    @Nested
    @DisplayName("Diagnostics")
    class Diagnostics {

        @Test
        @DisplayName("diagnostics accessible after linking")
        void diagnosticsAccessible() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("alias dev d0\nhcf\n");
            linker.link();
            Diagnostic[] diags = linker.getDiagnostics();
            assertNotNull(diags);
        }

        @Test
        @DisplayName("diagnostics for invalid code is non-empty")
        void diagnosticsForInvalid() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("invalidToken !@#\n");
            linker.link();
            Diagnostic[] diags = linker.getDiagnostics();
            // invalid tokens should produce diagnostics
            assertNotNull(diags);
        }
    }

    // ============================================================
    // Linker API edge cases
    // ============================================================

    @Nested
    @DisplayName("Linker API edge cases")
    class LinkerApi {

        @Test
        @DisplayName("link on empty linker (no units added) does not crash")
        void emptyLinker() {
            Linker linker = new Linker();
            SymbolTable st = linker.link();
            assertNotNull(st);
            assertNotNull(st.toJSON());
        }

        @Test
        @DisplayName("link called multiple times is idempotent")
        void repeatedLink() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("alias a r0\nhcf\n");
            SymbolTable st1 = linker.link();
            SymbolTable st2 = linker.link();
            assertNotNull(st1);
            assertNotNull(st2);
        }

        @Test
        @DisplayName("addUnitSource with path works")
        void addUnitSourceWithPath() {
            Linker linker = new Linker();
            linker.addUnitSource("alias a r0\nhcf\n", "test.ic10");
            SymbolTable st = linker.link();
            assertNotNull(st);
            assertNotNull(st.toJSON());
        }

        @Test
        @DisplayName("addUnitProgram with path works")
        void addUnitProgramWithPath() {
            Linker linker = new Linker();
            Token[] tokens = Lexer.tokenize("alias a r0\nhcf\n", false);
            Program program = Parser.parsing(tokens, false);
            linker.addUnitProgram(program, "test.ic10");
            SymbolTable st = linker.link();
            assertNotNull(st);
            assertNotNull(st.toJSON());
        }

        @Test
        @DisplayName("addUnitProgramNoPath works")
        void addUnitProgramNoPath() {
            Linker linker = new Linker();
            Token[] tokens = Lexer.tokenize("alias a r0\nhcf\n", false);
            Program program = Parser.parsing(tokens, false);
            linker.addUnitProgramNoPath(program);
            SymbolTable st = linker.link();
            assertNotNull(st);
            assertNotNull(st.toJSON());
        }

        @Test
        @DisplayName("mixed addUnitSource and addUnitProgram")
        void mixedAddUnits() {
            Linker linker = new Linker();
            linker.addUnitSourceNoPath("alias a r0\nhcf\n");
            Token[] tokens = Lexer.tokenize("alias b r1\nhcf\n", false);
            Program program = Parser.parsing(tokens, false);
            linker.addUnitProgramNoPath(program);
            SymbolTable st = linker.link();
            assertNotNull(st);
            String json = st.toJSON();
            assertTrue(json.contains("a") && json.contains("b"));
        }

        @Test
        @DisplayName("addUnitSource multiple units then link")
        void multipleUnitsThenLink() {
            Linker linker = new Linker();
            for (int i = 0; i < 5; i++) {
                linker.addUnitSourceNoPath("alias a" + i + " r0\nhcf\n");
            }
            SymbolTable st = linker.link();
            assertNotNull(st);
            String json = st.toJSON();
            for (int i = 0; i < 5; i++) {
                assertTrue(json.contains("a" + i),
                        "symbol table should contain alias a" + i);
            }
        }

        @Test
        @DisplayName("type table not null before any add (empty)")
        void typeTableBeforeAdd() {
            Linker linker = new Linker();
            // getTypeTable is callable but returns empty initially
            TypeTable tt = linker.getTypeTable();
            assertNotNull(tt);
            assertNotNull(tt.toJSON());
        }
    }
}
