// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;

/**
 * Device doc comment node ("DeviceDocComment") — "#> @device ... #> @end-device".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@Data
@EqualsAndHashCode(callSuper = true)
@JsonInclude(JsonInclude.Include.NON_NULL)
public class DeviceDocCommentNode extends ASTNode {
    private String name;
    private String desc;        // optional
    private List<SlotEntry> slots;
    private List<LogicEntry> logics;
    private List<ModeEntry> modes;
    private List<LogicSlotEntry> logicSlots;
    private List<ConnectEntry> connects;

    @Data
    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class SlotEntry {
        private String number;
        private String direction;  // "input" or "output"
        private String desc;       // optional
    }

    @Data
    public static class LogicEntry {
        private String name;
        private String access;  // "r", "w", or "rw"
    }

    @Data
    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class ModeEntry {
        private String number;
        private String desc;  // optional
    }

    @Data
    public static class LogicSlotEntry {
        private String name;
    }

    @Data
    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class ConnectEntry {
        private String number;
        private String desc;  // optional
    }
}
