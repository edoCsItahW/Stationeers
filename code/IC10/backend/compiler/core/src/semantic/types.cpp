// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/07/16 10:15
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/semantic/types.hpp"

#include <sstream>

namespace stationeers::ic10 {

    void TypeTable::registerType(CustomType type) {
        std::visit([this]<typename T>(T&& t) {
            types_[t.name] = std::move(t);
        }, type);
    }

    const CustomType* TypeTable::find(const std::string& name) const noexcept {
        if (const auto res = types_.find(name); res != types_.end())
            return &res->second;

        return nullptr;
    }

    std::string TypeTable::toJSON() const {
        std::stringstream ss;

        ss << "{";

        bool first = true;

        for (const auto& [name, type] : types_) {
            if (!first) [[likely]]
                ss << ", ";

            ss << std::format(R"("{}": )", name);

            std::visit(
                [&ss](const auto& t) {
                    using T = std::decay_t<decltype(t)>;

                    if constexpr (std::is_same_v<T, DeviceType>) {
                        ss << std::format(R"({{"type": "device", "name": "{}")", t.name);

                        if (t.desc)
                            ss << ", " << std::format(R"("desc": {})", t.desc->toJSON());

                        // slots
                        ss << R"(, "slots": [)";
                        for (size_t i = 0; i < t.slots.size(); ++i) {
                            if (i > 0) ss << ", ";
                            const auto& s = t.slots[i];
                            ss << std::format(
                                R"({{"index": "{}", "direction": "{}")", s.index,
                                s.direction == SlotDirection::INPUT ? "input" : "output"
                            );
                            if (s.desc)
                                ss << ", " << std::format(R"("desc": {})", s.desc->toJSON());
                            ss << "}";
                        }
                        ss << "]";

                        // logics
                        ss << R"(, "logics": [)";
                        for (size_t i = 0; i < t.logics.size(); ++i) {
                            if (i > 0) ss << ", ";
                            const auto& l = t.logics[i];
                            const char* access = l.access == LogicAccess::R    ? "R"
                                               : l.access == LogicAccess::W  ? "W"
                                                                             : "RW";
                            ss << std::format(
                                R"({{"name": "{}", "access": "{}"}})", l.name, access
                            );
                        }
                        ss << "]";

                        // modes
                        ss << R"(, "modes": [)";
                        for (size_t i = 0; i < t.modes.size(); ++i) {
                            if (i > 0) ss << ", ";
                            const auto& m = t.modes[i];
                            ss << std::format(R"({{"index": "{}")", m.index);
                            if (m.desc)
                                ss << ", " << std::format(R"("desc": {})", m.desc->toJSON());
                            ss << "}";
                        }
                        ss << "]";

                        // logicSlots
                        ss << R"(, "logicSlots": [)";
                        for (size_t i = 0; i < t.logicSlots.size(); ++i) {
                            if (i > 0) ss << ", ";
                            ss << std::format(R"("{}")", t.logicSlots[i].name);
                        }
                        ss << "]";

                        // connects
                        ss << R"(, "connects": [)";
                        for (size_t i = 0; i < t.connects.size(); ++i) {
                            if (i > 0) ss << ", ";
                            const auto& c = t.connects[i];
                            ss << std::format(R"({{"index": "{}")", c.index);
                            if (c.desc)
                                ss << ", " << std::format(R"("desc": {})", c.desc->toJSON());
                            ss << "}";
                        }
                        ss << "]";

                        ss << "}";
                    } else if constexpr (std::is_same_v<T, EnumType>) {
                        ss << std::format(R"({{"type": "enum", "name": "{}")", t.name);

                        if (t.desc)
                            ss << ", " << std::format(R"("desc": {})", t.desc->toJSON());

                        ss << R"(, "values": [)";
                        for (size_t i = 0; i < t.values.size(); ++i) {
                            if (i > 0) ss << ", ";
                            const auto& v = t.values[i];
                            ss << std::format(R"({{"name": "{}", "value": "{}")", v.name, v.value);
                            if (v.desc)
                                ss << ", " << std::format(R"("desc": {})", v.desc->toJSON());
                            ss << "}";
                        }
                        ss << "]";

                        ss << "}";
                    }
                },
                type
            );

            first = false;
        }

        ss << "}";

        return ss.str();
    }

}  // namespace stationeers::ic10
