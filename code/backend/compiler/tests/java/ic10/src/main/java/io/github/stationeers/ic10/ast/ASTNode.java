// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonSubTypes;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import lombok.Data;

/**
 * Base class for all IC10 AST nodes.
 * <p>
 * Uses Jackson polymorphic deserialization with the {@code type} field
 * as the discriminator. Each concrete subclass is registered via
 * {@link JsonSubTypes} with the exact name that appears in the C++
 * {@code nodeName} / {@code toJSON()} output.
 * </p>
 *
 * <p>
 * Instruction nodes (e.g. "addInstruction") share a common structure
 * per arity and are mapped to generic arity classes
 * ({@link NullaryInstruction}, {@link UnaryInstruction}, ...,
 * {@link SenaryInstruction}). The specific instruction is identified
 * by the {@link #getType()} field (e.g. {@code "addInstruction"}).
 * </p>
 *
 * <h3>Type Hierarchy</h3>
 * <pre>
 * ASTNode
 * ├── Leaf nodes: IntegerNode, FloatNode, ..., ErrorNode
 * ├── Directive nodes: LabelDefNode, DefineDirectiveNode, AliasDirectiveNode
 * ├── Doc comment nodes: DeviceDocCommentNode, EnumDocCommentNode
 * ├── ProgramNode
 * └── InstructionNode (abstract)
 *     ├── NullaryInstruction     (2 instructions)
 *     ├── UnaryInstruction       (10 instructions)
 *     ├── BinaryInstruction      (54 instructions)
 *     ├── TernaryInstruction     (59 instructions)
 *     ├── QuaternaryInstruction  (19 instructions)
 *     ├── QuinaryInstruction     (2 instructions)
 *     └── SenaryInstruction      (1 instruction)
 * </pre>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@JsonTypeInfo(
        use = JsonTypeInfo.Id.NAME,
        include = JsonTypeInfo.As.EXISTING_PROPERTY,
        property = "type",
        visible = true
)
@JsonSubTypes({
        // ============================================================
        // Leaf literal / identifier nodes
        // ============================================================
        @JsonSubTypes.Type(value = IntegerNode.class, name = "Integer"),
        @JsonSubTypes.Type(value = FloatNode.class, name = "Float"),
        @JsonSubTypes.Type(value = HexNumberNode.class, name = "HexNumber"),
        @JsonSubTypes.Type(value = BinaryNumberNode.class, name = "BinaryNumber"),
        @JsonSubTypes.Type(value = IdentifierNode.class, name = "Identifier"),
        @JsonSubTypes.Type(value = StringNode.class, name = "String"),
        @JsonSubTypes.Type(value = RegisterNode.class, name = "Register"),
        @JsonSubTypes.Type(value = DeviceNode.class, name = "Device"),
        @JsonSubTypes.Type(value = ConstantNode.class, name = "Constant"),

        // ============================================================
        // Special / error / macro nodes
        // ============================================================
        @JsonSubTypes.Type(value = ErrorNode.class, name = "Error"),
        @JsonSubTypes.Type(value = HashCallNode.class, name = "HashCall"),
        @JsonSubTypes.Type(value = StrCallNode.class, name = "StrCall"),

        // ============================================================
        // Statement / directive nodes
        // ============================================================
        @JsonSubTypes.Type(value = LabelDefNode.class, name = "LabelDef"),
        @JsonSubTypes.Type(value = DefineDirectiveNode.class, name = "DefineDirective"),
        @JsonSubTypes.Type(value = AliasDirectiveNode.class, name = "AliasDirective"),

        // ============================================================
        // Doc comment nodes
        // ============================================================
        @JsonSubTypes.Type(value = DeviceDocCommentNode.class, name = "DeviceDocComment"),
        @JsonSubTypes.Type(value = EnumDocCommentNode.class, name = "EnumDocComment"),

        // ============================================================
        // Program root
        // ============================================================
        @JsonSubTypes.Type(value = ProgramNode.class, name = "Program"),

        // ============================================================
        // Nullary instructions (0 operands)
        // ============================================================
        @JsonSubTypes.Type(value = NullaryInstruction.class, name = "hcfInstruction"),
        @JsonSubTypes.Type(value = NullaryInstruction.class, name = "yieldInstruction"),

        // ============================================================
        // Unary instructions (1 operand)
        // ============================================================
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "peekInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "randInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "popInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "clrInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "sleepInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "clrdInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "pushInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "jalInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "jrInstruction"),
        @JsonSubTypes.Type(value = UnaryInstruction.class, name = "jInstruction"),

        // ============================================================
        // Binary instructions (2 operands) — groups RI_RON, DR_RON, RI_DR, RON_RON
        // ============================================================
        // RI_RON group
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "absInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "acosInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "asinInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "atanInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "ceilInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "cosInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "expInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "floorInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "logInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "roundInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sinInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sqrtInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "tanInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "truncInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "notInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "moveInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sgnInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "seqzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "snezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sgezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sgtzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "slezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sltzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "snanInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "snanzInstruction"),
        // DR_RON group
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bdnsInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bdnsalInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bdseInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bdsealInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brdnsInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brdseInstruction"),
        // RI_DR group
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sdnsInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "sdseInstruction"),
        // RON_RON group
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "pokeInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "beqzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "beqzalInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bnezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bnezalInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bgezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bgezalInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bgtzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bgtzalInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "blezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "blezalInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bltzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bltzalInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "bnanInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "breqzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brnezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brgezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brgtzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brlezInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brltzInstruction"),
        @JsonSubTypes.Type(value = BinaryInstruction.class, name = "brnanInstruction"),

        // ============================================================
        // Ternary instructions (3 operands)
        // ============================================================
        // RI_RON_RON group
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "addInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "atan2Instruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "divInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "maxInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "minInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "modInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "mulInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "powInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "subInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "rolInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "rorInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "andInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "norInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "orInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "slaInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sllInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sraInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "srlInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "xorInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sapzInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "snazInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "seqInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sneInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sgeInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sgtInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sleInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sltInstruction"),
        // RI_DR_RON
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "getInstruction"),
        // RI_DAR_RON
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "rmapInstruction"),
        // DR_RON_RON
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "putInstruction"),
        // RI_DR_LT
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "lInstruction"),
        // DR_LT_RI
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sInstruction"),
        // RON_LT_RI
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "sbInstruction"),
        // DR_LT_RON
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bdnvlInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bdnvsInstruction"),
        // RON_RON_RON group
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "beqInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "beqalInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bneInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bnealInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bgeInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bgealInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bgtInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bgtalInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bleInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "blealInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bltInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bltalInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bapzInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bapzalInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bnazInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "bnazalInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "breqInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "brneInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "brgeInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "brgtInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "brleInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "brltInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "brapzInstruction"),
        @JsonSubTypes.Type(value = TernaryInstruction.class, name = "brnazInstruction"),

        // ============================================================
        // Quaternary instructions (4 operands)
        // ============================================================
        // RI_RON_RON_RON group
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "clampInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "lerpInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "extInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "insInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "sapInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "snaInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "selectInstruction"),
        // DR_SI_LS_RI
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "ssInstruction"),
        // RI_RON_LT_BM
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "lbInstruction"),
        // RON_RON_LT_RI
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "sbnInstruction"),
        // RON_SI_LS_RI
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "sbsInstruction"),
        // RON_RON_RON_RON group
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "bapInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "bapalInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "bnaInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "bnaalInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "brapInstruction"),
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "brnaInstruction"),
        // RI_DR_SI_LS
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "lsInstruction"),
        // RI_DR_RM_JT
        @JsonSubTypes.Type(value = QuaternaryInstruction.class, name = "lrInstruction"),

        // ============================================================
        // Quinary instructions (5 operands)
        // ============================================================
        @JsonSubTypes.Type(value = QuinaryInstruction.class, name = "lbnInstruction"),
        @JsonSubTypes.Type(value = QuinaryInstruction.class, name = "lbsInstruction"),

        // ============================================================
        // Senary instructions (6 operands)
        // ============================================================
        @JsonSubTypes.Type(value = SenaryInstruction.class, name = "lbnsInstruction"),
})
@Data
public abstract class ASTNode {

    /** Discriminator field ("Integer", "addInstruction", ...). Set by Jackson. */
    private String type;

    /** Source position of this node. */
    private Position position;

    private Position end;

    /**
     * Returns true if this node is an instruction (any arity).
     *
     * @return true for instruction nodes
     */
    public boolean isInstruction() {
        return this instanceof InstructionNode;
    }

    /**
     * Returns true if this node is a literal value node.
     *
     * @return true for IntegerNode, FloatNode, RegisterNode, etc.
     */
    public boolean isLiteral() {
        return this instanceof ValueNode;
    }

    /**
     * Casts this node to an {@link InstructionNode} if it is one.
     *
     * @return this as InstructionNode
     * @throws ClassCastException if not an instruction
     */
    public InstructionNode asInstruction() {
        return (InstructionNode) this;
    }

    public int length() {
        return end.getColumn() - position.getColumn();
    }

    @Override
    public String toString() {
        return type + "@" + position;
    }
}
