// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device_type.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/10/05 12:00
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/device_type.hpp"
#include "ic10_runtime/value/value.hpp"
#include <cstddef>
#include <format>
#include <optional>
#include <utility>

namespace stationeers::ic10 {

    namespace {

        /**
         * @if zh
         * @brief 解析注解里的哈希取值
         * @tparam T 注解行类型（`@device-hash` / `@name-hash`）
         * @param hash 注解行
         * @return 哈希值；缺失或不是数字文本时为 0
         * @else
         * @brief Parse the hash declared by an annotation line
         * @tparam T Annotation line type (`@device-hash` / `@name-hash`)
         * @param hash The annotation line
         * @return The hash, or 0 when absent or not a numeric text
         * @endif
         */
        template<typename T>
        std::int64_t hashOf(const std::optional<T>& hash) {
            if (!hash) return 0;

            const auto value = numericText(hash->value);

            return value ? static_cast<std::int64_t>(*value) : 0;
        }

    }  // namespace

    const DeviceMemberDecl* DeviceType::findMember(const std::string& name) const noexcept {
        for (const auto& member : members)
            if (member.name == name) return &member;

        return nullptr;
    }

    bool DeviceType::contains(const std::string& name) const noexcept {
        return findMember(name) != nullptr;
    }

    bool DeviceType::empty() const noexcept { return members.empty(); }

    void DeviceRegistry::add(const DeviceAnnotation& annotation) {
        DeviceType type;

        type.name = annotation.name;

        // 型号名缺失（注解块写坏了）时不登记：空名字的型号没有意义，还会被当成合法查找结果
        if (type.name.empty()) return;

        type.deviceHash = hashOf(annotation.deviceHash);
        type.nameHash   = hashOf(annotation.nameHash);

        // 哈希行也作为成员暴露，宿主与调试器才能看到型号的身份信息
        if (annotation.deviceHash)
            type.members.push_back({"deviceHash", annotation.deviceHash->value, "hash"});

        if (annotation.nameHash)
            type.members.push_back({"nameHash", annotation.nameHash->value, "hash"});

        for (const auto& logic : annotation.logics)
            type.members.push_back({logic.name, logic.value, "logic"});

        for (const auto& slot : annotation.logicSlots)
            type.members.push_back({slot.name, slot.value, "logic-slot"});

        for (const auto& slot : annotation.slots)
            type.members.push_back({slot.name, slot.value, "slot"});

        // 试剂哈希没有名字，只有哈希值，用序号区分
        for (std::size_t i = 0; i < annotation.reagentHashes.size(); ++i)
            type.members.push_back(
                {std::format("reagent-hash[{}]", i), annotation.reagentHashes[i].value, "reagent-hash"}
            );

        types_[type.name] = std::move(type);
    }

    bool DeviceRegistry::addFrom(const TypeTable& types, const std::string& name) {
        const CustomType* type = types.find(name);

        if (!type) return false;

        const auto* annotation = std::get_if<DeviceAnnotation>(type);

        if (!annotation) return false;

        add(*annotation);

        return true;
    }

    const DeviceType* DeviceRegistry::find(const std::string& name) const noexcept {
        if (const auto& it = types_.find(name); it != types_.end()) return &it->second;

        return nullptr;
    }

    std::size_t DeviceRegistry::size() const noexcept { return types_.size(); }

    bool DeviceRegistry::empty() const noexcept { return types_.empty(); }

}  // namespace stationeers::ic10
