// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.IOException;

/**
 * Utility for parsing IC10 AST JSON (produced by C++ {@code toJSON()})
 * into structured Java objects.
 * <p>
 * This class wraps a pre-configured Jackson {@link ObjectMapper} with
 * polymorphic type resolution enabled via {@link ASTNode}'s
 * {@code @JsonTypeInfo} + {@code @JsonSubTypes} annotations.
 * </p>
 *
 * <h3>Usage</h3>
 * <pre>{@code
 * // From the JNI binding:
 * io.github.stationeers.ic10.Program nativeProgram = ...;
 * String json = nativeProgram.toJSON();
 *
 * // Parse into typed AST:
 * ProgramNode ast = ProgramParser.parse(json);
 * for (ASTNode stmt : ast.getStatements()) {
 *     switch (stmt.getType()) {
 *         case "addInstruction" -> {
 *             TernaryInstruction ins = (TernaryInstruction) stmt;
 *             System.out.println("add: " + ins.getOperand1() + " + " + ins.getOperand2());
 *         }
 *         case "LabelDef" -> {
 *             LabelDefNode label = (LabelDefNode) stmt;
 *             System.out.println("label: " + label.getIdentifier());
 *         }
 *     }
 * }
 * }</pre>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
public final class ProgramParser {

    private static final ObjectMapper MAPPER = new ObjectMapper()
            .configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);

    private ProgramParser() {
        // utility class
    }

    /**
     * Returns the shared, pre-configured ObjectMapper.
     * The returned mapper is thread-safe for reading.
     *
     * @return the ObjectMapper instance
     */
    public static ObjectMapper mapper() {
        return MAPPER;
    }

    /**
     * Parses an IC10 Program JSON string into a typed {@link ProgramNode}.
     *
     * @param json the JSON string from C++ {@code Program::toJSON()}
     * @return the parsed Program AST
     * @throws IOException if parsing fails
     */
    public static ProgramNode parse(String json) throws IOException {
        return MAPPER.readValue(json, ProgramNode.class);
    }

    /**
     * Parses a generic AST node JSON string.
     * Used for parsing individual nodes (e.g. from incremental APIs).
     *
     * @param json the JSON string from any C++ {@code toJSON()}
     * @return the parsed AST node
     * @throws IOException if parsing fails
     */
    public static ASTNode parseNode(String json) throws IOException {
        return MAPPER.readValue(json, ASTNode.class);
    }
}
