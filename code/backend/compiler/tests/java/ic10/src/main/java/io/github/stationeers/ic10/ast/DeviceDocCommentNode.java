// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.

package io.github.stationeers.ic10.ast;

import com.fasterxml.jackson.annotation.JsonInclude;
import java.util.List;

/**
 * Device doc comment node ("DeviceDocComment") — "#> @device ... #> @end-device".
 *
 * @author edocsitahw
 * @since 1.1.0
 */
@JsonInclude(JsonInclude.Include.NON_NULL)
public class DeviceDocCommentNode extends ASTNode {
    private String name;
    private String desc;        // optional
    private List<SlotEntry> slots;
    private List<LogicEntry> logics;
    private List<ModeEntry> modes;
    private List<LogicSlotEntry> logicSlots;
    private List<ConnectEntry> connects;

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getDesc() { return desc; }
    public void setDesc(String desc) { this.desc = desc; }

    public List<SlotEntry> getSlots() { return slots; }
    public void setSlots(List<SlotEntry> slots) { this.slots = slots; }

    public List<LogicEntry> getLogics() { return logics; }
    public void setLogics(List<LogicEntry> logics) { this.logics = logics; }

    public List<ModeEntry> getModes() { return modes; }
    public void setModes(List<ModeEntry> modes) { this.modes = modes; }

    public List<LogicSlotEntry> getLogicSlots() { return logicSlots; }
    public void setLogicSlots(List<LogicSlotEntry> logicSlots) { this.logicSlots = logicSlots; }

    public List<ConnectEntry> getConnects() { return connects; }
    public void setConnects(List<ConnectEntry> connects) { this.connects = connects; }

    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class SlotEntry {
        private String number;
        private String direction;  // "input" or "output"
        private String desc;       // optional

        public String getNumber() { return number; }
        public void setNumber(String number) { this.number = number; }
        public String getDirection() { return direction; }
        public void setDirection(String direction) { this.direction = direction; }
        public String getDesc() { return desc; }
        public void setDesc(String desc) { this.desc = desc; }
    }

    public static class LogicEntry {
        private String name;
        private String access;  // "r", "w", or "rw"

        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
        public String getAccess() { return access; }
        public void setAccess(String access) { this.access = access; }
    }

    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class ModeEntry {
        private String number;
        private String desc;  // optional

        public String getNumber() { return number; }
        public void setNumber(String number) { this.number = number; }
        public String getDesc() { return desc; }
        public void setDesc(String desc) { this.desc = desc; }
    }

    public static class LogicSlotEntry {
        private String name;

        public String getName() { return name; }
        public void setName(String name) { this.name = name; }
    }

    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static class ConnectEntry {
        private String number;
        private String desc;  // optional

        public String getNumber() { return number; }
        public void setNumber(String number) { this.number = number; }
        public String getDesc() { return desc; }
        public void setDesc(String desc) { this.desc = desc; }
    }
}
