// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10;

import io.github.stationeers.ic10.ast.ASTNode;
import io.github.stationeers.ic10.ast.AliasDirectiveNode;
import io.github.stationeers.ic10.ast.DeviceAnnotationNode;
import io.github.stationeers.ic10.ast.DynamicRegisterNode;
import io.github.stationeers.ic10.ast.EnumAnnotationNode;
import io.github.stationeers.ic10.ast.ErrorNode;
import io.github.stationeers.ic10.ast.IntegerNode;
import io.github.stationeers.ic10.ast.ProgramNode;
import io.github.stationeers.ic10.ast.ProgramParser;
import io.github.stationeers.ic10.ast.StaticDeviceNode;
import io.github.stationeers.ic10.ast.StaticDevicePortNode;
import io.github.stationeers.ic10.ast.TernaryInstruction;
import io.github.stationeers.ic10.ast.TypeAnnotationLineNode;
import io.github.stationeers.ic10.ast.TypeAnnotationValueNode;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Tests for the Java AST model in {@code io.github.stationeers.ic10.ast}.
 * <p>
 * The model is a Jackson mirror of the C++ {@code toJSON()} output, and it can only stay useful if
 * the two agree on the node names ({@code nodeName} discriminator) and on every key. The other
 * suites never touch it — they assert substrings of {@link Program#toJSON()} — so a drifted model
 * (a node renamed to {@code DeviceDocComment} after the C++ moved to {@code DeviceAnnotation}, a
 * field renamed from {@code number} to {@code operand}, …) used to be invisible. This test pins the
 * model against a sample of the JSON the C++ core produces today; if the core renames a node or a
 * key, this test is expected to fail until the model is updated.
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@DisplayName("Java AST model tests")
class AstModelTest {

    /*
     * Hand-written sample of Program::toJSON(): one of every node family that the model mirrors,
     * written with the current node names and keys (optionals omitted, as the C++ core omits them).
     */
    private static final String PROGRAM_JSON = """
        {
          "nodeName": "Program",
          "position": { "line": 1, "column": 1, "offset": 0 },
          "end": { "line": 10, "column": 1, "offset": 200 },
          "statements": [
            {
              "nodeName": "LabelDef",
              "position": { "line": 1, "column": 1, "offset": 0 },
              "end": { "line": 1, "column": 6, "offset": 5 },
              "identifier": { "nodeName": "Identifier", "position": { "line": 1, "column": 1, "offset": 0 }, "end": { "line": 1, "column": 5, "offset": 4 }, "value": "start" }
            },
            {
              "nodeName": "addInstruction",
              "position": { "line": 2, "column": 1, "offset": 6 },
              "end": { "line": 2, "column": 15, "offset": 20 },
              "keyword": "add",
              "operand1": { "nodeName": "GeneralPurposeRegister", "position": { "line": 2, "column": 5, "offset": 10 }, "end": { "line": 2, "column": 7, "offset": 12 }, "value": "r0" },
              "type1": 0,
              "operand2": { "nodeName": "Integer", "position": { "line": 2, "column": 8, "offset": 13 }, "end": { "line": 2, "column": 9, "offset": 14 }, "value": "1" },
              "type2": 2,
              "operand3": { "nodeName": "DynamicRegister", "position": { "line": 2, "column": 10, "offset": 15 }, "end": { "line": 2, "column": 13, "offset": 18 },
                            "register": { "nodeName": "AddressRegister", "position": { "line": 2, "column": 12, "offset": 17 }, "end": { "line": 2, "column": 13, "offset": 18 }, "value": "ra" } },
              "type3": 2
            },
            {
              "nodeName": "AliasDirective",
              "position": { "line": 3, "column": 1, "offset": 21 },
              "end": { "line": 3, "column": 20, "offset": 40 },
              "identifier": { "nodeName": "Identifier", "position": { "line": 3, "column": 7, "offset": 27 }, "end": { "line": 3, "column": 10, "offset": 30 }, "value": "dev" },
              "registerOrDevice": { "nodeName": "StaticDevice", "position": { "line": 3, "column": 11, "offset": 31 }, "end": { "line": 3, "column": 16, "offset": 36 },
                                    "device": { "nodeName": "OrdinaryDevice", "position": { "line": 3, "column": 11, "offset": 31 }, "end": { "line": 3, "column": 13, "offset": 33 }, "value": "d0" },
                                    "pin": { "nodeName": "Integer", "position": { "line": 3, "column": 15, "offset": 35 }, "end": { "line": 3, "column": 16, "offset": 36 }, "value": "1" } },
              "typeHint": {
                "nodeName": "TypeHint",
                "position": { "line": 4, "column": 1, "offset": 41 },
                "end": { "line": 4, "column": 30, "offset": 70 },
                "type": "TestDevice",
                "desc": { "nodeName": "String", "position": { "line": 4, "column": 20, "offset": 60 }, "end": { "line": 4, "column": 26, "offset": 66 }, "value": "\\"blah\\"" },
                "defaults": [
                  { "nodeName": "TypeHintDefault", "position": { "line": 4, "column": 27, "offset": 67 }, "end": { "line": 4, "column": 30, "offset": 70 }, "category": "logic", "name": "Setting", "value": "1" }
                ],
                "builtin": false
              }
            },
            {
              "nodeName": "DeviceAnnotation",
              "position": { "line": 5, "column": 1, "offset": 71 },
              "end": { "line": 7, "column": 15, "offset": 150 },
              "name": "TestDevice",
              "logics": [
                { "nodeName": "DeviceAnnotationLogic", "position": { "line": 6, "column": 1, "offset": 90 }, "end": { "line": 6, "column": 20, "offset": 110 }, "name": "Setting", "value": "12", "access": "rw", "defaultValue": "1" },
                { "nodeName": "DeviceAnnotationLogic", "position": { "line": 6, "column": 21, "offset": 111 }, "end": { "line": 6, "column": 30, "offset": 120 }, "name": "On", "value": "28", "access": "w" }
              ],
              "logicSlots": [
                { "nodeName": "DeviceAnnotationLogicSlot", "position": { "line": 7, "column": 1, "offset": 121 }, "end": { "line": 7, "column": 20, "offset": 140 }, "name": "Quantity", "value": "3", "slotIndices": [0, 1] }
              ],
              "reagentHashes": [
                { "nodeName": "DeviceAnnotationReagentHash", "position": { "line": 7, "column": 21, "offset": 141 }, "end": { "line": 7, "column": 25, "offset": 145 }, "value": "$FF" }
              ],
              "slots": [
                { "nodeName": "DeviceAnnotationSlot", "position": { "line": 8, "column": 1, "offset": 146 }, "end": { "line": 8, "column": 10, "offset": 155 }, "name": "Slot0", "value": "0" }
              ]
            },
            {
              "nodeName": "EnumAnnotation",
              "position": { "line": 9, "column": 1, "offset": 156 },
              "end": { "line": 9, "column": 12, "offset": 170 },
              "name": "LogicType",
              "values": [
                { "nodeName": "EnumAnnotationValue", "position": { "line": 9, "column": 3, "offset": 158 }, "end": { "line": 9, "column": 12, "offset": 170 }, "name": "Pressure", "value": "5", "tag": "value" }
              ]
            },
            {
              "nodeName": "Error",
              "position": { "line": 10, "column": 1, "offset": 171 },
              "end": { "line": 10, "column": 4, "offset": 174 },
              "token": { "type": 12, "pos": { "line": 10, "column": 1, "offset": 171 }, "lexeme": "??", "category": 3 },
              "message": "unexpected token"
            }
          ]
        }
        """;

    private static ProgramNode parseSample() throws IOException {
        return ProgramParser.parse(PROGRAM_JSON);
    }

    // ============================================================
    // Envelope: nodeName discriminator and positions
    // ============================================================

    @Nested
    @DisplayName("Envelope")
    class Envelope {

        @Test
        @DisplayName("nodeName binds to every node, including nested ones")
        void nodeNameBinds() throws IOException {
            ProgramNode program = parseSample();

            // A nested node is the interesting case: it must not be shadowed by another key
            AliasDirectiveNode alias = (AliasDirectiveNode) program.getStatement(2);

            assertEquals("Program", program.getNodeName());
            assertEquals("LabelDef", program.getStatement(0).getNodeName());
            assertEquals("addInstruction", program.getStatement(1).getNodeName());
            assertEquals("TypeHint", alias.getTypeHint().getNodeName());
            assertEquals("TypeHintDefault", alias.getTypeHint().getDefaults().get(0).getNodeName());
        }

        @Test
        @DisplayName("position and end bind on every node")
        void positionsBind() throws IOException {
            ProgramNode program = parseSample();
            ASTNode alias = program.getStatement(2);

            assertEquals(1, program.getPosition().getLine());
            assertEquals(10, program.getEnd().getLine());
            assertEquals(3, alias.getPosition().getLine());
            assertEquals(20, alias.getEnd().getColumn());
            // length() reads end - position, so a missing end used to throw
            assertEquals(19, alias.getEnd().getColumn() - alias.getPosition().getColumn());
        }

        @Test
        @DisplayName("parseNode resolves a single node JSON")
        void parseSingleNode() throws IOException {
            ASTNode node = ProgramParser.parseNode(
                "{ \"nodeName\": \"Integer\", \"position\": { \"line\": 1, \"column\": 1, \"offset\": 0 },"
                    + " \"end\": { \"line\": 1, \"column\": 3, \"offset\": 2 }, \"value\": \"42\" }"
            );

            assertInstanceOf(IntegerNode.class, node);
            assertEquals("Integer", node.getNodeName());
            assertEquals("42", ((IntegerNode) node).getValue(), "integer value is the source spelling, a string");
        }
    }

    // ============================================================
    // Registers, devices and type hints
    // ============================================================

    @Nested
    @DisplayName("Registers, devices and type hints")
    class Operands {

        @Test
        @DisplayName("static spellings share a class, dynamic operands nest")
        void registerAndDeviceShapes() throws IOException {
            ProgramNode program = parseSample();
            TernaryInstruction add = (TernaryInstruction) program.getStatement(1);

            assertEquals("add", add.getKeyword());
            assertEquals(0, add.getType1());
            assertEquals("GeneralPurposeRegister", add.getOperand1().getNodeName());
            assertInstanceOf(DynamicRegisterNode.class, add.getOperand3());

            StaticDeviceNode device = (StaticDeviceNode)
                ((AliasDirectiveNode) program.getStatement(2)).getRegisterOrDevice();

            assertInstanceOf(StaticDevicePortNode.class, device.getDevice());
            assertEquals("d0", ((StaticDevicePortNode) device.getDevice()).getValue());
            assertEquals("1", ((IntegerNode) device.getPin()).getValue());
        }

        @Test
        @DisplayName("typeHint carries type, desc and defaults")
        void typeHint() throws IOException {
            AliasDirectiveNode alias = (AliasDirectiveNode) parseSample().getStatement(2);
            var hint = alias.getTypeHint();

            assertEquals("TestDevice", hint.getType());
            assertEquals("String", hint.getDesc().getNodeName());
            assertEquals(1, hint.getDefaults().size());
            assertEquals("logic", hint.getDefaults().get(0).getCategory());
            assertEquals("Setting", hint.getDefaults().get(0).getName());
            assertFalse(hint.isBuiltin());
        }
    }

    // ============================================================
    // Annotation nodes
    // ============================================================

    @Nested
    @DisplayName("Annotation nodes")
    class Annotations {

        @Test
        @DisplayName("device annotation lines keep their discriminator and optional keys")
        void deviceAnnotation() throws IOException {
            DeviceAnnotationNode device = (DeviceAnnotationNode) parseSample().getStatement(3);

            assertEquals("TestDevice", device.getName());
            assertEquals(2, device.getLogics().size());

            TypeAnnotationLineNode setting = device.getLogics().get(0);

            assertEquals("DeviceAnnotationLogic", setting.getNodeName());
            assertEquals("rw", setting.getAccess());
            assertEquals("1", setting.getDefaultValue());
            assertEquals("w", device.getLogics().get(1).getAccess());

            TypeAnnotationLineNode quantity = device.getLogicSlots().get(0);

            assertEquals("DeviceAnnotationLogicSlot", quantity.getNodeName());
            assertEquals(List.of(0, 1), quantity.getSlotIndices());
            assertNull(quantity.getAccess(), "a logic-slot line never carries access");

            TypeAnnotationValueNode hash = device.getReagentHashes().get(0);

            assertEquals("DeviceAnnotationReagentHash", hash.getNodeName());
            assertEquals("$FF", hash.getValue());

            assertEquals("DeviceAnnotationSlot", device.getSlots().get(0).getNodeName());
        }

        @Test
        @DisplayName("enum annotation values keep the tag key")
        void enumAnnotation() throws IOException {
            EnumAnnotationNode enumAnnotation = (EnumAnnotationNode) parseSample().getStatement(4);
            var value = enumAnnotation.getValues().get(0);

            assertEquals("LogicType", enumAnnotation.getName());
            assertEquals("EnumAnnotationValue", value.getNodeName());
            assertEquals("Pressure", value.getName());
            assertEquals("value", value.getTag());
        }

        @Test
        @DisplayName("error nodes embed the token JSON")
        void errorNode() throws IOException {
            ErrorNode error = (ErrorNode) parseSample().getStatement(5);

            assertEquals("??", error.getToken().getLexeme());
            assertEquals(12, error.getToken().getType());
            assertEquals(10, error.getToken().getPos().getLine());
        }
    }
}
