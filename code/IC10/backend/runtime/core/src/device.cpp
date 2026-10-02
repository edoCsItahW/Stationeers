// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file device.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/07 18:40
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_runtime/device.hpp"
#include <algorithm>
#include <utility>

namespace stationeers::ic10 {

    void IDevice::tick() {}

    std::string IDevice::typeName() const { return {}; }

    std::vector<DeviceMemberView> IDevice::snapshot() const { return {}; }

    void IDevice::setMemberDefault(const std::string&, double) {}

    VirtualDevice::VirtualDevice()
        : stack_(512, 0.0) {}

    double VirtualDevice::readLogic(const std::string& prop) {
        if (const auto& it = logicProps_.find(prop); it != logicProps_.end()) return it->second;

        return 0.0;
    }

    void VirtualDevice::writeLogic(const std::string& prop, double value) { logicProps_[prop] = value; }

    bool VirtualDevice::canReadLogic(const std::string& prop) const {
        return logicProps_.contains(prop);
    }

    bool VirtualDevice::canWriteLogic(const std::string& prop [[maybe_unused]]) const { return true; }

    double VirtualDevice::readSlot(std::size_t index, const std::string& slot [[maybe_unused]]) {
        return readStack(index);
    }

    void VirtualDevice::writeSlot(std::size_t index, const std::string& slot [[maybe_unused]], double value) {
        writeStack(index, value);
    }

    double VirtualDevice::readReagent(int mode [[maybe_unused]]) { return 0.0; }

    double VirtualDevice::queryReagentAmount(int64_t reagentHash [[maybe_unused]]) { return 0.0; }

    int64_t VirtualDevice::getTypeHash() const { return 0; }

    int64_t VirtualDevice::getNameHash() const { return 0; }

    double VirtualDevice::readStack(std::size_t index) { return stack_[index]; }

    void VirtualDevice::writeStack(std::size_t index, double value) { stack_[index] = value; }

    void VirtualDevice::clearStack() { std::fill(stack_.begin(), stack_.end(), 0.0); }

    // SimDevice

    SimDevice::SimDevice(DeviceType type, std::string typeName)
        : type_(std::move(type))
        , typeName_(typeName.empty() ? type_.name : std::move(typeName)) {}

    std::optional<double> SimDevice::memberDefault(const std::string& prop) const noexcept {
        const auto* member = type_.findMember(prop);

        return member ? member->defaultValue : std::nullopt;
    }

    double SimDevice::readLogic(const std::string& prop) {
        if (const auto& it = logicProps_.find(prop); it != logicProps_.end()) return it->second;

        // 从未赋值：注解声明了默认值就以默认值作答（默认值不一定是 0，故不能一律返回 0）
        if (const auto declared = memberDefault(prop); declared) return *declared;

        return VirtualDevice::readLogic(prop);
    }

    bool SimDevice::canReadLogic(const std::string& prop) const {
        if (VirtualDevice::canReadLogic(prop)) return true;

        // 声明了默认值的成员始终可读：默认值就是设备提供的取值
        return memberDefault(prop).has_value();
    }

    void SimDevice::setMemberDefault(const std::string& name, double value) {
        // 只有注解声明过的成员才接受覆写：注解给出的字段范围就是默认值的落点
        for (auto& member : type_.members)
            if (member.name == name) {
                member.defaultValue = value;

                return;
            }
    }

    int64_t SimDevice::getTypeHash() const { return type_.deviceHash; }

    int64_t SimDevice::getNameHash() const { return type_.nameHash; }

    std::string SimDevice::typeName() const { return typeName_; }

    std::vector<DeviceMemberView> SimDevice::snapshot() const {
        std::vector<DeviceMemberView> views;

        views.reserve(type_.members.size() + logicProps_.size());

        // 型号声明的成员：顺序与注解一致；只有被赋过值才有值，否则值为空（默认值不一定是 0）
        for (const auto& member : type_.members) {
            const auto& it = logicProps_.find(member.name);

            const bool assigned = it != logicProps_.end();

            views.push_back(
                {member.name, member.kind, member.value, member.defaultValue,
                 assigned ? std::optional<double>(it->second) : std::nullopt, true, assigned}
            );
        }

        // 程序写过而型号没有声明的属性：如实列出（无型号设备只有这一类）；按名字排序保证输出稳定
        std::vector<std::string> undeclared;

        for (const auto& [name, value] : logicProps_)
            if (!type_.contains(name)) undeclared.push_back(name);

        std::ranges::sort(undeclared);

        for (const auto& name : undeclared)
            views.push_back({name, "logic", {}, std::nullopt, logicProps_.at(name), false, true});

        return views;
    }

}  // namespace stationeers::ic10
