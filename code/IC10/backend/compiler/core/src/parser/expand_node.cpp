// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file expand_node.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/09/01 15:36
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/parser/ast/expand_node.hpp"

namespace stationeers::ic10 {

    // Link

    Pos Link::end() const { return endPos; }

    std::string Link::toString() const {
        std::stringstream ss;

        ss << ".";

        for (const auto& ref : references) {
            ss << "/";

            bool first = true;
            for (const auto& section : ref) {
                if (first)
                    first = false;
                else ss << ".";

                ss << section;
            }
        }

        return ss.str();
    }

    std::string Link::toJSON() const {
        return jsonBase<"references">(seqJSON(references, [](const auto& ref) {
            return toJson(ref);
        }));
    }

    // TypeHintDesc

    Pos TypeHintDesc::end() const {
        return call(desc, [](const auto& desc) { return desc.end(); });
    }

    std::string TypeHintDesc::toString() const {
        return std::format("@desc {}", call(desc, [](const auto& desc) {
                               return desc.toString();
                           }));
    }

    std::string TypeHintDesc::toJSON() const { return jsonBase<"desc", "tag">(desc, "desc"); }

    // TypeHint

    Pos TypeHint::end() const { return endPos; }

    std::string TypeHint::toString() const {
        std::stringstream ss;

        if (type) ss << type->toString() << " ";

        if (desc) ss << desc->toString() << " ";

        if (builtin) ss << "@builtin";

        return ss.str();
    }

    std::string TypeHint::toJSON() const {
        return jsonBase<"type", "desc", "builtin">(
            type ? std::optional(type->toJSON()) : std::nullopt,
            desc ? std::optional(desc->toJSON()) : std::nullopt, builtin
        );
    }

    // EnumAnnotationValue

    Pos EnumAnnotationValue::end() const { return endPos; }

    std::string EnumAnnotationValue::toString() const {
        auto result = std::format("@value {} {}", name, value);

        if (desc) result += " " + call(*desc, [](const auto& desc) { return desc.toString(); });

        return result;
    }

    std::string EnumAnnotationValue::toJSON() const {
        return jsonBase<"name", "value", "link", "tag">(
            name, value,
            desc ? std::optional(call(*desc, [](const auto& desc) { return desc.toJSON(); }))
                 : std::nullopt, "value"
        );
    }

    // EnumAnnotation

    Pos EnumAnnotation::end() const { return endPos; }

    std::string EnumAnnotation::toString() const {
        std::stringstream ss;

        ss << "#> @enum\n" << "#> @name " << name << "\n";

        if (desc)
            ss << "#> @desc " << call(*desc, [](const auto& desc) { return desc.toString(); })
               << "\n";

        for (const auto& value : values) ss << "#> " << value.toString() << "\n";

        ss << "#> @enum-end";

        return ss.str();
    }

    std::string EnumAnnotation::toJSON() const {
        return jsonBase<"name", "desc", "values">(
            name,
            desc ? std::optional(call(*desc, [](const auto& desc) { return desc.toJSON(); }))
                 : std::nullopt,
            seqJSON(values, [](const auto& value) { return value.toJSON(); })
        );
    }

    // DeviceAnnotation

    Pos DeviceAnnotation::end() const { return endPos; }

    std::string DeviceAnnotation::toString() const {
        std::stringstream ss;

        ss << "#> @device\n" << "#> @name " << name << "\n";

        if (desc)
            ss << "#> @desc " << call(*desc, [](const auto& desc) { return desc.toString(); })
               << "\n";

        if (deviceHash) ss << "#> @device-hash " << deviceHash->toString() << "\n";

        if (nameHash) ss << "#> @name-hash " << nameHash->toString() << "\n";

        for (const auto& logic : logics) ss << "#> " << logic.toString() << "\n";

        for (const auto& logicSlot : logicSlots) ss << "#> " << logicSlot.toString() << "\n";

        for (const auto& reagentHash : reagentHashes) ss << "#> " << reagentHash.toString() << "\n";

        ss << "#> @device-end";

        return ss.str();
    }

    std::string DeviceAnnotation::toJSON() const {
        return jsonBase<
            "name", "desc", "deviceHash", "nameHash", "logics", "logicSlots", "reagentHashes">(
            name,
            desc ? std::optional(call(*desc, [](const auto& desc) { return desc.toJSON(); }))
                 : std::nullopt,
            deviceHash ? std::optional(deviceHash->toJSON()) : std::nullopt,
            nameHash ? std::optional(nameHash->toJSON()) : std::nullopt,
            seqJSON(logics, [](const auto& logic) { return logic.toJSON(); }),
            seqJSON(logicSlots, [](const auto& logicSlot) { return logicSlot.toJSON(); }),
            seqJSON(reagentHashes, [](const auto& reagentHash) { return reagentHash.toJSON(); })
        );
    }

}  // namespace stationeers::ic10
