// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonToken;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Root AST node ("Program") representing the entire IC10 program.
 * <p>
 * Note: The C++ {@code Program::toJSON()} serializes the {@code statements}
 * array as a <b>JSON string</b> (double-encoded). This class uses a custom
 * deserializer to handle this two-step deserialization:
 * </p>
 * <ol>
 *   <li>Read the {@code "statements"} string value</li>
 *   <li>Parse it as a JSON array</li>
 *   <li>Deserialize each element as a polymorphic {@link ASTNode}</li>
 * </ol>
 *
 * <pre>{@code
 * // Example usage:
 * ProgramNode program = ProgramParser.parse(program.toJSON());
 * for (ASTNode stmt : program.getStatements()) {
 *     if (stmt.isInstruction()) {
 *         InstructionNode ins = stmt.asInstruction();
 *         System.out.println(ins.getKeyword() + " arity=" + ins.getArity());
 *     }
 * }
 * }</pre>
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonDeserialize(using = ProgramNode.ProgramDeserializer.class)
public class ProgramNode extends ASTNode {

    /** Top-level source node name. Always "Program". */
    public static final String NODE_NAME = "Program";

    private List<ASTNode> statements;

    public ProgramNode() {
        setType(NODE_NAME);
    }

    /**
     * Returns the number of top-level statements.
     *
     * @return statement count
     */
    public int size() {
        return statements != null ? statements.size() : 0;
    }

    /**
     * Returns the statement at the given index.
     *
     * @param index 0-based index
     * @return the statement, or null if out of bounds
     */
    public ASTNode getStatement(int index) {
        if (statements != null && index >= 0 && index < statements.size()) {
            return statements.get(index);
        }
        return null;
    }

    /**
     * Iterates over all statements and invokes the given visitor.
     *
     * @param visitor callback for each statement
     */
    public void forEachStatement(java.util.function.Consumer<ASTNode> visitor) {
        if (statements != null) {
            statements.forEach(visitor);
        }
    }

    /**
     * Custom deserializer that handles the double-encoded {@code statements} field.
     * The C++ serializer writes statements as a JSON-encoded string (not a raw array).
     */
    public static class ProgramDeserializer extends JsonDeserializer<ProgramNode> {

        private static final TypeReference<List<ASTNode>> STATEMENT_LIST_TYPE =
                new TypeReference<>() {
                };

        @Override
        public ProgramNode deserialize(JsonParser p, DeserializationContext ctxt)
                throws IOException {

            ProgramNode node = new ProgramNode();

            // Manually parse the JSON object fields
            if (p.currentToken() != JsonToken.START_OBJECT)
                throw new IOException("Expected START_OBJECT for Program");

            while (p.nextToken() != JsonToken.END_OBJECT) {
                String fieldName = p.currentName();
                p.nextToken(); // move to value

                switch (fieldName) {
                    case "type":
                        node.setType(p.getText());
                        break;
                    case "position":
                        Position pos = ctxt.readValue(p, Position.class);
                        node.setPosition(pos);
                        break;
                    case "statements":
                        // statements is a JSON string — parse it again
                        String statementsJson = p.getText();
                        if (statementsJson != null && !statementsJson.isEmpty()) {
                            ObjectMapper mapper = (ObjectMapper) p.getCodec();
                            List<ASTNode> stmts = mapper.readValue(
                                    statementsJson, STATEMENT_LIST_TYPE);
                            node.setStatements(stmts);
                        } else
                            node.setStatements(new ArrayList<>());

                        break;
                    default:
                        // skip unknown fields (future-proofing)
                        p.skipChildren();
                        break;
                }
            }

            return node;
        }
    }

    @Override
    public String toString() {
        int stmtCount = statements != null ? statements.size() : 0;
        return "Program[" + stmtCount + " statements]";
    }
}
